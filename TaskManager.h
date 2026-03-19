#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Task.h"
#include "Storage.h"

class TaskManager {
public:
	explicit TaskManager(Storage& storage);

	// High-level operations (UI or main menu will call these)
	// createTask persists and returns the Task with id set
	Task createTask(const std::string& title);

	// return current tasks from DB
	std::vector<Task> listTasks();

	// update and persist by id
	bool updateTitle(long id, const std::string& newTitle);
	bool markCompleted(long id);
	bool removeTask(long id);

	// convenience: return optional single task
	std::optional<Task> getTask(long id);

private:
	Storage& storage_;
};