#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iomanip>

// Simple JSON manipulation without external libraries
class Task {
public:
    int id;
    std::string title;
    std::string description;
    std::string status; // "todo", "in_progress", "done"
    std::string created_at;
    std::string updated_at;

    Task(int id, const std::string& title, const std::string& description)
        : id(id), title(title), description(description), status("todo") {
        time_t now = time(nullptr);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        created_at = timestamp;
        updated_at = timestamp;
    }

    Task() : id(0), status("todo") {}

    std::string toJson() const {
        std::string json = "  {\n";
        json += "    \"id\": " + std::to_string(id) + ",\n";
        json += "    \"title\": \"" + escapeJsonString(title) + "\",\n";
        json += "    \"description\": \"" + escapeJsonString(description) + "\",\n";
        json += "    \"status\": \"" + status + "\",\n";
        json += "    \"created_at\": \"" + created_at + "\",\n";
        json += "    \"updated_at\": \"" + updated_at + "\"\n";
        json += "  }";
        return json;
    }

private:
    std::string escapeJsonString(const std::string& str) const {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
};

class TaskManager {
private:
    std::vector<Task> tasks;
    std::string filename;
    int next_id;

    bool loadTasks() {
        std::ifstream file(filename);
        if (!file) {
            // File doesn't exist, create empty task list
            tasks.clear();
            next_id = 1;
            return saveTasks(); // Create the file
        }

        std::string line, json;
        while (std::getline(file, line)) {
            json += line + "\n";
        }
        file.close();

        if (json.empty()) {
            tasks.clear();
            next_id = 1;
            return true;
        }

        return parseJson(json);
    }

    bool parseJson(const std::string& json) {
        tasks.clear();
        next_id = 1;

        size_t taskStart = json.find('{', json.find('['));
        if (taskStart == std::string::npos) {
            return false;
        }

        while (taskStart != std::string::npos) {
            size_t taskEnd = json.find('}', taskStart);
            if (taskEnd == std::string::npos) {
                break;
            }

            std::string taskJson = json.substr(taskStart, taskEnd - taskStart + 1);
            Task task;
            
            // Extract id
            size_t idPos = taskJson.find("\"id\":");
            if (idPos != std::string::npos) {
                idPos = taskJson.find(':', idPos) + 1;
                task.id = std::stoi(taskJson.substr(idPos, taskJson.find(',', idPos) - idPos));
                next_id = std::max(next_id, task.id + 1);
            }

            // Extract title
            size_t titlePos = taskJson.find("\"title\":");
            if (titlePos != std::string::npos) {
                titlePos = taskJson.find('\"', titlePos + 8) + 1;
                size_t titleEnd = taskJson.find('\"', titlePos);
                task.title = taskJson.substr(titlePos, titleEnd - titlePos);
            }

            // Extract description
            size_t descPos = taskJson.find("\"description\":");
            if (descPos != std::string::npos) {
                descPos = taskJson.find('\"', descPos + 14) + 1;
                size_t descEnd = taskJson.find('\"', descPos);
                task.description = taskJson.substr(descPos, descEnd - descPos);
            }

            // Extract status
            size_t statusPos = taskJson.find("\"status\":");
            if (statusPos != std::string::npos) {
                statusPos = taskJson.find('\"', statusPos + 9) + 1;
                size_t statusEnd = taskJson.find('\"', statusPos);
                task.status = taskJson.substr(statusPos, statusEnd - statusPos);
            }

            // Extract created_at
            size_t createdPos = taskJson.find("\"created_at\":");
            if (createdPos != std::string::npos) {
                createdPos = taskJson.find('\"', createdPos + 13) + 1;
                size_t createdEnd = taskJson.find('\"', createdPos);
                task.created_at = taskJson.substr(createdPos, createdEnd - createdPos);
            }

            // Extract updated_at
            size_t updatedPos = taskJson.find("\"updated_at\":");
            if (updatedPos != std::string::npos) {
                updatedPos = taskJson.find('\"', updatedPos + 13) + 1;
                size_t updatedEnd = taskJson.find('\"', updatedPos);
                task.updated_at = taskJson.substr(updatedPos, updatedEnd - updatedPos);
            }

            tasks.push_back(task);
            taskStart = json.find('{', taskEnd + 1);
        }

        return true;
    }

    bool saveTasks() {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Error: Could not open file " << filename << " for writing.\n";
            return false;
        }

        file << "{\n";
        file << "  \"tasks\": [\n";
        
        for (size_t i = 0; i < tasks.size(); ++i) {
            file << tasks[i].toJson();
            if (i < tasks.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
        return true;
    }

    void updateTaskTimestamp(int taskIndex) {
        time_t now = time(nullptr);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        tasks[taskIndex].updated_at = timestamp;
    }

public:
    TaskManager(const std::string& file) : filename(file), next_id(1) {
        loadTasks();
    }

    bool addTask(const std::string& title, const std::string& description) {
        Task task(next_id++, title, description);
        tasks.push_back(task);
        return saveTasks();
    }

    bool updateTask(int id, const std::string& title, const std::string& description) {
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == id) {
                tasks[i].title = title;
                tasks[i].description = description;
                updateTaskTimestamp(i);
                return saveTasks();
            }
        }
        return false;
    }

