//
// Created by James Forman on 3/10/26.
//
#include "Task.h"

Task::Task(int id, std::string title) : id(id), title(title), completed(false) {}
Task::Task() : id(0), title(""), completed(false) {}

void Task::markCompleted() {
	completed = true;
}