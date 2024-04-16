function initMap() {
    // Create a new map instance centered on the United States
    var map = new google.maps.Map(document.getElementById('map'), {
        center: { lat: 39.8283, lng: -98.5795 },
        zoom: 4.8,
    });

    // Fetch earthquake data from JSON file using fetch API
    fetch('earthquake_data.json')
        .then(response => response.json())
        .then(data => {
            console.log('Retrieved earthquake data:', data);

            // Iterate over each earthquake data point and add a marker to the map
            data.forEach(earthquake => {
                console.log('Processing earthquake:', earthquake);

                var location = { lat: earthquake.latitude, lng: earthquake.longitude };

                // Create a smaller custom marker icon
                var customIcon = {
                    url: 'https://maps.google.com/mapfiles/ms/icons/blue-dot.png', // Default blue marker icon URL
                    scaledSize: new google.maps.Size(15, 15) // Adjust the size as needed (smaller)
                };

                // Create a marker for each earthquake location with the custom icon
                new google.maps.Marker({
                    position: location,
                    map: map,
                    title: earthquake.state + ' - Magnitude ' + earthquake.magnitude + ' - Year ' + earthquake.year,
                    icon: customIcon // Use the custom icon for the marker
                });
            });
        })
        .catch(error => {
            console.error('Error fetching earthquake data:', error);
        });
}