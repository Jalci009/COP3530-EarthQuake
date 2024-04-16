#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <list>

// Custom hash function for strings
size_t customHash(const std::string& key, size_t tableSize) {
    size_t hash = 0;
    for (char ch : key) {
        hash = (hash * 31 + ch) % tableSize; // Simple hash function using prime number 31
    }
    return hash;
}

template<typename T>
class HashNode {
public:
    T value;
    HashNode<T>* next;

    HashNode(const T& val) : value(val), next(nullptr) {}
};

template<typename T>
class HashSet {
private:
    std::vector<std::list<HashNode<T>*>> table;
    size_t size;

public:
    HashSet(size_t initialSize = 100) : table(initialSize), size(0) {}

    ~HashSet() {
        clear();
    }

    bool insert(const T& value) {
        size_t index = customHash(value, table.size());
        for (auto& node : table[index]) {
            if (node->value == value) {
                return false; // Value already exists
            }
        }

        HashNode<T>* newNode = new HashNode<T>(value);
        table[index].push_back(newNode);
        ++size;
        return true;
    }

    bool contains(const T& value) const {
        size_t index = customHash(value, table.size());
        for (auto& node : table[index]) {
            if (node->value == value) {
                return true;
            }
        }
        return false;
    }

    void clear() {
        for (auto& bucket : table) {
            for (auto& node : bucket) {
                delete node;
            }
            bucket.clear();
        }
        size = 0;
    }

    size_t getSize() const {
        return size;
    }
};

struct Earthquake {
    std::string state;
    double magnitude;
    std::string data_type;
    double longitude;
    double latitude;
    int year;
};

bool isState(const std::string& name, const HashSet<std::string>& validStates) {
    return validStates.contains(name);
}

int main() {
    std::ifstream file("earthquake_data.csv");
    std::vector<Earthquake> earthquakes;

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    std::string line;
    std::getline(file, line); // Skip header line

    HashSet<std::string> validStatesSet;

    // Insert valid state names into the set
    static const std::vector<std::string> validStatesList = {
        "Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado", "Connecticut", "Delaware",
        "Florida", "Georgia", "Hawaii", "Idaho", "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky",
        "Louisiana", "Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota", "Mississippi",
        "Missouri", "Montana", "Nebraska", "Nevada", "New Hampshire", "New Jersey", "New Mexico",
        "New York", "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon", "Pennsylvania",
        "Rhode Island", "South Carolina", "South Dakota", "Tennessee", "Texas", "Utah", "Vermont",
        "Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming"
    };

    for (const auto& state : validStatesList) {
        validStatesSet.insert(state);
    }

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
            std::string just_year = field.substr(0,4);
            eq.year = std::stoi(just_year);
        }

        // Check if earthquake data meets all criteria
        if (isState(eq.state, validStatesSet) && eq.magnitude >= 2.0 && eq.data_type == "earthquake") {
            if (eq.state == "Georgia" && eq.latitude > 40)
                continue;
            earthquakes.push_back(eq);
        }
    }

    // Output filtered earthquake data to JSON file
    std::ofstream jsonFile("earthquake_data.json");
    if (!jsonFile.is_open()) {
        std::cerr << "Error opening JSON file for writing!" << std::endl;
        return 1;
    }

    jsonFile << "[\n";
    for (size_t i = 0; i < earthquakes.size(); ++i) {
        const Earthquake& eq = earthquakes[i];
        jsonFile << "    {\n";
        jsonFile << "        \"state\": \"" << eq.state << "\",\n";
        jsonFile << "        \"magnitude\": " << eq.magnitude << ",\n";
        jsonFile << "        \"longitude\": " << eq.longitude << ",\n";
        jsonFile << "        \"latitude\": " << eq.latitude << ",\n";
        jsonFile << "        \"year\": " << eq.year << "\n";
        jsonFile << "    }";
        if (i < earthquakes.size() - 1) {
            jsonFile << ",";
        }
        jsonFile << "\n";
    }
    jsonFile << "]\n";

    jsonFile.close();
    std::cout << "Successfully wrote filtered earthquake data to earthquake_data.json" << std::endl;

    return 0;
}
