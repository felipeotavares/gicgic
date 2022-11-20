#RTC
import ds3231
from machine import Timer, SoftSPI, SoftI2C

#OLED
import machine
# import ssd1306
from ssd1306 import SSD1306_I2C
import framebuf
import gfx #biblioteca para ajudar no grafico
import desenhos

#SD
from machine import Pin
from sdcard import SDCard

#ADC
from machine import ADC

#Utilitys
import time
import os

time.sleep(5)

#Inicia OLED
i2c = machine.I2C(0,sda=machine.Pin(19), scl=machine.Pin(18),freq=107816)

# i2c.scan()
# oled = SSD1306_I2C(128, 64, i2c)
# 
# graphics = gfx.GFX(128, 64, oled.pixel) #inicia ajuda grafica
# 
# oled.fill(0)
# oled.text("Iniciando...", 0, 0)
# oled.blit(desenhos.torre, 50, 15)
# oled.blit(desenhos.sun, 0 , 15)
# oled.show()
# time.sleep(1)
# 
# #inicia ADC
# graphics.fill_rect(0,0,128,15,0) #limpa parte superior(amarelo)
# oled.text("Iniciando ADC...", 0, 0)
# oled.show()
# time.sleep(1)
# 
# adc_0 = ADC(Pin(2),atten = ADC.ATTN_11DB)        # create an ADC object acting on a pin
# val = adc_0.read_u16()  # read a raw analog value in the range 0-65535
# adc_0.block()
# 
# #inicia RTC
# rtc = ds3231.RTC()
# rtc.i2c
# 
# #inicia servidor ftp
# import uftpd
# # uftpd.stop()
# # uftpd.start(port = 21,verbose = 1)
# 
# #inicia SD
# graphics.fill_rect(0,0,128,15,0) #limpa parte superior(amarelo)
# oled.text("Iniciando SD...", 0, 0)
# oled.show()
# time.sleep(1)
# 
# spi = SoftSPI(miso=machine.Pin(5), mosi=machine.Pin(6), sck=machine.Pin(4))
# sd = SDCard(spi, machine.Pin(7))
# 
# vfs = os.VfsFat(sd)
# os.mount(vfs, '/sd')
# os.chdir('sd')
# print('SD Card contem:{}'.format(os.listdir()))
# 
# 
# 
# def adquirir(a):
#     #le sinal da porta ADC
#     adc_0_value = adc_0.read_u16()
#     
# #     #le data e hora
#     data = rtc.DS3231_ReadTime(1)
#     
#     #grava dados no SD
#     filename = rtc.DS3231_ReadTime(2) + '.csv'
# #   os.chdir('sd')
#     f = open(filename, 'a')
#     f.write('{},{}\n'.format(adc_0_value,rtc.DS3231_ReadTime(1)))
#     f.close()
# 
#     #mostra no OLED
#     graphics.fill_rect(0,0,128,15,0) #limpa parte superior(amarelo)
#     oled.text('{}<>{}'.format(adc_0_value,rtc.DS3231_ReadTime(3)), 0, 0)
#     oled.show()
# #     print('{}<>{}\n'.format(adc_0_value,rtc.DS3231_ReadTime(3)))
#     
# #     f = open(filename, 'r')
# #     print(f.read())
# #     f.close()
#     pass
# 
# tim0 = Timer(0)
# def start():   
#     tim0.init(period=500, mode=Timer.PERIODIC, callback=adquirir) #parece que esta rolando problema no periodo, 5000 ms esta chamando de 1 em 1 segundo
#     
#     
# # tim0.deinit()
# 
# 
# 

