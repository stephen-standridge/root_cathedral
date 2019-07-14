"""
OSC client that transmits walabot data over a network
"""
import os
from dotenv import Dotenv
dotenv = Dotenv(os.path.join(os.path.dirname(__file__), ".env"))
os.environ.update(dotenv)
import argparse
import random
import time

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

from SensorTargets import SensorTargetsApp

def sensorTargets():
    """ Main app function. Init the main app class, configure the window
        and start the mainloop.
    """
    root = tk.Tk()
    root.title("Walabot - Sensor Targets")
    iconFile = tk.PhotoImage(file="walabot-icon.gif")
    root.tk.call("wm", "iconphoto", root._w, iconFile)  # set app icon
    root.option_add("*Font", "TkFixedFont")
    SensorTargetsApp(root).pack(fill=tk.BOTH, expand=tk.YES)
    root.geometry("+{}+{}".format(os.getenv("APP_X"), os.getenv("APP_Y")))  # set window location
    root.update()
    root.minsize(width=root.winfo_reqwidth(), height=root.winfo_reqheight())
    root.mainloop()

if __name__ == "__main__":
    print('connecting')
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default=os.getenv("IP_OUT"),
        help="The ip of the OSC server")
    parser.add_argument("--port", type=int, default=os.getenv("PORT_OUT"),
        help="The port the OSC server is listening on")

    args = parser.parse_args()

    client = udp_client.SimpleUDPClient(args.ip, args.port)
    sensorTargets()

    for x in range(10):
        r= random.random()
        print('sending: ')
        print(r)
        client.send_message("{}/{}".format(os.getenv("CHANNEL_NAME"), os.getenv("DEVICE_NAME")), r)
        time.sleep(1)
