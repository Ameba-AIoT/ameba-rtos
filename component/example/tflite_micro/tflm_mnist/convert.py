import os
import argparse
import numpy as np

import tensorflow as tf
import tensorflow.keras as keras
from tensorflow.lite.python import interpreter as interpreter_wrapper


def onnx_to_tf(onnx_path, output_dir):
    import onnx # pylint:disable=import-outside-toplevel
    from onnx_tf.backend import prepare # pylint:disable=import-outside-toplevel

    onnx_model = onnx.load(onnx_path)
    output_dir = os.path.join(output_dir, 'saved_model')

    tf_rep = prepare(onnx_model)
    tf_rep.export_graph(output_dir)


def load_dataset(channel_order='keras'):
    # Load the data and split it between train and test sets
    (x_train, y_train), (x_test, y_test) = keras.datasets.mnist.load_data()

    # Scale images to the [0, 1] range
    x_train = x_train.astype('float32') / 255
    x_test = x_test.astype('float32') / 255
    if channel_order == 'keras':
        # shape (1, 28, 28, 1) NHWC format
        x_train = np.expand_dims(x_train, -1)
        x_test = np.expand_dims(x_test, -1)
    elif channel_order == 'torch':
        # shape (1, 1, 28, 28) NCHW format
        x_train = np.expand_dims(x_train, 1)
        x_test = np.expand_dims(x_test, 1)

    return x_train, y_train, x_test, y_test


def repr_dataset():
    x_train, y_train, x_test, y_test = load_dataset()
    calibration_num = 100
    for i in range(calibration_num):
        # expand with batch=1
        yield [np.expand_dims(x_train[i], 0)]


def repr_dataset_torch():
    x_train, y_train, x_test, y_test = load_dataset(channel_order='torch')
    calibration_num = 100
    for i in range(calibration_num):
        # expand with batch=1
        yield [np.expand_dims(x_train[i], 0)]


def quantize_data(data, quant_dict):
    """ quantize data from float to int8 """
    scale, zerop = quant_dict
    data_1 = data / scale + zerop
    data_1 = tf.where(data_1 > 0,
                      tf.math.nextafter(data_1, np.inf),
                      tf.math.nextafter(data_1, -np.inf))
    return tf.cast(tf.clip_by_value(tf.round(data_1), -128, 127), tf.int8)


def tflite_test(interpreter, x_test, y_test):
    test_dataset = tf.data.Dataset.from_tensor_slices((x_test, y_test))
    test_dataset = test_dataset.batch(1)

    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    interpreter.allocate_tensors()

    correct = 0
    for x, y in test_dataset:
        input_data = quantize_data(x, input_details[0]['quantization'])
        interpreter.set_tensor(input_details[0]['index'], input_data)
        interpreter.invoke()
        output = interpreter.get_tensor(output_details[0]['index'])
        pred = tf.cast(tf.math.argmax(output, axis=1), tf.uint8)
        correct += (pred==y).numpy().sum()
    print(f'Accuracy of tflite int8 model: ' \
          f'{correct}/{len(y_test)} {100*correct/len(y_test):.2f}%')


def main():
    # Convert settings
    parser = argparse.ArgumentParser(description='MNIST Tflite Convert Example')
    parser.add_argument('--input-path', type=str,
                        help='input model, support onnx or tf SavedModel')
    parser.add_argument('--output-path', type=str,
                        help='output directory for saving tflite.')
    parser.add_argument('--test-sample-num', type=int, default=100, metavar='N',
                        help='test sample num for use on SoC (default: 100)')
    args = parser.parse_args()

    model_format = args.input_path.split('.')[-1]
    if model_format == 'onnx':
        onnx_to_tf(args.input_path, args.output_path)
        saved_model_dir = os.path.join(args.output_path + '/saved_model')
        channel_order = 'torch'
    else:
        saved_model_dir = args.input_path
        channel_order = 'keras'

    converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    # keras and torch input have different channel order
    if channel_order == 'keras':
        converter.representative_dataset = repr_dataset
    elif channel_order == 'torch':
        converter.representative_dataset = repr_dataset_torch
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8
    tflite_int8_model = converter.convert()

    tflite_path = os.path.join(args.output_path, 'model_int8.tflite')
    with open(tflite_path, 'wb') as f:
        f.write(tflite_int8_model)

    interpreter = interpreter_wrapper.Interpreter(model_path=tflite_path)
    x_train, y_train, x_test, y_test = load_dataset(channel_order)
    # test on whole testset to check tflite int8 model performance
    print('Full Test:')
    tflite_test(interpreter, x_test, y_test)

    x_test = x_test[:args.test_sample_num]
    y_test = y_test[:args.test_sample_num]
    input_details = interpreter.get_input_details()
    x_test_quant = quantize_data(x_test, input_details[0]['quantization'])
    os.makedirs('testdata', exist_ok=True)
    np.save('testdata/input_int8', x_test_quant)
    np.save('testdata/label_int8', y_test)
    # test on part of testset which will run on SoC
    print('Part Test:')
    tflite_test(interpreter, x_test, y_test)


if __name__ == '__main__':
    main()
