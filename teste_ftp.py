import uftpd

import os
from machine import Pin, SoftSPI
from sdcard import SDCard

uftpd.stop()

# Pin assignment:
# MISO -> GPIO 13
# MOSI -> GPIO 12
# SCK  -> GPIO 14

# CS   -> GPIO 27
spi = SoftSPI(miso=Pin(5), mosi=Pin(6), sck=Pin(4))
sd = SDCard(spi, Pin(7))


print('Root directory:{}'.format(os.listdir()))
vfs = os.VfsFat(sd)
os.mount(vfs, '/sd')
print('Root directory:{}'.format(os.listdir()))
os.chdir('sd')
print('SD Card contem:{}'.format(os.listdir()))

f = open('sample.csv', 'r')
print(f.read())
f.close()

uftpd.start(port = 21,verbose = 1)