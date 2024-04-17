let map;
let markers = [];
let updateTimeout;
let sliderValues = [1990, 2004]; // Default year range

function initMap() {
    // Create a new map instance centered on the United States
    map = new google.maps.Map(document.getElementById('map'), {
        center: { lat: 39.8283, lng: -98.5795 },
        zoom: 4.8,
    });

    // Initialize noUiSlider for the year range selection
    const yearSlider = document.getElementById('yearSlider');
    const yearRangeDisplay = document.getElementById('yearRangeDisplay');
    const stateInput = document.getElementById('stateInput'); // Input field for state name
    const stateSearchBtn = document.getElementById('stateSearchBtn'); // Button to trigger state filtering

    noUiSlider.create(yearSlider, {
        start: sliderValues,
        connect: true,
        range: {
            'min': 1990,
            'max': 2004
        },
        tooltips: true,
        format: {
            to: value => parseInt(value),
            from: value => parseInt(value)
        }
    });

    // Set initial text content for the year range display
    yearRangeDisplay.textContent = `Selected Range: ${sliderValues[0]} - ${sliderValues[1]}`;

    // Update the year range display in real-time on slider slide (move) event
    yearSlider.noUiSlider.on('slide', function (values, handle) {
        yearRangeDisplay.textContent = `Selected Range: ${parseInt(values[0])} - ${parseInt(values[1])}`;
    });

    // Keep the existing 'change' event to handle the final update
    yearSlider.noUiSlider.on('change', function (values, handle) {
        sliderValues = values.map(value => parseInt(value));
        updateMapDisplay(); // This function should also apply state filtering
    });

    // Initialize the magnitude filters
    initMagnitudeFilters();

    // Add event listener to the state input field and search button
    stateSearchBtn.addEventListener('click', function() {
        updateMapDisplay(); // Apply the state filter along with others
    });

    stateInput.addEventListener('keypress', function(event) {
        if (event.key === "Enter") {
            updateMapDisplay(); // Apply the state filter along with others
        }
    });

    // Initial display of earthquakes with the default filters
    updateMapDisplay();
}


function updateMapDisplay() {
    // Get the currently selected year range from sliderValues
    const [startYear, endYear] = sliderValues;
    
    // Get the selected magnitude ranges from the checkboxes
    const selectedMagnitudes = getSelectedMagnitudes();

    // Get the entered state from the state input field
    const stateInput = document.getElementById('stateInput').value.trim().toLowerCase();

    // Clear any existing markers from the map
    clearMarkers();

    // Fetch earthquake data from JSON file
    fetch('earthquake_data.json')
        .then(response => response.json())
        .then(data => {
            // Filter the data according to the selected year range, magnitudes, and state
            const filteredData = data.filter(earthquake => {
                const yearInRange = earthquake.year >= startYear && earthquake.year <= endYear;
                const magnitudeInRange = selectedMagnitudes.some(([min, max]) => earthquake.magnitude >= min && earthquake.magnitude <= max);
                // State filter checks if the state input is empty or matches the earthquake's state
                const stateMatches = !stateInput || earthquake.state.trim().toLowerCase() === stateInput;
                return yearInRange && magnitudeInRange && stateMatches;
            });

            // Update the earthquake count display
            document.getElementById('earthquakeCount').textContent = `Number of Earthquakes: ${filteredData.length}`;

            // Add markers for the filtered earthquake data
            addMarkers(filteredData);
        })
        .catch(error => {
            console.error('Error fetching earthquake data:', error);
        });
}


function getSelectedMagnitudes() {
    // Retrieve the checked checkboxes and map their values to number ranges
    const checkboxes = document.querySelectorAll('.magnitude-filter:checked');
    return Array.from(checkboxes).map(checkbox => {
        const [min, max] = checkbox.value.split('_').map(Number);
        return [min, max];
    });
}

