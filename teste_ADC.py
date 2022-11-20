from machine import ADC, Pin
import time
from machine import Timer

adc = ADC(Pin(2),atten = ADC.ATTN_11DB)        # create an ADC object acting on a pin
val = adc.read_u16()  # read a raw analog value in the range 0-65535
val = adc.read_uv()   # read an analog value in microvolts
adc.block()


def mycallback(a):
#     val = adc.read_uv()
    val = adc.read_u16()
    print(val)
#     f = open('sample.csv', 'a')
#     f.write('{}\n'.format(t))
#     f.close()
    
#     f = open('sample.csv', 'r')
#     print(f.read())
#     f.close()
    pass

tim0 = Timer(0)
tim0.init(mode=Timer.PERIODIC, period=1000, callback=mycallback)  
#
tim0.deinit() #desativa timer

mycallback(1)

adc.