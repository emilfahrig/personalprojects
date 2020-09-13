import numpy as np
import pandas as pd
from sklearn.preprocessing import OneHotEncoder, StandardScaler
import matplotlib.pyplot as plt
import random

# download data from .csv file
iris_data = pd.read_csv('C:/Users/emilx/Downloads/IRISS.csv')

# initialize the one hot encoder and standard scalar
one_hot_encoder = OneHotEncoder()
standard_scale = StandardScaler()

# break up the data into input (X) and expected output (Y)
Y = iris_data['species'].values.reshape(-1, 1)
X = iris_data.loc[:, 'sepal_length':'petal_width'].values

# apply the standard scalar to the input data and the one hot encoder to the output data
inputs = standard_scale.fit_transform(X)
expected_outputs = one_hot_encoder.fit_transform(Y).toarray()


def shuffle_data(input_data, expected_output_data):
    data_length = len(input_data)
    random_index_list = list(range(0, data_length))
    random.shuffle(random_index_list)

    inputs_data_shuffled = []
    expected_output_data_shuffled = []
    for random_index in random_index_list:
        inputs_data_shuffled.append(input_data[random_index])
        expected_output_data_shuffled.append(expected_output_data[random_index])
    return np.array(inputs_data_shuffled), np.array(expected_output_data_shuffled)


def sigmoid_func(input_weight):
    return 1 / (1 + np.exp(-input_weight))


def empty_np_array(np_array):
    array_list = list(np_array)
    array_list.clear()
    return array_list


def one_hot_encode(outputs):
    unique_value = []
    for output in outputs:
        if output not in unique_value:
            unique_value.append(output)
    num_unique_values = len(unique_value)
    one_hot_encoded = []
    for output in outputs:
        ohe_value = [0 for x in range(num_unique_values)]
        ohe_value[unique_value.index(output)] = 1
        one_hot_encoded.append(ohe_value)
    return np.array(one_hot_encoded).reshape(-1, num_unique_values)


def get_cost_wrt_output_activation(actual_activation, expected_activation):
    return -(actual_activation - expected_activation)


def get_output_activation_wrt_net_output_activation(output_activation):
    return output_activation * (1 - output_activation)


def calculate_cost_function(network_output, expected_output):
    return np.sum((expected_output - network_output)**2)


def get_batches(inputs, expected_outputs_ohe, num_batches, samples_per_batch):
    inputs_batch = inputs.reshape(num_batches, samples_per_batch, inputs.shape[1])
    expected_outputs_ohe_batch = expected_outputs_ohe.reshape(num_batches, samples_per_batch, expected_outputs_ohe.shape[1])
    return inputs_batch, expected_outputs_ohe_batch


def zip_actual_expected_outputs(actual_activations, expected_activations, num_output_neurons):
    batch_size = len(expected_activations)
    actual_activations = actual_activations.reshape(-1, 1)
    expected_activations = expected_activations.reshape(-1, 1)
    actual_expected_zipped = np.array(list(zip(actual_activations, expected_activations)))
    actual_expected_zipped_batch = actual_expected_zipped.reshape((batch_size, num_output_neurons, 2))
    return actual_expected_zipped_batch


def get_accuracy_score(predicted_probability, actual_probability, samples_per_batch):
    if (predicted_probability.shape[0] is not actual_probability.shape[0]) or (predicted_probability.shape[1] is not actual_probability.shape[1]):
        raise ValueError("Shape of given probability distributions are not the same")
    else:
        actual_argmax_indices = actual_probability.argmax(axis=1)
        predicted_argmax_indices = predicted_probability.argmax(axis=1)
        indices_compared = list(actual_argmax_indices == predicted_argmax_indices)
        correct_counter = indices_compared.count(True)
        return correct_counter / samples_per_batch


def train_test_split(input_data, expected_output_data, percent_train, shuffle_train_data):
    samples = len(input_data)
    train_samples = int(samples * percent_train)
    x_train = input_data[:train_samples]
    y_train = expected_output_data[:train_samples]
    x_test = input_data[train_samples:]
    y_test = expected_output_data[train_samples:]

    if shuffle_train_data:
        x_train, y_train = shuffle_data(x_train, y_train)

    return x_train, y_train, x_test, y_test


class layer_dense():
    def __init__(self, num_inputs, num_neurons):
        self.num_inputs = num_inputs
        self.num_neurons = num_neurons
        self.weights = 0.1 * np.random.randn(num_inputs, num_neurons)
        self.biases = np.ones(num_neurons)

    def feedforward(self, input_activations, activation_func):
        outputs = np.dot(input_activations, self.weights) + self.biases
        if activation_func is 'relu':
            for activation_index in range(len(outputs)):
                if outputs[activation_index] <= 0:
                    outputs[activation_index] = 0
                else:
                    pass
        elif activation_func is 'sigmoid':
            for activation_index in range(len(outputs)):
                outputs[activation_index] = sigmoid_func(outputs[activation_index])
        elif activation_func is 'softmax':
            for activation_index in range(len(outputs)):
                activation_vector = outputs[activation_index]
                exponent_sum = np.sum(np.exp(activation_vector))
                outputs[activation_index] = np.exp(activation_vector) / exponent_sum
        else:
            raise ValueError("Given activation function is not supported. Use sigmoid, softmax or relu.")

        return outputs


