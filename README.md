# ESP_IrBlaster
This is code that implements a generic IR blaster -- Or as generic as it is for now -- usign only an ESP8266 board (Well, also some resistors, wires, and an IR LED)

# Requirements
### IRremoteESP8266 library:
https://github.com/marcosamarinho/IRremoteESP8266 It is currently working with commit ae6e8eb573789238825018994580985f404ea98c -- or https://github.com/marcosamarinho/IRremoteESP8266/tree/ae6e8eb573789238825018994580985f404ea98c

### ArduinoJson
https://github.com/bblanchon/ArduinoJson It is currently working with commit db9a76f7c60c98b1e0e18fca5673eafef56c8797 -- or https://github.com/bblanchon/ArduinoJson/tree/db9a76f7c60c98b1e0e18fca5673eafef56c8797

### WiFiManager
https://github.com/tzapu/WiFiManager It is currently working with commit 8ffb4cbfa9d1c065915924fa8956199eadf9b2a3 -- or https://github.com/tzapu/WiFiManager/tree/8ffb4cbfa9d1c065915924fa8956199eadf9b2a3

# What's the idea?
The idea is that you scan for IR codes with an Arduino, or maybe another ESP board, and get the protocol, and hex code (no support for raw data, since I didnt need it, but should be so hard to add) and send it to the board as a JSON message, that the board parses to send the IR data.

#### So what does that JSON look like?
Well, a typical message would look like such:
{'protocol':<desired_protocol>,'data':,<desired_hex_code>,'length':<numb_of_bytes>,'repeat':<numb_of_times>,'delay':<delay>'}

So a little more explanation:

The 'protocol' field basically get switched (more like an if statement), for the supported protocol by the current IR Remote library. I have only tested Sony, since that's the tv I have :/ so the other ones need to be tested, and/or added as needed.

The 'data' field get passed as received, there are no checks for validity, it will be a hex value (or int, or your pick), and will look something like 0xa90 (turn on/off code for Sony) -- or at least it does for my TV, and get decoded as Sony from IR lib.

The 'length' field is the lenght of the data being passed, for example 0xa90 is 12 bits long -- Don't worry about this, IR lib will tell you the length :D.

The 'repeat' field determines how many times that command will be sent -- Let's say you wanna put the volume up. You wouldn't wanna send 30 requests. So instead, just tell the ESP to send the command 30 times, throught the repeat field.

The 'delay' field is the delay between repetition of commands being send, and it means milliseconds. Let's say you wanna send a command 30 times, if you have no delay in between, it will not get registered by the TV as several button presses. So you gotta give it a delay -- 50 ms has worked for me.

# Some more insight
The idea, like I mentioned is that you don't have to reflash your board everytime something changes -- like the code you wanna send, or your WIFI name, or password. So we're using WiFiManager, which is a masterpiece -- it attempts to connect to wifi using the credentials that were used the last time a connections was established, and if that connection fails, it goes into AP mode, and broadcasts itself as a WiFi access point, called ESP_IrBlaster (You can change this), and if you do it through a mobile device, as soon as you connect to the wifi, it will prompt you to a site that allows you to scan for networks, and set the new credentials. Refer [here](https://tzapu.com/esp8266-wifi-connection-manager-library-arduino-ide/ "tzapu blog") for more information.

Then, once connected to the wifi, you do need to figure out the ESP's local IP. You can do this by checking your router's DHCP client list, or if you're developing, keep reading the board's serial debug (115200 baud), and it'll print out it's local IP when it connects to wifi. Then use that IP address to send requests to.

# How to quickly test?
I use Python, and the [requests library](http://docs.python-requests.org/en/master/ "requests website") which allows you to quickly send an http requests.
So after you figure out your boards IP address, send a request to it to /irdata, like so:

```python
import requests
from collections import OrderedDict
import json
payload = OrderedDict([('protocol',<prot>),('data',<data>),('length'<length>),('repeat':1),('delay',50)])
headers = {'Content-Type':'application/json'}
r = requests.post("http://<esp_ip>",headers=headers,data=json.dumps(payload))
print r.status_code
print r.text
```

If all went well, and communication witht eh ESP went well, r.status_code should return 200, and r.text should return u'Received IR code.'

# Some Ideas:
It is very easy to exted this to work with, say, and Amazon echo. In fact, I am doing it. Just use [this Python script](https://github.com/makermusings/fauxmo "WeMo switch emulation") -- gotta fix dbg statements on line 304, 310, and 314 if you get an error. The dbg() methos takes only one argument, but two are passed, so just delete the ',e' from it, or comment out the message all together if you prefer. Then you can easily set up another simple server to send requests to you ESP board.

# Enjoy!
