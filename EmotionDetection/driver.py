import cv2
import numpy as np
from keras.preprocessing.image import img_to_array
from keras.models import load_model
from keras.preprocessing.image import array_to_img


def load_image(image_input):
    image_resized = image_input.resize(size=(50, 50))
    img = img_to_array(image_resized)
    img = img.reshape(1, 50, 50, 3)
    return img


def get_emotion(index):
    if index == 0:
        return "angry"
    elif index == 1:
        return "disgust"
    elif index == 2:
        return "fear"
    elif index == 3:
        return "happy"
    elif index == 4:
        return "neutral"
    elif index == 5:
        return "sad"
    else:
        return "suprise"

model = load_model('emotion.h5')
video = cv2.VideoCapture(0)

while True:
    found, frame = video.read()

    cv2.imshow("WebCam feed", frame)

    key = cv2.waitKey(1)
    if key == ord('q'):
        break

    # when 'c' key is pressed a snapshot is created and used as input for the neural network
    elif key == ord('c'):
        image = array_to_img(frame)
        image_to_predict = load_image(image)
        result = model.predict(image_to_predict)
        max_index = np.argmax(result[0])
        predicted_emotion = get_emotion(max_index)
        print(predicted_emotion)


video.release()
cv2.destroyAllWindows()
