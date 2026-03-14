//
// Created by James Forman on 3/12/26.
//

#ifndef CLITASKMANAGER_TASKMANAGER_H
#define CLITASKMANAGER_TASKMANAGER_H
#include "Task.h"

#include <vector>


class TaskManager {
public:
	static int addTask(std::vector<Task>& tasks, int idCounter);
	static void deleteTask(std::vector<Task>& tasks);
	static void updateTask(std::vector<Task>& tasks);
	static void completeTask(std::vector<Task>& tasks);
	static void moveCompleted(std::vector<Task>& tasks);
};


#endif //CLITASKMANAGER_TASKMANAGER_H