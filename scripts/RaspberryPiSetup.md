# Pi setup
make a ssh.txt file in the boot directory after flashing
```
passwd
raspberry
#put your password here

```

# Pi cleanup
```
sudo apt-get update
sudo apt-get upgrade
```

# General stuff
```
sudo apt-get install git cmake
```

# Audio stuff
```
sudo apt-get install espeak
```

# Wiring pi stuff for gpio
I want to replace with pizero in the future
```
sudo apt-get install wiringpi
```

# Visual Studio remote development
```
sudo apt-get install openssh-server g++ gdb make ninja-build rsync zip
```

# PocketSphinx requirements
```
sudo apt-get install gcc automake autoconf libtool bison swig python-dev libasound2-dev
```

# Spdlog install
```
sudo apt-get install libspdlog-dev
```

# Fadecandy install
```
cd ~/git
git clone https://github.com/scanlime/fadecandy
cd fadecandy/server
make submodules
make
```

# Enable Fadecandy on boot
```
sudo mv fcserver /usr/local/bin
sudo nano /etc/rc.local
# add before exit0 /usr/local/bin/fcserver /usr/local/bin/fcserver.json >/var/log/fcserver.log 2>&1 &
```

# Enable audio daq hat thing
```
sudo echo 'dtoverlay=hifiberry-dac' >> /boot/config.txt
# comment #dtparam=audio=on maybe?
```



# Sphinxbase install
stolen from https://www.youtube.com/watch?v=Ft_agFsxyoc&t=4s&ab_channel=RoboPi
```
cd ~/git
git clone https://github.com/cmusphinx/sphinxbase
cd /sphinxbase
./autogen.sh
./configure
make clean all
make check
sudo make install
```

# Pocketsphinx install
```
git clone https://github.com/cmusphinx/pocketsphinx
cd ~/git/pocketsphinx
./autogen.sh 
./configure 
make clean all 
make check 
sudo make install
```

# Set Pocketsphinx library paths
```
export LD_LIBRARY_PATH=/usr/local/lib
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

# Raspi-config

enable spi

# Enable soft vol
write the following to `/etc/asound.conf`
```
pcm.softvol { 
    type softvol 
    slave { 
        pcm "default"
    }
    control { 
        name "SoftMaster" 
        card 0
    }
}
```
