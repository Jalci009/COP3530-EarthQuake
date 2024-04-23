#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <list>
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

// Custom hash function for string keys
size_t customHash(const string& key, size_t tableSize) 
{
    size_t hash = 0;
    for (char ch : key) 
    {
        hash = (hash * 31 + ch) % tableSize;
    }
    return hash;
}

template<typename K, typename V>
class HashNode 
{
public:
    K key;
    vector<V> value;
    HashNode<K, V>* next;

    HashNode(const K& k, const V& v) : key(k), next(nullptr) 
    {
        value.push_back(v);
    }
};

template<typename K, typename V>
class HashMap 
{
private:
    vector<list<HashNode<K, V>*>> table;  // Using list
    size_t size;
    size_t capacity;
    float loadFactor;

    size_t getHashIndex(const K& key) const 
    {
        return customHash(key, table.size());
    }

    void resizeTable() 
    {
        size_t newCapacity = capacity * 2;
        HashMap<K, V> newHashMap(newCapacity); // Create a new HashMap with doubled capacity
        for (auto& bucket : table) 
        {
            for (auto node : bucket) 
            {
                for (const auto& value : node->value) 
                {
                    newHashMap.insert(node->key, value); // Rehash all existing elements into the new HashMap
                }
            }
        }
        // Swap the contents of the current HashMap with the new one
        table = move(newHashMap.table);
        size = newHashMap.size;
        capacity = newCapacity;
    }

public:
    HashMap(size_t initialSize = 10, float loadFactor = 0.7) : table(initialSize), size(0), capacity(initialSize), loadFactor(loadFactor) {}

    ~HashMap() 
    {
        clear();
    }

    void insert(const K& key, const V& value) 
    {
        size_t index = getHashIndex(key);
        auto& list = table[index];
        for (auto& node : list) 
        {  // Use reference
            if (node->key == key) 
            {
                node->value.push_back(value);  // Append to existing vector
                return;
            }
        }
        list.push_back(new HashNode<K, V>(key, value));  // Create a new node
        ++size;

        // Check load factor and resize if needed
        if ((float)size / capacity > loadFactor) 
        {
            resizeTable();
        }
    }  

    vector<V>* find(const K& key) 
    {
        size_t index = getHashIndex(key);
        auto& list = table[index];
        for (auto& node : list) 
        {
            if (node->key == key) 
            {
                return &(node->value);
            }
        }
        return nullptr;
    }

    void clear() 
    {
        for (auto& bucket : table) 
        {
            for (auto node : bucket) 
            {
                delete node;
            }
            bucket.clear();
        }
        size = 0;
    }

    size_t getSize() const 
    {
        return size;
    }

    class Iterator 
    {
    private:
        typename vector<list<HashNode<K, V>*>>::iterator bucketIter;
        typename list<HashNode<K, V>*>::iterator nodeIter;

    public:
        Iterator(typename vector<list<HashNode<K, V>*>>::iterator bucketIter,
                 typename list<HashNode<K, V>*>::iterator nodeIter)
            : bucketIter(bucketIter), nodeIter(nodeIter) {}

        Iterator& operator++() 
        {
            ++nodeIter;
            while (nodeIter == (*bucketIter)->end() && ++bucketIter != table.end()) 
            {
                nodeIter = (*bucketIter)->begin();
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const 
        {
            return bucketIter != other.bucketIter || nodeIter != other.nodeIter;
        }

        HashNode<K, V>& operator*() const 
        {
            return **nodeIter;
        }
    };

    Iterator begin() 
    {
        auto bucketIter = table.begin();
        auto nodeIter = (bucketIter != table.end()) ? (*bucketIter)->begin() : typename list<HashNode<K, V>*>::iterator();
        return Iterator(bucketIter, nodeIter);
    }

    Iterator end() 
    {
        return Iterator(table.end(), typename list<HashNode<K, V>*>::iterator());
    }

    const vector<list<HashNode<K, V>*>>& getTable() const 
    {
        return table;
    }
};

// Read earthquake data from file and populate the hashmap
void readEarthquakeData(const string& filename, HashMap<string, Earthquake>& earthquakeMap) 
{
    ifstream file(filename);
    if (!file.is_open()) 
    {
        cerr << "Error opening file!" << endl;
        return;
    }

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
            earthquakeMap.insert(eq.state, eq);
        }
    }
    file.close();
}

// Write filtered earthquake data to JSON file
void writeFilteredEarthquakeDataToJson(const HashMap<string, Earthquake>& earthquakeMap, const string& filename) 
{
    ofstream jsonFile(filename);
    if (!jsonFile.is_open()) 
    {
        cerr << "Error opening JSON file for writing!" << endl;
        return;
    }

    jsonFile << "[\n";
    bool firstState = true;

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

    // Iterate through the earthquake data in the HashMap
    for (const auto& bucket : earthquakeMap.getTable()) 
    {
        for (const auto& nodePtr : bucket) 
        {
            const auto& node = *nodePtr;  // Dereference the pointer

            // Check if the state is within the valid states list
            bool isValidState = false;
            for (const auto& state : validStatesList) 
            {
                if (node.key == state) 
                {
                    isValidState = true;
                    break;
                }
            }
            if (!isValidState) 
            {
                continue;
            }

            // Iterate over earthquakes in the vector and write to JSON
            for (const auto& eq : node.value) 
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
    }

    jsonFile << "\n]\n";
    jsonFile.close();
}

int main() 
{
    HashMap<string, Earthquake> earthquakeMap;

    // Measure runtime
    auto startTime = chrono::steady_clock::now();
    readEarthquakeData("earthquake_data.csv", earthquakeMap);
    writeFilteredEarthquakeDataToJson(earthquakeMap, "public/earthquake_data.json");
    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    cout << "Runtime: " << duration.count() << " ms" << endl;
    cout << "Successfully wrote filtered earthquake data to earthquake_data.json" << endl;

    return 0;
}
