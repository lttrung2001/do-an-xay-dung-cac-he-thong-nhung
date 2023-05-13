import time
import paho.mqtt.client as paho
from paho import mqtt
import paho.mqtt.publish as publish


broker ="4a05b8e1d6604c8c8c84238e32f702e8.s2.eu.hivemq.cloud"
port=8883
username= "dangbac872001"
password= "ejURUQaYZWn2@t"
topic= "topic2"
message="iloveu"
def on_connect(client, userdata, flags, rc):
 
    if rc == 0:
 
        print("Connected to broker")
 
        global Connected                #Use global variable
        Connected = True                #Signal connection 
 
    else:
 
        print("Connection failed")
 
Connected = False   #global variable for the state of the connection


client= paho.Client("111")                           #create client object
client.tls_set()
client.username_pw_set(username, password)
client.connect(broker,port)                                 #establish connection
client.on_connect= on_connect
client.loop_start()  
while Connected != True:    #Wait for connection
    time.sleep(0.1)
 
# client.publish(topic,message)

# client.loop_stop()
    
try:
    while True:
 
        value = input('Enter the message:')
        client.publish(topic,value)
 
except KeyboardInterrupt:
 
    client.disconnect()
    client.loop_stop()
