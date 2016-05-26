#ifndef PARAMS_H
#define	PARAMS_H

#include "program.h"
#include <unistd.h>

string AppPath() {
    char path[255];
    return (getcwd(path, 255) ? std::string(path) : "");
}

enum Param {
    param_none,
    param_help,
    param_version,
    param_name,
    param_path,
    param_example,
    param_output,
    param_square,
    param_extra,
    param_retention,
    param_print,
    param_testrun,
    param_noJson
};

// uses this
Param GetParam(char* value) {
    string s = value;
    if (s == "--help")                return param_help;
    if (s == "--no-field-data")       return param_noJson;
    if (s == "--version")             return param_version;
    if (s == "--name")                return param_name;
    if (s == "--path")                return param_path;
    if (s == "--example")             return param_example;
    if (s == "--output")              return param_output;
    if (s.substr(0, 8) == "--square") return param_square;
    if (s == "-extra")                return param_extra;
    if (s == "-retention")            return param_retention;
    if (s == "-print")                return param_print;
    if (s.substr(0, 8) == "-testrun") return param_testrun;
    //std::cout << "No such parameter " << s << std::endl;
    return param_none;
}

void PrintHelp() {
    std::cout << "Usage:\t" << program_name << " [ARGUMENT] CONFIG\t"
            << "Run the program with current settings" << std::endl;
    std::cout << "\t" << program_name << " --help\t\t"
            << "Display this help and exit" << std::endl;
    std::cout << "\t" << program_name << " --version\t\t"
            << "Output version information and exit" << std::endl;
    std::cout << "\t" << program_name << " --name\t\t"
            << "Display program name and exit" << std::endl;
    std::cout << "\t" << program_name << " --path\t\t"
            << "Display program path and exit" << std::endl;
    std::cout << "\t" << program_name << " --example\t\t"
            << "Print example of configuration file and exit" << std::endl;
    std::cout << "\t" << program_name << " --output filename\t"
            << "Print half-life in output file and exit" << std::endl;
    std::cout << "\t" << program_name << " --squareNx-Ny-Nz filename\t"
            << "Print surface square in output file and exit" << std::endl;
    std::cout << "\t\tNx\tField x size" << std::endl;
    std::cout << "\t\tNy\tField y size" << std::endl;
    std::cout << "\t\tNz\tField z size" << std::endl;
    std::cout << std::endl;
    std::cout << "\t" << program_name << " [filename]\t\t"
            << "Use filename configuration file" << std::endl;
    std::cout << std::endl;
    std::cout << "\t" << program_name << " [] -extra\t\t"
            << "Use express database out" << std::endl;
    std::cout << "\t" << program_name << " [] -retention\t"
            << "Use retention out" << std::endl;
    std::cout << "\t" << program_name << " [] -print\t\t"
            << "Print field parameters" << std::endl;
    std::cout << "\t" << program_name << " -testrunNx-Ny-I-C-G-T-M\t"
            << "Run test calculation" << std::endl;
    std::cout << "\t\tNx\tField x size" << std::endl;
    std::cout << "\t\tNy\tField y size" << std::endl;
    std::cout << "\t\tI\tIteration count" << std::endl;
    std::cout << "\t\tC\tCalculation count" << std::endl;
    std::cout << "\t\tG\tSubiteration type (0 - 2 subiters, 1 - 8 subiters)" << std::endl;
    std::cout << "\t\tT\tMargolus type (0 - default, 1 - quazi, 2 - quazix)" << std::endl;
    std::cout << "\t\tM\tFile mode (N - new, A - append)" << std::endl;
}

void PrintVersion() {
    std::cout << program_name << "-" << program_version << std::endl;
}

#endif	/* PARAMS_H */
