
// File: cliconfig_parser.h
#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>

// Struct data type to store the parsed command-line arguments.
// Each member corresponds to one of the parameters needed for TPCH Query 5.

struct ConfigOptions {
    std::string region;
    std::string fromDate;
    std::string toDate;
    int threadCount;
    std::string dataFolder;
    std::string resultFile;
};

// Function declaration to parse CLI arguments into a ConfigOptions struct
// This function is defined in cliconfig_parser.cpp
ConfigOptions parseArguments(int argc, char* argv[]);

#endif