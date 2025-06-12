#include "../assets/data_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>


// Helper to safely convert string to int
int safeStoi(const std::string& s, const std::string& context = "") {
    try {
        return std::stoi(s);
    } catch (...) {
        std::cerr << "[stoi error] context: " << context << " value: '" << s << "'\n";
        return -1;
    }
}

// Helper to safely convert string to double
double safeStod(const std::string& s, const std::string& context = "") {
    try {
        return std::stod(s);
    } catch (...) {
        std::cerr << "[stod error] context: " << context << " value: '" << s << "'\n";
        return -1.0;
    }
}

//Function to read data from .tbl
// Each line format: region|r_name|r_comment|
// Only 2 parameters are extracted
// Load GeoZones (Regions)
std::vector<GeoZone> loadRegions(const std::string& filePath) {
    std::vector<GeoZone> regions; // Vector to store all parsed Region records
    std::ifstream file(filePath);      // Open the file at the given path
    std::string line;  // Temporary string to store each line

    while (std::getline(file, line)) {  // Read the file line by line
        std::stringstream ss(line);  // Create a stringstream to tokenize the line
        std::string data;  // Temporary string to store each data
        GeoZone gz; // Create an empty Region struct to populate gz

        std::getline(ss, data, '|'); // Get the first data (region key)
        gz.key = safeStoi(data, "region.key");  // Convert region key to integer 
        std::getline(ss, gz.label, '|');  // Get the second data (region name)

        regions.push_back(gz);
    }

    return regions;
}

// Load Nations
std::vector<NationInfo> loadNations(const std::string& filePath) {
    std::vector<NationInfo> nations;
    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string data;
        NationInfo n;

        std::getline(ss, data, '|'); n.id = safeStoi(data, "nation.id");
        std::getline(ss, n.name, '|');
        std::getline(ss, data, '|'); n.regionId = safeStoi(data, "nation.regionId");

        nations.push_back(n);
    }

    return nations;
}

// Load Clients (Customers)
std::vector<ClientInfo> loadClients(const std::string& filePath) {
    std::vector<ClientInfo> clients;
    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string data;
        ClientInfo c;

        std::getline(ss, data, '|'); c.id = safeStoi(data, "client.id");
        std::getline(ss, data, '|'); // skip name
        std::getline(ss, data, '|'); // skip address
        std::getline(ss, data, '|'); c.nationality = safeStoi(data, "client.nationality");

        clients.push_back(c);
    }

    return clients;
}

// Load Orders
std::vector<PurchaseOrder> loadOrders(const std::string& filePath) {
    std::vector<PurchaseOrder> orders;
    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string data;
        PurchaseOrder o;

        std::getline(ss, data, '|'); o.id = safeStoi(data, "order.id");
        std::getline(ss, data, '|'); o.clientId = safeStoi(data, "order.clientId");
        std::getline(ss, data, '|'); // skip order status
        std::getline(ss, data, '|'); // skip total price
        std::getline(ss, o.orderDate, '|');

        orders.push_back(o);
    }

    return orders;
}

// Load Line Items
std::vector<ItemDetail> loadLineItems(const std::string& filePath) {
    std::vector<ItemDetail> items;
    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string data;
        ItemDetail item;

        std::getline(ss, data, '|'); item.orderId = safeStoi(data, "item.orderId");
        std::getline(ss, data, '|'); item.vendorId = safeStoi(data, "item.vendorId");
        std::getline(ss, data, '|'); // skip part id
        std::getline(ss, data, '|'); // skip line number
        std::getline(ss, data, '|'); // skip quantity
        std::getline(ss, data, '|'); item.price = safeStod(data, "item.price");
        std::getline(ss, data, '|'); item.markdown = safeStod(data, "item.markdown");

        items.push_back(item);
    }

    return items;
}

// Load Vendors (Suppliers)
std::vector<VendorData> loadVendors(const std::string& filePath) {
    std::vector<VendorData> vendors;
    std::ifstream file(filePath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string data;
        VendorData v;

        std::getline(ss, data, '|'); v.id = safeStoi(data, "vendor.id");
        std::getline(ss, data, '|'); // skip name
        std::getline(ss, data, '|'); // skip address
        std::getline(ss, data, '|'); v.nation = safeStoi(data, "vendor.nation");
        std::getline(ss, data, '|'); // skip phone
        std::getline(ss, data, '|'); v.balance = safeStod(data, "vendor.balance");

        vendors.push_back(v);
    }

    return vendors;
}
