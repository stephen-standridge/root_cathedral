#Walabot setup on raspberry PI

##Install Pyton
```
sudo apt-get install python3-dev libffi-dev libssl-dev -y
wget https://www.python.org/ftp/python/3.6.3/Python-3.6.3.tar.xz
tar xJf Python-3.6.3.tar.xz
cd Python-3.6.3
./configure
make
sudo make install
sudo pip3 install --upgrade pip
```
from: http://www.knight-of-pi.org/installing-python3-6-on-a-raspberry-pi/

```
sudo apt-get install python3.6-tk
```
from: https://askubuntu.com/questions/815874/importerror-no-named-tkinter-please-install-the-python3-tk-package

##Install Walabot SDK
```
sudo dpkg -i WalabotSDK_2018.09.02_v1.2.2_RPi.deb
```
from: https://api.walabot.com/_install.html


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
