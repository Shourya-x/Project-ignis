"""
IGNIS-NET-SIM - Data Loader
Parses CSV logs from IGNIS-CORE flight computer
"""

import pandas as pd
import numpy as np
from dataclasses import dataclass
from typing import Optional


@dataclass
class FlightData:
    """Container for all flight data"""
    time: np.ndarray
    time_ms: np.ndarray
    pitch: np.ndarray
    roll: np.ndarray
    yaw: np.ndarray
    pressure: np.ndarray
    altitude_baro: np.ndarray
    temperature: np.ndarray
    latitude: np.ndarray
    longitude: np.ndarray
    altitude_gps: np.ndarray
    speed: np.ndarray
    satellites: np.ndarray
    
    velocity_n: Optional[np.ndarray] = None
    velocity_e: Optional[np.ndarray] = None
    velocity_u: Optional[np.ndarray] = None
    acceleration: Optional[np.ndarray] = None
    g_force: Optional[np.ndarray] = None
    
    apogee_idx: int = 0
    max_speed_idx: int = 0
    max_g_idx: int = 0
    launch_idx: int = 0
    landing_idx: int = 0


class IGNISDataLoader:
    """Load and parse IGNIS-CORE CSV logs"""
    
    def __init__(self, filepath: str):
        self.filepath = filepath
        self.raw_data = None
        self.flight_data = None
        
    def load(self) -> FlightData:
        """Load CSV and return FlightData object"""
        self.raw_data = pd.read_csv(self.filepath)
        
        # Convert time to seconds
        time_ms = self.raw_data['Time(ms)'].values
        time_s = time_ms / 1000.0
        
        # Create FlightData object
        data = FlightData(
            time=time_s,
            time_ms=time_ms,
            pitch=self.raw_data['Pitch'].values,
            roll=self.raw_data['Roll'].values,
            yaw=self.raw_data['Yaw'].values,
            pressure=self.raw_data['Pressure(hPa)'].values,
            altitude_baro=self.raw_data['Altitude(m)'].values,
            temperature=self.raw_data['Temp(C)'].values,
            latitude=self.raw_data['Lat'].values,
            longitude=self.raw_data['Lon'].values,
            altitude_gps=self.raw_data['GPSAlt(m)'].values,
            speed=self.raw_data['Speed(km/h)'].values,
            satellites=self.raw_data['Sats'].values
        )
        
        # Calculate derived values
        self._calculate_velocities(data)
        self._calculate_acceleration(data)
        self._detect_flight_events(data)
        
        self.flight_data = data
        return data
    
    def _calculate_velocities(self, data: FlightData):
        """Calculate velocity components"""
        dt = np.diff(data.time)
        dt = np.insert(dt, 0, dt[0] if len(dt) > 0 else 0.01)
        
        lat_rad = np.radians(data.latitude)
        lon_rad = np.radians(data.longitude)
        R = 6371000
        
        data.velocity_n = np.zeros_like(data.time)
        data.velocity_e = np.zeros_like(data.time)
        data.velocity_u = np.zeros_like(data.time)
        
        for i in range(1, len(data.time)):
            dlat = lat_rad[i] - lat_rad[i-1]
            data.velocity_n[i] = (dlat * R) / dt[i]
            dlon = lon_rad[i] - lon_rad[i-1]
            data.velocity_e[i] = (dlon * R * np.cos(lat_rad[i])) / dt[i]
            data.velocity_u[i] = (data.altitude_gps[i] - data.altitude_gps[i-1]) / dt[i]
    
    def _calculate_acceleration(self, data: FlightData):
        """Calculate acceleration and G-force"""
        dt = np.diff(data.time)
        dt = np.insert(dt, 0, dt[0] if len(dt) > 0 else 0.01)
        speed_ms = data.speed / 3.6
        
        data.acceleration = np.zeros_like(data.time)
        for i in range(1, len(data.time)):
            data.acceleration[i] = (speed_ms[i] - speed_ms[i-1]) / dt[i]
        
        data.g_force = data.acceleration / 9.81
        data.g_force = np.clip(data.g_force, 0, 20)
    
    def _detect_flight_events(self, data: FlightData):
        """Detect key flight events"""
        data.apogee_idx = np.argmax(data.altitude_baro)
        data.max_speed_idx = np.argmax(data.speed)
        if data.g_force is not None and len(data.g_force) > 0:
            data.max_g_idx = np.argmax(data.g_force)
        
        above_10m = np.where(data.altitude_baro > 10)[0]
        if len(above_10m) > 0:
            data.launch_idx = above_10m[0]
        
        speed_ms = data.speed / 3.6
        low_speed = np.where(speed_ms < 0.5)[0]
        if len(low_speed) > 0 and low_speed[-1] > data.apogee_idx:
            data.landing_idx = low_speed[-1]