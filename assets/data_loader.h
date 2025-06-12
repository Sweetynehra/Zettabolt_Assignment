// File: data_loader.h

#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "schema_types.h"
#include <vector>
#include <string>

// Load GeoZones (regions.tbl)
std::vector<GeoZone> loadRegions(const std::string& filePath);

// Load Nations (nation.tbl)
std::vector<NationInfo> loadNations(const std::string& filePath);

// Load Clients (customer.tbl)
std::vector<ClientInfo> loadClients(const std::string& filePath);

// Load Orders (orders.tbl)
std::vector<PurchaseOrder> loadOrders(const std::string& filePath);

// Load Line Items (lineitem.tbl)
std::vector<ItemDetail> loadLineItems(const std::string& filePath);

// Load Vendors (supplier.tbl)
std::vector<VendorData> loadVendors(const std::string& filePath);

#endif // DATA_LOADER_H
