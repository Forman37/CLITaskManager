//
// Created by James Forman on 3/10/26.
//
#pragma once
#include <string>
#include <chrono>
#include <optional>

class Task {
public:
	int id;
	std::string title;

	Task(int id, std::string title);
	Task();
	Task(int id, std::string title, std::chrono::system_clock::time_point completedTime);

	void markCompleted();

	//Sets completion time to now
	void setCompletedTime();
	// Choose date for completion time
	void setCompletedTime(std::chrono::system_clock::time_point tp);

	bool isCompleted() const noexcept { return completed; }
	bool hasCompletedTime() const noexcept { return completedTime.has_value(); }

	//Returns string for Date:Time
	std::string getFormattedCompleteTime() const;

	std::optional<std::chrono::system_clock::time_point> getCompletedTime() const noexcept { return completedTime; };

private:
	bool completed;
	std::optional<std::chrono::system_clock::time_point> completedTime;
};