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

	// Table management
	bool moveCompletedToNewTable(const std::string &newTablePath);
	std::vector<std::string> showTables() { return storage_.showTables(); }
	void changeTable(std::string newTablePath);
	void deleteTable(const std::string &tablePath) { storage_.deleteTable(tablePath); }

	// convenience: return optional single task
	std::optional<Task> getTask(long id);

private:
	Storage& storage_;
};