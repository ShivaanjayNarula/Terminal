# macOS Terminal Simulator (C)

A lightweight macOS-style terminal simulator written in C that mimics basic shell commands such as `ls`, `cd`, `mkdir`, `touch`, `rm`, `cat`, `edit`, and more using a custom tree-based virtual filesystem synced with the real local filesystem.

---

## Features

* **macOS-Style Terminal Prompt**: Interactive command-line prompt reflecting current directory pathing.
* **Modular Codebase**: Highly organized into clear source and header modules separating CLI, virtual filesystem, and generic utilities.
* **On-Demand VFS Lazy Loading**: Efficiently crawls and synchronizes local filesystem directories only when they are accessed.
* **Access Diagnostics**: Provides standard POSIX warnings when trying to traverse permission-restricted directories.
* **Colored Terminal Output**: Enhanced readability using tailored ANSI color palettes.
* **Simulated File Permission Engine**: Interactive `chmod` command to control simulated node permissions.
* **Built-in Interactive Editor**: Supports editing virtual file contents on the fly.
* **Command History**: Tracks and displays history of entered commands.

---

# Supported Commands

| Command               | Description                   |
| --------------------- | ----------------------------- |
| `help`                | Show all commands             |
| `ls`                  | List files and folders        |
| `tree`                | Display directory tree        |
| `pwd`                 | Show current directory        |
| `cd <dir>`            | Change directory              |
| `mkdir <dir>`         | Create directory              |
| `touch <file>`        | Create file                   |
| `rm <path>`           | Remove file or directory      |
| `cat <file>`          | Display file content          |
| `edit <file>`         | Edit file content             |
| `chmod <mode> <file>` | Change simulated permissions  |
| `history`             | Show command history          |
| `clear`               | Clear terminal                |
| `finder`              | Open current folder in Finder |
| `exit`                | Exit simulator                |

---

# Project Structure

The codebase is refactored into modular components for clean separation of concerns:

```bash
├── common.h         # Color ANSI escape constants and global configuration
├── utils.h          # String utility and timestamp declarations
├── utils.c          # Tokenizer functions, path splitting/joining, and timer implementation
├── fs.h             # Virtual Filesystem (VFS) declarations and tree structure
├── fs.c             # Core VFS logic, dynamic lazy loading, and POSIX sync implementation
├── main.c           # Front-end CLI program entry point and interactive shell loop
├── Makefile         # Compiler and linker rules for gcc
└── README.md        # Technical project documentation
```

---

# 🚀 Step-by-Step Guide: Compile & Run

Follow these commands in your standard system terminal to build and run the project:

### 1. Clone the repository
   ```sh
   git clone https://github.com/ShivaanjayNarula/Terminal.git
   cd Terminal
   ```

### 2. Open Workspace
Make sure your terminal is located in the root directory of the project:
```bash
cd /Users/shivaanjaynarula/Documents/Projects/terminal
```

### 3. Compile the Project
Build the modular modules into the unified `terminal_simulator` binary:
```bash
make
```

### 4. Launch the Simulator
Start the interactive simulator shell:
```bash
./terminal_simulator
```

### 5. (Optional) Reset/Clean Build Outputs
To reset the build environment by removing compiled `.o` object files and the generated binary, run:
```bash
make clean
```

---

# 🧠 How It Works

### Virtual Filesystem (VFS) Tree Structure
The simulator maintains an in-memory, tree-based directory model using linked node structures:

```c
typedef struct TreeNode {
    char* name;
    char type;                // 'd' (directory) or '-' (file)
    int perm;                 // Simulated octal permission (0-7)
    char* cdate;              // Creation timestamp
    char* mdate;              // Modification timestamp
    StringArray content;      // Virtual file text line contents
    struct TreeNode* parent;
    struct TreeNode* child;
    struct TreeNode* link;
} TreeNode;
```

### On-Demand Lazy Loading (Dynamic Synchronization)
* **The Problem**: A full recursive crawl of `/` at startup would traverse millions of files, causing gigabytes of RAM usage, severe startup delays, or Out-Of-Memory system crashes.
* **The Solution**: On startup, the simulator loads a shallow skeleton of the filesystem up to a depth of `2` (`/`, `/Users`, and `/Users/<username>`). As you navigate (`cd`), list (`ls`), or manipulate paths (`mkdir`, `touch`), the core VFS dynamically invokes POSIX APIs (`opendir`, `readdir`) to scan and populate child directories on-demand under the corresponding tree nodes. This keeps startup instant while allowing full path exploration!

### macOS Directory Protection (Access Diagnostics)
Modern macOS versions restrict terminal applications from accessing personal folders (e.g., `Documents`, `Desktop`, `Downloads`) without explicit "Full Disk Access" permissions. 
If the simulator processes attempt to load a protected directory without these privileges, the VFS diagnoses the POSIX failure and prints an elegant warning:
```bash
Permission denied: Cannot access '/Users/shivaanjaynarula/Documents'
```
Publicly readable folders (like `Public`) are synchronized dynamically and can be listed without permission issues.

---

# Example Usage Flow

Run the following commands inside the simulator prompt:

```bash
user@terminal:/$ cd Users/shivaanjaynarula/Public
user@terminal:/Users/shivaanjaynarula/Public$ ls
drwx Drop Box/
user@terminal:/Users/shivaanjaynarula/Public$ mkdir test_directory
user@terminal:/Users/shivaanjaynarula/Public$ cd test_directory
user@terminal:/Users/shivaanjaynarula/Public/test_directory$ touch note.txt
user@terminal:/Users/shivaanjaynarula/Public/test_directory$ edit note.txt
# Enter text and type \n to save
user@terminal:/Users/shivaanjaynarula/Public/test_directory$ cat note.txt
user@terminal:/Users/shivaanjaynarula/Public/test_directory$ tree
user@terminal:/Users/shivaanjaynarula/Public/test_directory$ cd ..
user@terminal:/Users/shivaanjaynarula/Public$ rm test_directory/note.txt
user@terminal:/Users/shivaanjaynarula/Public$ rm test_directory
user@terminal:/Users/shivaanjaynarula/Public$ exit
```

---

# Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

---

# License
This project is licensed under the [GNU Affero General Public License v3.0](LICENSE).

---
