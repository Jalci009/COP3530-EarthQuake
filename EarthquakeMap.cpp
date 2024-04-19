#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <chrono>

// Earthquake structure
struct Earthquake {
    std::string state;
    double magnitude;
    std::string data_type;
    double longitude;
    double latitude;
    int year;
};

// Function to validate if a state is in the list of valid states
bool isValidState(const std::string& state, const std::vector<std::string>& validStatesList) {
    for (const auto& validState : validStatesList) {
        if (state == validState) {
            return true;
        }
    }
    return false;
}

// Function to read earthquake data from file and populate the map
void readEarthquakeData(std::ifstream& file, std::map<std::string, std::vector<Earthquake>>& earthquakeMap, const std::vector<std::string>& validStatesList) {
    std::string line;
    std::getline(file, line); // Skip header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;
        Earthquake eq;

        // Read state (first column)
        if (std::getline(ss, eq.state, ',')) {
            eq.state.erase(0, eq.state.find_first_not_of(" \t\n\r\f\v"));
            eq.state.erase(eq.state.find_last_not_of(" \t\n\r\f\v") + 1);
        }

        // Read magnitude (second column)
        if (std::getline(ss, field, ',')) {
            eq.magnitude = std::stod(field);
        }

        // Read data type (third column)
        if (std::getline(ss, eq.data_type, ',')) {
            eq.data_type.erase(0, eq.data_type.find_first_not_of(" \t\n\r\f\v"));
            eq.data_type.erase(eq.data_type.find_last_not_of(" \t\n\r\f\v") + 1);
        }

        // Read longitude (fourth column)
        if (std::getline(ss, field, ',')) {
            eq.longitude = std::stod(field);
        }

        // Read latitude (fifth column)
        if (std::getline(ss, field, ',')) {
            eq.latitude = std::stod(field);
        }

        // Read year (sixth column)
        if (std::getline(ss, field, ',')) {
            std::string just_year = field.substr(0, 4);
            eq.year = std::stoi(just_year);
        }

        // Check if earthquake data meets all criteria
        if (eq.magnitude >= 2.0 && eq.data_type == "earthquake") {
            if (eq.state == "Georgia" && eq.latitude > 40)
                continue;

            // Store earthquake in the map using state as key
            earthquakeMap[eq.state].push_back(eq);
        }
    }
}

// Function to write earthquake data to JSON file
void writeEarthquakeDataToJson(std::ofstream& jsonFile, const std::map<std::string, std::vector<Earthquake>>& earthquakeMap, const std::vector<std::string>& validStatesList) {
    bool firstState = true;

    jsonFile << "[\n";

    for (const auto& entry : earthquakeMap) {
        const std::string& state = entry.first;
        const std::vector<Earthquake>& earthquakes = entry.second;

        // Check if the state is within the valid states list
        if (!isValidState(state, validStatesList))
            continue;

        // If the state is Georgia and latitude is greater than 40, continue
        if (state == "Georgia") {
            bool isGeorgiaValid = false;
            for (const auto& eq : earthquakes) {
                if (eq.latitude > 40) {
                    isGeorgiaValid = true;
                    break;
                }
            }
            if (isGeorgiaValid) {
                continue;
            }
        }

        // Iterate over earthquakes in the vector and write to JSON
        for (const auto& eq : earthquakes) {
            if (!firstState) {
                jsonFile << ",\n";
            }
            jsonFile << "    {\n";
            jsonFile << "        \"state\": \"" << eq.state << "\",\n";
            jsonFile << "        \"magnitude\": " << eq.magnitude << ",\n";
            jsonFile << "        \"longitude\": " << eq.longitude << ",\n";
            jsonFile << "        \"latitude\": " << eq.latitude << ",\n";
            jsonFile << "        \"year\": " << eq.year << "\n";
            jsonFile << "    }";
            firstState = false;
        }
    }

    jsonFile << "\n]\n";
}

int main() {
    std::ifstream file("earthquake_data.csv");
    std::map<std::string, std::vector<Earthquake>> earthquakeMap;

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }


    // List of valid states within the United States
    static const std::vector<std::string> validStatesList = {
        "Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado", "Connecticut", "Delaware",
        "Florida", "Georgia", "Hawaii", "Idaho", "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky",
        "Louisiana", "Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota", "Mississippi",
        "Missouri", "Montana", "Nebraska", "Nevada", "New Hampshire", "New Jersey", "New Mexico",
        "New York", "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon", "Pennsylvania",
        "Rhode Island", "South Carolina", "South Dakota", "Tennessee", "Texas", "Utah", "Vermont",
        "Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming"
    };

    auto startTime = std::chrono::steady_clock::now();

    readEarthquakeData(file, earthquakeMap, validStatesList);


    std::ofstream jsonFile("earthquake_data.json");
    if (!jsonFile.is_open()) {
        std::cerr << "Error opening JSON file for writing!" << std::endl;
        return 1;
    }

    writeEarthquakeDataToJson(jsonFile, earthquakeMap, validStatesList);

    jsonFile.close();
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Time taken to populate the map: " << duration.count() << " milliseconds" << std::endl;

    std::cout << "Successfully wrote filtered earthquake data to earthquake_data.json" << std::endl;
    

    return 0;
}