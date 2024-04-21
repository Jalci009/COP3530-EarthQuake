# Earthquake Risk Mapping: Indentifying Safe Havens
## COP3530-Project-3 

Terminal commands to run code:
- Install express with command - "npm install express"
- To start the server run command - "node server.js"
- Open the link - "http://localhost:####"

Features:

● Choose Algorithm: Allows the user to select different algorithms for populating the earthquake data map. Users can choose between an unordered map and a map, each with their own runtime characteristics. 
- Earthquake Statistics: This feature categorizes states into a spectrum from most vulnerable to least vulnerable based on historical earthquake data. It specifically highlights the top five states with the most and least seismic activity, excluding those with zero recorded earthquakes, providing a clear and practical guide for safety assessments.
- Interactive Google Maps Integration: An interactive map feature allows users to view seismic activities either via satellite or terrain maps. This tool places markers at specific locations of recorded earthquakes, offering users a visual understanding of earthquake distribution. When a user hovers over a marker, a small info window appears directly above the marker. This window provides information such as state, magnitude, and year.
- Year Range Selection: Users have the flexibility to filter earthquake data based on different time frames. This feature supports viewing all years, a specific year, or a custom range of years. Filtering is crucial for tracking seismic activity trends over time and making informed predictions for future building and safety regulations.
- Marker legend: The website includes a legend for markers, which are color-coded according to the earthquake's magnitude. This visual aid enhances user understanding by allowing quick identification of the severity of seismic events based on color differentiation.
- Magnitude filter: This tool allows users to filter earthquake data based on magnitude, selectable from a range between 2.0 to 7.9, all of the magnitudes, and only magnitudes between for a specific range, on the Richter scale. It provides a more tailored analysis for users interested in examining the frequency and distribution of earthquakes of specific intensities.
- Toggle Light/Dark Mode: This feature offers the option to switch between light and dark mode which may enhance user experience and accommodate diverse preferences.
- Reset Earthquake Data: When the user refreshes the page the earthquake data, magnitude filters, year ranges will be reset. The user will have to populate the map again by choosing their preferred algorithm.

Algorithms implement:

● Unordered_map backed by a hash map that was constructed from scratch
- Map utilizing the integrated library

Distribution of Responsibility and Roles:

● Shayan Akhoondan & Jair Alcivar: Ensuring user-friendly and accessible interface of the application.
Integrate various visual APIs to enhance data visualization to make analysis results
comprehensible and visually appealing to users.

● Jair Alcivar: Integrate mapping API to visualize earthquake data on a geographical map,
which will allow users to visualize earthquakes by location.

● Shayan Akhoondan: Backend management of earthquake dataset, ensuring data
integrity, relevance, and working on algorithm analysis to prepare data effectively.

References:

https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp](https://github.com/COP3530/P3-template?tab=readme-ov-file#markdown-and-readme)
https://www.kaggle.com/datasets/alessandrolobello/the-ultimate-earthquake-dataset-from-1990-2023?resource=download
https://console.cloud.google.com/google/maps-apis/api-list?project=ivory-sentry-416603
https://mapsplatform.google.com/
https://bridgesuncc.github.io/assignments/data//4-GraphEQ/README.html

