//
// Created by James Forman on 3/12/26.
//

#include "Storage.h"
#include <stdexcept>
#include <chrono>
#include <iostream>

Storage::Storage(const std::string& dbPath) {
	if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
		std::string msg = db_ ? sqlite3_errmsg(db_) : "unknown";
		if (db_) sqlite3_close(db_);
		throw std::runtime_error("Failed to open DB: " + msg);
	}
	createTable();
}

Storage::~Storage() {
	if (db_) sqlite3_close(db_);
}

void Storage::createTable() {
	if (!db_) throw std::runtime_error("DB not opened");

	const char* sql =
		"CREATE TABLE IF NOT EXISTS tasks("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"title TEXT NOT NULL,"
		"completed INTEGER NOT NULL DEFAULT 0,"
		"completed_at INTEGER"
		");";

	char* err = nullptr;
	int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &err);
	if (rc != SQLITE_OK) {
		std::string msg = err ? err : "unknown";
		sqlite3_free(err);
		throw std::runtime_error("Error creating table: " + msg);
	}
}

long Storage::addTask(Task &task) {
	if (!db_) throw std::runtime_error("DB not opened");
	const char* sql = "INSERT INTO tasks (title, completed, completed_at) VALUES (?, ?, ?);";
	sqlite3_stmt *stmt = nullptr;

	int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
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
	const char* sql = "UPDATE tasks SET title = ?, completed = ?, completed_at = ? WHERE id = ?;";
	sqlite3_stmt *stmt = nullptr;
	int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
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
	const char* sql = "DELETE FROM tasks WHERE id = ?;";
	sqlite3_stmt *stmt = nullptr;
	if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) { return false; }
	sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(id));
	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return rc == SQLITE_DONE;
}

std::vector<Task> Storage::getAllTasks() {
	std::vector<Task> out;
	const char* sql = "SELECT id, title, completed, completed_at FROM tasks ORDER BY id;";
	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cout << "There was an issue fetching tasks\n";
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
	const char* sql = "SELECT id, title, completed, completed_at FROM tasks WHERE id = ? LIMIT 1;";
	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
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