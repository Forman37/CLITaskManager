#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>

#include "Task.h"
#include "Storage.h"
#include "TaskManager.h"

enum class MenuOptions {
	ADD = 1,
	UPDATE = 2,
	REMOVE = 3,
	COMPLETE = 4,
	MOVECOMPLETED = 5,
	SHOWTABLES = 6,
	SWITCHTABLE = 7,
	DELETETABLE = 8,
	PRINTTASK = 9,
	MOVETASK = 10,
	EXIT = 11
};

void printChoices() {
	constexpr int SPACING = 4;
	std::cout << "1. Add Task" << std::string(SPACING, ' ')
			  << "2. Update Task" << std::string(SPACING, ' ')
		      << "3. Remove Task" << std::string(SPACING, ' ')
		      << "4. Complete" << std::string(SPACING, ' ')
	          << "5. Move completed tasks to a different table\n" //<< std::string(SPACING, ' ')
	          << "6. Show Tables" << std::string(SPACING, ' ')
			  << "7. Switch Tables" << std::string(SPACING, ' ')
			  << "8. Delete Table" << std::string(SPACING, ' ')
			  << "9. Print Task" << std::string(SPACING, ' ')
			  << "10. Move Task to Different Table" << std::string(SPACING, ' ')
			  << "11. Exit\n";
	std::cout << "Enter Choice: ";
}
void printTasks(const std::vector<Task>& tasks, std::string path) {
	constexpr int IDWIDTH = 8;
	constexpr int TITLEWIDTH = 60;
	constexpr int COMPLTEDWIDTH = 9;
	constexpr int COMPLETEDTIMEWIDTH = 20;

	constexpr int PADDING_BETWEEN = 1;

	constexpr int TOTALWIDTH = IDWIDTH + TITLEWIDTH + COMPLTEDWIDTH + COMPLETEDTIMEWIDTH + 4 + (PADDING_BETWEEN * 3); // +4 for the beginning and final '|' and the spaces around them

	auto printBorder = [&]() {
		std::cout << std::string(TOTALWIDTH, '-') << "\n";
	};

	// Header
	std::cout << "\n";
	printBorder();
	std::string header = " " + path + " table ";
	int paddingLeft = std::max(0, (TOTALWIDTH - static_cast<int>(header.size())) / 2 - 1);
	int paddingRight = std::max(0, TOTALWIDTH - static_cast<int>(header.size()) - paddingLeft - 2); // -2 for the '|'
	std::cout << "|" << std::string(paddingLeft, ' ') << header << std::string(paddingRight, ' ') << "|\n";
	printBorder();

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
	printBorder();
	std::cout << "\n";
}
std::vector<std::string> showTables(TaskManager& tm) {
	try {
		std::vector<std::string> tableNames = tm.showTables();
		int postedIndex = 1;
		std::cout << "\nTables:\n";
		for (const auto& tableName : tableNames) {
			std::cout << postedIndex << ". " << tableName << std::endl;
			postedIndex++;
		}
		std::cout << "\n";

		return tableNames;
	} catch (const std::exception& e) {
		std::cout << "Error showing tables: " << e.what() << "\n";
	}
	return std::vector<std::string>();
}

int main() {
	std::string dbPath;
	std::cout << "Please enter database name or press [Enter] for default : ";
	std::getline(std::cin, dbPath);

	size_t pos = dbPath.rfind(".db");
	if (pos == std::string::npos && dbPath.length() > 0) {
		dbPath = dbPath + ".db";
	}

	Storage storage(dbPath);
	TaskManager tm(storage);

	std::vector<Task> tasks = tm.listTasks();

	std::cout << "Opened Database Successfully\n";
	showTables(tm);

	int choice;
	while (true) {
		printTasks(tasks, dbPath);
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
					showTables(tm);
					std::string newDbPath;
					std::cout << "Enter new table name to move completed tasks to : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::getline(std::cin, newDbPath);
					tm.moveCompletedToNewTable(newDbPath);
					tasks = tm.listTasks(); // Refresh list after moving completed tasks
				} catch (const std::exception& e) {
					std::cout << "Error moving completed tasks: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::SHOWTABLES: {
				showTables(tm);
				break;
			}
			case MenuOptions::SWITCHTABLE: {
				try {
					std::vector<std::string> tableNames = showTables(tm);
					int chosenIndex = 0;

					std::cout << "\nPlease select the number of the table you wish to switch to : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cin >> chosenIndex;
					chosenIndex -= 1;

					if (chosenIndex < tableNames.size()) {
						tm.changeTable(tableNames[chosenIndex]);
						std::cout << "\nTable changed successfully.\n";
					}else {
						std::cout << "\nInvalid table choice!\n";
						break;
					}
					tasks = tm.listTasks(); // Refresh list after switching tables
					dbPath = tableNames[chosenIndex];
				}catch (const std::exception& e) {
					std::cerr << "Error switching tables: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::DELETETABLE: {
				try {
					std::vector<std::string> tableNames = showTables(tm);
					int chosenIndex = 0;

					std::cout << "\nPlease select the number of the table you wish to delete : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cin >> chosenIndex;
					chosenIndex -= 1;

					if (chosenIndex < tableNames.size()) {
						tm.deleteTable(tableNames[chosenIndex]);
					}else {
						std::cout << "\nInvalid table choice!\n";
						break;
					}
					tasks = tm.listTasks(); // Refresh list after deleting tables
				}catch (const std::exception& e) {
					std::cerr << "Error deleting tables: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::PRINTTASK: {
				try {
					long selectedTaskId;
					std::cout << "Which task id would you like to print the task for? ";
					std::cin >> selectedTaskId;
					std::optional<Task> t;
					t = tm.getTaskById(selectedTaskId);

					if (t.has_value()) {
						std::cout << "\nID(" << t->getId() << "):  " << t->getTitle() << std::endl;
					}else {
						std::cout << "\nThere was an error printing this task!\n";
					}
				}catch (std::exception& e) {
					std::cerr << "Error printing task: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::MOVETASK: {
				try {
					long taskOption;
					std::string newTablePath;

					std::cout << "Please enter the ID of the task you would like to move : ";
					std::cin >> taskOption;
					showTables(tm);
					std::cout << "Enter new table name to move completed tasks to : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::getline(std::cin, newTablePath);
					tm.moveTaskToNewTable(taskOption, newTablePath);
					tasks = tm.listTasks(); // Refresh list after moving completed tasks
				}catch (std::exception& e) {
					std::cerr << "Error moving completed tasks: " << e.what() << "\n";
				}
				break;
			}
			case MenuOptions::EXIT: {
				std::cout << "\nExiting...\n";
				return 0;
			}
			default: {
				std::cout << "\nInvalid choice!" << std::endl;
				break;
			}
		}
	}
}