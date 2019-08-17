#!/bin/bash

set -xe

ensure_workdir() {
    tmp_dir="/opt/install-`date +%s`/"
    sudo mkdir -p $tmp_dir
    sudo chown -R `whoami` $tmp_dir
    cd $tmp_dir
}


clone_repo () {
    git clone https://github.com/stephen-standridge/root_cathedral root_cathedral
    cd root_cathedral
}

install_walabot() {
    # Assumed to be in root_cathedral
    sudo dpkg -i walabot/WalabotSDK_2018.09.02_v1.2.2_RPi.deb
    pip install WalabotAPI --no-index --find-links="/usr/share/walabot/python/"
    sudo cp walabot/walabot.service /etc/systemd/system
    sudo systemctl enable walabot.service
    sudo systemctl start walabot.service
    sudo systemctl status walabot.service
    # Follow logs with `sudo journalctl -f -u walabot.service`
}

install_osc_deps() {
    pip install python-osc python-dotenv --user
}


install_python() {
    # Discourage interactive prompts while using apt
    DEBIAN_FRONTEND=noninteractive

    sudo apt-get update
    sudo apt-get install -y python3-dev libffi-dev libssl-dev
    
    
    wget https://www.python.org/ftp/python/3.6.3/Python-3.6.3.tar.xz
    tar xJf Python-3.6.3.tar.xz
    cd Python-3.6.3
    ./configure
    make
    sudo make install
    sudo pip3 install --upgrade pip
}

ensure_workdir
clone_repo
install_walabot
install_osc_deps
install_python

