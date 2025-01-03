import os
import argparse
import numpy as np

import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms
from torch.optim.lr_scheduler import StepLR

import onnxruntime as ort

from ptflops import get_model_complexity_info


class ConvNet(nn.Module):
    """ A simple convolution based model """
    def __init__(self):
        super(ConvNet, self).__init__()
        self.convs = nn.Sequential(
                nn.Conv2d(1, 16, 3, 1),
                torch.nn.ReLU(),
                nn.MaxPool2d(2),
                nn.Conv2d(16, 32, 3, 1),
                torch.nn.ReLU(),
                nn.MaxPool2d(2),
                nn.Dropout(0.25)
                )
        self.fcs = nn.Sequential(
                nn.Linear(800, 64),
                torch.nn.ReLU(),
                nn.Dropout(0.5),
                nn.Linear(64, 10)
                )

    def forward(self, x):
        x = self.convs(x)
        # permute from nchw to nhwc, not necessary for model training
        # but can remove a transpose ops in final .tflite file
        x = x.permute(0, 2, 3, 1)
        x = torch.flatten(x, 1)
        output = self.fcs(x)

        return output


def train(args, model, device, train_loader, optimizer, epoch):
    model.train()
    for batch_idx, (data, target) in enumerate(train_loader):
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)
        loss = F.cross_entropy(output, target)
        loss.backward()
        optimizer.step()
        if batch_idx % args.log_interval == 0:
            print(f'Train Epoch: {epoch} [{batch_idx * len(data)}/' \
                  f'{len(train_loader.dataset)} ' \
                  f'({100. * batch_idx / len(train_loader):.0f}%)]' \
                  f'\tLoss: {loss.item():.6f}')


def test(model, device, test_loader):
    model.eval()
    test_loss = 0
    correct = 0
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)
            test_loss += F.cross_entropy(output, target, reduction='sum').item()  # sum up batch loss
            pred = output.argmax(dim=1, keepdim=True)  # get the index of the max log-probability
            correct += pred.eq(target.view_as(pred)).sum().item()

    test_loss /= len(test_loader.dataset)

    print('Test set: Average loss: {test_loss:.4f}, '\
          f'Accuracy: {correct}/{len(test_loader.dataset)} '\
          f'({100. * correct / len(test_loader.dataset):.2f}%)')


def export_onnx(model, onnx_path, input_shape):
    dummy_input = torch.randn(input_shape)
    torch.onnx.export(model.cpu(),
                      (dummy_input,),
                      onnx_path,
                      do_constant_folding=True,
                      export_params=True)
    check_consistency(model, onnx_path, input_shape)


def to_numpy(tensor):
    return tensor.detach().cpu().numpy() if tensor.requires_grad \
        else tensor.cpu().numpy()


def check_consistency(model, onnx_path, input_shape):
    model.eval()
    dummy_input = torch.randn(input_shape)
    torch_output = model(dummy_input)

    ort_session = ort.InferenceSession(
            onnx_path,
            providers=['CPUExecutionProvider'])
    ort_input = {ort_session.get_inputs()[0].name: to_numpy(dummy_input)}
    ort_output = ort_session.run(
            [ort_session.get_outputs()[0].name], ort_input)[0]
    np.testing.assert_allclose(to_numpy(torch_output), ort_output,
                               rtol=1e-03, atol=1e-05)

    print('torch and onnx consistency test pass!')


def human_readable(number):
    if number/1000 < 1:
        return f'{number}'
    elif number/1000/1000 <1:
        return f'{number/1000:.2f}K'
    elif number/1000/1000/1000 < 1:
        return f'{number/1000/1000:.2f}M'
    else:
        return f'{number/1000/1000/1000:.2f}G'


def main():
    # Training settings
    parser = argparse.ArgumentParser(description='PyTorch MNIST Example')
    parser.add_argument('--batch-size', type=int, default=64, metavar='N',
                        help='input batch size for training (default: 64)')
    parser.add_argument('--test-batch-size', type=int, default=1000, metavar='N',
                        help='input batch size for testing (default: 1000)')
    parser.add_argument('--epochs', type=int, default=10, metavar='N',
                        help='number of epochs to train (default: 14)')
    parser.add_argument('--lr', type=float, default=0.01, metavar='LR',
                        help='learning rate (default: 0.01)')
    parser.add_argument('--gamma', type=float, default=0.7, metavar='M',
                        help='Learning rate step gamma (default: 0.7)')
    parser.add_argument('--no-cuda', action='store_true', default=False,
                        help='disables CUDA training')
    parser.add_argument('--seed', type=int, default=1, metavar='S',
                        help='random seed (default: 1)')
    parser.add_argument('--log-interval', type=int, default=100, metavar='N',
                        help='how many batches to wait before logging training status')
    parser.add_argument('--output', type=str, default='torch_mnist_conv',
                        help='directory for saving model')
    args = parser.parse_args()
    use_cuda = not args.no_cuda and torch.cuda.is_available()

    torch.manual_seed(args.seed)

    if use_cuda:
        device = torch.device('cuda')
    else:
        device = torch.device('cpu')

    train_kwargs = {'batch_size': args.batch_size}
    test_kwargs = {'batch_size': args.test_batch_size}
    if use_cuda:
        cuda_kwargs = {'num_workers': 1,
                       'pin_memory': True,
                       'shuffle': True}
        train_kwargs.update(cuda_kwargs)
        test_kwargs.update(cuda_kwargs)

    transform=transforms.Compose([
        transforms.ToTensor(),
        ]) # change image type to tensor, CHW format
    dataset1 = datasets.MNIST('../data', train=True, download=True,
                       transform=transform)
    dataset2 = datasets.MNIST('../data', train=False,
                       transform=transform)
    train_loader = torch.utils.data.DataLoader(dataset1, **train_kwargs)
    test_loader = torch.utils.data.DataLoader(dataset2, **test_kwargs)

    model = ConvNet().to(device)
    optimizer = optim.Adam(model.parameters(), lr=args.lr)
    scheduler = StepLR(optimizer, step_size=1, gamma=args.gamma)

    # paying attention to model size and operation numbers
    macs, params = get_model_complexity_info(model, (1,28,28), as_strings=False)
    print(f'Multiply-Accumulate per inference: {human_readable(macs)}')
    # FLOPs is about 2*MACs
    print(f'Floating-point Operations per inference: {human_readable(2*macs)}')
    print(f'Number of model parameters: {human_readable(params)}')

    for epoch in range(1, args.epochs + 1):
        train(args, model, device, train_loader, optimizer, epoch)
        test(model, device, test_loader)
        scheduler.step()
    os.makedirs(args.output, exist_ok=True)
    torch_output_path = os.path.join(args.output, 'model.pt')
    torch.save(model.state_dict(), torch_output_path)
    onnx_output_path = os.path.join(args.output, 'model.onnx')
    input_shape = (1, 1, 28, 28)
    export_onnx(model, onnx_output_path, input_shape)


if __name__ == '__main__':
    main()
