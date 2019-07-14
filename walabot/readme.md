#Walabot setup on raspberry PI

##Install Walabot SDK
```
sudo dpkg -i WalabotSDK_2018.09.02_v1.2.2_RPi.deb
```
from: https://api.walabot.com/_install.html

##Install Conda
```
wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
sudo md5sum Miniconda3-latest-Linux-armv7l.sh # (optional) check md5
sudo /bin/bash Miniconda3-latest-Linux-armv7l.sh # -> change default directory to /home/pi/miniconda3
sudo nano /home/pi/.bashrc # -> add: export PATH="/home/pi/miniconda3/bin:$PATH"
sudo reboot -h now
```
from: https://gist.github.com/simoncos/a7ce35babeaf73f512be24135c0fbafb

##Create Conda Environment
```
conda env create -f environment.yml
```
from: https://docs.conda.io/projects/conda/en/latest/user-guide/tasks/manage-environments.html#sharing-an-environment

##Install WalabotAPI
```
pip install WalabotAPI --no-index --find-links="/usr/share/walabot/python/"
```
from: https://github.com/Walabot-Projects/Walabot-HelloWalabot


#TODO
- Send only the data over OSC
    - Count, left, center, right, high, low, position
- Create script that runs the walabot forever and restarts occasionally