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

    noUiSlider.create(yearSlider, {
        start: sliderValues,
        connect: true,
        range: {
            'min': 1990,
            'max': 2004
        }
    });

    // Update the year range display and earthquake markers on slider change (debounced)
    yearSlider.noUiSlider.on('change', function (values) {
        const [startYear, endYear] = values.map(value => parseInt(value));

        // Update sliderValues to reflect the current year range
        sliderValues = [startYear, endYear];

        // Update the year range display
        yearRangeDisplay.textContent = `${startYear} - ${endYear}`;

        // Debounce the earthquake data fetching and marker display
        clearTimeout(updateTimeout);
        updateTimeout = setTimeout(() => {
            displayEarthquakesByYearRange(startYear, endYear);
        }, 100); // Delay in milliseconds (adjust as needed)
    });

    // Initial display of earthquake markers for the default year range (1990 to 2004)
    displayEarthquakesByYearRange(sliderValues[0], sliderValues[1]);
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

// Function to clear all markers from the map
function clearMarkers() {
    markers.forEach(marker => {
        marker.setMap(null);
    });
    markers = []; // Clear the markers array
}
