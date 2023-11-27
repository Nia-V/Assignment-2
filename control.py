
import paho.mqtt.client as mqtt
import json


class IoT_Controller:
    configuration = []
    client = None
    mqtt_data = {}
#{ "topic":value, "topic2":value2 }

    def configure(filename):
        IoT_Controller.client = mqtt.Client()
        #load the configuration from a file
        with open(filename,'r') as file:
            IoT_Controller.configuration = json.load(file)
        # print(configuration)
        # connecting to the MQTT broker
        IoT_Controller.client.on_message = IoT_Controller.on_message
        IoT_Controller.client.connect("localhost",1883)
        # subscribe to the appropriate topics (the ones from the conditions)
        for rule in IoT_Controller.configuration:
            for condition in rule["conditions"]:
                IoT_Controller.client.subscribe(condition["topic"])
                print(condition["topic"])

    def run():
        # start the MQTT client loop
        print("run method before loop_forever")
        IoT_Controller.client.loop_forever()
        print("run method after loop_forever")

    def on_message(client, userdata, message):
        try: 
            value = int(message.payload.decode("utf-8"))
            print("int")
        except: 
            value = str(message.payload.decode("utf-8"))
            print("str")
        topic = message.topic
        IoT_Controller.mqtt_data[topic] = value
        # e.g., IoT_Controller.mqtt_data["kitchen/lighting"] = 25
        IoT_Controller.run_rules()

    def run_rules():
        print("runing")
        for rule in IoT_Controller.configuration:
            conditions_met = all(IoT_Controller.evaluate_condition(IoT_Controller.mqtt_data, condition) for condition in rule["conditions"])
            if conditions_met:
                # todo: call the events to happen
                for message in rule["results"]:
                    IoT_Controller.client.publish(message["topic"],message["value"])
                    print("runing2")

    def evaluate_condition(data, condition):
        #{"topic":"kitchen/lighting","comparison":"<","value":30},
        topic = condition["topic"]
        value = data.get(topic,None) # not getting a None when something is missing
        if value == None:
            return False
        comparison = condition["comparison"]
        if comparison == "<":
            return value < condition["value"]
        elif comparison == "<=":
            return value <= condition["value"]
        elif comparison == "==":
            return value == condition["value"]
        elif comparison == "!=":
            return value != condition["value"]
        elif comparison == ">":
            return value > condition["value"]
        elif comparison == ">=":
            return value >= condition["value"]
        else:
            return False

def main():
    IoT_Controller.configure("config.json")
    IoT_Controller.run()

if __name__ == "__main__":
    main()
