//
// Created by James Forman on 3/12/26.
//

#ifndef CLITASKMANAGER_STORAGE_H
#define CLITASKMANAGER_STORAGE_H

#include "Task.h"
#include <vector>
#include <optional>

extern "C" {
#include "sqlite3.h"
}

class Storage {
public:
	explicit Storage(std::string& dbPath);
	~Storage();

	void createTable(std::string& dbPath);

	//CRUD
	long addTask(Task& task, std::string db = "");
	bool updateTask(const Task& task);
	bool deleteTask(long id);

	//Read
	std::vector<Task> getAllTasks();
	std::optional<Task> getTaskById(long id);

	//Move
	void moveCompletedToNewTable(std::string newTablePath);
	void moveTaskToNewTable(long id, const std::string& newTablePath);

	// Raw access
	sqlite3* get() { return db_; }
	std::vector<std::string> showTables();
	void changeTable(std::string newTablePath);
	void deleteTable(const std::string &tablePath);

private:
	sqlite3* db_ = nullptr;
	std::string dbName_;
};


#endif //CLITASKMANAGER_STORAGE_H
