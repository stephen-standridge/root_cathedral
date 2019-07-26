"""
OSC client that transmits walabot data over a network
"""
from dotenv import load_dotenv
import os
import random
import time

# from walabot_osc_publisher import WalabotOSC
# from targets_handler import TargetsHandler
# from breathing_handler import BreathingHandler
# from walabot_osc_publisher import WalabotOSC
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

# from SensorTargets import SensorTargetsApp

# def sensorTargets():
#     """ Main app function. Init the main app class, configure the window
#         and start the mainloop.
#     """
#     root = tk.Tk()
#     root.title("Walabot - Sensor Targets")
#     iconFile = tk.PhotoImage(file="walabot-icon.gif")
#     root.tk.call("wm", "iconphoto", root._w, iconFile)  # set app icon
#     root.option_add("*Font", "TkFixedFont")
#     SensorTargetsApp(root).pack(fill=tk.BOTH, expand=tk.YES)
#     root.geometry("+{}+{}".format(os.getenv("APP_X"), os.getenv("APP_Y")))  # set window location
#     root.update()
#     root.minsize(width=root.winfo_reqwidth(), height=root.winfo_reqheight())
#     root.mainloop()

if __name__ == "__main__":
    load_dotenv()
    print('connecting')

    # walabotOSC = WalabotOSC(targetsHandler)
    print("Sending to {}:{}".format(os.getenv("IP_OUT"), int(os.getenv("PORT_OUT")) ))
    print(int(os.getenv("PORT_OUT")))
    client = udp_client.SimpleUDPClient(os.getenv("IP_OUT"), int(os.getenv("PORT_OUT")))
    for x in range(10):
        r= random.random()
        print('sending: ')
        print(r)
        client.send_message("{}/{}".format(os.getenv("CHANNEL_NAME"), os.getenv("DEVICE_NAME")), r)
        time.sleep(1) 