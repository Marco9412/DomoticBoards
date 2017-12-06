# This file is executed on every boot (including wake-boot from deepsleep)
import esp
import gc
import webrepl
import wifiConnect
import time
import machine


RELAY_PIN = 14
p = machine.Pin(RELAY_PIN, machine.Pin.OUT)
p.off()
del p

esp.osdebug(None)
wifiConnect.wifiConnect()
webrepl.start()
gc.collect()

print('Launching core.py')
from core import run_all
run_all()
