# This python server create for testing purpose


import random
from flask import Flask, jsonify

app = Flask(__name__)

def generate_sensor_data():
    """Generate mock sensor data."""
    soil_moisture = random.randint(100, 1000)
    
    # Water pump status based on soil moisture level
    if soil_moisture > 750:
        water_pump_status = "ON"
    elif soil_moisture < 500:
        water_pump_status = "OFF"
    else:
        water_pump_status = "ON"
    
    return {
        "temperature": round(random.uniform(20, 35), 1),
        "humidity": random.randint(40, 80),
        "soilMoisture": soil_moisture,
        "waterPumpStatus": water_pump_status
    }

@app.route('/sensor-data', methods=['GET'])
def get_sensor_data():
    """Endpoint to get sensor data."""
    return jsonify(generate_sensor_data())  # Send generated data as JSON

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # Flask server running on port 5000
