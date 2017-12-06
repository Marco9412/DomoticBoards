import gc
import network
import wifiConnect
import machine
import ntptime
import time
import json
import umqtt.simple

from debug_utils import DPRINT


# constants
STATE_ON = True
STATE_OFF = False

OPEN = 0
CLICKED = 1
UNINITIALIZED = 2

ON_CMD = "ON"
OFF_CMD = "OFF"
SWITCH_CMD = "SW"
ON_TIME_CMD = "TON"
OFF_TIME_CMD = "TOFF"

# settings ---------------------------------------------------------------------
RELAY_PIN = 14 # See also boot.py
RELAY_INVERTED = False
BUTTON_PIN = 12
BUTTON_PIN_GND = 13
ENABLE_GND_BUTTON = True
BUTTON_TWO_STATES = False
BOARD_TOPIC = b'studio/lampada'
MQTT_HOST_ID = 'espwemos_lampada_studio_client'
MQTT_BROKER_HOST = '192.168.0.239'
MQTT_SSL = False
MQTT_RECONNECT_TIME = 2
BOARD_IP = '192.168.0.205'
NTP_UPDATE_TIME_SECS = 300 # 5 minutes
# TODO ota as http page?

DPRINT('Settings')

# variables --------------------------------------------------------------------
DPRINT('Variables')
sta = network.WLAN(network.STA_IF)
state = STATE_OFF
lastButtonState = UNINITIALIZED
lastClickTime = 0

mqtt_conn = umqtt.simple.MQTTClient(MQTT_HOST_ID, MQTT_BROKER_HOST, user='username', password='passwordddd', ssl=MQTT_SSL)
lastMqttConn = 0

relay = None
button = None

timer_running = False
must_switch = False
sw_timer = machine.Timer(0)

lastNtpUpdate = -NTP_UPDATE_TIME_SECS - 10 # force update at boot

# sys init ---------------------------------------------------------------------
DPRINT('Sysinit')

# ntptime.settime()
# lastNtpUpdate = time.time()
# yield


# functions --------------------------------------------------------------------
DPRINT('Functions')


def is_wifi_ok():
    global sta
    return sta.isconnected()


def publish_current_state():
    global state
    if is_wifi_ok():
        msg = dict()
        msg['stato'] = "ON" if state == STATE_ON else "OFF"
        msg['ts'] = time.time()
        try:
            mqtt_conn.publish(BOARD_TOPIC, json.dumps(msg), True)
        except Exception:
            DPRINT('Cannot publish current state!')


def state_switch():
    global state, timer_running
    if state == STATE_OFF:
        DPRINT("Switching state to enabled")
        state = STATE_ON
        if RELAY_INVERTED:
            relay.off()
        else:
            relay.on()
    else:
        DPRINT("Switching state to disabled")
        state = STATE_OFF
        if RELAY_INVERTED:
            relay.on()
        else:
            relay.off()
    timer_running = False
    must_switch = False
    publish_current_state()
    

def timerc(t):
    global timer_running, must_switch
    DPRINT("Timer expired!")
    must_switch = True


def execute_command(cmd, payload=0):
    global sw_timer, timer_running

    DPRINT('executeCommand(%s,%s)' % (str(cmd),str(payload)))
    if (cmd == ON_CMD and state == STATE_OFF) or \
       (cmd == OFF_CMD and state == STATE_ON) or \
       (cmd == SWITCH_CMD):
        state_switch()
        return
    if ((cmd == ON_TIME_CMD and state == STATE_OFF) or \
       (cmd == OFF_TIME_CMD and state == STATE_ON)) and \
       (not timer_running):
        state_switch()
        timer_running = True
        sw_timer.init(period=(payload*1000), mode=machine.Timer.ONE_SHOT, callback=timerc)
        return
    DPRINT('Unknown command!')


def umqtt_callback(topic, msg):
    DPRINT('umqttCallback(%s, %s)' % (str(topic), str(msg)))
    # DPRINT('topic -> %s, msg -> %s' % ( str(type(topic)), str(type(msg)) )  )

    global state

    if topic != BOARD_TOPIC:
        DPRINT('umqttCallback: invalid topic')
        return
    data = json.loads(msg)
    if 'stato' in data and 'ts' in data:
        if data['stato'] != 'ON' and data['stato'] != 'OFF':
            publish_current_state()
        elif (state == STATE_ON and data['stato'] == 'OFF') or \
           (state == STATE_OFF and data['stato'] == 'ON'):
            if time.time() < data['ts']:
                ntp_update()
            state_switch()
    elif 'cmd' in data:
        execute_command(data['cmd'], data['payload'] if 'payload' in data else 0)
    else:
        publish_current_state()


def check_button():
    global lastButtonState, lastClickTime, button
    bState = OPEN if button.value() == 1 else CLICKED
    tnow = time.time()

    if BUTTON_TWO_STATES and lastButtonState != bState:
        lastButtonState = bState
        DPRINT('Button pressed!')
        execute_command(SWITCH_CMD)
    elif bState == CLICKED and tnow != lastClickTime:
        lastClickTime = tnow
        DPRINT('Button pressed!')
        execute_command(SWITCH_CMD)
        

def check_mqtt():
    global lastMqttConn
    if not is_wifi_ok() or time.time() - lastMqttConn <= MQTT_RECONNECT_TIME:
        return
    try:
        try:
            mqtt_conn.check_msg()
        except Exception as e:
            if not (str(e) == str(OSError(-1))): # OSError(-1) useless
                DPRINT('Exception in mqtt %s' % (str(e)))
                mqtt_conn.connect()
                mqtt_conn.subscribe(BOARD_TOPIC)
    except Exception:
        DPRINT('Other exception, skipping mqtt reconnection...!')
        lastMqttConn = time.time()
        

def ntp_update():
    global lastNtpUpdate
    tnow = time.time()
    if tnow - lastNtpUpdate > NTP_UPDATE_TIME_SECS and is_wifi_ok():
        DPRINT('Updating ntp time')
        try:
            ntptime.settime()
            lastNtpUpdate = tnow
        except Exception:
            DPRINT('Cannot update ntp! Retrying later...')
        

def check_timed_event():
    global must_switch, timer_running
    if must_switch and timer_running:
        must_switch = False
        DPRINT('Timer switching...')
        execute_command(SWITCH_CMD)


# user init --------------------------------------------------------------------
def setup():
    global relay, button, mqtt_conn
    DPRINT('Userinit')
    relay = machine.Pin(RELAY_PIN, machine.Pin.OUT)
    relay.off()

    if ENABLE_GND_BUTTON:
        buttongnd = machine.Pin(BUTTON_PIN_GND, machine.Pin.OUT)
        buttongnd.off()
    button = machine.Pin(BUTTON_PIN, machine.Pin.PULL_UP)
    button.on()

    mqtt_conn.set_callback(umqtt_callback)

# loop -------------------------------------------------------------------------
def loop():
    gc.collect()
    time.sleep_ms(1)
    DPRINT('Loop')
    while True:
        check_button()
        try:
            check_mqtt()
            ntp_update()
            check_timed_event()
        except Exception as e:
            DPRINT('Exception in loop: %s' % (str(e)))
            pass
            
        # yield
        time.sleep_ms(10)


def run_all():
    setup()
    time.sleep_ms(1)
    loop()

