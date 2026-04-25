"""
IGNIS Flight Simulator - Main GUI Window
"""

import sys
import numpy as np
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QPushButton, QSlider, QGroupBox,
    QFileDialog
)
from PyQt6.QtCore import Qt, QTimer
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from data.data_loader import IGNISDataLoader


class MplCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=3, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi, facecolor='#1a1a2e')
        self.axes = self.fig.add_subplot(111)
        self.axes.set_facecolor('#16213e')
        self.axes.tick_params(colors='#e0e0e0', labelsize=8)
        self.axes.xaxis.label.set_color('#e0e0e0')
        self.axes.yaxis.label.set_color('#e0e0e0')
        self.axes.title.set_color('#DCBFE4')
        self.fig.tight_layout(pad=2.0)
        super().__init__(self.fig)
        self.setParent(parent)


class IGNISFlightSimulator(QMainWindow):
    def __init__(self):
        super().__init__()
        self.flight_data = None
        self.current_frame = 0
        self.is_playing = False
        self.play_timer = QTimer()
        self.play_timer.timeout.connect(self.next_frame)
        self.init_ui()
        
    def init_ui(self):
        self.setWindowTitle("IGNIS Flight Simulator v1.0")
        self.setGeometry(50, 50, 1400, 850)
        
        self.setStyleSheet("""
            QMainWindow { background-color: #0f0f1a; }
            QGroupBox { color: #DCBFE4; border: 1px solid #2a2a3e; border-radius: 5px; margin-top: 10px; font-weight: bold; }
            QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }
            QLabel { color: #e0e0e0; }
            QPushButton { background-color: #2a2a3e; color: #DCBFE4; border: 1px solid #DCBFE4; border-radius: 4px; padding: 5px 10px; }
            QPushButton:hover { background-color: #DCBFE4; color: #0f0f1a; }
            QSlider::groove:horizontal { height: 4px; background: #2a2a3e; }
            QSlider::handle:horizontal { background: #DCBFE4; width: 12px; border-radius: 6px; }
        """)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setSpacing(5)
        
        # Control bar
        bar = QHBoxLayout()
        for text in ["Load CSV", "Play", "Pause", "Reset"]:
            btn = QPushButton(text)
            btn.clicked.connect(lambda checked, t=text: self.on_button_click(t))
            bar.addWidget(btn)
        bar.addStretch()
        main_layout.addLayout(bar)
        
        # 3D View
        group = QGroupBox("3D TRAJECTORY VIEW")
        layout = QVBoxLayout(group)
        self.traj_canvas = MplCanvas(self, width=10, height=4)
        layout.addWidget(self.traj_canvas)
        main_layout.addWidget(group)
        
        # Graphs row 1
        row1 = QHBoxLayout()
        
        g1 = QGroupBox("ALTITUDE")
        l1 = QVBoxLayout(g1)
        self.alt_canvas = MplCanvas(self, width=3, height=2)
        l1.addWidget(self.alt_canvas)
        row1.addWidget(g1)
        
        g2 = QGroupBox("SPEED")
        l2 = QVBoxLayout(g2)
        self.speed_canvas = MplCanvas(self, width=3, height=2)
        l2.addWidget(self.speed_canvas)
        row1.addWidget(g2)
        
        g3 = QGroupBox("ATTITUDE")
        l3 = QVBoxLayout(g3)
        self.att_canvas = MplCanvas(self, width=3, height=2)
        l3.addWidget(self.att_canvas)
        row1.addWidget(g3)
        
        main_layout.addLayout(row1)
        
        # Graphs row 2
        row2 = QHBoxLayout()
        
        g4 = QGroupBox("ACCELERATION")
        l4 = QVBoxLayout(g4)
        self.acc_canvas = MplCanvas(self, width=3, height=2)
        l4.addWidget(self.acc_canvas)
        row2.addWidget(g4)
        
        g5 = QGroupBox("PRESSURE")
        l5 = QVBoxLayout(g5)
        self.press_canvas = MplCanvas(self, width=3, height=2)
        l5.addWidget(self.press_canvas)
        row2.addWidget(g5)
        
        g6 = QGroupBox("TEMPERATURE")
        l6 = QVBoxLayout(g6)
        self.temp_canvas = MplCanvas(self, width=3, height=2)
        l6.addWidget(self.temp_canvas)
        row2.addWidget(g6)
        
        main_layout.addLayout(row2)
        
        # Info panel
        info_layout = QHBoxLayout()
        
        gps_group = QGroupBox("GPS DATA")
        gps_layout = QVBoxLayout(gps_group)
        self.gps_label = QLabel("No data")
        gps_layout.addWidget(self.gps_label)
        info_layout.addWidget(gps_group)
        
        sum_group = QGroupBox("FLIGHT SUMMARY")
        sum_layout = QVBoxLayout(sum_group)
        self.summary_label = QLabel("No data")
        sum_layout.addWidget(self.summary_label)
        info_layout.addWidget(sum_group)
        
        main_layout.addLayout(info_layout)
        
        # Orientation bar
        orient_layout = QHBoxLayout()
        self.orient_label = QLabel("Pitch: 0° | Roll: 0° | Yaw: 0°")
        self.orient_label.setStyleSheet("font-weight: bold; color: #DCBFE4;")
        orient_layout.addWidget(self.orient_label)
        
        orient_layout.addWidget(QLabel("Time:"))
        self.time_slider = QSlider(Qt.Orientation.Horizontal)
        self.time_slider.valueChanged.connect(self.slider_changed)
        orient_layout.addWidget(self.time_slider)
        self.time_label = QLabel("0.0 s")
        orient_layout.addWidget(self.time_label)
        
        main_layout.addLayout(orient_layout)
        
        self.statusBar().showMessage("Ready - Load a CSV file")
        
    def load_csv(self):
        path, _ = QFileDialog.getOpenFileName(self, "Open CSV", "", "CSV (*.csv)")
        if path:
            loader = IGNISDataLoader(path)
            self.flight_data = loader.load()
            self.time_slider.setMaximum(len(self.flight_data.time) - 1)
            self.plot_all()
            self.statusBar().showMessage(f"Loaded: {path}")
            
    def plot_all(self):
        if not self.flight_data:
            return
        d = self.flight_data
        
        # Altitude
        self.alt_canvas.axes.clear()
        self.alt_canvas.axes.plot(d.time, d.altitude_baro, 'b-')
        self.alt_canvas.axes.set_xlabel("Time (s)")
        self.alt_canvas.axes.set_ylabel("Altitude (m)")
        self.alt_canvas.axes.grid(True, alpha=0.3)
        self.alt_canvas.draw()
        
        # Speed
        speed_ms = d.speed / 3.6
        self.speed_canvas.axes.clear()
        self.speed_canvas.axes.plot(d.time, speed_ms, 'g-')
        self.speed_canvas.axes.set_xlabel("Time (s)")
        self.speed_canvas.axes.set_ylabel("Speed (m/s)")
        self.speed_canvas.axes.grid(True, alpha=0.3)
        self.speed_canvas.draw()
        
        # Attitude
        self.att_canvas.axes.clear()
        self.att_canvas.axes.plot(d.time, d.pitch, 'r-', label='Pitch')
        self.att_canvas.axes.plot(d.time, d.roll, 'g-', label='Roll')
        self.att_canvas.axes.plot(d.time, d.yaw, 'b-', label='Yaw')
        self.att_canvas.axes.legend(fontsize=8)
        self.att_canvas.axes.set_xlabel("Time (s)")
        self.att_canvas.axes.set_ylabel("Angle (deg)")
        self.att_canvas.axes.grid(True, alpha=0.3)
        self.att_canvas.draw()
        
        # Acceleration
        if d.g_force is not None:
            self.acc_canvas.axes.clear()
            self.acc_canvas.axes.plot(d.time, d.g_force, 'm-')
            self.acc_canvas.axes.set_xlabel("Time (s)")
            self.acc_canvas.axes.set_ylabel("G-Force")
            self.acc_canvas.axes.grid(True, alpha=0.3)
            self.acc_canvas.draw()
        
        # Pressure
        self.press_canvas.axes.clear()
        self.press_canvas.axes.plot(d.time, d.pressure, 'orange')
        self.press_canvas.axes.set_xlabel("Time (s)")
        self.press_canvas.axes.set_ylabel("Pressure (hPa)")
        self.press_canvas.axes.grid(True, alpha=0.3)
        self.press_canvas.draw()
        
        # Temperature
        self.temp_canvas.axes.clear()
        self.temp_canvas.axes.plot(d.time, d.temperature, 'r-')
        self.temp_canvas.axes.set_xlabel("Time (s)")
        self.temp_canvas.axes.set_ylabel("Temp (°C)")
        self.temp_canvas.axes.grid(True, alpha=0.3)
        self.temp_canvas.draw()
        
        # 3D Trajectory
        lat0, lon0 = d.latitude[0], d.longitude[0]
        east = (d.longitude - lon0) * 111320 * np.cos(np.radians(lat0))
        north = (d.latitude - lat0) * 110574
        
        self.traj_canvas.axes.clear()
        sc = self.traj_canvas.axes.scatter(east, north, c=d.altitude_baro, cmap='viridis', s=10)
        self.traj_canvas.axes.plot(east[0], north[0], 'go', markersize=10, label='Launch')
        if d.apogee_idx:
            self.traj_canvas.axes.plot(east[d.apogee_idx], north[d.apogee_idx], 'ro', markersize=10, label='Apogee')
        self.traj_canvas.axes.set_xlabel("East (m)")
        self.traj_canvas.axes.set_ylabel("North (m)")
        self.traj_canvas.axes.set_title("Flight Trajectory")
        self.traj_canvas.axes.grid(True, alpha=0.3)
        self.traj_canvas.draw()
        
        # GPS Panel
        idx = -1
        self.gps_label.setText(f"Lat: {d.latitude[idx]:.6f}\nLon: {d.longitude[idx]:.6f}\nAlt: {d.altitude_gps[idx]:.1f}m\nSpeed: {d.speed[idx]:.1f} km/h\nSats: {int(d.satellites[idx])}")
        
        # Summary
        max_alt = np.max(d.altitude_baro)
        max_sp = np.max(d.speed) / 3.6
        max_gf = np.max(d.g_force) if d.g_force is not None else 0
        apogee_t = d.time[d.apogee_idx] if d.apogee_idx else 0
        self.summary_label.setText(f"Max Alt: {max_alt:.0f}m\nMax Speed: {max_sp:.0f}m/s\nMax G: {max_gf:.1f}\nApogee: {apogee_t:.1f}s")
        
    def slider_changed(self, val):
        self.current_frame = val
        if self.flight_data:
            self.time_label.setText(f"{self.flight_data.time[val]:.1f}s")
            d = self.flight_data
            self.orient_label.setText(f"Pitch: {d.pitch[val]:.1f}° | Roll: {d.roll[val]:.1f}° | Yaw: {d.yaw[val]:.1f}°")
            
    def next_frame(self):
        if self.flight_data and self.current_frame < len(self.flight_data.time) - 1:
            self.current_frame += 1
            self.time_slider.setValue(self.current_frame)
        else:
            self.is_playing = False
            self.play_timer.stop()
            
    def on_button_click(self, text):
        if text == "Load CSV":
            self.load_csv()
        elif text == "Play":
            if self.flight_data:
                self.is_playing = True
                self.play_timer.start(50)
        elif text == "Pause":
            self.is_playing = False
            self.play_timer.stop()
        elif text == "Reset":
            self.current_frame = 0
            self.time_slider.setValue(0)
            self.orient_label.setText("Pitch: 0° | Roll: 0° | Yaw: 0°")


def main():
    app = QApplication(sys.argv)
    win = IGNISFlightSimulator()
    win.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()