import matplotlib as mpl
import matplotlib.pyplot as plt

class RealTimePlot():
    def __init__(self):
        # matplotlab init.
        self.fig = plt.figure(figsize=(14,10))
        # do initialize.
        self.initialize()

    # Put out a min value in a list of three
    def get_minvalue(self, d1, d2, d3):
        m = 0
        if min(d1) < min(d2):
            m = min(d1)
        else:
            m = min(d2)
        if min(d3) < m:
            m = min(d3)
        return m
    # Put out a max value in a list of three
    def get_maxvalue(self, d1, d2, d3):
        m = 0
        if max(d2) < max(d1):
            m = max(d1)
        else:
            m = max(d2)
        if m < max(d3):
            m = max(d3)
        return m

    def initialize(self):
        # subtile and layout setting
        self.fig.suptitle('IMU monitoring', size=14)
        self.fig.subplots_adjust(left=0.05, bottom=0.1, right=0.95, top=0.90, wspace=0.1, hspace=0.2)

        ### Gyto ###
        # Plotting the Gyrograph
        self.gyro_ax = plt.subplot2grid((2,2),(0,0))
        self.gyro_ax.grid(True)

        # Title and label settings(gyro plot)
        self.gyro_ax.set_title('Gyro data')
        self.gyro_ax.set_xlabel('time')
        self.gyro_ax.set_ylabel('o/s')

        # plot initilize(GyroData)
        self.lines_gx, = self.gyro_ax.plot([-1,-1],[1,1],label='gyro_x')
        self.lines_gy, = self.gyro_ax.plot([-1,-1],[1,1],label='gyro_y')
        self.lines_gz, = self.gyro_ax.plot([-1,-1],[1,1],label='gyro_z')

        ### Accel ###
        # Plotting the Accelgraph
        self.accel_ax = plt.subplot2grid((2,2),(0,1))
        self.accel_ax.grid(True)

        # Title and label settings(accel plot)
        self.accel_ax.set_title('Accel data')
        self.accel_ax.set_xlabel('time')
        self.accel_ax.set_ylabel('mg')

        # plot initilize(AccelData)
        self.lines_acc_ax, = self.accel_ax.plot([-1,-1],[1,1],label='accel_x')
        self.lines_acc_ay, = self.accel_ax.plot([-1,-1],[1,1],label='accel_y')
        self.lines_acc_az, = self.accel_ax.plot([-1,-1],[1,1],label='accel_z')

        ### Synthetic acceleration ###
        # Plotting the synthetic accel data graph
        self.synthetic_acc_ax = plt.subplot2grid((2,2),(1,0), colspan=2)
        self.synthetic_acc_ax.grid(True)

        # Title and label settings(synthetic acceleration plot)
        self.synthetic_acc_ax.set_title('Synthetic acceleration data')
        self.synthetic_acc_ax.set_xlabel('time')
        self.synthetic_acc_ax.set_ylabel('Synthetic acceleration')

        # plot initilize(Synthetic acceleration data)
        self.lines_sa, = self.synthetic_acc_ax.plot([-1,-1],[1,1], label='synthetic acc')

    # Assign the necessary data from the data of the dictionary type to the value of the subplot, with the value name and value assigned
    def set_data(self,data):
        # gyro plot line.
        self.lines_gx.set_data(range(data["plotdata"]), data['gyro_x'])
        self.lines_gy.set_data(range(data["plotdata"]), data['gyro_y'])
        self.lines_gz.set_data(range(data["plotdata"]), data['gyro_z'])
        # Set to draw up to a number of draws
        self.gyro_ax.set_xlim(0, data["plotdata"])
        # Dynamically switch the maximum value
        self.gyro_ax.set_ylim(self.get_minvalue(data['gyro_x'], data['gyro_y'], data['gyro_z']) - 200,
                                self.get_maxvalue(data['gyro_x'], data['gyro_y'], data['gyro_z']) + 200
                                )
        # Need to fix the legend
        self.gyro_ax.legend(loc = 'upper right')

        # accel plot line.
        self.lines_acc_ax.set_data(range(data["plotdata"]), data['accel_x'])
        self.lines_acc_ay.set_data(range(data["plotdata"]), data['accel_y'])
        self.lines_acc_az.set_data(range(data["plotdata"]), data['accel_z'])
        # Set to draw up to a number of draws
        self.accel_ax.set_xlim(0, data["plotdata"])
        # Dynamically switch the maximum value
        self.accel_ax.set_ylim(self.get_minvalue(data['accel_x'], data['accel_y'], data['accel_z']) - 200,
                                self.get_maxvalue(data['accel_x'], data['accel_y'], data['accel_z']) + 200
                                )
        # Need to fix the legend
        self.accel_ax.legend(loc = 'upper right')

        # synthetic acceleration plot line.
        self.lines_sa.set_data(range(data["plotdata"]), data["synthe_acc"])
        # Set to draw up to a number of draws
        self.synthetic_acc_ax.set_xlim(0, data["plotdata"])
        self.synthetic_acc_ax.set_ylim(min(data["synthe_acc"]) - 30, max(data["synthe_acc"]) + 30)
        # Need to fix the legend
        self.synthetic_acc_ax.legend(loc = 'upper right')

    def pause(self, second):
        plt.pause(second)
