#include "task_manager.h"
#include <stdexcept>
#include <iostream>

/**
 * TaskManager class constructor.
 * Initializes the TaskManager with a reference to a Database object.
 * @param db Reference to the Database object for data persistence
 */
TaskManager::TaskManager(Database& db)
	: db_(db)
{
	std::cout << "TaskManager initialized\n";
}

/**
 * Creates a new task with the given title and description.
 * Validates input parameters before creating the task.
 * @param title Task title (required, max 100 characters)
 * @param description Task description (optional)
 * @return int ID of the newly created task
 * @throws std::invalid_argument If title is empty or exceeds 100 characters
 * @throws std::runtime_error If database operation fails
 */
int TaskManager::create_task(const std::string& title, const std::string& description) {

	if (title.empty() || title.length() > 100) {
		if (title.empty()) throw std::invalid_argument("Task title cannot be empty");
		if (title.length() > 100) throw std::invalid_argument("Task title too long (max 100 chars)");
	}

	Task task{ 0, title, description, false };

	return db_.add_task(task);

}

/**
 * Updates an existing task with new data.
 * Validates input parameters and checks if the task exists before updating.
 * @param id ID of the task to update
 * @param title New task title (required)
 * @param description New task description
 * @param completed Completion status of the task
 * @return bool True if update was successful, false otherwise
 * @throws std::invalid_argument If ID is invalid or title is empty
 * @throws std::runtime_error If database operation fails or task not found
 */
bool TaskManager::update_task(int id, const std::string& title, const std::string& description, bool complited) {

	if (id <= 0 || title.empty()) {
		if (id <= 0) throw std::invalid_argument("Invalid task ID");
		if (title.empty()) throw std::invalid_argument("Task title cannot be empty");
	}

	Task existing_task = db_.get_task_by_id(id);
	if (existing_task.id == 0) return false;

	Task updated_task = existing_task;
	updated_task.title = title;
	updated_task.description = description;
	updated_task.completed = complited;

	return db_.update_task(updated_task);

}

/**
 * Deletes a task from the system.
 * Validates the task ID before attempting deletion.
 * @param id ID of the task to delete
 * @return bool True if deletion was successful, false otherwise
 * @throws std::invalid_argument If ID is invalid
 * @throws std::runtime_error If database operation fails
 */
bool TaskManager::delete_task(int id) {

	if (id <= 0) throw std::invalid_argument("Invalid task ID");

	return db_.delete_task(id);

}


/**
 * Retrieves a specific task by its ID.
 * Validates the task ID and checks if the task exists.
 * @param id ID of the task to retrieve
 * @return Task Task object with the requested data
 * @throws std::invalid_argument If ID is invalid
 * @throws std::runtime_error If task not found or database operation fails
 */
Task TaskManager::get_task(int id) {

	if (id <= 0) throw std::invalid_argument("Invalid task ID");

	Task task = db_.get_task_by_id(id);
	if (task.id <= 0) throw std::runtime_error("Task not found");

	return task;

}

/**
 * Retrieves all tasks from the system.
 * @return std::vector<Task> Vector containing all task objects
 * @throws std::runtime_error If database operation fails
 */
std::vector<Task> TaskManager::get_all_tasks() {

	return db_.get_all_tasks();

}
