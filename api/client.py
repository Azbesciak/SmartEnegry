import paho.mqtt.client as mqtt
import time
import threading

started = False
lock = threading.Lock()

class myThread (threading.Thread):
   def __init__(self, threadID, name, counter):
      threading.Thread.__init__(self)
      self.threadID = threadID
      self.name = name
      self.counter = counter
   def run(self):
      publish(self.name)


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc, a):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    #client.subscribe("$SYS/#")
    client.subscribe("/switches/switch1")
    client.subscribe("/switches/state/switch1")
    client.subscribe("/power/response")
    client.subscribe("outTopic")


def publish(name):
    for i in range(0, 10000):
        #client.publish("/sth/example", "payload_{na}_{name}".format(na = i, name = name))
        time.sleep(1000)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))


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
        
        