from keras.models import Model
from keras.layers import Conv2D, Dense, Flatten, Input, Dropout, MaxPool2D, Concatenate, BatchNormalization
from keras.optimizers import Adam
from keras.preprocessing.image import ImageDataGenerator
import matplotlib.pyplot as plt


def define_model():
    # define the input for each channel of the CNN (two channels are detail and feature)
    image_input_array = Input((50, 50, 3))

    # define the first channel that is intended to pick up on small details hence the small kernel size in the
    # max ppoling layer and Conv2D layer and the small stride length
    detail_conv_1 = (Conv2D(64, (3, 3), kernel_initializer='he_uniform', activation='relu', input_shape=(50, 50)))(image_input_array)
    detail_dropout_1 = Dropout(0.3)(detail_conv_1)
    detail_maxpool_1 = MaxPool2D((3, 3), strides=2)(detail_dropout_1)
    detail_conv_2 = Conv2D(32, (2, 2), padding='same', kernel_initializer='he_uniform', activation='relu')(detail_maxpool_1)
    detail_dropout_2 = Dropout(0.3)(detail_conv_2)
    detail_maxpool_2 = MaxPool2D((2, 2), strides=2)(detail_dropout_2)
    detail_batchnormalization_1 = BatchNormalization()(detail_maxpool_2)
    detail_conv_3 = Conv2D(32, (2, 2), padding='same', kernel_initializer='he_uniform', activation='relu')(detail_batchnormalization_1)
    detail_dropout_3 = Dropout(0.3)(detail_conv_3)
    detail_maxpool_3 = MaxPool2D((2, 2), strides=2)(detail_dropout_3)
    detail_conv_4 = Conv2D(32, (2, 2), padding='same', kernel_initializer='he_uniform', activation='relu')(detail_maxpool_3)
    detail_dropout_4 = Dropout(0.3)(detail_conv_4)
    detail_maxpool_4 = MaxPool2D((2, 2), strides=2)(detail_dropout_4)
    detail_batchnormalization_2 = BatchNormalization()(detail_maxpool_4)
    detail_output = Flatten()(detail_batchnormalization_2)

    # define the channels meant to pick up on features in the input image with the help of a larger kernel size and
    # a greater stride length (stride length is 3 by default)
    feature_conv_1 = (Conv2D(32, (7, 7), kernel_initializer='he_uniform', activation='relu', input_shape=(50, 50)))(image_input_array)
    feature_dropout_1 = Dropout(0.3)(feature_conv_1)
    feature_maxpool_1 = MaxPool2D(5, 5)(feature_dropout_1)
    feature_conv_2 = Conv2D(26, (5, 5), padding='same', kernel_initializer='he_uniform', activation='relu')(feature_maxpool_1)
    feature_dropout_2 = Dropout(0.3)(feature_conv_2)
    feature_maxpool_2 = MaxPool2D(4, 4)(feature_dropout_2)
    feature_batchnormalization_1 = BatchNormalization()(feature_maxpool_2)
    feature_conv_3 = Conv2D(24, (5, 5), padding='same', kernel_initializer='he_uniform', activation='relu')(feature_batchnormalization_1)
    feature_dropout_3 = Dropout(0.3)(feature_conv_3)
    # feature_maxpool_3 = MaxPool2D(3, 3)(feature_dropout_3)
    feature_conv_4 = Conv2D(20, (4, 4), padding='same', kernel_initializer='he_uniform', activation='relu')(feature_dropout_3)
    feature_dropout_4 = Dropout(0.3)(feature_conv_4)
    # feature_maxpool_4 = MaxPool2D(3, 3)(feature_dropout_4)
    feature_batchnormalization_2 = BatchNormalization()(feature_dropout_4)
    feature_output = Flatten()(feature_batchnormalization_2)

    # concatenate the outputs of both channels and further process then with two Dense layers
    concat_layer = Concatenate(axis=-1)([detail_output, feature_output])
    dense_layer_1 = Dense(128, kernel_initializer='he_uniform', activation='relu')(concat_layer)
    dropout_1 = Dropout(0.3)(dense_layer_1)
    dense_layer_2 = Dense(64, kernel_initializer='he_uniform', activation='relu')(dropout_1)
    dropout_2 = Dropout(0.45)(dense_layer_2)
    dense_layer_3 = Dense(64, kernel_initializer='he_uniform', activation='relu')(dropout_1)
    dropout_3 = Dropout(0.5)(dense_layer_3)
    final_dense = Dense(7, activation='softmax')(dropout_3)

    # define the model and specify the inputs (image rgb values and the output layers)
    model = Model(inputs=[image_input_array], outputs=[final_dense])

    # initialize the stochastic gradient descent algorithm we will be using with a learning rate of 0.01 and momentum
    # of 0.9
    optimization = Adam(lr=0.001, beta_1=0.9, beta_2=0.999, epsilon=1e-7)

    # compile the model with a categorical_crossentropy loss function as there are more than 2 potential output
    model.compile(optimizer=optimization, loss='categorical_crossentropy', metrics=['accuracy'])

    return model


def load_model():
    model = define_model()

    # define an interpreter for the image data that will rescale the rgb values by multiplying by 1/255 (to keep
    # input activations between 0 and 1) and shifting the image slightly to prevent any findings in the feature
    # map from being location dependant
    image_data = ImageDataGenerator(rescale=(1.0/255.0), height_shift_range=0.05, width_shift_range=0.05)

    # load the train and test data
    train_data = image_data.flow_from_directory('C:/Users/emilx/Downloads/images/train', class_mode='categorical',
                                                batch_size=32, target_size=(50, 50))
    test_data = image_data.flow_from_directory('C:/Users/emilx/Downloads/images/validation', class_mode='categorical',
                                                batch_size=32, target_size=(50, 50))
    history = model.fit_generator(train_data, steps_per_epoch=len(train_data), epochs=100, shuffle=True, verbose=1)
    model.save('emotion.h5')
    plt.plot(history.history['accuracy'])
    plt.show()


load_model()