class model():
    def __init__(self):
        self.layers = list()

    def add_layer(self, layer):
        self.layers.append(layer)

    def forward_propogate(self, input_activations):
        layer_output = self.layers[0].feedforward(input_activations, activation_func='softmax')
        if len(self.layers) > 2:
            for layer_index in range(len(self.layers) - 2):
                layer_output = self.layers[layer_index + 1].feedforward(layer_output, activation_func='relu')
            model_output_activations = self.layers[len(self.layers) - 1].feedforward(layer_output, activation_func='softmax')
            return model_output_activations
        return np.array(layer_output)

    def get_weights(self):
        for layer in self.layers:
            return layer.weights

    def set_weights(self, weights_update):
        for layer in self.layers:
            layer.weights = weights_update

    def get_num_output_neurons(self):
        num_layers = len(self.layers)
        return self.layers[num_layers - 1].num_neurons

    def back_propogation(self, actual_activations, expected_activations, previous_activations, learning_rate):
        num_layers = len(self.layers)
        num_output_neurons = self.layers[num_layers - 1].num_neurons
        actual_expected_batched = zip_actual_expected_outputs(actual_activations, expected_activations, num_output_neurons)
        weight_adjust_matrix = []
        num_samples_per_batch = len(actual_expected_batched)
        for layer in self.layers:
            weight_adjust_matrix_avg = np.array([[float(0) for i in range(layer.num_neurons)] for x in range(layer.num_inputs)])
            for index in range(num_samples_per_batch):
                previous_activation_vector = np.array(previous_activations[index])
                for activation_pair in actual_expected_batched[index]:
                    actual = activation_pair[0]
                    expected = activation_pair[1]
                    cost_wrt_output_activation = get_cost_wrt_output_activation(actual, expected)
                    output_activation_wrt_net_output_activation = get_output_activation_wrt_net_output_activation(actual)
                    cost_wrt_net_output_activation = np.array([cost_wrt_output_activation * output_activation_wrt_net_output_activation for x in range(len(previous_activation_vector))])
                    weight_error_gradient = previous_activation_vector * cost_wrt_net_output_activation
                    weight_adjust_matrix.append(list(weight_error_gradient))
                weight_adjust_matrix = np.array(weight_adjust_matrix).T
                weight_adjust_matrix_avg += weight_adjust_matrix
                weight_adjust_matrix = empty_np_array(weight_adjust_matrix)
            weight_adjust_matrix_avg /= num_samples_per_batch
            layer.weights = layer.weights + learning_rate * weight_adjust_matrix_avg

    def make_predictions(self, input_activations, expected_activations):
        correct_counter = 0
        for activation_index in range(len(input_activations)):
            activation = input_activations[activation_index]
            expected_activation = expected_activations[activation_index]
            output_activation = self.layers[0].feedforward(activation.reshape((-1, activation.shape[0])), activation_func='softmax')
            if np.argmax(output_activation) == np.argmax(expected_activation):
                correct_counter = correct_counter + 1
        return float(correct_counter / len(input_activations))


cost_array = []

if len(inputs) != len(expected_outputs):
    raise ValueError("Fatal error: input array must be the same length as expected output array")

# separate the input and expected data into training and testing categories
X_train, Y_train, X_test, Y_test = train_test_split(inputs, expected_outputs, 0.8, shuffle_train_data=True)

num_batches = 60
num_samples = len(X_train)
if num_samples % num_batches != 0:
    raise ValueError("Cannot separate {} samples into {} batches".format(num_samples, num_batches))
num_samples_per_batch = int(num_samples / num_batches)
print(num_samples_per_batch)
learning_rate = 5

# one hot encode the outputs
# expected_outputs_ohe = one_hot_encode(expected_outputs)

# initialize the model
neural_network = model()
neural_network.add_layer(layer_dense(4, 3))

# separate the inputs and outputs into batches with the given batch size
inputs_batched, expected_outputs_ohe_batched = get_batches(X_train, Y_train, num_batches, num_samples_per_batch)

for epoch in range(num_batches):
    # forward propagate the input activations
    output_activations = neural_network.forward_propogate(inputs_batched[epoch])

    accuracy_score = get_accuracy_score(output_activations, expected_outputs_ohe_batched[epoch], num_samples_per_batch)

    # back-propagate the error and update all weights in neural network
    neural_network.back_propogation(output_activations, expected_outputs_ohe_batched[epoch], inputs_batched[epoch], learning_rate)

    # calculate the cost of the current model with the given input activations
    cost_value = calculate_cost_function(output_activations, expected_outputs_ohe_batched[epoch])
    print("Epoch " + str(epoch + 1) + "    cost value: " + str(cost_value) + "    accuracy: " + str(accuracy_score))
    cost_array.append(accuracy_score)

plt.plot(cost_array, color='black')
plt.show()

print("Evaluating test data...")
test_data_accuracy = neural_network.make_predictions(X_test, Y_test)
print("Test data accuracy: " + str(test_data_accuracy))

