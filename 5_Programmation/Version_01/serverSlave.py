from microdot import Microdot, send_file
import machine, neopixel
from machine import Pin
import time

# setup webserver
app = Microdot()

btn = Pin(17, Pin.IN)

n = 5 #number of Neo Pixel LED
p = 16 #Pin number
np = neopixel.NeoPixel(machine.Pin(p), n)

btn.irq(trigger=Pin.IRQ_RISING, handler=isr)


@app.route('/received', methods=['GET', 'POST'])
def received(request):
    if request.method == 'POST':
        print("received m-1 : ", request.form.get('instruction'))
        for i in range(n):
            np[i] = [0, 255, 255]
        np.write()
    
    return 'yo'


@app.route('/', methods=['GET', 'POST'])
def received(request):
    
    return 'hello there'


@app.before_request
def before_request(request):
    print(".. ..")


@app.after_request
def after_request(request):
    print("... ...")


def isr(pin):
    print("Click")
    for i in range(n):
        np[i] = [255, 255, 0]
    np.write()
    
