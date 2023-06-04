import paho.mqtt.client as paho
import cv2
import numpy as np

broker ="4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
port=8883
username= "n19dccn018"
password= "ejURUQaYZWn2@t"
topic= "image"



from camera_calibration import calib
from process import pipeline

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#   Tune Parameters for different inputs        #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
th_sobelx, th_sobely, th_mag, th_dir = (35, 100), (30, 255), (30, 255), (0.7, 1.3)
th_h, th_l, th_s = (10, 100), (0, 60), (85, 255)

# camera matrix & distortion coefficient
mtx, dist = calib()

# Define callback functions for events
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topic)

def on_message(client, userdata, message):
    # print("Received message: " + message.payload.decode())
    image = np.asarray(bytearray(message.payload), dtype="uint8")
    image = cv2.imdecode(image, cv2.IMREAD_COLOR)
    # Image processing
    result_image, direction = pipeline(image)
    print(direction)
    client.publish("output", str(direction))

# Create an MQTT client
client = paho.Client(client_id="456")

# client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
client.tls_set()
# Set the username and password for the client
client.username_pw_set(username, password)
client.connect(broker, port, 60)

# Assign the callback functions
client.on_connect = on_connect
client.on_message = on_message


# Connect to the MQTT broker and start the network loop
client.loop_forever()