function applyFilters(data) {
    // Log the original data count
    console.log(`Original data count: ${data.length}`);

    const selectedCheckboxes = Array.from(document.querySelectorAll('.magnitude-filter:checked'));
    // If the "All Magnitudes" checkbox is checked, we want to ignore other magnitude filters
    const isAllMagnitudesChecked = selectedCheckboxes.some(cb => cb.value === '3_9.9');
    
    // Get the entered state from the state input field
    const stateInput = document.getElementById('stateInput').value.trim().toLowerCase();

    let selectedMagnitudes;
    if (isAllMagnitudesChecked) {
        selectedMagnitudes = [[2.0, 9.9]]; // This range should cover all possible magnitudes
    } else {
        selectedMagnitudes = selectedCheckboxes.map(cb => cb.value.split('_').map(Number));
    }
    
    const filteredData = data.filter(earthquake => {
        const yearInRange = earthquake.year >= sliderValues[0] && earthquake.year <= sliderValues[1];
        const magnitudeInRange = isAllMagnitudesChecked || selectedMagnitudes.some(([min, max]) => earthquake.magnitude >= min && earthquake.magnitude <= max);
        // State filter checks if the state input is empty or matches the earthquake's state
        const stateMatches = !stateInput || earthquake.state.trim().toLowerCase() === stateInput;
        return yearInRange && magnitudeInRange && stateMatches;
    });

    // Log the filtered data count
    console.log(`Filtered data count: ${filteredData.length}`);
    return filteredData;
}


// Function to display earthquake markers based on the selected year range
function displayEarthquakesByYearRange(startYear, endYear) {
    // Clear existing markers from the map
    clearMarkers();

    // Fetch earthquake data from JSON file
    fetch('earthquake_data.json')
        .then(response => response.json())
        .then(data => {
            console.log('Retrieved earthquake data:', data);

            // Filter earthquake data for the selected year range
            const filteredData = data.filter(earthquake => earthquake.year >= startYear && earthquake.year <= endYear);

            // Add markers for the filtered earthquake data
            filteredData.forEach(earthquake => {
                const location = { lat: earthquake.latitude, lng: earthquake.longitude };

                // Create a smaller custom marker icon
                const customIcon = {
                    url: 'https://maps.google.com/mapfiles/ms/icons/blue-dot.png',
                    scaledSize: new google.maps.Size(15, 15)
                };

                // Create a marker title with state, magnitude, and year
                const markerTitle = `${earthquake.state} - Magnitude ${earthquake.magnitude} - Year ${earthquake.year}`;

                // Create a marker for each earthquake location with the custom icon and title
                const marker = new google.maps.Marker({
                    position: location,
                    map: map,
                    title: markerTitle, // Use the custom marker title
                    icon: customIcon
                });

                // Push marker to the markers array
                markers.push(marker);
            });
        })
        .catch(error => {
            console.error('Error fetching earthquake data:', error);
        });
}

// This function will now also consider the magnitude filter when displaying markers
function displayEarthquakesByYearRangeAndMagnitude(startYear, endYear, magnitudeFilter) {
    // Clear existing markers from the map
    clearMarkers();

    // Fetch earthquake data from JSON file
    fetch('earthquake_data.json')
        .then(response => response.json())
        .then(data => {
            // Filter earthquake data for the selected year range and magnitude
            const filteredData = data.filter(earthquake => {
                const yearInRange = earthquake.year >= startYear && earthquake.year <= endYear;
                const magnitudeInRange = magnitudeFilter ? earthquake.magnitude >= magnitudeFilter.min && earthquake.magnitude <= magnitudeFilter.max : true;
                return yearInRange && magnitudeInRange;
            });

            // Add markers for the filtered earthquake data
            addMarkers(filteredData);
        })
        .catch(error => {
            console.error('Error fetching earthquake data:', error);
        });
}

// Separate function for adding markers to the map
function addMarkers(filteredData) {
    filteredData.forEach(earthquake => {
        const location = { lat: earthquake.latitude, lng: earthquake.longitude };
        const customIcon = {
            url: 'https://maps.google.com/mapfiles/ms/icons/blue-dot.png',
            scaledSize: new google.maps.Size(15, 15)
        };
        const markerTitle = `${earthquake.state} - Magnitude ${earthquake.magnitude} - Year ${earthquake.year}`;
        const marker = new google.maps.Marker({
            position: location,
            map: map,
            title: markerTitle,
            icon: customIcon
        });
        markers.push(marker);
    });
}

// Initialize magnitude filter checkboxes
function initMagnitudeFilters() {
    const magnitudeCheckboxes = document.querySelectorAll('.magnitude-filter');
    magnitudeCheckboxes.forEach(checkbox => {
        checkbox.addEventListener('change', updateMapDisplay);
    });
}

// Function to clear all markers from the map
function clearMarkers() {
    markers.forEach(marker => {
        marker.setMap(null);
    });
    markers = []; // Clear the markers array
}
