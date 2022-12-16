from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QPushButton, QWidget
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import sys  # We need sys so that we can pass argv to QApplication
import os
from random import randint
import serial
import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial("COM5", 115200)
ser.close()
ser.open()

class MainWindow(QtWidgets.QMainWindow):

    def keyReleaseEvent(self, event):
        if event.key() == QtCore.Qt.Key.Key_Q:
            # apasam q ca sa schimbam senzorul
            ser.write(b'q')
            self.is_key_ctrl_pressed = False

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        # initializarea graficului
        self.graphWidget = pg.PlotWidget()
        self.setCentralWidget(self.graphWidget)
        self.graphWidget.setBackground((245, 245, 245))
        self.graphWidget.setTitle("\nLight + Vibration Sensor\n", color="black", size="25px")

        styles = {'color': 'black', 'font-size': '12px'}
        self.graphWidget.setLabel('left', 'Lumina (Lux)', **styles)
        self.graphWidget.setLabel('bottom', 'Time (ms)', **styles)
        self.graphWidget.showGrid(x=True, y=True)
        self.graphWidget.setYRange(0, 6000, padding=0)


        # intervalul de timp pe care il salveaza graficul
        self.x = list(range(50))
        self.y = list(range(50))

        # culoarea cu care deseneaza graficul
        pen = pg.mkPen(color=(0, 255, 255), width=5)
        self.data_line =  self.graphWidget.plot(self.x, self.y, pen=pen)


        # timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(1)
        self.timer.timeout.connect(self.update_plot_data)
        self.timer.start()

    def update_plot_data(self):
        data = ser.readline().decode()

        # parsam datele primite de la placuta si le inseram in continuarea graficului
        arduinoData_string2 = data.split("\x01")[-1]
        val = arduinoData_string2.split('\n')[0]
        if val == '':
            floatdata = 0
        else:
            floatdata = float(val)

        self.x = self.x[1:]  # Remove the first y element.
        self.x.append(self.x[-1] + 1)  # Add a new value 1 higher than the last.

        self.y = self.y[1:]  # Remove the first

        self.y.append(floatdata)  # Add a new random value.
        self.data_line.setData(self.x, self.y)  # Update the data.


def main():
    app = QtWidgets.QApplication(sys.argv)

    LightWindow = MainWindow()
    LightWindow.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()