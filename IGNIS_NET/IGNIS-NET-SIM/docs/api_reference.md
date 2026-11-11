# IGNIS-NET-SIM — Python API Reference

## 1. `data.data_loader.FlightDataLoader`
- **`FlightDataLoader(filepath: str)`**: Loads and validates a 17-column IGNIS CSV telemetry file into a `pandas.DataFrame`.
- **`get_summary() -> dict`**: Returns mission summary metrics (`max_altitude`, `max_velocity`, `flight_duration`, `num_samples`).

## 2. `utils.geo_utils`
- **`haversine_distance(lat1, lon1, lat2, lon2) -> float`**: Computes great-circle ground range in meters between pad and rocket coordinates.
- **`calculate_bearing(lat1, lon1, lat2, lon2) -> float`**: Computes azimuth bearing ($0^\circ\text{–}360^\circ$) from launch pad to recovery site.

## 3. `utils.kml_exporter.KMLExporter`
- **`export_flight_to_kml(df: DataFrame, output_path: str)`**: Generates a 3D Google Earth `.kml` file with altitude-extruded trajectory coordinates and key flight event placemarks.
