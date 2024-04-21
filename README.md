# Earthquake Risk Mapping: Identifying Safe Havens
## COP3530-Project-3 

Terminal commands to run the web app:
- Install express with command - "npm install express"
- To start the server run command - "node server.js"
- Open the link - "http://localhost:####"

Features:

- Choose Algorithm: Allows the user to select from two different algorithms for populating the earthquake data map. Users can choose between an unordered map and an ordered map, each with their own runtime characteristics. 
- Earthquake Statistics: This feature categorizes states into a spectrum from most vulnerable to least vulnerable based on historical earthquake data. It specifically highlights the top states with the greatest and least seismic activity, excluding those with zero recorded earthquakes, providing a clear and practical guide for safety assessments.
- Interactive Google Maps Integration: Our interactive map feature enables users to explore seismic activity using satellite or terrain views. The map displays markers at precise locations using longitude and latitude data of recorded earthquakes, offering a visual representation of earthquake distribution. When users hover over a marker, a small information window appears above it, detailing key data such as state, magnitude, and year of occurrence.
- Year Range Selection: Users have the flexibility to filter earthquake data based on different time frames. This feature supports viewing all years in the range, a specific year, or a custom range of years. Filtering is crucial for tracking seismic activity trends over time and making informed predictions for future infrastructure and safety regulations.
- Marker legend: The application includes a legend for the markers, which are color-coded according to magnitude range. This visual aid enhances user understanding by allowing quick identification of the severity of seismic events based on color differentiation.
- Magnitude filter: This tool enables users to filter earthquake data by magnitude on the Richter scale, ranging from 2.0 to 7.9 in our instance. Users can choose to view earthquakes of all magnitudes or specify a custom range of magnitudes, offering a tailored analysis for those interested in exploring the frequency and distribution of earthquakes at specific intensities.
- Toggle Light/Dark Mode: This feature offers the option to switch between light and dark mode which may enhance user experience and accommodate diverse preferences.
- Reset Earthquake Data: When the user refreshes the page, the earthquake data, magnitude filters, and year ranges will be reset. The user will have to repopulate the map again by choosing their preferred algorithm. This feature helps ensure a seamless experience in case of crashes or long loading times.

Algorithms implemented:

- Unordered_map backed by a hash map constructed from scratch
- Map utilizing the integrated library

Distribution of Responsibility and Roles:

- Shayan Akhoondan & Jair Alcivar: Ensuring user-friendly and accessible interface of the application.
Integration of Google Maps API to enhance data visualization to make analysis results
comprehensible and visually appealing to users.

- Jair Alcivar: Integration of mapping API to visualize earthquake data on a geographical map,
which allows users to visualize earthquakes by location.

- Shayan Akhoondan: Backend management of earthquake dataset, ensuring data
integrity, relevance, and working on algorithm analysis to prepare data effectively.

References:

https://github.com/COP3530/P3-template

https://www.kaggle.com/datasets/alessandrolobello/the-ultimate-earthquake-dataset-from-1990-2023?resource=download

https://developers.google.com/maps/documentation/embed/get-api-key

https://mapsplatform.google.com/

https://bridgesuncc.github.io/assignments/data//4-GraphEQ/README.html

