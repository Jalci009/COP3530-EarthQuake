const { exec } = require('child_process');
const express = require('express');
const path = require('path');
const fs = require('fs'); // Node.js filesystem module for file operations
const app = express();

const port = 3000;

// Serve static files from the "public" directory
app.use(express.static(path.join(__dirname, 'public')));

// Define a route to handle incoming HTTP POST requests for executing algorithms
app.post('/execute', (req, res) => {
    const algorithm = req.query.algorithm;
    const executablePath = path.join(__dirname, `Earthquake${algorithm.charAt(0).toUpperCase()}${algorithm.slice(1)}.exe`);
    const command = `"${executablePath}"`;

    console.log(`Executing: ${command}`);

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing ${command}: ${error.message}`);
            console.error(`stderr: ${stderr}`);
            res.status(500).send(`Failed to execute ${command}`);
        } else {
            console.log(`${command} executed successfully`);
            const successMessage = `${algorithm.charAt(0).toUpperCase() + algorithm.slice(1)} Map Population Successful!`;

            // Process stdout to extract runtime (assuming it's printed as the last line)
            const lines = stdout.trim().split('\n');
            const filteredLines = lines.filter(line => !line.includes('Successfully wrote filtered earthquake data to earthquake_data.json'));
            const filteredOutput = filteredLines.join('\n').trim();
            const runtime = filteredOutput.split('\n').pop().trim();

            res.status(200).send({ message: successMessage, runtime });
        }
    });
});

// Define a route to handle HTTP POST requests for deleting earthquake_data.json
app.post('/deleteEarthquakeData', (req, res) => {
    const filePath = path.join(__dirname, 'public', 'earthquake_data.json');

    // Use fs.unlink to delete the file
    fs.unlink(filePath, (error) => {
        if (error) {
            console.error('Error deleting earthquake_data.json:', error);
            res.status(500).send('Failed to delete earthquake_data.json');
        } else {
            console.log('earthquake_data.json deleted');
            res.status(200).send('earthquake_data.json deleted');
        }
    });
});

// Start the server
app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});