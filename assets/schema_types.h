// File: schema_types.h it is struct data type

#ifndef SCHEMA_TYPES_H
#define SCHEMA_TYPES_H

#include <string>

struct GeoZone {
    int key;
    std::string label;
};

struct NationInfo {
    int id;
    std::string name;
    int regionId;
};

struct ClientInfo {
    int id;
    int nationality;
};

struct PurchaseOrder {
    int id;
    int clientId;
    std::string orderDate;
};

struct ItemDetail {
    int orderId;
    int vendorId;
    double price;
    double markdown;
};

struct VendorData {
    int id;
    int nation;
    double balance;
};

#endif // SCHEMA_TYPES_H