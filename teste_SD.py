import os
from machine import Pin, SoftSPI
from sdcard import SDCard

# Pin assignment:
spi = SoftSPI(miso=Pin(5), mosi=Pin(6), sck=Pin(4))
sd = SDCard(spi, Pin(7))

print('Root directory:{}'.format(os.listdir()))
vfs = os.VfsFat(sd)
os.mount(vfs, '/sd')
print('Root directory:{}'.format(os.listdir()))
os.chdir('sd')
print('SD Card contem:{}'.format(os.listdir()))

# 2. To create a new file for writing:
# corrente = float(35)
# f = open('sample.csv', 'a')
# f.write('{}\n'.format(corrente))
# f.close()
# 
# # 1. To read file from the root directory:
# f = open('sample.csv', 'r')
# print(f.read())
# f.close()

# 3. To append some text in existing file:
# f = open('sample3.txt', 'a')
# f.write('Some text for sample 3')
# f.close()

# 4. To delete a file:
# os.remove('file to delete')

# 5. To list all directories and files:
# os.listdir()

# 6. To create a new folder:
# os.mkdir('sample folder')

# 7. To change directory:
# os.chdir('directory you want to open')

# 8. To delete a folder:
# os.rmdir('folder to delete')

# 9.  To rename a file or a folder:
# os.rename('current name', 'desired name')

# def web_page():
#   html = """<html><head><meta name="viewport" content="width=device-width, initial-scale=1"></head>
#   <body><h1>Hello, World!</h1></body></html>"""
#   return html
# 
# s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# s.bind(('', 80))
# s.listen(5)