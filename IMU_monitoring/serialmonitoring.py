import ctypes
import serial
import time
import math
import numpy as np

from realtimeplot.realtimeplot import RealTimePlot

# Serial port.
ser_port = "COM8"
# Serial speed.
ser_speed = 115200

# reload plot data.. (The writing style is creepy+<<)
def reload_data(plotdata, gyro_list, accel_list, synthe_list):
    plotdata["gyro_x"] = gyro_list[0]
    plotdata["gyro_y"] = gyro_list[1]
    plotdata["gyro_z"] = gyro_list[2]

    plotdata["accel_x"] = accel_list[0]
    plotdata["accel_y"] = accel_list[1]
    plotdata["accel_z"] = accel_list[2]

    plotdata["synthe_acc"] = synthe_list

    return plotdata

def getkey(key):
    return(bool(ctypes.windll.user32.GetAsyncKeyState(key) & 0x8000))

def main():
    """ Main sequence """
    # Setting Serial info.
    ser = serial.Serial(ser_port, ser_speed)
    # class init.
    plt = RealTimePlot()

    # Utilize data in a dictionary type to make the display. (Initialization for that)
    plot_data = {}
    # Number of data to be plotted
    plot_data["plotdata"] = 100
    # gyro data (list[3] <= x, y, z)
    gyro_list = [[0.0] * plot_data["plotdata"] for i in range(3)]
    # accel data (list[3] <= x, y, z)
    accel_list = [[0.0] * plot_data["plotdata"] for i in range(3)]
    # Synthe accel data list
    synthe_list = [0] * plot_data["plotdata"]
    # reload plot data.
    plot_data = reload_data(plot_data, gyro_list, accel_list, synthe_list)

    while True:
        # for run time measurement
        start = time.time()
        # read serial data.
        line = ser.readline()
        # Read the data as a comma-separated list.
        readdata = line.decode("utf-8").strip().split("\n")
        readdata = readdata[0].split(",")
        # for debug.
        print("read data :",readdata)

        # For the calculation of synthetic acceleration
        synthe = 0
        # Add new data
        for cnt, data in enumerate(readdata):
            if cnt < 3:
                gyro_list[cnt].insert(0, float(data))
                # Add the squared value for the composite acceleration
                synthe += float(data) ** 2
            elif 2 < cnt:
                accel_list[3 - cnt].insert(0, float(data))
        # add Add to the list of composite acceleration (Add the last multiplied value)
        synthe_list.insert(0, math.sqrt(synthe))
        # Reset of calculation for synthetic acceleration
        print(math.sqrt(synthe))
        synthe = 0

        # Remove the oldest data
        for gyro_d, accel_d in zip(gyro_list, accel_list):
            del gyro_d[plot_data["plotdata"]]
            del accel_d[plot_data["plotdata"]]
        del synthe_list[plot_data["plotdata"] - 1]

        # reload plot data.
        plot_data = reload_data(plot_data, gyro_list, accel_list, synthe_list)

        # set and draw plot.
        plt.set_data(plot_data)
        plt.pause(0.01)

        # Run time measurement
        elapsed_time = time.time() - start
        print ("elapsed_time:{0}".format(elapsed_time) + "[sec]")

        # If you press the ESC key, exit the program.
        if getkey(0x1B):
            break

if __name__ == "__main__":
    print("We're ready! Start monitoring...")
    main()