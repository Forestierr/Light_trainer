# test_led.py | Light_Trainer | Robin Forestier | 09.10.2021

import machine, neopixel
import time

n = 5 #number of Neo Pixel LED
p = 16 #Pin number
wait = 10

np = neopixel.NeoPixel(machine.Pin(p), n)

# function to go through all colors
def wheel(pos):
  # Input a value 0 to 255 to get a color value.
  # The colours are a transition r - g - b - back to r.
  if pos < 0 or pos > 255:
    return (0, 0, 0)
  if pos < 85:
    return (255 - pos * 3, pos * 3, 0)
  if pos < 170:
    pos -= 85
    return (0, 255 - pos * 3, pos * 3)
  pos -= 170
  return (pos * 3, 0, 255 - pos * 3)

while True:
    
  for j in range(255):
    for i in range(n):
      rc_index = (i * 256 // n) + j
      np[i] = wheel(rc_index & 255)
      
    np.write()
    time.sleep_ms(wait)