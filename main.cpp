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
	EXIT = 6
};

void printTasks(const std::vector<Task>& tasks) {
	size_t index = 1;
	std::cout << "\n";
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "|                Task List                       |" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
	for (const auto& task : tasks) {
		constexpr int WIDTH = 50;
		std::cout << "|  " << std::left << std::setw(WIDTH - 4) <<
			(std::to_string(index) + ": " + task.title + (task.isCompleted() ? " [Done]" : "")) << "|" << std::endl;
		index++;
	}
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "\n\n";
}

int main() {
	std::vector<Task> tasks = Storage::loadTasks();
	std::vector<Task> completedTasks;
	int choice;
	int idCounter = 1;

	while (true) {
		printTasks(tasks);
		std::cout << "\n1. Add Task\n2. Update Task\n3. Remove Task\n4. Complete\n5. Transfer Completed Tasks to Save File\n6. Exit\n";
		std::cout << "Enter Choice: ";

		if (!(std::cin >> choice)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input!\n";
			continue;
		}

		switch (static_cast<MenuOptions>(choice)) {
			case MenuOptions::ADD: {
				idCounter = TaskManager::addTask(tasks, idCounter);
				break;
			}
			case MenuOptions::UPDATE: {
				TaskManager::updateTask(tasks);
				break;
			}
			case MenuOptions::REMOVE: {
				TaskManager::deleteTask(tasks);
				break;
			}
			case MenuOptions::COMPLETE: {
				TaskManager::completeTask(tasks);
				break;
			}
			case MenuOptions::MOVECOMPLETED: {
				completedTasks = TaskManager::moveCompleted(tasks);
				break;
			}
			case MenuOptions::EXIT: {
				Storage::saveTasks(tasks, "tasks.txt", true);
				if (!completedTasks.empty()) {
					Storage::saveTasks(completedTasks, "completedTasks.txt", false);
				}
				// Exit
				return 0;
			}
			default: {
				std::cout << "\nInvalid choice!" << std::endl;
				break;
			}
		}
	}

	return 0;
}