import paho.mqtt.client as mqtt
import threading
from .models import Consumption, Device
from time import gmtime, strftime

DEVICE_ID = '1'

POWER_TOPIC = "/power/response"
STATE_CHANGE_REQUEST = "/switches/request"
STATE_CHANGE_RESPONSE = "/switches/response"
CURRENT_STATE_REQUEST = "/switches/state/request"
CURRENT_STATE_RESPONSE = "/switches/state/response"

started = False
lock = threading.Lock()


class Client:
    client = None
    publisher = None

    def __init__(self, client):
        self.client = client

    def start(self):
        pass

    @classmethod
    def initialize(cls, client, publisher):
        cls.client = client
        cls.publisher = publisher

    @classmethod
    def get_client(cls):
        return cls.client

    @classmethod
    def get_publisher(cls):
        return cls.publisher

    @classmethod
    def change_device_state(cls, device_id, state):
        cls.client.publish(STATE_CHANGE_REQUEST, state)

    @classmethod
    def get_device_state(cls, device_id):
        cls.client.publish(CURRENT_STATE_REQUEST)


class myThread(threading.Thread):
    def __init__(self, threadID, name, counter):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.counter = counter

    def run(self):
        publish(self.name)


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc, a):
    print("Connected with result code " + str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    # client.subscribe("$SYS/#")
    client.subscribe("/switches/switch1")
    client.subscribe("/switches/state/switch1")
    client.subscribe(STATE_CHANGE_RESPONSE)
    client.subscribe(CURRENT_STATE_RESPONSE)
    client.subscribe(POWER_TOPIC)
    client.subscribe("outTopic")


def publish(name):
    Client.client.publish("/sth/example", name)


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    if msg.topic == POWER_TOPIC:
        # TODO value ?
        consumption = Consumption.objects.create(
            device_id=DEVICE_ID, value=msg.payload,
            measure_time=strftime("%Y-%m-%d %H:%M:%S", gmtime()))
        consumption.save()

    elif msg.topic == STATE_CHANGE_RESPONSE or msg.topic == CURRENT_STATE_RESPONSE:
        Device.update_state(DEVICE_ID, msg.payload)
    else:
        print("not Managed...")


def start_client_thread():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("127.0.0.1", 1884, 60)
    # Blocking call that processes network traffic, dispatches callbacks and
    # handles reconnecting.
    # Other loop*() functions are available that give a threaded interface and a
    # manual interface.
    publisher = myThread(1, "publisherThread_2", 1)
    publisher.start()
    Client.initialize(client, publisher)

    client.loop_forever()


def init_client():
    global started
    global lock
    lock.acquire()
    if not started:
        started = True
        thread = threading.Thread(target=start_client_thread)
        thread.start()
    lock.release()
