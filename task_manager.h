#pragma once
#include "database.h"

class TaskManager {
public:

	explicit TaskManager(Database& db);

	// CRUD operations
	int create_task(const std::string& title, const std::string& description = "");
	bool update_task(int id, const std::string& title, const std::string& description, bool completed);
	bool delete_task(int id);
	Task get_task(int id);
	std::vector<Task> get_all_tasks();

private:

	Database& db_;

};