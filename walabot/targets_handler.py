from dotenv import load_dotenv
import os
import traceback
from walabot_handler import WalabotHandler

load_dotenv()

arena_params = {
                'minInCm': 30,          'maxInCm': 200,        'resInCm': 3,
                'minIndegrees': -15,    'maxIndegrees': 15,    'resIndegrees': 5,
                'minPhiInDegrees': -60, 'maxPhiInDegrees': 60, 'resPhiInDegrees': 5,
                'mtiMode': True
                }
import socket
import fcntl
import struct

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', bytes(ifname[:15], 'utf-8'))
    )[20:24])



class TargetsHandler(WalabotHandler):

    out_ip = os.getenv("IP_OUT")
    out_port = os.getenv("PORT_OUT")
    in_ip = get_ip_address('eth0')
    in_port = os.getenv("PORT_IN")
    device_name = os.getenv("DEVICE_NAME")

    def __init__(self):
        super(TargetsHandler, self).__init__()
        self.__is_connected = False

    def start(self):
        if self.__is_connected:
            return True
        try:
            # Initializes walabot lib
            self._wlbt.Initialize()
            # 1) Connect : Establish communication with walabot.
            self._wlbt.ConnectAny()
            self._wlbt.SetProfile(self._wlbt.PROF_TRACKER)
            self._wlbt.SetThreshold(30)
            filter_type = self._wlbt.FILTER_TYPE_MTI if arena_params['mtiMode'] else self._wlbt.FILTER_TYPE_NONE
            self._wlbt.SetDynamicImageFilter(filter_type)
            # Setup arena - specify it by Cartesian coordinates.
            self._wlbt.SetArenaR(arena_params['minInCm'], arena_params['maxInCm'], arena_params['resInCm'])
            # Sets polar range and resolution of arena (parameters in degrees).
            self._wlbt.SetArenaTheta(arena_params['minIndegrees'], arena_params['maxIndegrees'], arena_params['resIndegrees'])
            # Sets azimuth range and resolution of arena.(parameters in degrees).
            self._wlbt.SetArenaPhi(arena_params['minPhiInDegrees'], arena_params['maxPhiInDegrees'], arena_params['resPhiInDegrees'])
            # 3) Start: Start the system in preparation for scanning.
            self._wlbt.Start()   
            # calibrates scanning to ignore or reduce the signals
            self._wlbt.StartCalibration()
            while self._wlbt.GetStatus()[0] == self._wlbt.STATUS_CALIBRATING:
                self._wlbt.Trigger()
            self.__is_connected = True
            return True
        except self._wlbt.WalabotError:
            traceback.print_exc()
        return False

    def get_data(self, data_dict):
        try:
            self._wlbt.Trigger()
            raw_targets = self._wlbt.GetTrackerTargets()
            data = [[target.xPosCm, target.yPosCm, target.zPosCm] for target in raw_targets]
            for key, value in enumerate(data):
                data_dict['target_{}'.format(key)] = value
        except self._wlbt.WalabotError:
            traceback.print_exc()
            return False
        return True

    def stop(self):
        if self.__is_connected:
            self._wlbt.Stop()
            self._wlbt.Disconnect()
        print('Terminate successfully')
        self._wlbt.Clean()
        self.__is_connected = False
