#include "TaskManager.h"
#include <chrono>
#include <iostream>

TaskManager::TaskManager(Storage& storage) : storage_(storage) {}

Task TaskManager::createTask(const std::string& title) {
	Task t(title);

	// not completed by default; storage.addTask will set id
	storage_.addTask(t);
	return t; // id set in t
}

std::vector<Task> TaskManager::listTasks() {
	return storage_.getAllTasks();
}

bool TaskManager::updateTitle(long id, const std::string& newTitle) {
	auto maybe = storage_.getTaskById(id);
	if (!maybe.has_value()) return false;
	Task t = *maybe;
	t.setTitle(newTitle);
	return storage_.updateTask(t);
}

bool TaskManager::markCompleted(long id) {
	auto maybe = storage_.getTaskById(id);
	if (!maybe.has_value()) return false;
	Task t = *maybe;
	t.markCompleted(); // sets time if needed
	return storage_.updateTask(t);
}

bool TaskManager::removeTask(long id) {
	return storage_.deleteTask(id);
}

std::optional<Task> TaskManager::getTask(long id) {
	return storage_.getTaskById(id);
}