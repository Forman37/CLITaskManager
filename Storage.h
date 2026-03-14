//
// Created by James Forman on 3/12/26.
//

#ifndef CLITASKMANAGER_STORAGE_H
#define CLITASKMANAGER_STORAGE_H

#include "Task.h"
#include <vector>

class Storage {
public:
	Storage();
	~Storage();

	static std::vector<Task> loadTasks();

	static bool saveTasks(const std::vector<Task>& tasks, const std::string &fileName, bool truncate);
};


#endif //CLITASKMANAGER_STORAGE_H
