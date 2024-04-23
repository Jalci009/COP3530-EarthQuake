#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <chrono>

using namespace std;

struct Earthquake 
{
    string state;
    double magnitude;
    string data_type;
    double longitude;
    double latitude;
    int year;
};

// Validate if the state is valid
bool isValidState(const string& state, const vector<string>& validStatesList) 
{
    for (const auto& validState : validStatesList) 
    {
        if (state == validState) 
        {
            return true;
        }
    }
    return false;
}

// Read earthquake data from file and populate the map
void readEarthquakeData(ifstream& file, map<string, vector<Earthquake>>& earthquakeMap, const vector<string>& validStatesList) 
{
    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)) 
    {
        stringstream ss(line);
        string field;
        Earthquake eq;

        // Read state (first column)
        if (getline(ss, eq.state, ',')) 
        {
            eq.state.erase(0, eq.state.find_first_not_of(" \t\n\r\f\v"));
            eq.state.erase(eq.state.find_last_not_of(" \t\n\r\f\v") + 1);
        }

        // Read magnitude (second column)
        if (getline(ss, field, ',')) 
        {
            eq.magnitude = stod(field);
        }

        // Read data type (third column)
        if (getline(ss, eq.data_type, ',')) 
        {
            eq.data_type.erase(0, eq.data_type.find_first_not_of(" \t\n\r\f\v"));
            eq.data_type.erase(eq.data_type.find_last_not_of(" \t\n\r\f\v") + 1);
        }

        // Read longitude (fourth column)
        if (getline(ss, field, ',')) 
        {
            eq.longitude = stod(field);
        }

        // Read latitude (fifth column)
        if (getline(ss, field, ',')) 
        {
            eq.latitude = stod(field);
        }

        // Read year (sixth column)
        if (getline(ss, field, ',')) 
        {
            string just_year = field.substr(0, 4);
            eq.year = stoi(just_year);
        }

        // Check if earthquake data meets all criteria
        if (eq.magnitude >= 2.0 && eq.data_type == "earthquake") 
        {
            if (eq.state == "Georgia" && eq.latitude > 40)
                continue;

            // Store earthquake in the map using state as key
            earthquakeMap[eq.state].push_back(eq);
        }
    }
}

// Function to write earthquake data to JSON file
void writeEarthquakeDataToJson(ofstream& jsonFile, const map<string, vector<Earthquake>>& earthquakeMap, const vector<string>& validStatesList) 
{
    bool firstState = true;

    jsonFile << "[\n";

    for (const auto& entry : earthquakeMap) 
    {
        const string& state = entry.first;
        const vector<Earthquake>& earthquakes = entry.second;

        // Check if the state is within the valid states list
        if (!isValidState(state, validStatesList))
            continue;

        // Iterate over earthquakes in the vector and write to JSON
        for (const auto& eq : earthquakes) 
        {
            if (!firstState) 
            {
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

int main() 
{
    ifstream file("earthquake_data.csv");
    map<string, vector<Earthquake>> earthquakeMap;

    if (!file.is_open()) 
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }


    // List of valid states within the United States
    static const vector<string> validStatesList = 
    {
        "Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado", "Connecticut", "Delaware",
        "Florida", "Georgia", "Hawaii", "Idaho", "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky",
        "Louisiana", "Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota", "Mississippi",
        "Missouri", "Montana", "Nebraska", "Nevada", "New Hampshire", "New Jersey", "New Mexico",
        "New York", "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon", "Pennsylvania",
        "Rhode Island", "South Carolina", "South Dakota", "Tennessee", "Texas", "Utah", "Vermont",
        "Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming"
    };

    auto startTime = chrono::steady_clock::now();

    readEarthquakeData(file, earthquakeMap, validStatesList);


    ofstream jsonFile("public/earthquake_data.json");
    if (!jsonFile.is_open()) 
    {
        cerr << "Error opening JSON file for writing!" << endl;
        return 1;
    }

    writeEarthquakeDataToJson(jsonFile, earthquakeMap, validStatesList);

    jsonFile.close();
    
    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    cout << "Runtime: " << duration.count() << " ms" << endl;
    cout << "Successfully wrote filtered earthquake data to earthquake_data.json" << endl;
    
    return 0;
}