    bool deleteTask(int id) {
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == id) {
                tasks.erase(tasks.begin() + i);
                return saveTasks();
            }
        }
        return false;
    }

    bool markTaskAsInProgress(int id) {
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == id) {
                tasks[i].status = "in_progress";
                updateTaskTimestamp(i);
                return saveTasks();
            }
        }
        return false;
    }

    bool markTaskAsDone(int id) {
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == id) {
                tasks[i].status = "done";
                updateTaskTimestamp(i);
                return saveTasks();
            }
        }
        return false;
    }

    std::vector<Task> listAllTasks() const {
        return tasks;
    }

    std::vector<Task> listDoneTasks() const {
        std::vector<Task> doneTasks;
        for (const auto& task : tasks) {
            if (task.status == "done") {
                doneTasks.push_back(task);
            }
        }
        return doneTasks;
    }

    std::vector<Task> listNotDoneTasks() const {
        std::vector<Task> notDoneTasks;
        for (const auto& task : tasks) {
            if (task.status != "done") {
                notDoneTasks.push_back(task);
            }
        }
        return notDoneTasks;
    }

    std::vector<Task> listInProgressTasks() const {
        std::vector<Task> inProgressTasks;
        for (const auto& task : tasks) {
            if (task.status == "in_progress") {
                inProgressTasks.push_back(task);
            }
        }
        return inProgressTasks;
    }

    Task getTask(int id) const {
        for (const auto& task : tasks) {
            if (task.id == id) {
                return task;
            }
        }
        return Task();
    }
};

void displayTask(const Task& task) {
    std::cout << "ID: " << task.id << std::endl;
    std::cout << "Title: " << task.title << std::endl;
    std::cout << "Description: " << task.description << std::endl;
    std::cout << "Status: " << task.status << std::endl;
    std::cout << "Created: " << task.created_at << std::endl;
    std::cout << "Updated: " << task.updated_at << std::endl;
    std::cout << "----------------------------" << std::endl;
}

void displayTasks(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        std::cout << "No tasks found." << std::endl;
        return;
    }

    for (const auto& task : tasks) {
        displayTask(task);
    }
}

void displayHelp() {
    std::cout << "Task Tracker - A simple CLI for tracking tasks" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  tasktracker add \"Task Title\" \"Task Description\"" << std::endl;
    std::cout << "  tasktracker update <id> \"New Title\" \"New Description\"" << std::endl;
    std::cout << "  tasktracker delete <id>" << std::endl;
    std::cout << "  tasktracker start <id>   # Mark as in progress" << std::endl;
    std::cout << "  tasktracker finish <id>  # Mark as done" << std::endl;
    std::cout << "  tasktracker list         # List all tasks" << std::endl;
    std::cout << "  tasktracker list-done    # List completed tasks" << std::endl;
    std::cout << "  tasktracker list-todo    # List tasks not done" << std::endl;
    std::cout << "  tasktracker list-progress # List in-progress tasks" << std::endl;
    std::cout << "  tasktracker get <id>     # Show details of a specific task" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        displayHelp();
        return 1;
    }

    std::string command = argv[1];
    TaskManager manager("tasks.json");

    if (command == "add") {
        if (argc < 4) {
            std::cerr << "Error: 'add' requires a title and description.\n";
            return 1;
        }
        if (manager.addTask(argv[2], argv[3])) {
            std::cout << "Task added successfully.\n";
        } else {
            std::cerr << "Error: Failed to add task.\n";
            return 1;
        }
    } else if (command == "update") {
        if (argc < 5) {
            std::cerr << "Error: 'update' requires an id, new title, and new description.\n";
            return 1;
        }
        try {
            int id = std::stoi(argv[2]);
            if (manager.updateTask(id, argv[3], argv[4])) {
                std::cout << "Task updated successfully.\n";
            } else {
                std::cerr << "Error: Task not found or update failed.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid task ID.\n";
            return 1;
        }
    } else if (command == "delete") {
        if (argc < 3) {
            std::cerr << "Error: 'delete' requires a task ID.\n";
            return 1;
        }
        try {
            int id = std::stoi(argv[2]);
            if (manager.deleteTask(id)) {
                std::cout << "Task deleted successfully.\n";
            } else {
                std::cerr << "Error: Task not found or delete failed.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid task ID.\n";
            return 1;
        }
    } else if (command == "start") {
        if (argc < 3) {
            std::cerr << "Error: 'start' requires a task ID.\n";
            return 1;
        }
        try {
            int id = std::stoi(argv[2]);
            if (manager.markTaskAsInProgress(id)) {
                std::cout << "Task marked as in progress.\n";
            } else {
                std::cerr << "Error: Task not found or update failed.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid task ID.\n";
            return 1;
        }
    } else if (command == "finish") {
        if (argc < 3) {
            std::cerr << "Error: 'finish' requires a task ID.\n";
            return 1;
        }
        try {
            int id = std::stoi(argv[2]);
            if (manager.markTaskAsDone(id)) {
                std::cout << "Task marked as done.\n";
            } else {
                std::cerr << "Error: Task not found or update failed.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid task ID.\n";
            return 1;
        }
    } else if (command == "list") {
        std::cout << "All Tasks:\n";
        displayTasks(manager.listAllTasks());
    } else if (command == "list-done") {
        std::cout << "Completed Tasks:\n";
        displayTasks(manager.listDoneTasks());
    } else if (command == "list-todo") {
        std::cout << "Tasks Not Done:\n";
        displayTasks(manager.listNotDoneTasks());
    } else if (command == "list-progress") {
        std::cout << "Tasks In Progress:\n";
        displayTasks(manager.listInProgressTasks());
    } else if (command == "get") {
        if (argc < 3) {
            std::cerr << "Error: 'get' requires a task ID.\n";
            return 1;
        }
        try {
            int id = std::stoi(argv[2]);
            Task task = manager.getTask(id);
            if (task.id == id) {
                std::cout << "Task Details:\n";
                displayTask(task);
            } else {
                std::cerr << "Error: Task not found.\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid task ID.\n";
            return 1;
        }
    } else if (command == "help") {
        displayHelp();
    } else {
        std::cerr << "Error: Unknown command '" << command << "'.\n";
        displayHelp();
        return 1;
    }

    return 0;
}