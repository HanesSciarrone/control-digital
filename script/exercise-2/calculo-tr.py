import sys
import serial
import csv
import matplotlib.pyplot as plt

RELATION_ADC_CONVERSION = (3.3)/1023
SAMPLE_FREQUENCY = 1000

port = sys.argv[1]
print("Port serial is = {}\n".format(port))

y_data = list()
x_data = list()

def main():
    index = 0
    fd = serial.Serial(port, 115200)
    fd.flushInput()

    while (index <= 1200):
        var = fd.read(1)
        data = bytearray()

        while(var != b'\x0a'):
            data+=var
            var = fd.read(1)

        if (index != 0):
            x_data.append(index*1/SAMPLE_FREQUENCY)
            y_data.append(int(data.decode('UTF-8'))*RELATION_ADC_CONVERSION)
        index=index+1

    fd.close()

    data = (x_data, y_data)

    with open('data.csv', 'w') as file:
        writer = csv.writer(file)
        writer.writerows(data)


main()
"""
with open('data.csv', 'r') as file:
    reader = csv.reader(file)
    index = 1
    for row in reader:
        if (index == 1):
            data_1 = row
        else:
            data_2 = row
         index = index+1

    print("Data 1 is = {}\n".format(data_1))
    print("Data 2 is = {}\n".format(data_2))
"""

fig = plt.figure(1)
resp = plt.plot(x_data, y_data, 'r-',linewidth = 2, alpha = 1, label="Respuesta al escalon a lazo abierto (Cuadrada de 10Hz)")
plt.grid(True)
plt.show()
