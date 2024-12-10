import random
from flask import Flask, jsonify, request

app = Flask(__name__)

# Dummy in-memory storage for sensor data (replace with a database for production)
sensor_data = {
    "temperature": None,
    "humidity": None,
    "soilMoisture": None,
    "waterPumpStatus": "OFF"
}

@app.route('/')
def index():
    return "Flask server is running"

@app.route('/sensor-data', methods=['POST'])
def update_sensor_data():
    """Endpoint to receive sensor data from Arduino and process it."""
    if request.is_json:
        data = request.get_json()
        # Extract sensor data from the incoming request
        sensor_data["temperature"] = data.get("temperature")
        sensor_data["humidity"] = data.get("humidity")
        sensor_data["soilMoisture"] = data.get("soilMoisture")

        # Water pump status based on soil moisture
        if sensor_data["soilMoisture"] < 500:
            sensor_data["waterPumpStatus"] = "ON"  # Turn ON if soil moisture < 500
        else:
            sensor_data["waterPumpStatus"] = "OFF"  # Turn OFF if soil moisture >= 500

        return jsonify({"status": "success", "message": "Sensor data updated"}), 200
    else:
        return jsonify({"error": "Invalid JSON"}), 400

@app.route('/sensor-data', methods=['GET'])
def get_sensor_data():
    """Endpoint to get current sensor data."""
    return jsonify(sensor_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # Flask server running on port 5000
