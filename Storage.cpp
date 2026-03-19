//
// Created by James Forman on 3/12/26.
//

#include "Storage.h"
#include <stdexcept>
#include <chrono>
#include <iostream>

Storage::Storage(std::string& dbPath) {
	if (dbPath.empty()) {
		dbPath = "tasks.db";
	}

	if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
		std::string msg = db_ ? sqlite3_errmsg(db_) : "unknown";
		if (db_) sqlite3_close(db_);
		throw std::runtime_error("Failed to open DB: " + msg);
	}
	createTable(dbPath);
	dbName_ = dbPath;
}

Storage::~Storage() {
	if (db_) sqlite3_close(db_);
}

bool isValid(std::string dbPath) {
	return std::all_of(dbPath.begin(), dbPath.end(), [](char c) {
		return std::isalnum(c) || c == '_';
	});
}

void Storage::createTable(std::string& dbPath) {
	if (!db_) throw std::runtime_error("DB not opened");

	// Remove .db extension if present for table name
	size_t pos = dbPath.rfind(".db");
	if (pos != std::string::npos && pos == dbPath.length() - 3) {
		dbPath = dbPath.substr(0, pos);
	}

	if (dbPath.empty()) {
		dbPath = "tasks";
	}else if (!isValid(dbPath)) {
		throw std::runtime_error("Invalid database path: " + dbPath);
	}

	std::string sql =
		"CREATE TABLE IF NOT EXISTS \"" + dbPath + "\" ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"title TEXT NOT NULL,"
		"completed INTEGER NOT NULL DEFAULT 0,"
		"completed_at INTEGER"
		");";

	char* err = nullptr;
	int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
	if (rc != SQLITE_OK) {
		std::string msg = err ? err : "unknown";
		sqlite3_free(err);
		throw std::runtime_error("Error creating table: " + msg);
	}
}

long Storage::addTask(Task &task, std::string db) {
	if (!db_) throw std::runtime_error("DB not opened");

	if (db == ""){
		db = dbName_;
	}else if(!isValid(db)) {
		throw std::runtime_error("Invalid database name to save to : " + db);
	}

	std::string sql = "INSERT INTO \"" + db + "\" (title, completed, completed_at) VALUES (?, ?, ?);";
	sqlite3_stmt *stmt = nullptr;

	int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db_));
	}

	sqlite3_bind_text(stmt, 1, task.getTitle().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 2, task.isCompleted() ? 1 : 0);
	if (task.hasCompletedTime()) {
		auto duration = task.getCompletedTime()->time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(millis));
	}else {
		sqlite3_bind_null(stmt, 3);
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		std::string msg = sqlite3_errmsg(db_);
		sqlite3_finalize(stmt);
		throw std::runtime_error("Insert failed : " + msg);
	}
	sqlite3_finalize(stmt);
	long id = static_cast<long>(sqlite3_last_insert_rowid(db_));
	task.setId(id);
	return id;
}

bool Storage::updateTask(const Task &task) {
	if (task.getId() <= 0) return false;
	std::string sql = "UPDATE \"" + dbName_ + "\" SET title = ?, completed = ?, completed_at = ? WHERE id = ?;";
	sqlite3_stmt *stmt = nullptr;
	int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		return false;
	}

	sqlite3_bind_text(stmt, 1, task.getTitle().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 2, task.isCompleted() ? 1 : 0);

	if (task.hasCompletedTime()) {
		auto duration = task.getCompletedTime()->time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(millis));
	}else {
		sqlite3_bind_null(stmt, 3);
	}

	sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(task.getId()));

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return rc == SQLITE_DONE;
}

bool Storage::deleteTask(long id) {
	std::string sql = "DELETE FROM \"" + dbName_ + "\" WHERE id = ?;";
	sqlite3_stmt *stmt = nullptr;
	if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) { return false; }
	sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(id));
	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return rc == SQLITE_DONE;
}

std::vector<Task> Storage::getAllTasks() {
	std::vector<Task> out;
	std::string sql = "SELECT id, title, completed, completed_at FROM \"" + dbName_ + "\" ORDER BY id;";
	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		std::cout << "There was an issue fetching all tasks\n";
		throw std::runtime_error(sqlite3_errmsg(db_));
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		Task t = Task::fromSqliteRow(stmt);
		out.push_back(std::move(t));
	}

	sqlite3_finalize(stmt);
	return out;
}

std::optional<Task> Storage::getTaskById(long id) {
	std::string sql = "SELECT id, title, completed, completed_at FROM \"" + dbName_ + "\" WHERE id = ? LIMIT 1;";
	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return std::nullopt;
	}
	sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(id));
	std::optional<Task> result;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		result = Task::fromSqliteRow(stmt);
	}
	sqlite3_finalize(stmt);
	return result;
}

void Storage::moveCompletedToNewTable(std::string newTablePath) {
	std::vector<Task> savedTasks;
	if (newTablePath.empty()) {
		throw std::runtime_error("Empty newDbPath");
	}

	std::string sql = "SELECT id, title, completed, completed_at FROM \"" + dbName_ + "\" WHERE completed = 1 ORDER BY completed_at;";
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cout << "There was an issue fetching tasks while saving\n";
		throw std::runtime_error(sqlite3_errmsg(db_));
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		Task task = Task::fromSqliteRow(stmt);
		savedTasks.push_back(task);
	}
	sqlite3_finalize(stmt);

	createTable(newTablePath);

	for (const Task task : savedTasks) {
		addTask(const_cast<Task&>(task), newTablePath);
		std::cout << "Task added to table : " << task.getTitle() << "\n";
	}

	for (const Task task : savedTasks) {
		if (!deleteTask(task.getId())) {
			throw std::runtime_error("Unable to delete task: " + std::to_string(task.getId()));
		}
	}
}

std::vector<std::string> Storage::showTables() {
	std::vector<std::string> out;
	const char* sql = "SELECT name FROM sqlite_master WHERE type='table';";
	sqlite3_stmt* stmt;
	const char* blackList = "sqlite_sequence";

	sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		const unsigned char* tableName = sqlite3_column_text(stmt, 0);
		if (strcmp(blackList, reinterpret_cast<const char*>(tableName)) != 0) {
			out.emplace_back(reinterpret_cast<const char*>(tableName));
		}
	}
	sqlite3_finalize(stmt);

	return out;
}

void Storage::changeTable(std::string newTablePath) {
	if (newTablePath.empty()) {
		throw std::runtime_error("Empty Table Path");
	}

	if (isValid(newTablePath)) {
		dbName_ = newTablePath;
	}else {
		throw std::runtime_error("Invalid Table Path");
	}
}
