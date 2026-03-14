//
// Created by James Forman on 3/12/26.
//

#include "Storage.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>

static std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> out;
	std::string cur;
	std::istringstream ss(s);
	while (std::getline(ss, cur, delim)) {
		out.push_back(cur);
	}
	return out;
}

Storage::Storage() {}

Storage::~Storage() {}

std::vector<Task> Storage::loadTasks() {
	std::ifstream file("tasks.txt");
	std::vector<Task> tasks;
	std::string line;

	while (std::getline(file, line)) {
		if (line.empty()) continue;

		auto tokens = split(line, ',');
		if (tokens.size() < 3) {
			tokens = split(line, '\t');
		}

		if (tokens.size() < 3) {
			// Line is formed wrong
			continue;
		}

		Task task;
		try {
			task.id = std::stoi(tokens[0]);
		}catch (...) {
			continue;
		}

		task.title = tokens[1];

		try {
			int completedInt = std::stoi(tokens[2]);
			if (completedInt == 1) {
				task.markCompleted();
			}
		}catch (...) {
			// leave as default (not completed)
		}

		if (tokens.size() >= 4 && !tokens[3].empty()) {
			try {
				// Store as a time point
				std::time_t epoch = static_cast<std::time_t>(std::stoi(tokens[3]));
				std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(epoch);
				task.setCompletedTime(tp);
			}catch (...) {
				// ignore bad epoch and keep task marked or unmarked as above
			}
		}

		tasks.push_back(std::move(task));
	}

	file.close();
	return tasks;
}

bool Storage::saveTasks(const std::vector<Task> &tasks) {
	std::ofstream outfile("tasks.txt", std::ios::trunc);
	if (!outfile) return false;

	for (const auto& task : tasks) {
		// We will use tab here to avoid commas in titles (unlike before where we used commas)
		outfile << task.id << '\t';
		std::string safeTitle = task.title;
		for (auto &c : safeTitle) if (c == '\t') c = ' ';

		outfile << safeTitle << '\t';
		outfile << (task.isCompleted() ? "1" : "0") << '\t';

		//set completed epoch : empty if not set
		if (task.getCompletedTime().has_value()) {
			std::time_t epoch = std::chrono::system_clock::to_time_t(*task.getCompletedTime());
			outfile << epoch;
		}
		outfile << '\n';
	}

	outfile.close();
	return true;
}
