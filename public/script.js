document.addEventListener('DOMContentLoaded', () => 
{
    initMap();
});

document.addEventListener('DOMContentLoaded', () => 
{
    const generateButton = document.getElementById('generateButton');

    // Event listener for "Generate" button click
    generateButton.addEventListener('click', () => 
    {
        executeEarthquakeExecutable();
    });

    // Execute the earthquake executable based on selected algorithm
    function executeEarthquakeExecutable() 
    {
        const algorithm = document.querySelector('input[name="algorithm"]:checked').value;

        resetMagnitudeFilters();

        resetYearSlider();

        // Make a POST request to trigger the execution of the corresponding executable
        fetch(`/execute?algorithm=${algorithm}`, { method: 'POST' })
            .then(response => 
            {
                if (response.ok) 
                {
                    return response.json(); // Parse response as JSON
                } 
                else 
                {
                    throw new Error('Failed to execute algorithm');
                }
            })
            .then(data => 
                {
                const { message, runtime } = data;
                console.log(message);
                
                // Display success message along with runtime in alert
                alert(`${message}\n${runtime}`);

                // Update the runtime display under the corresponding algorithm option
                const runtimeDisplay = document.getElementById(`${algorithm}Runtime`);
                if (runtimeDisplay) 
                {
                    runtimeDisplay.textContent = runtime;
                }

                // Fetch earthquake data and initialize map after execution
                return fetchEarthquakeDataAndInitializeMap();
            })
            .catch(error => 
            {
                console.error('Error executing algorithm:', error);
                alert('Failed to execute algorithm');
            });
    }

    // Delete earthquake_data.json upon page reload
    window.addEventListener('beforeunload', () => 
    {
        // Make a POST request to delete earthquake_data.json
        fetch('/deleteEarthquakeData', { method: 'POST' })
            .then(response => {
                if (response.ok) 
                {
                    console.log('earthquake_data.json deleted');
                } else 
                {
                    console.error('Failed to delete earthquake_data.json');
                }
            })
            .catch(error => 
                {
                console.error('Error deleting earthquake_data.json:', error);
            });
    });
});


function resetMagnitudeFilters() 
{
    // Check all magnitude filter checkboxes
    const magnitudeCheckboxes = document.querySelectorAll('.magnitude-filter');
    magnitudeCheckboxes.forEach(checkbox => 
    {
        checkbox.checked = true;
    });
}

function resetYearSlider() 
{
    const defaultSliderValues = [1990, 2004]; // Default year range values

    // Get the yearSlider element reference
    const yearSlider = document.getElementById('yearSlider');

    // noUiSlider API to set the slider values to the default range
    yearSlider.noUiSlider.set(defaultSliderValues);

    // Update the year range display with the default values
    updateYearRangeDisplay(document.getElementById('yearRangeDisplay'), defaultSliderValues);
}

document.addEventListener('DOMContentLoaded', () => 
{
    const magnitudeCheckboxes = document.querySelectorAll('.magnitude-filter');
    const selectAllCheckbox = document.getElementById('selectAllCheckbox');

    // Event listener for "Select All" checkbox
    selectAllCheckbox.addEventListener('change', () => 
    {
        const isChecked = selectAllCheckbox.checked;

        // Update all magnitude filter checkboxes based on the state of "Select All" checkbox
        magnitudeCheckboxes.forEach((checkbox) => 
        {
            checkbox.checked = isChecked;
        });

        updateMagnitudeFilters();
    });

    const onlyButtons = document.querySelectorAll('.only-button');

    onlyButtons.forEach((button) => 
    {
        button.addEventListener('click', () => 
        {
            const associatedCheckbox = button.previousElementSibling;
            associatedCheckbox.checked = true; // Check the associated magnitude filter checkbox

            // Uncheck all other checkboxes except the clicked one
            magnitudeCheckboxes.forEach((checkbox) => 
            {
                if (checkbox !== associatedCheckbox) 
                {
                    checkbox.checked = false;
                }
            });

            // Update the magnitude filters after toggling checkboxes
            updateMagnitudeFilters();
        });
    });

    function updateMagnitudeFilters() 
    {
        console.log('Magnitude filters updated');
        debounceUpdateMapDisplay();
        updateEarthquakeRankingTables();
    }
});

let map;
let markers = [];
let sliderValues = [1990, 2004]; // Default year range
let infoWindow; // Single InfoWindow instance
let timeoutId; // Variable to store timeout ID for debouncing
let cachedEarthquakeData = null; // Global variable to store earthquake data

