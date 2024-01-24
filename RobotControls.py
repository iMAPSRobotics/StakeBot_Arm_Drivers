import serial
import threading
import time
#from pynq.overlays.base import BaseOverlay
#from pynq_peripherals import ArduinoSEEEDGroveAdapter
#base = BaseOverlay('base.bit')


####################################################
## StakeBot Driver ##
####################################################

class SerialHandler():
    def __init__(self, ser, name, baud) -> None:
        self.ser = ser
        self.baud = baud
        self.name = name
        self.connections = {}
        self.queue = []
        for i in range(len(name)):
            self.connections[name[i]] = [ser[i], []]
        thread = threading.Thread(target=self.handelSerial, args=())
        thread.start()    
    
    def handelSerial(self):    
        while True:
            # print(self.queue)
            if len(self.queue) != 0:
                name, message = self.queue.pop(0)
                # print(name, message, self.connections)
                ser = serial.Serial(self.connections[name][0], self.baud, timeout=0.1)
                #print(ser)
                if ser.is_open == False:
                    # print("open")
                    ser.open()
                data = bytes(message, 'utf-8')
                ser.write(data)
                ## Read Data
                while ser.is_open is True:
                    out = ser.readline()
                    # print(out)
                    # print(self.connections)
                    if out != b'' :
                        self.connections[name][1].append(out)
                        if len(self.connections[name][1]) >= 2:
                            ser.flush()
                            ser.close()
                            # print("Close Connection")
                            self.connections[name][1] = []
                            break

                            
            

class StakeBotMotorDrivers(SerialHandler):
    def __init__(self, dev, name, buad):
        self.posA = 0.0
        self.posB = 0.0
        self.gripL_pos = 0
        self.gripR_pos = 0
        self.boundsA = 0.0
        self.boundsB = 0.0
        self.cmd = ""
        self.refCmd = ""
  
        super().__init__(dev, name, buad)
        self.thread = threading.Thread(target=self.connectToStepperDriver, args=())
        self.thread.start()

    def connectToStepperDriver(self):
        while True:
            #print(self.connections, self.queue)
            if self.cmd != self.refCmd:
                super().write(self.name, self.cmd)
                output = self.connections[self.name][1][0]
                if output[0] == b"motorA":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.posA = float(self.connections[self.name][1][1])
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"motorB":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.posB = float(self.connections[self.name][1][1][1])
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"measureA":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.boundsA = float(self.connections[self.name][1][1][0])
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"measureB":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.boundsB = float(self.connections[self.name][1][1][0])
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"calibrate":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.posA = 0.0
                            self.posB = 0.0
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"movement":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.posA = float(self.connections[self.name][1][1][0])
                            self.posB = float(self.connections[self.name][1][1][1])
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"gl":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"gr":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.connections[self.name][1] = []
                            break
                elif output[0] == b"gp":
                    refConnection = self.connections[self.name][1]
                    while True:
                        if self.connections[self.name][1] != refConnection:
                            self.gripL_pos = int(self.connections[self.name][1][1][0])
                            self.gripR_pos = int(self.connections[self.name][1][1][1])
                            self.connections[self.name][1] = []
                            break
            self.refCmd = self.cmd


    def getPosition(self, motor, name):
        self.queue.append([name, "getPosition\n{0}".format(motor)])

    def goToPosition(self, posA, posB, name):
        self.queue.append([name, "goToPosition\n{0},{1}".format(posA, posB)])

    def calibrate(self, name):
        self.queue.append([name, "calibrate"])

    def measureBounds(self, motor, name):
        self.queue.append([name,"measureBounds\n{0}".format(motor)])

    def stop(self, name):
        self.queue = [name, "stop"]

    def gripperL(self, pos, name):
        self.queue.append([name, f"gl {pos}"])

    def gripperR(self, pos, name):
        self.queue.append([name, f"gr {pos}"])

    def actuator(self, move, name):
        if move == "in":
            self.queue.append([name, "ao 0"])
            self.queue.append([name, "ai 1"])
        elif move == "out":
            self.queue.append([name, "ai 0"])
            self.queue.append([name, "ao 1"])
    
    def stopActuator(self, name):
        self.queue.append([name, "ao 0"])
        self.queue.append([name, "ai 0"])







