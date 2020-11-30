# wifiRelay
a way to create an electrical relay with a wifi command.
using IDE :
- Arduino (ino, h, cpp files)
- gedit (html files)
- freeCad (stl files)
- kicad (and gimp, to provide a .pdf file for the PCB)

wifiRealy.ino drives a relay from a web client, Firefox, Chrome, Midori and maybe Edge (I only use Linux Debian)
Before using wifiRelay.ino, ssid and password values must be written in the EEPROM memory by eepromPutEsp.ino.
Then C_Eprom class (c_eprom.h, c_eprom.cpp) reads those values for wifiRelay.ino, and handles user's changes.

webPage.html and boxPage.html are in the data directory, and uploaded in the SPIFFS memory.

eepromPutEsp.ino must provide values to connect as an Access Point (WIFI_AP mode)
 - an ssid like "192.168.x.x", for example 192.168.4.10
 - a password 8..31 bytes
With boxPage.html, the user may change these values. He can change his password, or change for a WIFI_STA mode connexion. The esp8266 becomes a station depending on his box.	In this case, the user must find by himself the address he has be given by his box with the command line 
user@user:~$ nmap -sP 192.168.1.* (if the box has 192.168.1.0 address) or 192.168.*.* (wil takes more time).

wifiRelay setup
- 1 tries to connect in WIFI_AP mode with eepromPutEsp.ino values
- 2 if it fails (the ssid is not the original one) , it tries to connect in WIFI_STA mode
- 3 if it fails, whatever the reason, C_Eprom takes the eepromPutEsp.ino values back and the esp8266 is connected as an Access Point.
With this method, you ensure a connection, as closed as possible to what the user expects.

On and Off buttons in webPage.html allow to switch the relay on or off.

Thanks for your help, 
 * https://circuits4you.com/
 * https://github.com/esp8266/Arduino
 * https://projetsdiy.fr/e66-serveur-ftp-echange-fichiers-spiffs/
 * https://www.screenfeed.fr/blog/personnalisation-chesp82ckbox-boutons-radio-sans-image-0570/
 * http://tiptopboards.free.fr/arduino_forum/viewtopic.php?f=2&t=54

.stl files are written by freeCad, to create .gcode files for any 3D printer. (see the pictures)
You need :
2 connectors able to support AC 220V
1 HKL-PM01 
1 ESP8266-01 relay (5V DC)
1 male and 1 female electrical outlet
3 feet of electrical wire
Connexions are easier with a small PCB, and still easier to wire if you stick the parts on plaque.stl and rehausse.stl
The red wires are connected to the relay, one to com, one to nc (neutral close) because of digitalWrtite(GPIO, HIGH)






