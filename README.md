
# Open Drone ID transmitter example for Linux

This program supports transmitting static drone ID data via Wi-Fi Beacon or Bluetooth on a desktop Linux PC or Raspberry Pi HW.

For further information about Open Drone ID and the related specifications, please see the documentation in [opendroneid-core-c](https://github.com/opendroneid/opendroneid-core-c).

The drone ID data is just static data, since the main purpose is to demonstrate how to setup the Bluetooth and Wi-Fi Beacon HW + SW to transmit valid drone ID data.
This could easily be extended to simulate a bit more dynamic flight example.

## How to compile

The below instructions have been tested on Ubuntu 20.04 and on Raspbian 10 (Buster) / Rasbian 11 (Bullseye).

```
git clone git@github.com:opendroneid/transmitter-linux.git
cd transmitter-linux
git submodule update --init
```
The Bluez source code repository has been set to version 5.53, since this matched the version installed on Ubuntu 20.04.
It might be required to check what version of Bluez is installed on your environment and adjust accordingly:
Check any of `btmon --version`, `btmgmt --version`, `bluetoothctl --version`, `hcitool` or `hciconfig`.

Compile hostapd:

```
sudo apt install build-essential git libpcap-dev libsqlite3-dev binutils-dev bc pkg-config libssl-dev libiberty-dev libdbus-1-dev libnl-3-dev libnl-genl-3-dev libnl-route-3-dev
cd hostapd/hostapd
cp defconfig .config
make -j4
cd -
```

Compile gpsd (see [gpsd/build.adoc](https://gitlab.com/gpsd/gpsd/-/blob/master/build.adoc) for all build requirements):
```
sudo apt install scons
cd gpsd
sed -i 's/\(variantdir *=\).*$/\1 "gpsd-dev"/' SConstruct
scons minimal=yes shared=True gpsd=False gpsdclients=False socket_export=yes
cd -
```

Compile the transmitter example application:
```
mkdir build && cd build
cmake ../.
make -j4
```

## Command line parameters

* `b` Enable Wi-Fi Beacon transmission
* `l` Enable Bluetooth 4 Legacy Advertising transmission using non-Extended Advertising API
* `4` Enable Bluetooth 4 Legacy Advertising transmission using Extended Advertising API
* `5` Enable Bluetooth 5 Long Range + Extended Advertising transmission
* `p` Use message packs instead of single messages
* `g` Use gpsd to dynamically update location messages after each loop of messages

## Starting Wi-Fi Beacon transmission

The Wi-Fi Beacon transmission only works properly when the PC is not connected to any Wi-Fi hotspots.
If it is connected, it somehow prevents configuring the Wi-Fi HW to act as a local access point.

On Ubuntu, open the system settings -> Wi-Fi and for each of the networks that are stored on the machine, click the gear icon and then click forget network.

On the Raspberry Pi OS, try first to click the Wi-Fi icon in the top right corner.
For each network that is already configured, right-click it and forget the network.
(On Raspbian 11 Bullseye, left-click the network and it will ask if you want to forget it).
It is also possible that there are connection settings stored in the file `/etc/wpa_supplicant/wpa_supplicant.conf`.
Edit the file and delete/comment out all known Wi-Fi networks, then reboot.

Check that there is nothing preventing the usage of the Wi-Fi HW by running the tool `rfkill`.
Any SW block should be possible to unblock via the same tool.

To start hostapd, a configuration file must be present with the following content.
E.g. modify the beacon.conf that is provided in the root of the project folder.
Make sure the second line matches the name of your WLAN device.
Can be checked e.g. with `ip link show` or `iw dev`:

```
country_code=DE
interface=wlan0
ssid=DroneIDTest
hw_mode=g
channel=6
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=thisisaverylongpassword
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
ctrl_interface=/var/run/hostapd
beacon_int=200 # Does this work? Doesn't seem to have any effect?

#This is an empty information element. dd indicates hex. 01 is the length of the data and 00 the actual data:
vendor_elements=dd0100

#This information element has one fixed Location message:
#vendor_elements=dd1EFA0BBC0D00102038000058D6DF1D9055A308820DC10ACF072803D20F0100
```

Open a separate shell at the top of the project and start the hostapd daemon.
This only works with `sudo` rights:
```
sudo hostapd/hostapd/hostapd beacon.conf
```

In the original shell, start the opendroneid transmitter.
Again, this only works with `sudo` rights:
```
sudo ./transmit b p
```

This has been tested on a [CometLake Z490 desktop](https://rog.asus.com/motherboards/rog-strix/rog-strix-z490-i-gaming-model) with built-in Wi-Fi HW on the motherboard.
For some reason, a fair amount of the messages being sent to hostapd are not received or at least not properly acknowledged by the lower SW layers.
This is clearly visible when following the command line output.
It is not clear why this is happening.

Another unclear issue is that setting the beacon interval in the beacon.conf file does not seem to have any effect.

When tested on Raspberry Pi 3B and 4B, the beacon transmit only partly worked.
The pre-defined single location message that can be uncommented in the beacon.conf file works okay.
The data that the `transmit` program tries to write to the hostap daemon works okay for single messages (`sudo ./transmit b`) but not for message packs (`sudo ./transmit b p`).
There must be some size limitation in the Wi-Fi driver for vendor specific element data.
Tested on Raspberry Pi 3B with Raspbian 11 Bullseye.
Please note that the drone ID standards mandate message packs to be used for Wi-Fi Beacon transmissions.

## Starting Bluetooth transmission

The program must be run with `sudo` rights:
```
sudo ./transmit 5 p
```

For educational purposes, it is possible to follow the HCI command flow using the tool `sudo btmon`.
This requires Bluez to be installed: `sudo apt install bluez`.

This has been tested on a [CometLake Z490 desktop](https://rog.asus.com/motherboards/rog-strix/rog-strix-z490-i-gaming-model) with built-in Bluetooth HW on the motherboard.
On this HW, BT4 Legacy Advertisements using the Extended Advertising API (`option 4`) and BT5 Long Range + Extended Advertisements (`option 5 p`) would work just fine when run individually.
However, when configuring both simultaneously, for some reason the HW/driver SW would only broadcast the BT4 signals.

Please note that when using Bluetooth transmission for drone ID in the USA, it will be mandatory to transmit both BT4 and BT5 [simultaneously](https://github.com/opendroneid/opendroneid-core-c#relevant-specifications).
And although it is not mandated for Europe or Japan, it would be a good idea to do the same there, in order to maximize the compatibility with receivers and the range of the signals.

Using the BT4 Legacy Advertising non-Extended Advertising HCI commands (`option l`) for some reason didn't work.

When tested on a Raspberry Pi 3B and 4B, the Extended Advertising HCI interface commands are not supported (`option 4` and `option 5 p`).
Probably due to the HW/driver only supporting BT4 Legacy Advertising.
The older BT4 Legacy non-Extended Advertising HCI commands (`option l`) worked okay.

A BT5 USB adapter/dongle of the brand ONVIAN and with the chipset RTL8761B has been tested on a PC with Ubuntu 20.04 and proven to be able to successfully transmit in Long Range mode.

## How to clean up

If the program is terminated abnormally, Beacon and Bluetooth broadcasts can remain running.
* To stop Beacon broadcast, stop the `hostapd` instance.
  It can be difficult to stop the `transmit` instance. After stopping `hostapd`, use `sudo pkill transmit`.
* To stop Bluetooth, use `sudo btmgmt power off` and then `sudo btmgmt power on`.
