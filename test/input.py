#!/usr/bin/env python3
import sys
import serial
import struct
import threading
from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QLabel, QLineEdit, QPushButton, QTextEdit)
from PyQt6.QtCore import pyqtSignal, QObject

class SerialWorker(QObject):
    recv_signal = pyqtSignal(str)

    def __init__(self, device='/dev/ttyUSB0', baudrate=115200):
        super().__init__()
        try:
            self.ser = serial.Serial(device, baudrate, timeout=0.01)
            self.running = True
            self.recv_thread = threading.Thread(target=self._recv_loop, daemon=True)
            self.recv_thread.start()
        except serial.SerialException as e:
            self.recv_signal.emit(f'Failed to open: {e}')
            self.ser = None

    def _recv_loop(self):
        while self.running and self.ser:
            try:
                data = self.ser.readline()
                if data:
                    text = data.decode('utf-8', errors='ignore').strip()
                    if text:
                        self.recv_signal.emit(text)
            except:
                pass

    def send(self, f1: float, f2: float, f3: float):
        if not self.ser:
            return
        data = bytearray(16)
        data[0] = 0xAA
        data[1] = 0x55
        struct.pack_into('<f', data, 2, f1)
        struct.pack_into('<f', data, 6, f2)
        struct.pack_into('<f', data, 10, f3)
        data[14] = 0x0D
        data[15] = 0x0A
        self.ser.write(bytes(data))

    def close(self):
        self.running = False
        if self.ser:
            self.ser.close()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.worker = SerialWorker()
        self.worker.recv_signal.connect(self.on_message)
        self.init_ui()

    def init_ui(self):
        self.setWindowTitle('UART Tool')
        self.setGeometry(100, 100, 800, 500)

        central = QWidget()
        main_layout = QHBoxLayout()

        # Left: Send with log
        left_layout = QVBoxLayout()
        left_layout.addWidget(QLabel('Send'))
        left_layout.addWidget(QLabel('f1 (vx):'))
        self.f1_input = QLineEdit('0.0')
        left_layout.addWidget(self.f1_input)
        left_layout.addWidget(QLabel('f2 (vy):'))
        self.f2_input = QLineEdit('0.0')
        left_layout.addWidget(self.f2_input)
        left_layout.addWidget(QLabel('f3 (omega):'))
        self.f3_input = QLineEdit('0.0')
        left_layout.addWidget(self.f3_input)
        self.send_btn = QPushButton('Send')
        self.send_btn.clicked.connect(self.on_send)
        left_layout.addWidget(self.send_btn)

        self.send_output = QTextEdit()
        self.send_output.setReadOnly(True)
        left_layout.addWidget(self.send_output)

        # Right: Receive log
        right_layout = QVBoxLayout()
        right_layout.addWidget(QLabel('Receive'))
        self.recv_output = QTextEdit()
        self.recv_output.setReadOnly(True)
        right_layout.addWidget(self.recv_output)

        main_layout.addLayout(left_layout, 1)
        main_layout.addLayout(right_layout, 1)

        central.setLayout(main_layout)
        self.setCentralWidget(central)

    def on_send(self):
        try:
            f1 = float(self.f1_input.text())
            f2 = float(self.f2_input.text())
            f3 = float(self.f3_input.text())

            # Build hex string for display
            data = bytearray(16)
            data[0] = 0xAA
            data[1] = 0x55
            struct.pack_into('<f', data, 2, f1)
            struct.pack_into('<f', data, 6, f2)
            struct.pack_into('<f', data, 10, f3)
            data[14] = 0x0D
            data[15] = 0x0A
            hex_str = ' '.join(f'{b:02x}' for b in data)
            self.send_output.append(f'[SENT] f1={f1}, f2={f2}, f3={f3}\n{hex_str}')

            self.worker.send(f1, f2, f3)
        except ValueError:
            self.send_output.append('[ERROR] Invalid input')

    def on_message(self, msg):
        self.recv_output.append(msg)

    def closeEvent(self, event):
        self.worker.close()
        event.accept()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