function initMap() 
{
    // Create a new map instance centered on the United States
    map = new google.maps.Map(document.getElementById('map'), 
    {
        center: { lat: 39.8283, lng: -98.5795 },
        zoom: 4.0,
    });

    // Initialize noUiSlider for the year range selection
    const yearSlider = document.getElementById('yearSlider');
    const yearRangeDisplay = document.getElementById('yearRangeDisplay');

    noUiSlider.create(yearSlider, 
        {
        start: sliderValues,
        connect: true,
        range: 
        {
            'min': 1990,
            'max': 2004
        },
        tooltips: false,
        pips: 
        {
            mode: 'values',
            values: [1990, 1995, 2000, 2004], // Specify positions of small ticks
            density: 7, // Adjust spacing between ticks (percentage)
            format: 
            {
                to: value => '' // Empty string to prevent displaying any labels
            }
        }
    });

    // Update the year range display with the initial default slider values
    updateYearRangeDisplay(yearRangeDisplay, sliderValues);

    // Update the year range display dynamically on slider slide (move) event
    yearSlider.noUiSlider.on('slide', function (values, handle) 
    {
        const [startYear, endYear] = values.map(value => parseInt(value));
        updateYearRangeDisplay(yearRangeDisplay, values); // Always update with selected range
    });

    // Update the map and earthquake markers after slider handle is released (change event)
    yearSlider.noUiSlider.on('change', function (values) 
    {
        sliderValues = values.map(value => parseInt(value));
        debounceUpdateMapDisplay();
        updateEarthquakeRankingTables(); // Update earthquake tables when slider changes
    });

    // Add event listeners to magnitude filter checkboxes with debouncing
    const magnitudeCheckboxes = document.querySelectorAll('.magnitude-filter');
    magnitudeCheckboxes.forEach(checkbox => 
        {
        checkbox.addEventListener('change', () => 
        {
            debounceUpdateMapDisplay();
            updateEarthquakeRankingTables(); // Update earthquake tables when filters change
        });
    });

    // Create a single InfoWindow instance
    infoWindow = new google.maps.InfoWindow();

    // Fetch earthquake data and initialize map
    fetchEarthquakeDataAndInitializeMap();
}


function fetchEarthquakeDataAndInitializeMap() 
{
    if (cachedEarthquakeData) 
    {
        // Use cached data for initialization
        displayEarthquakes(cachedEarthquakeData);
        updateEarthquakeCount(cachedEarthquakeData);
        updateEarthquakeRankingTables(cachedEarthquakeData);
    } 
    else 
    {
        // Fetch data from JSON file and cache it
        fetch('earthquake_data.json')
            .then(response => response.json())
            .then(data => 
            {
                cachedEarthquakeData = data;
                displayEarthquakes(data);
                updateEarthquakeCount(data);
                updateEarthquakeRankingTables(data);
            })
            .catch(error => 
            {
                console.error('Error fetching earthquake data:', error);
            });
    }
}

function debounceUpdateMapDisplay() 
{
    // Clear previous timeout to prevent rapid consecutive function calls
    clearTimeout(timeoutId);

    // Set a new timeout for debouncing
    timeoutId = setTimeout(() => 
    {
        updateMapDisplay();
    }, 300); // Adjust the delay (in milliseconds)
}

function updateMapDisplay() 
{
    const [startYear, endYear] = sliderValues;
    const selectedMagnitudes = getSelectedMagnitudes();
    
    // Filter cached data based on selected criteria
    const filteredData = cachedEarthquakeData.filter(earthquake => 
    {
        const yearInRange = earthquake.year >= startYear && earthquake.year <= endYear;
        const magnitudeInRange = selectedMagnitudes.some(([min, max]) => earthquake.magnitude >= min && earthquake.magnitude <= max);
        return yearInRange && magnitudeInRange;
    });

    // Clear existing markers from the map
    clearMarkers();

    // Display filtered earthquakes as markers on the map
    displayEarthquakes(filteredData);

    // Update earthquake count and statistics based on filtered data
    updateEarthquakeCount(filteredData);
    updateEarthquakeRankingTables(filteredData);
}

function displayEarthquakes(data) 
{
    // Display earthquake markers on the map
    data.forEach(earthquake => 
    {
        const location = { lat: earthquake.latitude, lng: earthquake.longitude };
        const marker = createMarker(location, earthquake);
        markers.push(marker);
    });
}

