
import network
import time

def wifiConnect():
    ap = network.WLAN(network.AP_IF)
    ap.active(False)
    sta = network.WLAN(network.STA_IF)
    if not sta.isconnected():
        sta.active(True)
        sta.connect('ssid','passworddd')
        while not sta.isconnected():
            time.sleep_ms(100)

