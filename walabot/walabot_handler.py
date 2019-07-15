from __future__ import print_function  # WalabotAPI works on both Python 2 an 3.
from sys import platform
from imp import load_source
from os.path import join
import abc


class WalabotHandler:
    """
        Abstract class for Walabot app handler
        Static Params:
            device_name: defines a unique topic with the walabot application.
            out_ip, out_port: defines the outgoing ip address and port.
            in_ip, in_port: defines the incoming ip address and port.
    """

    @property
    @abc.abstractstaticmethod
    def device_name(self):
        pass

    @property
    @abc.abstractstaticmethod
    def out_port(self):
        ...

    @property
    @abc.abstractstaticmethod
    def in_port(self):
        ...        

    @property
    @abc.abstractstaticmethod
    def in_ip(self):
        ...

    @property
    @abc.abstractstaticmethod
    def out_ip(self):
        ...   

    def __init__(self):
        if self.device_name == None or self.out_ip == None or self.out_port == None:
            assert(False)
        module_path = ''
        if platform == 'win32':
            module_path = join('C:/', 'Program Files', 'Walabot', 'WalabotSDK', 'python', 'WalabotAPI.py')
        elif platform.startswith('linux'):
            module_path = join('/usr', 'share', 'walabot', 'python', 'WalabotAPI.py')
        self._wlbt = load_source('WalabotAPI', module_path)
        self._wlbt.Init()

    @abc.abstractclassmethod
    def start(self):
        """
            Sets walabot configurations ant start recording.
            Returns: True/ False on success/ failure
        """
        ...

    @abc.abstractclassmethod
    def get_data(self, data_dict):
        """
            Fills data_dict
            Returns: True/ False on success/ failure
        """
        ...

    @abc.abstractclassmethod
    def stop(self):
        """
            Stop recording
        """
        ...