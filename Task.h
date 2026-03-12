//
// Created by James Forman on 3/10/26.
//
#pragma once
#include <string>

class Task {
public:
	int id;
	std::string title;
	bool completed;

	Task(int id, std::string title);
	Task();
	void markCompleted();
};