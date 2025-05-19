#include <filesystem>
#include <string>
#include <iostream>
#include <cstdlib> // 添加头文件用于getenv函数
#include <set>    // 添加头文件用于std::set
#include <sstream> // 添加头文件用于字符串流处理

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
    
    // 读取当前PATH环境变量
    const char* currentPath = std::getenv("PATH");
    if (!currentPath) {
        std::cerr << "Warning: Cannot read PATH environment variable." << std::endl;
        currentPath = "";
    }
    
    // 将当前PATH拆分为单独的路径，存入集合中便于检查
    std::string pathStr(currentPath);
    std::set<std::string> existingPaths;
    std::istringstream pathStream(pathStr);
    std::string path;
    
    // 在Windows下，PATH使用分号分隔
    while (std::getline(pathStream, path, ';')) {
        if (!path.empty()) {
            existingPaths.insert(path);
        }
    }
    
    // 检查所需的路径是否都已存在于PATH中
    bool pathsExist = true;
    std::string command;
    
    // 检查环境根目录
    if (existingPaths.find(condaEnvRoot.string()) == existingPaths.end()) {
        pathsExist = false;
    }
    
    // 检查所需的子目录
    const char* subdirs[] = {
        "Library\\mingw-w64\\bin",
        "Library\\usr\\bin",
        "Library\\bin", 
        "Scripts",
        "bin"
    };
    
    for (const char* subdir : subdirs) {
        fs::path p = condaEnvRoot / subdir;
        if (fs::exists(p) && existingPaths.find(p.string()) == existingPaths.end()) {
            pathsExist = false;
            break;
        }
    }
    
    // 如果所有路径都已存在，则直接启动目标程序
    if (pathsExist) {
        command = "\"" + targetPath.string() + "\"";
    } else {
        // 构建新的PATH环境变量命令
        command = "set PATH=" + condaEnvRoot.string() + ";";
        
        // 添加标准路径
        for (const char* subdir : subdirs) {
            fs::path p = condaEnvRoot / subdir;
            if (fs::exists(p)) {
                command += p.string() + ";";
            }
        }
        
        command += "%PATH% && \"" + targetPath.string() + "\"";
    }
    
    // 添加命令行参数
    for (int i = 1; i < argc; i++) {
        command += " \"";
        command += argv[i];
        command += "\"";
    }
    
    // 执行命令
    return system(command.c_str());
}
