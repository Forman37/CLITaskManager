#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include "sqlite3.h"

#include "Task.h"
#include "Storage.h"
#include "TaskManager.h"

enum class MenuOptions {
	ADD = 1,
	UPDATE = 2,
	REMOVE = 3,
	COMPLETE = 4,
	MOVECOMPLETED = 5,
	EXIT = 6
};

void printChoices() {
	constexpr int SPACING = 4;
	std::cout << "1. Add Task" << std::string(SPACING, ' ')
			  << "2. Update Task" << std::string(SPACING, ' ')
		      << "3. Remove Task\n"
		      << "4. Complete" << std::string(SPACING, ' ')
	          << "5. Transfer Completed Tasks to a different database" << std::string(SPACING, ' ')
			  << "6. Exit\n";
	std::cout << "Enter Choice: ";
}
void printTasks(const std::vector<Task>& tasks) {
	constexpr int IDWIDTH = 8;
	constexpr int TITLEWIDTH = 60;
	constexpr int COMPLTEDWIDTH = 9;
	constexpr int COMPLETEDTIMEWIDTH = 20;

	constexpr int PADDING_BETWEEN = 1;

	constexpr int TOTALWIDTH = IDWIDTH + TITLEWIDTH + COMPLTEDWIDTH + COMPLETEDTIMEWIDTH + 4 + (PADDING_BETWEEN * 3); // +4 for the beginning and final '|' and the spaces around them

	auto printBoarder = [&]() {
		std::cout << std::string(TOTALWIDTH, '-') << "\n";
	};

	// Header
	std::cout << "\n\n";
	printBoarder();
	std::string header = " Task List ";
	int paddingLeft = std::max(0, (TOTALWIDTH - static_cast<int>(header.size())) / 2 - 1);
	int paddingRight = std::max(0, TOTALWIDTH - static_cast<int>(header.size()) - paddingLeft - 2); // -2 for the '|'
	std::cout << "|" << std::string(paddingLeft, ' ') << header << std::string(paddingRight, ' ') << "|\n";
	printBoarder();

	// Content
	for (const auto& task : tasks) {
		std::string idField = "ID(" + std::to_string(task.getId()) + "): ";
		std::string titleField = task.getTitle();
		if (titleField.length() > static_cast<size_t>(TITLEWIDTH - 3)) { // -3 for "..."
			titleField = titleField.substr(0, TITLEWIDTH - 6) + "..."; // Truncate and add "..."
		}
		std::string completedField = (task.isCompleted() ? "[Done]" : "");
		std::string completedTimeField = task.getFormattedCompleteTime();

		std::cout << "| "
				  << std::left << std::setw(IDWIDTH) << idField << std::string(PADDING_BETWEEN, ' ')
				  << std::left << std::setw(TITLEWIDTH) << titleField << std::string(PADDING_BETWEEN, ' ')
				  << std::left << std::setw(COMPLTEDWIDTH) << completedField << std::string(PADDING_BETWEEN, ' ')
				  << std::left << std::setw(COMPLETEDTIMEWIDTH) << completedTimeField
				  << " |\n";
	}

	// Footer
	printBoarder();
	std::cout << "\n";
}

int main() {
	std::string dbPath;
	std::cout << "Please enter database name : ";
	std::getline(std::cin, dbPath);

	size_t pos = dbPath.rfind(".db");
	if (pos == std::string::npos && dbPath.length() > 0) {
		dbPath = dbPath + ".db";
	}

	Storage storage(dbPath);
	TaskManager tm(storage);

	std::vector<Task> tasks = tm.listTasks();

	std::cout << "Opened Database Successfully\n";

	int choice;
	while (true) {
		printTasks(tasks);
		printChoices();

		if (!(std::cin >> choice)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input!\n";
			continue;
		}

		switch (static_cast<MenuOptions>(choice)) {
			case MenuOptions::ADD: {
				try {
					std::string title;

					std::cout << "Title : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::getline(std::cin, title);
					tm.createTask(title);
					tasks = tm.listTasks(); // Refresh list after adding
				}catch (std::exception& e) {
					std::cout << "Error adding task : " << e.what() << std::endl;
				}
				break;
			}
			case MenuOptions::UPDATE: {
				try {
					long id;
					std::string newTitle;
					std::cout << "Which title would you like to update? Enter Task ID: ";
					if (!(std::cin >> id)) {
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						std::cout << "Invalid input!\n";
						break;
					}
					std::cout << "New title : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::getline(std::cin, newTitle);
					if (tm.updateTitle(id, newTitle)) {
						std::cout << "Task updated successfully.\n";
					}else {
						std::cout << "Failed to update task. Check if the ID is correct.\n";
					}
					tasks = tm.listTasks(); // Refresh list after updating
				}catch (const std::exception& e) {
					std::cout << "Error updating task: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::REMOVE: {
				try {
					long id;
					std::cout << "Enter Task ID to Remove: ";
					if (!(std::cin >> id)) {
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						std::cout << "Invalid input!\n";
						break;
					}
					if (tm.removeTask(id)) {
						std::cout << "Task removed successfully.\n";
					} else {
						std::cout << "Failed to remove task. Check if the ID is correct.\n";
					}
					tasks = tm.listTasks(); // Refresh list after removal
				}catch (const std::exception& e) {
					std::cerr << "Error removing task: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::COMPLETE: {
				try {
					long id;
					std::cout << "Enter Task ID to Mark as Completed: ";
					if (!(std::cin >> id)) {
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						std::cout << "Invalid input!\n";
						break;
					}
					if (tm.markCompleted(id)) {
						std::cout << "Task marked as completed successfully.\n";
					} else {
						std::cout << "Failed to mark task as completed. Check if the ID is correct.\n";
					}
					tasks = tm.listTasks(); // Refresh list after marking completed
				} catch (const std::exception& e) {
					std::cerr << "Error completing tasks: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::MOVECOMPLETED: {
				try {
					std::string newDbPath;
					std::cout << "Enter new database name to move completed tasks to: ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::getline(std::cin, newDbPath);
					tm.moveCompletedToNewDatabase(newDbPath);
					tasks = tm.listTasks(); // Refresh list after moving completed tasks
				} catch (const std::exception& e) {
					std::cout << "Error moving completed tasks: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::EXIT: {
				std::cout << "Exiting...\n";
				return 0;
			}
			default: {
				std::cout << "\nInvalid choice!" << std::endl;
				break;
			}
		}
	}
}