function createMarker(location, earthquake) 
{
    // Determine marker color based on earthquake magnitude
    let markerColor;
    if (earthquake.magnitude >= 2 && earthquake.magnitude < 3) 
    {
        markerColor = 'blue'; // Green for magnitude 2.0 - 2.9
    } else if (earthquake.magnitude >= 3 && earthquake.magnitude < 4) 
    {
        markerColor = 'purple'; // Yellow for magnitude 3.0 - 3.9
    } else if (earthquake.magnitude >= 4 && earthquake.magnitude < 5) 
    {
        markerColor = 'green'; // Orange for magnitude 4.0 - 4.9
    } else if (earthquake.magnitude >= 5 && earthquake.magnitude < 6) 
    {
        markerColor = 'yellow'; // Orange for magnitude 5.0 - 5.9
    } else if (earthquake.magnitude >= 6 && earthquake.magnitude < 7) 
    {
        markerColor = 'orange'; // Orange for magnitude 6.0 - 6.9
    } else 
    {
        markerColor = 'red'; // Red for magnitude 7.0 and above
    }

    // Create a custom marker icon based on the determined color
    const customIcon = 
    {
        url: `http://maps.google.com/mapfiles/ms/icons/${markerColor}-dot.png`,
        scaledSize: new google.maps.Size(15, 15)
    };

    // Create a marker for each earthquake
    const marker = new google.maps.Marker(
    {
        position: location,
        map: map,
        icon: customIcon
    });

    // Attach mouseover event listener to show InfoWindow with debounce
    marker.addListener('mouseover', () => 
    {
        debounceOpenInfoWindow(marker, earthquake);
    });

    // Attach mouseout event listener to close InfoWindow
    marker.addListener('mouseout', () => 
    {
        infoWindow.close();
    });

    return marker;
}

function debounceOpenInfoWindow(marker, earthquake) 
{
    // Close any previously opened InfoWindow
    infoWindow.close();

    // Set a timeout to open the InfoWindow after a short delay
    setTimeout(() => 
    {
        const infoContent = getInfoWindowContent(earthquake);
        infoWindow.setContent(infoContent);
        infoWindow.open(map, marker);
    }, 100); // Adjust the delay (in milliseconds)
}

function getInfoWindowContent(earthquake) 
{
    const { state, magnitude, year } = earthquake;
    return `
        <div>
            <strong>State:</strong> ${state}<br>
            <strong>Magnitude:</strong> ${magnitude}<br>
            <strong>Year:</strong> ${year}
        </div>
    `;
}

function clearMarkers() 
{
    // Clear all existing markers from the map
    markers.forEach(marker => 
    {
        marker.setMap(null);
    });

    // Reset the markers array
    markers = [];
}

function getSelectedMagnitudes() 
{
    // Retrieve the checked checkboxes and map their values to number ranges
    const checkboxes = document.querySelectorAll('.magnitude-filter:checked');
    return Array.from(checkboxes).map(checkbox => {
        const [min, max] = checkbox.value.split('_').map(Number);
        return [min, max];
    });
}

function updateEarthquakeCount(data) 
{
    // Update the earthquake count display
    document.getElementById('earthquakeCount').textContent = `Number of Earthquakes: ${data.length}`;
}

function updateEarthquakeRankingTables(data) 
{
    const stateCounts = calculateStateEarthquakeCounts(data);

    // Convert stateCounts into an array of objects for sorting
    const stateCountsArray = Object.entries(stateCounts).map(([state, count]) => ({ state, count }));

    // Sort states by earthquake count (descending order)
    stateCountsArray.sort((a, b) => b.count - a.count);

    // Extract top 5 safest states (least earthquakes)
    const topSafestStates = stateCountsArray.slice(0, 5);

    // Extract top 5 most dangerous states (most earthquakes)
    const topDangerousStates = stateCountsArray.slice(-5).reverse();

    // Generate and update tables for safest and most dangerous states
    const safestStatesTable = generateTableHTML(topSafestStates, 'Most Vulnerable States');
    const dangerousStatesTable = generateTableHTML(topDangerousStates, 'Least Vulnerable States');

    // Update HTML content of tables
    document.getElementById('safestStatesTable').innerHTML = safestStatesTable;
    document.getElementById('dangerousStatesTable').innerHTML = dangerousStatesTable;
}


function updateEarthquakeCount(data) 
{
    const earthquakeCount = data.length;
    document.getElementById('earthquakeCount').textContent = `Number of Earthquakes: ${earthquakeCount}`;
}

function calculateStateEarthquakeCounts(data) 
{
    const stateCounts = {};

    // Count earthquakes by state
    data.forEach(earthquake => 
    {
        const state = earthquake.state;
        if (stateCounts[state]) 
        {
            stateCounts[state]++;
        } 
        else 
        {
            stateCounts[state] = 1;
        }
    });

    return stateCounts;
}

function generateTableHTML(states, title) 
{
    let tableHTML = `<h3>${title}</h3>
                     <table>
                        <tr>
                            <th>State</th>
                            <th>Earthquake Count</th>
                        </tr>`;

    states.forEach((stateData, index) => 
    {
        tableHTML += `<tr>
                        <td>${stateData.state}</td>
                        <td>${stateData.count}</td>
                      </tr>`;
    });

    tableHTML += `</table>`;
    return tableHTML;
}

function updateYearRangeDisplay(displayElement, values) 
{
    const [startYear, endYear] = values.map(value => Math.round(parseInt(value)));

    if (startYear === endYear) 
    {
        // Display the single year if start year equals end year
        displayElement.textContent = `Selected Year: ${startYear}`;
    } 
    else 
    {
        // Display the year range if start year is different from end year
        displayElement.textContent = `Selected Range: ${startYear} - ${endYear}`;
    }
}