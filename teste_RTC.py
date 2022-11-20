import ds3231
import time
from machine import Timer

# Initialisation of RTC object. Several settings are possible but everything is optional.
# If you meet the standards (see /my_lib/RTC_DS3231.py) no parameters are needed.
rtc = ds3231.RTC()
rtc.i2c

# It is encoded like this: sec min hour week day month year.
# Uncomment the line below to set time. Do this only once otherwise time will be set everytime the code is executed.
# rtc.DS3231_SetTime(b'\x00\x14\x18\x28\x14\x07\x21')

# while True:
#     t = rtc.DS3231_ReadTime(1)  # Read RTC and receive data in Mode 1 (see /my_lib/RTC_DS3231.py)
#     print(t)
#     time.sleep(1)
def mycallback(a):
    t = rtc.DS3231_ReadTime(1)  # Read RTC and receive data in Mode 1 (see /my_lib/RTC_DS3231.py)
    print(t)
#     f = open('sample.csv', 'a')
#     f.write('{}\n'.format(t))
#     f.close()
    
#     f = open('sample.csv', 'r')
#     print(f.read())
#     f.close()
    pass
# 
tim0 = Timer(0)
tim0.init(mode=Timer.PERIODIC, period=1000, callback=mycallback)  
#
tim0.deinit() #desativa timer

mycallback(1)


