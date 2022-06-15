 #sender

import network
from esp import espnow
import time
from machine import Pin
import machine, neopixel

np = neopixel.NeoPixel(machine.Pin(16), 5)
for i in range(5):
    np[i] = (100, 100, 0)
    np.write()
time.sleep_ms(2000)
print(123)

# A WLAN interface must be active to send()/recv()
w0 = network.WLAN(network.AP_IF)  # Or network.AP_IF # Or network.STA_IF
w0.active(True)

e = espnow.ESPNow()
e.init() #\xac\x67\xb2\x06\x47\xb1



push_button = Pin(17, Pin.IN)
logic_state = False

peer = b'\xff\xff\xff\xff\xff\xff'   # MAC address of peer's wifi interface
for i in range(5):
    np[i] = (0, 0, 125)
    np.write()
time.sleep_ms(1000)
#print(e.get_peers())

try:
    for i in range(5):
        np[i] = (125, 0, 125)
        np.write()
    time.sleep_ms(1000)
    e.add_peer(peer)
except:
    for i in range(5):
        np[i] = (125, 125, 0)
        np.write()
    time.sleep_ms(1000)
    pass

for i in range(5):
    np[i] = (0, 125, 0)
    np.write()
time.sleep_ms(1000)

#e.send("Starting...")       # Send to all peers

#msg = input("oui ? : ")
#e.send(peer, msg, True)
x = 0
while True:
    """
    if e.poll():             # msg == None if timeout in irecv()
        host, msg = e.irecv()
        for mac, msg in e:
            print(f"Recv: mac={mac}, message={msg}")
        msg = input("oui ? : ")
        e.send(peer, msg, True)
        if msg == b'end':
            break
    """

    if push_button.value() == 1:
        x = x + 1
        for i in range(5):
            np[i] = (125, 125, 125)
            np.write()
        time.sleep_ms(200)
        for i in range(5):
            np[i] = (0, 0, 0)
            np.write()
        e.send(peer, str(x), True)
        time.sleep_ms(500)
# for i in range(300):
#     time.sleep(1)
#     e.send(peer, str(i), True)
e.send(b'end')