//
// Created by James Forman on 3/10/26.
//

#pragma once // Testing instead of the standard #ifndef
#include <string>
#include <chrono>
#include <optional>

#include "sqlite3.h"

class Task {
public:
	Task();
	explicit Task(std::string title);
	Task(std::string title, std::chrono::system_clock::time_point completedTime);

	// id handling
	[[nodiscard]] long getId() const noexcept { return id_; }
	void setId(long id) noexcept { id_ = id; }

	// title access
	[[nodiscard]] const std::string& getTitle() const noexcept { return title_; }
	void setTitle(std::string t) noexcept { title_ = std::move(t); }

	void markCompleted();

	//Sets completion time to now
	void setCompletedTime();
	// Choose date for completion time
	void setCompletedTime(std::chrono::system_clock::time_point tp);

	bool isCompleted() const noexcept { return completed_; }
	bool hasCompletedTime() const noexcept { return completedTime_.has_value(); }

	//Returns string for Date:Time (empty if none)
	std::string getFormattedCompleteTime() const;

	std::optional<std::chrono::system_clock::time_point> getCompletedTime() const noexcept { return completedTime_; };


	static Task fromSqliteRow(sqlite3_stmt* stmt);
private:
	long id_{-1}; // not persisted yet
	std::string title_;
	bool completed_{false};
	std::optional<std::chrono::system_clock::time_point> completedTime_;
};
