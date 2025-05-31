import serial.tools.list_ports
import os
import inspect
import time
from sending_commands_over_serial import SendCommandsOverSerial

class TestingSTM32UI:
    def __init__(self):
        self.com = SendCommandsOverSerial("/dev/ttyACM2")
        self.commands = self.get_commands()

    def clear_screen(self):
        os.system("clear")

    def print_menu(self, device_name : str):
        print_str = f"Connected device: {device_name}\nCommnads:\n"
        for i in range(len(self.commands)):
            print_str += f"{i}) {self.commands[i][0]}\n"
        print_str += f"{i+1}) end_script\n"
        print(print_str)

    def get_commands(self)->list[str]:
        commands = inspect.getmembers(SendCommandsOverSerial, predicate=inspect.isfunction, )
        return [(name, func) for name, func in commands if (name != "__init__" and name != "send_command")]
    
    def generate_UI(self):
        while True:
            self.clear_screen()
            self.print_menu("/dev/ttyACM3")
            user_input = input("Insert command number > ")
            if 0 <= int(user_input) < len(self.commands):
                func = self.commands[int(user_input)][1]
                sig = inspect.signature(func)
                param = None
                print(self.commands[int(user_input)][0])
                if len(sig.parameters) > 1:
                    param = input(f"Please provide {list(sig.parameters)[1]} > ")
                self.com.send_command(func, param)

            elif int(user_input) == len(self.commands):
                print("Ending script")
                break
            else:
                print("Invalid number")
            
            input("Press ENTER for continue")



testing_script = TestingSTM32UI()
testing_script.generate_UI()


