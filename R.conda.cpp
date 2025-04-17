#include <filesystem>
#include <string>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // Get the path of the current executable
    fs::path exeFullPath;
    
    try {
        // Check if running with a relative path
        fs::path inputPath(argv[0]);
        if (!inputPath.is_absolute()) {
            std::cerr << "Warning: Program is running with a relative path, which may cause environment detection issues." << std::endl;
            std::cerr << "Current path: " << inputPath.string() << std::endl;
            std::cerr << "Recommendation: Use the full path to run this program." << std::endl;
            
            // Try to get the absolute path by combining current_path and relative path
            try {
                exeFullPath = fs::absolute(inputPath);
                std::cerr << "Attempted to convert to absolute path: " << exeFullPath.string() << std::endl;
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error: Cannot convert to absolute path: " << e.what() << std::endl;
                return 1;
            }
        } else {
            // Already an absolute path
            exeFullPath = inputPath;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Cannot analyze program path: " << e.what() << std::endl;
        std::cerr << "Please run this program with a complete absolute path." << std::endl;
        return 1;
    }
    
    // Parse path information
    fs::path exeDir;
    std::string exeFileName;
    
    try {
        exeDir = exeFullPath.parent_path();
        exeFileName = exeFullPath.filename().string();
    } catch (const std::exception& e) {
        std::cerr << "Error: Cannot parse program directory or filename: " << e.what() << std::endl;
        std::cerr << "Program path: " << exeFullPath.string() << std::endl;
        return 1;
    }
    
    // Quick check for directory name and filename
    if (exeDir.empty() || !fs::exists(exeDir)) {
        std::cerr << "Error: Cannot access the program directory." << std::endl;
        std::cerr << "Program directory: " << exeDir.string() << std::endl;
        std::cerr << "This may be because the program is not run with a complete path. Please use an absolute path." << std::endl;
        return 1;
    }
    
    if (exeDir.filename() != "Scripts" || 
        exeFileName.length() <= 10 || 
        exeFileName.substr(exeFileName.length() - 10) != ".conda.exe") {
        std::cerr << "Error: Program must be placed in the /path/to/conda/env/Scripts directory of a virtual environment, and the filename must end with .conda.exe." << std::endl;
        std::cerr << "Current directory: " << exeDir.string() << std::endl;
        std::cerr << "Current file: " << exeFileName << std::endl;
        return 1;
    }
    
    // Extract target program name and environment root directory
    std::string targetProgram = exeFileName.substr(0, exeFileName.length() - 10) + ".exe";
    fs::path condaEnvRoot = exeDir.parent_path();
    fs::path targetPath = exeDir / targetProgram;
    
    // Quick check for target program and environment root directory
    if (!fs::exists(condaEnvRoot)) {
        std::cerr << "Error: Cannot find virtual environment root directory: " << condaEnvRoot.string() << std::endl;
        return 1;
    }
    
    if (!fs::exists(targetPath)) {
        std::cerr << "Error: Cannot find target program: " << targetPath.string() << std::endl;
        return 1;
    }
    
    // Build simplified PATH environment variable command (avoid checking directory existence multiple times)
    std::string command = "set PATH=" + condaEnvRoot.string() + ";";
    
    // Add standard paths
    const char* subdirs[] = {
        "Library\\mingw-w64\\bin",
        "Library\\usr\\bin",
        "Library\\bin", 
        "Scripts",
        "bin"
    };
    
    for (const char* subdir : subdirs) {
        fs::path p = condaEnvRoot / subdir;
        if (fs::exists(p)) {
            command += p.string() + ";";
        }
    }
    
    // Execute target program
    command += "%PATH% && \"" + targetPath.string() + "\"";
    
    // Add command line arguments
    for (int i = 1; i < argc; i++) {
        command += " \"";
        command += argv[i];
        command += "\"";
    }
    
    // Execute command
    return system(command.c_str());
}
