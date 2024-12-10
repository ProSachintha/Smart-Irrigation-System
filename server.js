const express = require('express');
const bodyParser = require('body-parser');
const http = require('http');
const { Server } = require('socket.io');
const axios = require('axios');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// Database setup
const db = new sqlite3.Database(path.join(__dirname, 'sensor_data.db'));

// Create sensors table if not exists
db.run(`CREATE TABLE IF NOT EXISTS sensors (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    temperature REAL,
    humidity INTEGER,
    soilMoisture INTEGER,
    waterPumpStatus TEXT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
)`);

// Middleware
app.use(express.static('public'));
app.use(bodyParser.json());

// Variable to store latest sensor data
let sensorData = {};

// Fetch and store sensor data from Python server
const fetchSensorDataFromPython = async () => {
    try {
        const response = await axios.get('http://localhost:5000/sensor-data');
        if (response.data) {
            sensorData = response.data;
            
            // Store data in database
            db.run(`INSERT INTO sensors 
                (temperature, humidity, soilMoisture, waterPumpStatus) 
                VALUES (?, ?, ?, ?)`, 
                [
                    sensorData.temperature, 
                    sensorData.humidity, 
                    sensorData.soilMoisture, 
                    sensorData.waterPumpStatus
                ]
            );

            // Emit to connected clients
            io.emit('sensor-update', sensorData);
        }
    } catch (error) {
        console.error('Error fetching sensor data from Python server:', error);
    }
};

// Get latest sensor data
app.get('/api/sensors', (req, res) => {
    res.json(sensorData);
});

// Get historical sensor data
app.get('/api/sensor-history', (req, res) => {
    const limit = parseInt(req.query.limit) || 50; // Default to last 50 records

    db.all(`SELECT temperature, humidity, soilMoisture, waterPumpStatus, timestamp 
            FROM sensors 
            ORDER BY timestamp DESC 
            LIMIT ?`, [limit], (err, rows) => {
        if (err) {
            res.status(500).json({ error: err.message });
            return;
        }
        res.json(rows);
    });
});

// Real-time communication with clients
io.on('connection', (socket) => {
    console.log('New client connected');
    socket.emit('sensor-update', sensorData);

    socket.on('disconnect', () => {
        console.log('Client disconnected');
    });
});

// Start the server
const PORT = 3000;
server.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
    setInterval(fetchSensorDataFromPython, 5000); // Fetch data every 5 seconds
});