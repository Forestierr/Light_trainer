#receiver
import network
from esp import espnow
from machine import Pin
from time import sleep

led = Pin(2, Pin.OUT)

# A WLAN interface must be active to send()/recv()
w0 = network.WLAN(network.STA_IF)
w0.active(True)

e = espnow.ESPNow()
e.init()
peer = b'\xff\xff\xff\xff\xff\xff'   # MAC address of peer's wifi interface
e.add_peer(peer)

while True:
    led.value(not led.value())
    sleep(0.1)
    
    
    if e.poll():             # msg == None if timeout in irecv()
        host, msg = e.irecv()
        print(host, msg)
        msg += " n2_ok"
        e.send(peer, msg, True)
        if msg == b'end':
            break