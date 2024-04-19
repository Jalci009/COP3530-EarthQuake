#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <list>
#include <chrono>

// Custom hash function for std::string keys
size_t customHash(const std::string& key, size_t tableSize) {
    size_t hash = 0;
    for (char ch : key) {
        hash = (hash * 31 + ch) % tableSize;  // Using a simple prime number hash
    }
    return hash;
}

template<typename K, typename V>
class HashNode {
public:
    K key;
    std::vector<V> value;
    HashNode<K, V>* next;

    HashNode(const K& k, const V& v) : key(k), next(nullptr) {
        value.push_back(v);
    }
};

template<typename K, typename V>
class HashMap {
private:
    std::vector<std::list<HashNode<K, V>*>> table;  // Using std::list here
    size_t size;
    size_t capacity;
    float loadFactor;

    size_t getHashIndex(const K& key) const {
        return customHash(key, table.size());
    }

    void resizeTable() {
        size_t newCapacity = capacity * 2;
        HashMap<K, V> newHashMap(newCapacity); // Create a new HashMap with doubled capacity
        for (auto& bucket : table) {
            for (auto node : bucket) {
                for (const auto& value : node->value) {
                    newHashMap.insert(node->key, value); // Rehash all existing elements into the new HashMap
                }
            }
        }
        // Swap the contents of the current HashMap with the new one
        table = std::move(newHashMap.table);
        size = newHashMap.size;
        capacity = newCapacity;
    }

public:
    HashMap(size_t initialSize = 10, float loadFactor = 0.7) : table(initialSize), size(0), capacity(initialSize), loadFactor(loadFactor) {}

    ~HashMap() {
        clear();
    }

    void insert(const K& key, const V& value) {
        size_t index = getHashIndex(key);
        auto& list = table[index];
        for (auto& node : list) {  // Use reference to avoid unnecessary copying
            if (node->key == key) {
                node->value.push_back(value);  // Append to existing vector
                return;
            }
        }
        list.push_back(new HashNode<K, V>(key, value));  // Create a new node
        ++size;

        // Check load factor and resize if necessary
        if ((float)size / capacity > loadFactor) {
            resizeTable();
        }
    }

    std::vector<V>* find(const K& key) {
        size_t index = getHashIndex(key);
        auto& list = table[index];
        for (auto& node : list) {
            if (node->key == key) {
                return &(node->value);
            }
        }
        return nullptr;  // Not found
    }

    void clear() {
        for (auto& bucket : table) {
            for (auto node : bucket) {
                delete node;
            }
            bucket.clear();
        }
        size = 0;
    }

    size_t getSize() const {
        return size;
    }

    // Iterator support
    class Iterator {
    private:
        typename std::vector<std::list<HashNode<K, V>*>>::iterator bucketIter;
        typename std::list<HashNode<K, V>*>::iterator nodeIter;

    public:
        Iterator(typename std::vector<std::list<HashNode<K, V>*>>::iterator bucketIter,
                 typename std::list<HashNode<K, V>*>::iterator nodeIter)
            : bucketIter(bucketIter), nodeIter(nodeIter) {}

        Iterator& operator++() {
            ++nodeIter;
            while (nodeIter == (*bucketIter)->end() && ++bucketIter != table.end()) {
                nodeIter = (*bucketIter)->begin();
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return bucketIter != other.bucketIter || nodeIter != other.nodeIter;
        }

        HashNode<K, V>& operator*() const {
            return **nodeIter;
        }
    };

    Iterator begin() {
        auto bucketIter = table.begin();
        auto nodeIter = (bucketIter != table.end()) ? (*bucketIter)->begin() : typename std::list<HashNode<K, V>*>::iterator();
        return Iterator(bucketIter, nodeIter);
    }

    Iterator end() {
        return Iterator(table.end(), typename std::list<HashNode<K, V>*>::iterator());
    }

    const std::vector<std::list<HashNode<K, V>*>>& getTable() const {
        return table;
    }
};

// Earthquake structure
struct Earthquake {
    std::string state;
    double magnitude;
    std::string data_type;
    double longitude;
    double latitude;
    int year;
};

// Function to read earthquake data from file and populate the hashmap
void readEarthquakeData(const std::string& filename, HashMap<std::string, Earthquake>& earthquakeMap) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

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
            earthquakeMap.insert(eq.state, eq);
        }
    }
    file.close();
}

// Function to write filtered earthquake data to JSON file
void writeFilteredEarthquakeDataToJson(const HashMap<std::string, Earthquake>& earthquakeMap, const std::string& filename) {
    std::ofstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        std::cerr << "Error opening JSON file for writing!" << std::endl;
        return;
    }

    jsonFile << "[\n";
    bool firstState = true;

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

    // Iterate through the earthquake data in the HashMap
    for (const auto& bucket : earthquakeMap.getTable()) {
        for (const auto& nodePtr : bucket) {
            const auto& node = *nodePtr;  // Dereference the pointer

            // Check if the state is within the valid states list
            bool isValidState = false;
            for (const auto& state : validStatesList) {
                if (node.key == state) {
                    isValidState = true;
                    break;
                }
            }
            if (!isValidState) {
                continue;
            }

            // If the state is Georgia and latitude is greater than 40, continue
            if (node.key == "Georgia") {
                bool isGeorgiaValid = false;
                for (const auto& eq : node.value) {
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
            for (const auto& eq : node.value) {
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
    }

    jsonFile << "\n]\n";
    jsonFile.close();
}

int main() {
    HashMap<std::string, Earthquake> earthquakeMap;

    // Measure runtime
    auto startTime = std::chrono::steady_clock::now();
    readEarthquakeData("earthquake_data.csv", earthquakeMap);
    writeFilteredEarthquakeDataToJson(earthquakeMap, "earthquake_data.json");
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Successfully wrote filtered earthquake data to earthquake_data.json" << std::endl;

    return 0;
}
