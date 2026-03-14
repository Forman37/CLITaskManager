//
// Created by James Forman on 3/12/26.
//

#include "TaskManager.h"

#include <iostream>
#include <vector>

int TaskManager::addTask(std::vector<Task>& tasks, int idCounter) {
	std::string title;
	std::cout << "Enter title: ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(std::cin, title);

	tasks.emplace_back(idCounter++, title);

	return idCounter;
}

void TaskManager::deleteTask(std::vector<Task>& tasks) {
	int removeChoice;
	std::cout << "Please enter the number of the task you would like to remove : ";
	if (!(std::cin >> removeChoice)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input!\n";
		return;
	}

	if (removeChoice > tasks.size()) {
		std::cout << "That is not a valid choice for this task!\n";
	}else {
		tasks.erase(tasks.begin() + removeChoice - 1);
		std::cout << "The task was removed successfully!\n";
	}
}

void TaskManager::updateTask(std::vector<Task>& tasks) {
	int updateChoice;
	std::cout << "Please enter the number of the task you would like to update : ";
	if (!(std::cin >> updateChoice)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input!\n";
		return;
	}

	if (updateChoice > tasks.size()) {
		std::cout << "That is not a valid choice for this task!\n";
	}else {
		std::cout << "What would you like to change the task to?\n";
		std::cout << "Answer : ";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::getline(std::cin, tasks[updateChoice - 1].title);
	}

}

void TaskManager::completeTask(std::vector<Task>& tasks) {
	int index = 0;
	std::cout << "Please select which task you would like to complete : ";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	if (!(std::cin >> index)){
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "Invalid input!\n";
	}
	std::cout << "Index : " << index << "   Task size : " << tasks.size() << "\n";
	if (index <= tasks.size()) {
		tasks[index - 1].markCompleted();
	}else {
		std::cout << "There is no task of that value\n";
	}
}

std::vector<Task> TaskManager::moveCompleted(std::vector<Task> &tasks) {
	std::vector<Task> completedTasks;
	std::vector<int> completedIndexes;

	for (int i = 0; i < tasks.size(); i++) {
		if (tasks[i].isCompleted()) {
			completedTasks.emplace_back(tasks[i]);
		}
	}

	std::erase_if(tasks, [](const Task& task) { return task.isCompleted(); });
	return completedTasks;
}
