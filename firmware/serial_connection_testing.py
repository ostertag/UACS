import serial

with open("commands_for_serial_communication.txt", "r") as file:
    for line in file:
        print(line, end="")
device = input(f"\rInsert name of device for connection >")

ser = serial.Serial(f'/dev/{device}', baudrate=115200, timeout=1)
print(f"Connection established with {ser.name}")

while True:
    command = input(f"\rInsert command >")
    ser.write(command.encode())
    line = ser.readline().decode('utf-8')
    if line:
        print(line)