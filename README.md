# File Integrity Monitoring (FIM) Tool 🔒

A simple yet effective File Integrity Monitoring tool written in C++ that helps detect unauthorized changes to files in a directory.

## Features ✨

- **Hash-based Monitoring**: Uses MD5 hashing to detect file changes
- **Recursive Directory Scanning**: Monitors all files in a directory and subdirectories
- **Change Detection**: Identifies modified, added, and deleted files
- **Persistent Database**: Stores file hashes for comparison across sessions
- **Easy to Use**: Simple command-line interface

## How It Works 🔧

The FIM tool calculates and stores MD5 hashes of files in a monitored directory. On subsequent checks, it compares current file hashes with stored values to detect:
- **Modified files**: Files whose content has changed
- **New files**: Files that have been added to the directory
- **Deleted files**: Files that have been removed from the directory

## Requirements 📋

- C++ compiler with C++17 support (g++, clang++)
- OpenSSL library (for MD5 hashing)

## Installation 🚀

### Linux/MacOS

1. Install OpenSSL development library:
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# MacOS
brew install openssl
```

2. Clone the repository:
```bash
git clone https://github.com/yourusername/file-integrity-monitor.git
cd file-integrity-monitor
```

3. Compile the program:
```bash
# Linux
g++ -std=c++17 file_integrity_monitor.cpp -o fim -lssl -lcrypto

# MacOS (if you have issues, specify OpenSSL path)
g++ -std=c++17 file_integrity_monitor.cpp -o fim -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include
```

### Windows

1. Install OpenSSL from https://slproweb.com/products/Win32OpenSSL.html
2. Use MinGW or Visual Studio to compile
3. Make sure to link against OpenSSL libraries

## Usage 💻

### Initialize Monitoring

First, initialize the monitoring for a directory:
```bash
./fim init /path/to/directory
```

This will:
- Scan all files in the directory
- Calculate MD5 hashes for each file
- Store hashes in `fim_database.txt`

### Check for Changes

To check if any files have been modified:
```bash
./fim check /path/to/directory
```

This will:
- Scan the directory again
- Compare current hashes with stored hashes
- Report any changes detected
- Update the database with current state

### List Monitored Files

To see all currently monitored files:
```bash
./fim list
```

### Help

To see usage information:
```bash
./fim help
```

## Example Output 📊

### Initialization
```
[INITIALIZE] Scanning directory: /home/user/documents
------------------------------------------------------------
✓ Added: /home/user/documents/file1.txt
✓ Added: /home/user/documents/file2.pdf
✓ Added: /home/user/documents/subfolder/file3.doc
------------------------------------------------------------
Total files initialized: 3

Database saved with 3 files.
```

### Integrity Check
```
[INTEGRITY CHECK] Sun Feb 01 10:30:45 2026
============================================================
[MODIFIED] /home/user/documents/file1.txt
  Old hash: 5d41402abc4b2a76b9719d911017c592
  New hash: 7d793037a0760186574b0282f2f435e7
[NEW] /home/user/documents/newfile.txt
[DELETED] /home/user/documents/file2.pdf
============================================================
Summary:
  Unchanged: 1
  Modified:  1
  Added:     1
  Deleted:   1
============================================================

Database saved with 3 files.
```

## Use Cases 🎯

- **Security Monitoring**: Detect unauthorized changes to critical files
- **System Administration**: Monitor configuration files for changes
- **Compliance**: Ensure file integrity for audit requirements
- **Development**: Track changes in project directories
- **Learning**: Great project for understanding file operations and hashing

## Project Structure 📁

```
file-integrity-monitor/
├── file_integrity_monitor.cpp    # Main source code
├── README.md                      # This file
├── Makefile                       # Build automation (optional)
├── fim_database.txt              # Auto-generated hash database
└── .gitignore                    # Git ignore file
```

## Technical Details 🔬

- **Language**: C++17
- **Hash Algorithm**: MD5 (fast and sufficient for integrity checking)
- **File I/O**: Standard C++ file streams
- **Directory Traversal**: C++17 filesystem library
- **Data Storage**: Simple text-based database

## Limitations ⚠️

- MD5 is not cryptographically secure (but sufficient for integrity monitoring)
- Large files may take time to hash
- Database is plain text (could be enhanced with encryption)
- No real-time monitoring (scheduled or manual checks only)

## Future Enhancements 🚀

- [ ] Add SHA-256 hashing option for better security
- [ ] Implement real-time monitoring with inotify/fswatch
- [ ] Add email/log alerts for detected changes
- [ ] Create a GUI version
- [ ] Add configuration file support
- [ ] Support for file exclusion patterns
- [ ] Encrypted database storage

## Contributing 🤝

Contributions are welcome! Feel free to:
- Report bugs
- Suggest new features
- Submit pull requests

## License 📄

This project is open source and available under the MIT License.

## Author ✍️

Created as a learning project for file system operations and cryptographic hashing in C++.

## Acknowledgments 🙏

- OpenSSL for cryptographic functions
- C++ Standard Library for filesystem operations

---

**Note**: This is an educational project. For production use, consider using established FIM solutions like AIDE, Tripwire, or OSSEC.
