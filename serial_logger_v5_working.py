import serial
import time
import csv
import sys
from datetime import datetime
from os import path
import platform

class RelaySerial():
    def __init__(self, comport, debug=False):
        print('Opening communication...')
        self.debug = debug
        self.ser = serial.Serial()
        self.ser.port = comport 
        self.ser.baudrate = 115200
        self.ser.bytesize = serial.EIGHTBITS 
        self.ser.parity = serial.PARITY_NONE
        self.ser.stopbits = serial.STOPBITS_ONE 
        self.ser.xonxoff = False
        self.ser.writeTimeout = 2
        try: 
            self.ser.open()
            read_response = self.ser.readline()   # read a '\n' terminated line --> NEEDED!!! Arduino flash memory can otherwise be overwriten during serial initiation (then needs to be rewritten)
            # print('read_response returned: ', read_response)            
            print('Communication established')
        except Exception:
            print("Class RelaySerial Init: Error open serial port: " + str(comport))
            sys.exit('Class RelaySerial Init: Error open serial port: ' + str(comport)) 
            
    def send(self, commandToSend):            
        if self.ser.isOpen():
            try:
                self.ser.flushInput() 
                self.ser.flushOutput()
                #print("writing to serial: ", commandToSend)
                self.ser.write((commandToSend + '\n').encode())
                # read back the command we just send
                while True:
                    readback_of_send_command = str(self.ser.readline())[3:-1]
                    # print('readback_of_send_command: ', readback_of_send_command)
                    if readback_of_send_command == 'a\\n':
                        print('relay(s) activated with: ', commandToSend)
                        break
                time.sleep(0.5)
                # print('readback_of_send_command: ', readback_of_send_command)
            except Exception:
                print('Error in function send() in class RelaySerial')
                response = None
                
    def read(self):            
        if self.ser.isOpen():
            try:
                self.ser.flushInput() 
                self.ser.flushOutput()
                readback = str(self.ser.readline().decode("utf-8"))
                readback = readback.replace('\r\n','')
                #print(readback)
                time.sleep(0.5)
            except Exception:
                print('Error in function send() in class RelaySerial')
                readback = None
        return readback
                
    def read_loop(self):            
        if self.ser.isOpen():
            try:
                self.ser.flushInput() 
                self.ser.flushOutput()
                # read back the command we just send
                while True:
                    readback = str(self.ser.readline().decode("utf-8"))
                    readback = readback.replace('\r\n','')
                    print(readback)
                    if readback == 'EOF':
                        print('EOF reached')
                        break
                    time.sleep(1)
                time.sleep(0.5)
                # print('readback_of_send_command: ', readback_of_send_command)
            except Exception:
                print('Error in function send() in class RelaySerial')
                response = None                 
                
                
    def closeCom(self):
        """Close communication"""
        print('Closing communication...')
        try:
            self.ser.close()
            if not self.ser.isOpen():
                print('Communication closed successfully')
            else:
                print('Error: Communication could not be closed')
        except:
            print('Error: Communication could not be closed')
            

            
if __name__ == "__main__":
    try:
        delimiter_symbol = ';'
        running_data_file_counter = 1
        
        if platform.system() == 'Windows':
            serial_relay = RelaySerial('COM10', debug=False)
        elif platform.system() == 'Linux':
            serial_relay = RelaySerial('/dev/ttyUSB0', debug=False)
        else:
            serial_relay = RelaySerial('/dev/ttyUSB0', debug=False)
        initial_date = datetime.utcnow().strftime('%Y-%m-%d')
        output_file = initial_date + '_logging_data_' + str(running_data_file_counter) + '.csv'
        while path.exists(output_file):
            running_data_file_counter = running_data_file_counter + 1
            output_file = initial_date + '_logging_data_' + str(running_data_file_counter) + '.csv' 
                
        while(True):
            current_date = datetime.utcnow().strftime('%Y-%m-%d')
            current_time = datetime.utcnow().strftime('%Y-%m-%d;%H:%M:%S.%f')[:-3]
            if current_date != initial_date:
                initial_date = current_date
                running_data_file_counter = 1
                output_file = initial_date + '_logging_data_' + str(running_data_file_counter) + '.csv'
                while path.exists(output_file):
                    running_data_file_counter = running_data_file_counter + 1
                    output_file = initial_date + '_logging_data_' + str(running_data_file_counter) + '.csv'                
                print('output file is:', output_file)
               
            readback = serial_relay.read()      # init all relays to: deactivated
            print(readback)
            with open(output_file, 'a', encoding='utf-8') as f:   #newline='\r\n', 
                output_string = current_time + delimiter_symbol + readback + '\n'
                f.write(output_string)
                #writer = csv.writer(f, delimiter=delimiter_symbol)
                #writer.writerows([current_time, readback])
        
    except Exception as e:
        print(('Exception in main function: '+ str(e)))
        serial_relay.closeCom()       # Close serial communication to SIB module at end of test
                     
    except: # default exception (if no exception type is given, e.g. with KeyboardInterrupt)
        serial_relay.closeCom()       # Close serial communication to SIB module at end of test










         