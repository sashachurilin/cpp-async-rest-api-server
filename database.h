#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include <stdexcept>

// Database task structure
struct Task {

	int id;
	std::string title;
	std::string description;
	bool completed;

};


class Database {
public:

	// Constructor
	Database(const std::string& db_path);

	// Destructor
	~Database();

	// Methods
	void initialize();
	int add_task(const Task& task);
	bool update_task(const Task& task);
	bool delete_task(int id);
	Task get_task_by_id(int id);
	std::vector<Task> get_all_tasks();

private:
	
	sqlite3* db_;
	void execute_sql(const char* sql);

};