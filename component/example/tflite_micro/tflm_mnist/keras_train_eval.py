import os
import argparse
import numpy as np

import tensorflow.keras as keras
from keras import layers
from keras_flops import get_flops


def load_dataset():
    # Load the data and split it between train and test sets
    (x_train, y_train), (x_test, y_test) = keras.datasets.mnist.load_data()

    # Scale images to the [0, 1] range
    x_train = x_train.astype('float32') / 255
    x_test = x_test.astype('float32') / 255
    # Make sure images have shape (28, 28, 1) HWC format
    x_train = np.expand_dims(x_train, -1)
    x_test = np.expand_dims(x_test, -1)

    # convert class vectors to binary class matrices
    y_train = keras.utils.to_categorical(y_train, 10)
    y_test = keras.utils.to_categorical(y_test, 10)

    return x_train, y_train, x_test, y_test


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
    parser = argparse.ArgumentParser(description='Keras MNIST Example')
    parser.add_argument('--batch-size', type=int, default=64, metavar='N',
                        help='input batch size for training (default: 64)')
    parser.add_argument('--epochs', type=int, default=10, metavar='N',
                        help='number of epochs to train (default: 14)')
    parser.add_argument('--lr', type=float, default=0.01, metavar='LR',
                        help='learning rate (default: 0.01)')
    parser.add_argument('--output', type=str, default='keras_mnist_conv',
                        help='directory for saving model')
    args = parser.parse_args()

    x_train, y_train, x_test, y_test = load_dataset()

    model = keras.Sequential(
        [
            keras.Input(shape=(28, 28, 1)),
            layers.Conv2D(16, kernel_size=(3, 3), activation='relu'),
            layers.MaxPooling2D(pool_size=(2, 2)),
            layers.Conv2D(32, kernel_size=(3, 3), activation='relu'),
            layers.MaxPooling2D(pool_size=(2, 2)),
            layers.Dropout(0.25),
            layers.Flatten(),
            layers.Dense(64, activation='relu'),
            layers.Dropout(0.5),
            layers.Dense(10),
        ]
    )

    model.summary()
    flops = get_flops(model, batch_size=1)
    print(f'Floating-point operations per inference: {human_readable(flops)}')

    loss_fn = keras.losses.CategoricalCrossentropy(from_logits=True)
    opt = keras.optimizers.Adam(learning_rate=args.lr)
    model.compile(loss=loss_fn, optimizer=opt, metrics=['accuracy'])

    model.fit(x_train, y_train,
              batch_size=args.batch_size,
              epochs=args.epochs, validation_split=0.1)
    score = model.evaluate(x_test, y_test, verbose=0)
    print(f'Test loss: {score[0]:.4f}')
    print(f'Test accuracy: {100. * score[1]:.2f}%')

    model.save(os.path.join(args.output, 'saved_model'), save_format='tf')


if __name__ == '__main__':
    main()
