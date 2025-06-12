
// File: cliconfig_parser.cpp
#include "../assets/cliconfig_parser.h"
#include <iostream>
#include <stdexcept>   // For exception handling (used in std::stoi)

ConfigOptions parseArguments (int argc, char* argv[]) {
// Check if the correct number of arguments is passed in cli or not (int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <REGION> <START_DATE> <END_DATE> <NUM_THREADS> <DATA_DIR> <OUTPUT_PATH>\n";
        std::cerr << "Example: ./query_exec ASIA 1995-01-01 1996-01-01 4 ./data/ output.txt\n";
        exit(1);
    }

    ConfigOptions opts;// Create an instance of ConfigOptions to store parsed values

    // Assign values from command-line arguments to the struct
    opts.region = argv[1];
    opts.fromDate = argv[2];
    opts.toDate = argv[3];
    opts.threadCount = std::stoi(argv[4]);
    // Ensure the data folder path ends with '/' to simplify file path joining later
    opts.dataFolder = argv[5];
    if (opts.dataFolder.back() != '/') opts.dataFolder += '/';
    opts.resultFile = argv[6];

    return opts;
}
