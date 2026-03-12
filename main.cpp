#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Task.h"
#include <limits>
#include <iomanip>

enum class MenuOptions {
	ADD = 1,
	UPDATE = 2,
	REMOVE = 3,
	COMPLETE = 4,
	EXIT = 5,
};

std::vector<Task> loadTasks() {
	std::ifstream file("tasks.txt");
	std::vector<Task> tasks;
	std::string line;

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		Task task;
		std::string completedStr;

		std::getline(ss, line, ',');
		task.id = std::stoi(line);

		std::getline(ss, task.title, ',');

		std::getline(ss, completedStr);
		task.completed = std::stoi(completedStr);

		tasks.push_back(task);
	}

	file.close();
	return tasks;
}

void saveTasks(std::vector<Task>& tasks) {
	std::ofstream outfile("tasks.txt");

	for (const auto& task : tasks) {
		outfile << task.id <<
			"," << task.title <<
				"," << task.completed << "\n";
	}

	outfile.close();
}

void printTasks(std::vector<Task>& tasks) {
	size_t index = 1;
	std::cout << "\n";
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "|                Task List                       |" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
	for (const auto& task : tasks) {
		constexpr int WIDTH = 50;
		std::cout << "|  " << std::left << std::setw(WIDTH - 4) <<
			(std::to_string(index) + ": " + task.title + (task.completed ? " [Done]" : "")) << "|" << std::endl;
		index++;
	}
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "\n\n";
}

int main() {
	std::vector<Task> tasks = loadTasks();
	int choice;
	int idCounter = 1;

	while (true) {
		printTasks(tasks);
		std::cout << "\n1. Add Task\n2. Update Task\n3. Remove Task\n4. Complete\n5. Exit\n";
		std::cout << "Enter Choice: ";

		if (!(std::cin >> choice)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid input!\n";
			continue;
		}

		switch (static_cast<MenuOptions>(choice)) {
			case MenuOptions::ADD: {
				std::string title;
				std::cout << "Enter title: ";
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::getline(std::cin, title);

				tasks.emplace_back(idCounter++, title);
				break;
			}
			case MenuOptions::UPDATE: {
				int updateChoice;
				std::cout << "Please enter the number of the task you would like to update : ";
				if (!(std::cin >> updateChoice)) {
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input!\n";
					continue;
				}

				if (updateChoice > tasks.size()) {
					std::cout << "That is not a valid choice for this task!\n";
				}else {
					std::cout << "What would you like to change the task to?\n";
					std::cout << "Answer : ";
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::getline(std::cin, tasks[updateChoice - 1].title);
				}
				break;
			}
			case MenuOptions::REMOVE: {
				int removeChoice;
				std::cout << "Please enter the number of the task you would like to remove : ";
				if (!(std::cin >> removeChoice)) {
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input!\n";
					continue;
				}

				if (removeChoice > tasks.size()) {
					std::cout << "That is not a valid choice for this task!\n";
				}else {
					tasks.erase(tasks.begin() + removeChoice - 1);
					std::cout << "The task was removed successfully!\n";
				}
				break;
			}
			case MenuOptions::COMPLETE: {
				int index = 0;
				std::cout << "Please select which task you would like to complete : ";
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				if (!(std::cin >> index)){
					std::cin.clear();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					std::cout << "Invalid input!\n";
				}
				if (index < tasks.size()) {
					tasks[index - 1].completed = true;
				}else {
					std::cout << "There is no task of that value\n";
				}

				break;
			}
			case MenuOptions::EXIT: {
				saveTasks(tasks);
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