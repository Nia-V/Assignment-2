## First of all...
https://photos.app.goo.gl/JYtPncG3UU9RZ4YFA
^^^ Demo Video
***So sorry for submitting this so late!!***
## ESP32.ino
So basically the esp code uses the pubsub MQTT library to publish the analog read value to the topic **"terra/hum"** for terrarium humidity. 
## Control.py & Conditions
Then the control.py uses the paho MQTT library and a class with multiple functions to:
- open and load the config.json file
- connect to the mqtt broker
- subscribe to all the topics in the conditions dictionaries
- defines a callback method "on_message" that calls the function "run_rules"
- run rules calls the function "evaluate_condition" that uses the arguements of the conditions dictionaries from the config file and the MQTT data and returns whether the data satisfies the conditions.
- if evaluate condition returns true, then run rules publishes the value message to the topic in the results dictionary 
## Actor.py
- subscribes to the **"terra/led"** topic and decodes the messages received.
- If the message received is "on" the LED turns on
- If the message received is "off" the LED turns off
