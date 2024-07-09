#GPS

import time

class GPS():

    def __init__(self, uart, debug=False):
        self.uart = uart
        self.debug = debug
        
        while True : 
            try :               
                self.uart.readline()
                break
            except UnicodeError :
                continue
    
    def config_gnss(self):
        self.send_cmd("$CFGSYS,h35155*68\r\n")
        
    def config_dyn(self):
        self.send_cmd("$CFGDYN,*3D\r\n")
    
    def config_filtre(self):        
        self.send_cmd("$CFGMSG,0,0,1*06\r\n")
        self.send_cmd("$CFGMSG,0,2,0*05\r\n")
        self.send_cmd("$CFGMSG,0,1,0*06\r\n")
        self.send_cmd("$CFGMSG,0,3,0*04\r\n")
        self.send_cmd("$CFGMSG,0,4,1*02\r\n")
        self.send_cmd("$CFGMSG,0,5,0*02\r\n")
        self.send_cmd("$CFGMSG,6,0,0*01\r\n")
    
    def config_PPS(self):
        self.send_cmd("$CFGTP,100000,50000,h5,0,0,0\r\n")
        
    def config_5Hz(self):
        self.send_cmd("$CFGNAV,200,200,1000\r\n")
        
    def send_cmd(self, command):
        if self.debug: print("Sending command:", command.strip())
        if not command.endswith("\r\n"): command += "\r\n"
        
        self.uart.flush()
        self.uart.write(command)
        if not self.verify():
            raise Exception("Verify error for command:", command)
        
    def verify(self, max_tries = 50):

        for tries in range(max_tries):
            line = self.uart.readline()
            
            if line is not None:
                line = line.decode().strip()
                
                if self.debug: print("Try:", tries, "Verify:", line)
                
                if "OK" in line:
                    return True
                elif "FAIL" in line:
                    return False
            
        return False
    
    def config_all(self):
        self.config_gnss()
        self.config_dyn()
        self.config_filtre()
        self.config_PPS()
        self.config_5Hz()
        
    def read_gps(self):
        output = list()
        while self.uart.any():
            line = self.uart.readline()
            if not line.startswith(b"$"):
                continue
            output.append(line.decode())
        return output
    
    def get_time_now(self):
        line = self.uart.readline().decode()
        if (line.split(","))[1] == "" :
            return False
        
        heure_ent = (line.split(","))[1]
        heure = int(((heure_ent).split("."))[0][0:2])
        minute = int(((heure_ent).split("."))[0][2:4])
        seconde = int(((heure_ent).split("."))[0][4:6])
        mili = int(((heure_ent).split("."))[1])*10_000 # Converts tenths of a second to microseconds
        return heure, minute, seconde, mili

def nmea_to_latlong(nmea):
    try:
        gps_data = nmea.split(",")
        lat_str = gps_data[2].split(".")
        long_str = gps_data[4].split(".")
        lat_converted = float(lat_str[0][:-2]) + float(lat_str[0][-2:]+"."+lat_str[1])/60
        long_converted = float(long_str[0][:-2]) + float(long_str[0][-2:]+"."+long_str[1])/60
        if gps_data[5] == "W" : long_converted *= -1
    except Exception as e:
        return 0, 0
        
    return lat_converted, long_converted


