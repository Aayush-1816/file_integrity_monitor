#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <openssl/md5.h>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace fs = std::filesystem;

class FileIntegrityMonitor {
private:
    std::map<std::string, std::string> fileHashes;
    std::string databaseFile;

    // Calculate MD5 hash of a file
    std::string calculateMD5(const std::string& filepath) {
        std::ifstream file(filepath, std::ifstream::binary);
        if (!file) {
            return "";
        }

        MD5_CTX md5Context;
        MD5_Init(&md5Context);

        char buffer[1024];
        while (file.read(buffer, sizeof(buffer))) {
            MD5_Update(&md5Context, buffer, file.gcount());
        }
        MD5_Update(&md5Context, buffer, file.gcount());

        unsigned char result[MD5_DIGEST_LENGTH];
        MD5_Final(result, &md5Context);

        std::stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
        }

        file.close();
        return ss.str();
    }

    // Get current timestamp
    std::string getCurrentTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        std::string timestamp(dt);
        timestamp.pop_back(); // Remove newline
        return timestamp;
    }

public:
    FileIntegrityMonitor(const std::string& dbFile = "fim_database.txt") 
        : databaseFile(dbFile) {
        loadDatabase();
    }

    // Load existing hash database
    void loadDatabase() {
        std::ifstream file(databaseFile);
        if (!file) {
            std::cout << "No existing database found. Starting fresh.\n";
            return;
        }

        std::string filepath, hash;
        while (file >> filepath >> hash) {
            fileHashes[filepath] = hash;
        }
        file.close();
        std::cout << "Loaded " << fileHashes.size() << " files from database.\n";
    }

    // Save hash database
    void saveDatabase() {
        std::ofstream file(databaseFile);
        for (const auto& pair : fileHashes) {
            file << pair.first << " " << pair.second << "\n";
        }
        file.close();
        std::cout << "Database saved with " << fileHashes.size() << " files.\n";
    }

    // Initialize monitoring for a directory
    void initializeMonitoring(const std::string& directory) {
        std::cout << "\n[INITIALIZE] Scanning directory: " << directory << "\n";
        std::cout << std::string(60, '-') << "\n";

        int count = 0;
        try {
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filepath = entry.path().string();
                    std::string hash = calculateMD5(filepath);
                    
                    if (!hash.empty()) {
                        fileHashes[filepath] = hash;
                        std::cout << "✓ Added: " << filepath << "\n";
                        count++;
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error accessing directory: " << e.what() << "\n";
        }

        std::cout << std::string(60, '-') << "\n";
        std::cout << "Total files initialized: " << count << "\n\n";
        saveDatabase();
    }

    // Check for file changes
    void checkIntegrity(const std::string& directory) {
        std::cout << "\n[INTEGRITY CHECK] " << getCurrentTime() << "\n";
        std::cout << std::string(60, '=') << "\n";

        int modified = 0, added = 0, deleted = 0, unchanged = 0;
        std::map<std::string, std::string> currentFiles;

        // Scan current files
        try {
            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filepath = entry.path().string();
                    std::string hash = calculateMD5(filepath);
                    
                    if (!hash.empty()) {
                        currentFiles[filepath] = hash;
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error accessing directory: " << e.what() << "\n";
            return;
        }

        // Check for modifications and additions
        for (const auto& pair : currentFiles) {
            const std::string& filepath = pair.first;
            const std::string& currentHash = pair.second;

            if (fileHashes.find(filepath) == fileHashes.end()) {
                std::cout << "[NEW] " << filepath << "\n";
                added++;
            } else if (fileHashes[filepath] != currentHash) {
                std::cout << "[MODIFIED] " << filepath << "\n";
                std::cout << "  Old hash: " << fileHashes[filepath] << "\n";
                std::cout << "  New hash: " << currentHash << "\n";
                modified++;
            } else {
                unchanged++;
            }
        }

        // Check for deletions
        for (const auto& pair : fileHashes) {
            if (currentFiles.find(pair.first) == currentFiles.end()) {
                std::cout << "[DELETED] " << pair.first << "\n";
                deleted++;
            }
        }

        // Update database with current state
        fileHashes = currentFiles;

        std::cout << std::string(60, '=') << "\n";
        std::cout << "Summary:\n";
        std::cout << "  Unchanged: " << unchanged << "\n";
        std::cout << "  Modified:  " << modified << "\n";
        std::cout << "  Added:     " << added << "\n";
        std::cout << "  Deleted:   " << deleted << "\n";
        std::cout << std::string(60, '=') << "\n\n";

        if (modified > 0 || added > 0 || deleted > 0) {
            saveDatabase();
        } else {
            std::cout << "No changes detected. Database not updated.\n\n";
        }
    }

    // Display all monitored files
    void listMonitoredFiles() {
        std::cout << "\n[MONITORED FILES]\n";
        std::cout << std::string(60, '-') << "\n";
        
        if (fileHashes.empty()) {
            std::cout << "No files are currently being monitored.\n";
        } else {
            for (const auto& pair : fileHashes) {
                std::cout << pair.first << "\n  Hash: " << pair.second << "\n";
            }
        }
        std::cout << std::string(60, '-') << "\n";
        std::cout << "Total: " << fileHashes.size() << " files\n\n";
    }
};

void printHelp() {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        File Integrity Monitoring Tool v1.0             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    std::cout << "Usage:\n";
    std::cout << "  ./fim init <directory>     - Initialize monitoring\n";
    std::cout << "  ./fim check <directory>    - Check for changes\n";
    std::cout << "  ./fim list                 - List monitored files\n";
    std::cout << "  ./fim help                 - Show this help\n\n";
    std::cout << "Example:\n";
    std::cout << "  ./fim init /path/to/directory\n";
    std::cout << "  ./fim check /path/to/directory\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::string command = argv[1];
    FileIntegrityMonitor fim;

    if (command == "init" && argc == 3) {
        std::string directory = argv[2];
        fim.initializeMonitoring(directory);
    } 
    else if (command == "check" && argc == 3) {
        std::string directory = argv[2];
        fim.checkIntegrity(directory);
    } 
    else if (command == "list") {
        fim.listMonitoredFiles();
    } 
    else if (command == "help") {
        printHelp();
    } 
    else {
        std::cout << "Invalid command or arguments.\n";
        printHelp();
        return 1;
    }

    return 0;
}
