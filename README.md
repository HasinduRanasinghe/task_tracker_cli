# Task Tracker CLI

A simple command-line application to track and manage your tasks written in C++.



## Features

- Add, update, and delete tasks
- Mark tasks as in progress or done
- View tasks by different status filters (all, in progress, completed, not done)
- Get detailed information about specific tasks
- Persistent storage using JSON
- No external dependencies

## Screenshot

![tasktracker_cli](https://github.com/user-attachments/assets/0a313f3c-6959-49f3-a14c-4b55b9166554)



## Installation

### Prerequisites

- C++ compiler with C++11 support
- Make or CMake for building



### Building with CMake

```bash
# Clone the repository
git clone https://github.com/HasinduRanasinghe/task_tracker_cli.git
cd task_tracker_cli

# Create build directory
mkdir build && cd build

# Generate build files and compile
cmake ..
cmake --build .
```

## Usage

```bash
# Add a new task
./tasktracker add "Task name" "Task description"

# Mark a task as in progress
./tasktracker start <id>

# Mark a task as done
./tasktracker finish <id>

# Update a task
./tasktracker update <id> "New task name" "New task description"

# Delete a task
./tasktracker delete <id>

# List all tasks
./tasktracker list

# List tasks by status
./tasktracker list-done
./tasktracker list-todo
./tasktracker list-progress

# Get details about a specific task
./tasktracker get <id>

# Show help
./tasktracker help
```

## Commands

| Command | Description |
|---------|-------------|
| `add <title> <description>` | Add a new task |
| `update <id> <title> <description>` | Update an existing task |
| `delete <id>` | Delete a task |
| `start <id>` | Mark a task as in progress |
| `finish <id>` | Mark a task as done |
| `list` | List all tasks |
| `list-done` | List completed tasks |
| `list-todo` | List tasks not yet done |
| `list-progress` | List in-progress tasks |
| `get <id>` | Get details about a specific task |
| `help` | Show usage information |

## Project Structure

```
task_tracker_cli/
├── task_tracker.cpp        # Main source code
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
└── build/                  # Build directory (created during build)
    ├── tasktracker         # Executable file
    └── tasks.json          # Data storage (created when the app runs)
```

## Data Storage

Tasks are stored in a JSON file (`tasks.json`) in the build directory where the executable is run. The file is created automatically if it doesn't exist. Each task contains:

- ID (unique identifier)
- Title
- Description
- Status (todo, in_progress, or done)
- Creation timestamp
- Last update timestamp
