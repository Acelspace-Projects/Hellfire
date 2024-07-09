#test tx

from sx1262 import SX1262
from machine import *
from GPS import GPS
import time, struct

sf, cr = 11, 8
msg_fmt = "!22sQ"
print("Taille du message:", struct.calcsize(msg_fmt), "octets")

pin_alim_GNSS = Pin(3, Pin.OUT)
pin_alim_GNSS.on()
time.sleep(1)

sx = SX1262(spi_bus=1, clk=9, mosi=10, miso=11, cs=8, irq=14, rst=12, gpio=13)
gps = GPS(UART(1, baudrate=115200, tx=Pin(34), rx=Pin(33), timeout=1000), debug=True)

sx.begin(freq=869.5, bw=125.0, sf=sf, cr=cr, syncWord=0x12,
         power=22, currentLimit=140.0, preambleLength=8,
         implicit=False, implicitLen=0xFF,
         crcOn=True, txIq=False, rxIq=False,
         tcxoVoltage=1.8, useRegulatorLDO=False, blocking=True)

gps.config_all()

while True:
    print(gps.read_gps())
    time.sleep(0.2)