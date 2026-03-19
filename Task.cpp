//
// Created by James Forman on 3/10/26.
//
#include "Task.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cassert>
#include <iostream>

extern "C" {
#include "sqlite3.h"
}

Task::Task() = default;
Task::Task(std::string title) : title_(std::move(title)), completed_(false), completedTime_(std::nullopt) {}
Task::Task(std::string title, std::chrono::system_clock::time_point completedTime) : title_(std::move(title)), completed_(true), completedTime_(completedTime) {}

void Task::markCompleted() {
	if (!completed_) {
		completed_ = true;
	}
	if (!completedTime_.has_value()) {
		setCompletedTime();
	}
}

void Task::setCompletedTime() {
	completedTime_ = std::chrono::system_clock::now();
	completed_ = true;
}

void Task::setCompletedTime(std::chrono::system_clock::time_point tp) {
	completedTime_ = tp;
	completed_ = true;
}

std::string Task::getFormattedCompleteTime() const {
	if (!completedTime_.has_value()) return std::string();

	//Convert time_point to time_t
	std::time_t t = std::chrono::system_clock::to_time_t(*completedTime_);

	// Convert to local tm in a thread-safe manner : no shared memory
	std::tm local_tm;
#ifdef _WIN32
	localtime_s(&local_tm, &t);
#else
	localtime_r(&t, &local_tm);
#endif

	std::ostringstream oss;
	oss << std::put_time(&local_tm, "%m/%d/%Y %H:%M");
	return oss.str();
}

// Returns a task from a sqlite3_stmt row
// sqlite3_column_x API extracts values
Task Task::fromSqliteRow(sqlite3_stmt* stmt) {
	Task t;

	// id
	t.id_ = static_cast<long>(sqlite3_column_int64(stmt, 0));

	//title
	const unsigned char* text = sqlite3_column_text(stmt, 1);
	t.title_ = text ? reinterpret_cast<const char*>(text) : std::string();

	// completed
	t.completed_ = sqlite3_column_int(stmt, 2) != 0;

	// completed at
	if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
		sqlite3_int64 ms = sqlite3_column_int64(stmt, 3);
		auto tp = std::chrono::system_clock::time_point{ std::chrono::milliseconds { static_cast<int64_t>(ms) } };
		t.completedTime_ = tp;
	}else {
		t.completedTime_.reset();
	}
	return t;
}
