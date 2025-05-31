import serial

class SendCommandsOverSerial:
    def __init__(self, device_name : str):
        #self.ser = serial.Serial(device_name, baudrate=115200, timeout=1)
        pass

    def blick_all_diodes(self, number_of_cycles):
        print("blick_all_diodes")
        self.ser.write(f"LB-{number_of_cycles}".encode())
    
    def togle_diode(self, diode : str):
        print("togle_diode")
        self.ser.write(f"L-{diode}".encode())
    

    def start_pwm(self, frequency):
        print("start_pwm")
        self.ser.write(f"T-{frequency}".encode())

    def stop_pwm(self):
        print("stop_pwm")
        self.ser.write(f"TS".encode())
        

    def start_uart_transmision(self, payload : str):
        print("start_uart_transmision")
        self.ser.write(f"U-{payload}".encode())
    
    def stop_uart_transmision(self):
        print("stop_uart_transmision")
        self.ser.write(f"US".encode())
        

    def receive_from_cdc(self)-> str:
        print("receive_from_cdcs")
        return_str = ""
        while return_str == "":
            return_str = self.ser.readline().decode("utf-8").strip
        
        print(return_str)

    def send_command(self, func, param = None):
        if param == None:
            func(self)
        else:
            func(self, param)

