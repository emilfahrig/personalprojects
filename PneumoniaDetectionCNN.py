from keras.models import Model
from keras.layers import Conv2D
from keras.layers import Flatten
from keras.layers import Dense
from keras.layers import Dropout
from keras.layers import MaxPool2D
from keras.layers import Concatenate
from keras.layers import Input
from keras.optimizers import SGD
from keras.preprocessing.image import ImageDataGenerator
import matplotlib.pyplot as plt
from datetime import datetime
start = datetime.now()


def define_model():
    network_input = Input((100, 95, 3))

    detail_convolutional_layer1 = Conv2D(32, (3, 3), kernel_initializer='he_uniform', activation='relu', padding='same', input_shape=(100, 95, 3))(network_input)
    detail_max_pool1 = MaxPool2D(2, 2)(detail_convolutional_layer1)
    detail_dropout1 = Dropout(0.2)(detail_max_pool1)
    detail_convolutional_layer2 = Conv2D(26, (2, 2), kernel_initializer='he_uniform', activation='relu', padding='same')(detail_dropout1)
    detail_max_pool2 = MaxPool2D(2, 2)(detail_convolutional_layer2)
    detail_dropout2 = Dropout(0.2)(detail_max_pool2)
    detail_output = Flatten()(detail_dropout2)

    features_convolutional_layer1 = Conv2D(16, (7, 7), kernel_initializer='he_uniform', activation='relu', padding='same', input_shape=(100, 95, 3))(network_input)
    features_max_pool1 = MaxPool2D(4, 4)(features_convolutional_layer1)
    features_dropout1 = Dropout(0.2)(features_max_pool1)
    features_convolutional_layer2 = Conv2D(12, (5, 5), kernel_initializer='he_uniform', activation='relu', padding='same')(features_dropout1)
    features_max_pool2 = MaxPool2D(3, 3)(features_convolutional_layer2)
    features_dropout2 = Dropout(0.2)(features_max_pool2)
    features_output = Flatten()(features_dropout2)

    merged = Concatenate(axis=-1)([detail_output, features_output])
    merged_dense_1 = Dense(128, kernel_initializer='he_uniform', activation='relu')(merged)
    merged_dropout_1 = Dropout(0.2)(merged_dense_1)
    merged_dense_2 = Dense(128, kernel_initializer='he_uniform', activation='relu')(merged_dropout_1)
    merged_dropout_2 = Dropout(0.5)(merged_dense_2)
    output_layer = Dense(1, activation='sigmoid')(merged_dropout_2)

    model = Model(inputs=[network_input], outputs=[output_layer])

    optimization_function = SGD(lr=0.001, momentum=0.9)
    model.compile(optimizer=optimization_function, loss='binary_crossentropy', metrics=['accuracy'])
    return model


def run_model():
    model = define_model()
    image_data = ImageDataGenerator(rescale=(1.0/255.0), width_shift_range=0.05, height_shift_range=0.05,
                                    zoom_range=0.05)
    train_data = image_data.flow_from_directory('C:/Users/emilx/Downloads/chest_xray/train', class_mode='binary',
                                                batch_size=2, target_size=(100, 95))
    test_data = image_data.flow_from_directory('C:/Users/emilx/Downloads/chest_xray/test', class_mode='binary',
                                                batch_size=2, target_size=(100, 95))
    history = model.fit_generator(train_data, steps_per_epoch=len(train_data), validation_data=test_data,
                        validation_steps=len(test_data), epochs=20)
    print("Training time: " + str(datetime.now() - start))
    plt.plot(history.history['accuracy'])
    plt.show()


run_model()
