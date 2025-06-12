#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>

#include "../assets/schema_types.h"
#include "../assets/data_loader.h"
#include "../assets/cliconfig_parser.h"

std::mutex lockGuard;  // Mutex to synchronize access to shared `revenue` map across threads


// Function to process a revenue of orders and accumulate revenue per nation
void calculateRevenue(const std::vector<PurchaseOrder>& orders,
    const std::unordered_map<int, int>& clientNationMap,
    const std::unordered_map<int, std::string>& nationLabelMap,
    const std::unordered_map<int, int>& nationRegionMap,
    const std::unordered_map<int, std::string>& regionLabelMap,
    const std::unordered_map<int, std::vector<ItemDetail>>& itemsMap,
    const std::unordered_map<int, int>& vendorNationMap,
    const std::string& targetRegion,
    const std::string& beginDate,
    const std::string& finishDate,
    std::unordered_map<std::string, double>& finalMap,
    int begin, int end) {

    std::unordered_map<std::string, double> localSum; // Thread-local revenue sum map


    //orders
    for (int i = begin; i < end; ++i) {
        const auto& order = orders[i];

         // Filter orders based on date range
        if (order.orderDate < beginDate || order.orderDate >= finishDate)
            continue;

        auto clientIt = clientNationMap.find(order.clientId);
        if (clientIt == clientNationMap.end())
            continue;
        int nationId = clientIt->second;

         // Check if customer belongs to correct region
        auto nationRegionIt = nationRegionMap.find(nationId);
        if (nationRegionIt == nationRegionMap.end())
            continue;

        auto regionLabelIt = regionLabelMap.find(nationRegionIt->second);
        if (regionLabelIt == regionLabelMap.end())
            continue;

        if (regionLabelIt->second != targetRegion)
            continue;

         // Get associated line items for the order
        auto itemsIt = itemsMap.find(order.id);
        if (itemsIt == itemsMap.end())
            continue;

             // vendor and client must be from same nation
        for (const auto& item : itemsIt->second) {
            auto vendorNationIt = vendorNationMap.find(item.vendorId);
            if (vendorNationIt == vendorNationMap.end())
                continue;

            if (vendorNationIt->second != nationId)
                continue;

                
    // Calculate revenue and accumulate in local map

            auto nationLabelIt = nationLabelMap.find(nationId);
            if (nationLabelIt == nationLabelMap.end())
                continue;

            double amount = item.price * (1 - item.markdown);
            localSum[nationLabelIt->second] += amount;
        }
    }


      // Merge localMap into shared result under mutex
    std::lock_guard<std::mutex> guard(lockGuard);
    for (const auto& [nation, val] : localSum)
        finalMap[nation] += val;
}


/////////main function
int main(int argc, char* argv[]) {
     //--- Parse CLI Arguments ---
    ConfigOptions opts = parseArguments(argc, argv);
    std::ofstream log(opts.resultFile);


    // --- Load Data ---

    auto regions = loadRegions(opts.dataFolder + "region.tbl");
    auto nations = loadNations(opts.dataFolder + "nation.tbl");
    auto clients = loadClients(opts.dataFolder + "customer.tbl");
    auto orders = loadOrders(opts.dataFolder + "orders.tbl");
    auto items = loadLineItems(opts.dataFolder + "lineitem.tbl");
    auto vendors = loadVendors(opts.dataFolder + "supplier.tbl");

    log << "Data loaded. Launching computation...\n";


     // Preprocess lookup maps for fast access during query

    // Region ID → Region Name
    std::unordered_map<int, std::string> regionLabels;
    for (const auto& r : regions) regionLabels[r.key] = r.label;


    // Nation ID → Region ID and Nation ID → Nation Name
    std::unordered_map<int, std::string> nationLabels;
    std::unordered_map<int, int> nationRegions;
    for (const auto& n : nations) {
        nationLabels[n.id] = n.name;
        nationRegions[n.id] = n.regionId;
    }

     // Client ID → Nation ID
    std::unordered_map<int, int> clientMap;
    for (const auto& c : clients) clientMap[c.id] = c.nationality;

     // vendor ID → Nation ID
    std::unordered_map<int, int> vendorMap;
    for (const auto& v : vendors) vendorMap[v.id] = v.nation;


    // Order ID → List of LineItems
    std::unordered_map<int, std::vector<ItemDetail>> itemMap;
    for (const auto& it : items) itemMap[it.orderId].push_back(it);


     //------ Multithreading Logic program ------

    std::unordered_map<std::string, double> revenue;  // Shared result map that will hold revenue aggregated by nation
    std::vector<std::thread> threads;  // Vector to store thread objects

    int split = orders.size() / opts.threadCount; // Size of each split to be processed by a thread
    auto t0 = std::chrono::high_resolution_clock::now();
     // Record and note down start time for performance measurement


      // Loop to create and launch each thread
    for (int i = 0; i < opts.threadCount; ++i) {
        int start = i * split;
        int stop = (i == opts.threadCount - 1) ? orders.size() : (i + 1) * split;

          //Create a new thread to process a specific range of orders using calculateRevenue()
        threads.emplace_back(
            calculateRevenue,  // Function to run
             std::cref(orders),  // Orders vector (shared across threads)
              std::cref(clientMap), // Client → Nation lookup (shared)
            std::cref(nationLabels), // Nation ID → Name (shared)
            std::cref(nationRegions), // Nation → Region lookup (shared)
            std::cref(regionLabels), // Region ID → Name (shared)
            std::cref(itemMap),  // Order ID → Line items (shared)
             std::cref(vendorMap),// vendor → Nation lookup (shared)
            opts.region, 
            opts.fromDate,
             opts.toDate, 
             std::ref(revenue), // Shared result map (protected by mutex)
             start,
              stop);
    }

    for (auto& t : threads) t.join();  // Wait for all threads to finish execution



    //End time for performance measurement
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t1 - t0;


     // Sort results by descending revenue
    std::vector<std::pair<std::string, double>> output(revenue.begin(), revenue.end());
    std::sort(output.begin(), output.end(), [](auto& a, auto& b) { return a.second > b.second; });


    // Output the final revenue per nation
    log << "\nResults final revenue by nations:\n";
    for (const auto& [label, val] : output)
        log << label << ": " << val << "\n";

    log << "\nTime taken: " << elapsed.count() << " seconds\n";
    std::cout << "Done. Output in: " << opts.resultFile << std::endl;
    return 0;
}
