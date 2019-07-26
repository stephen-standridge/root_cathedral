"""
OSC client that transmits walabot data over a network
"""
from dotenv import load_dotenv
import os
import random
import time

# from walabot_osc_publisher import WalabotOSC
from targets_handler import TargetsHandler
# from breathing_handler import BreathingHandler
from walabot_osc_publisher import WalabotOSC

from pythonosc import osc_message_builder
from pythonosc import udp_client
try:  # for Python 2
    import Tkinter as tk
except ImportError:  # for Python 3
    import tkinter as tk
try:  # for Python 2
    range = xrange
except NameError:
    pass


if __name__ == "__main__":
    # load_dotenv()
    # print('connecting')

    walabotOSC = WalabotOSC(TargetsHandler)
    # print("Sending to {}:{}".format(os.getenv("IP_OUT"), int(os.getenv("PORT_OUT")) ))
    # client = udp_client.SimpleUDPClient(os.getenv("IP_OUT"), int(os.getenv("PORT_OUT")))
    # for x in range(10):
    #     r= random.random()
    #     channel = "/{}/{}".format(os.getenv("CHANNEL_NAME"), os.getenv("DEVICE_NAME"))
    #     print('sending on channel {}'.format(channel))
    #     print(r)
    #     client.send_message(channel, r)
    #     time.sleep(1) 