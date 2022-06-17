import network
import time
from server import app

def detect_wlan():
    wlan = network.WLAN(network.STA_IF)
    nets = wlan.scan()
    
    flag_conn = False
    
    for net in nets:
        if b'Light Trainer' in net:
            print("ok")
            connect_to_wifi()
            flag_conn = True
    
    if not flag_conn:
        print("non")
        create_wifi()
        
    # print(nets)
        

def connect_to_wifi():
    ap_ssid = "Light Trainer"
    ap_password = "12345678"
    # ap_authmode = 3  # 0 Open / 3 WPA2-PSK
    
    wlan_ap = network.WLAN(network.STA_IF)
    wlan_ap.active(True)
    if not wlan_ap.isconnected():
        print("connection to the wifi")
        wlan_ap.connect(ap_ssid, ap_password)
        while not wlan_ap.isconnected():
            pass
        
    print("network config : ", wlan_ap.ifconfig())


def create_wifi():
    ap_ssid = "Light Trainer"
    ap_password = "12345678"
    ap_authmode = 3  # 0 Open / 3 WPA2-PSK

    wlan_ap = network.WLAN(network.AP_IF)

    wlan_ap.active(True)
    wlan_ap.config(essid=ap_ssid, password=ap_password, authmode=ap_authmode)

    wlan_ap.ifconfig(('1.1.1.1', '255.255.255.0', '1.1.1.4', '8.8.8.8'))

    print(wlan_ap.ifconfig())
    print('Create WiFi ssid ' + ap_ssid + ', default password: ' + ap_password)
    
    print("start server")
    app.run(host="0.0.0.0", port=80, debug=True)


if __name__ == "__main__":
    detect_wlan()
