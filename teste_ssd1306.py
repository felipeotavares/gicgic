import ds3231
import time
from machine import Timer, SoftSPI
from sdcard import SDCard
import os
import gfx
import framebuf
import desenhos

import machine
import ssd1306
from ssd1306 import SSD1306_I2C

spi = SoftSPI(miso=machine.Pin(5), mosi=machine.Pin(6), sck=machine.Pin(4))
sd = SDCard(spi, machine.Pin(7))

print('Root directory:{}'.format(os.listdir()))
vfs = os.VfsFat(sd)
os.mount(vfs, '/sd')
print('Root directory:{}'.format(os.listdir()))
os.chdir('sd')
print('SD Card contem:{}'.format(os.listdir()))

# rtc = ds3231.RTC()

# i2c = machine.I2C(sda=machine.Pin(19), scl=machine.Pin(18),address=60)
i2c = machine.I2C(0,sda=machine.Pin(19), scl=machine.Pin(18),freq=213333)
# i2c = machine.I2C(machine.Pin(19), machine.Pin(18), freq=100000)

i2c.scan()

rtc = ds3231.RTC()
rtc.i2c

oled = SSD1306_I2C(128, 64, i2c)

oled.fill(1)
oled.text("teste6", 0, 0)
oled.show()

def mycallback(a):
    t = rtc.DS3231_ReadTime(1)  # Read RTC and receive data in Mode 1 (see /my_lib/RTC_DS3231.py)
    print(t)
    oled.fill(0)
    oled.text(t, 0, 0)
#     oled.text('teste9', 0, 0)

    
    f = open('gic.csv', 'a')
    f.write(t+'\n')
    f.close()
#     # print("foi\n")
#     # 1. To read file from the root directory:
    f = open('gic.csv', 'r')
    print(f.read())
    f.close()
#     
    
#     oled.text(t.split('>')[0], 0, 0)
#     oled.text(t.split('>')[1], 0, 0)
    oled.show()
    pass

mycallback(1)

tim0 = Timer(0)
tim0.init(mode=Timer.PERIODIC, period=1000, callback=mycallback)  
#
tim0.deinit() #desativa timer

mycallback(1)

graphics = gfx.GFX(128, 64, oled.pixel)

oled.fill(0)
graphics.line(0, 15, 128, 15, 1)
graphics.circle(64, 32, 15, 1)
oled.show()







oled.fill(0)
oled.blit(desenhos.torre, 0, 15)
oled.blit(sun, 50 , 15)
oled.show()

oled.fill(0)
oled.blit(desenhos.sun, 50 , 15)
oled.show()
