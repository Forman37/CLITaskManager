//
// Created by James Forman on 3/10/26.
//
#include "Task.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

Task::Task(int id, std::string title) : id(id), title(std::move(title)), completed(false), completedTime(std::nullopt) {}
Task::Task() : id(0), title(), completed(false), completedTime(std::nullopt) {}
Task::Task(int id, std::string title, std::chrono::system_clock::time_point completedTime) : id(id), title(std::move(title)), completed(false), completedTime(completedTime) {}

void Task::markCompleted() {
	completed = true;
}

void Task::setCompletedTime() {
	auto now = std::chrono::system_clock::now();
	completedTime = now;
}

void Task::setCompletedTime(std::chrono::system_clock::time_point tp) {
	completedTime = tp;
	completed = true;
}

std::string Task::getFormattedCompleteTime() const {
	if (!completedTime.has_value()) return std::string();

	//Convert time_point to time_t
	std::time_t t = std::chrono::system_clock::to_time_t(*completedTime);

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
