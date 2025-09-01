#include "database.h"
#include <iostream>


/**
 * Database class constructor.
 * Opens a connection to the SQLite database at the specified path.
 * @param db_path Path to the database file
 * @throws std::runtime_error If failed to open the database
 */
Database::Database(const std::string& db_path) {

	if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));

}


/**
 * Database class destructor.
 * Closes the database connection.
 */
Database::~Database() {

	sqlite3_close(db_);

}



/**
 * Initializes the database structure.
 * Creates the tasks table if it doesn't exist.
 * @throws std::runtime_error If SQL execution fails
 */
void Database::initialize() {

    const char* sql = "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "title TEXT NOT NULL, "
        "description TEXT, "
        "completed BOOLEAN DEFAULT 0);";

    execute_sql(sql);

}


/**
 * Adds a new task to the database.
 * @param task Task object containing task details
 * @return int ID of the newly inserted task
 * @throws std::runtime_error If SQL preparation or execution fails
 */
int Database::add_task(const Task& task) {

    // sql query
    const char* sql = "INSERT INTO tasks (title, description, completed) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db_));
        
    sqlite3_bind_text(stmt, 1, task.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, task.completed ? 1 : 0);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to inserd task");
    }

    int id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return id;
}


/**
 * Updates an existing task in the database.
 * @param task Task object with updated data
 * @return bool True if update was successful, false otherwise
 * @throws std::runtime_error If SQL preparation fails or task not found
 */
bool Database::update_task(const Task& task) {

    const char* sql = "UPDATE tasks SET title = ?, description = ?, completed = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db_));

    sqlite3_bind_text(stmt, 1, task.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, task.completed);
    sqlite3_bind_int(stmt, 4, task.id);

    bool success =  (sqlite3_step(stmt) == SQLITE_DONE);

    if (success && sqlite3_changes(db_) == 0) {
        throw std::runtime_error(sqlite3_errmsg(db_));
        success = false;
    }

    sqlite3_finalize(stmt);
    return success;

}


/**
 * Deletes a task from the database by ID.
 * @param id ID of the task to delete
 * @return bool True if deletion was successful, false otherwise
 * @throws std::runtime_error If SQL preparation fails or task not found
 */
bool Database::delete_task(int id) {

    const char* sql = "DELETE FROM tasks WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db_));

    sqlite3_bind_int(stmt, 1, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (success && sqlite3_changes(db_) == 0) {
        throw std::runtime_error(sqlite3_errmsg(db_));
        success = false;
    }

    sqlite3_finalize(stmt);
    return success;

}


/**
 * Retrieves a task from the database by ID.
 * @param id ID of the task to retrieve
 * @return Task Task object with the retrieved data
 * @throws std::runtime_error If SQL preparation fails or task not found
 */
Task Database::get_task_by_id(int id) {

    const char* sql = "SELECT id, title, description, completed FROM tasks WHERE id = ?;";
    sqlite3_stmt* stmt;
    Task task;
    task.id = -1;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db_));

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        task.id = sqlite3_column_int(stmt, 0);
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.completed = sqlite3_column_int(stmt, 3) != 0;
    }
    else {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Task not found with id: " + std::to_string(id));
    }

    sqlite3_finalize(stmt);
    return task;

}



/**
 * Retrieves all tasks from the database.
 * @return std::vector<Task> Vector containing all task objects
 * @throws std::runtime_error If SQL preparation fails
 */
std::vector<Task> Database::get_all_tasks() {

    std::vector<Task> tasks;
    const char* sql = "SELECT id, title, description, completed FROM tasks;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db_));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Task task;
        task.id = sqlite3_column_int(stmt, 0);
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        task.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.completed = sqlite3_column_int(stmt, 3);
        
        tasks.push_back(task);
    }

    sqlite3_finalize(stmt);
    return tasks;

}


/**
 * Executes a raw SQL query.
 * Primarily used for database initialization and schema changes.
 * @param sql SQL query string to execute
 * @throws std::runtime_error If SQL execution fails
 */
void Database::execute_sql(const char* sql) {

    char* err_msg = nullptr;

    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::string error = err_msg;
        sqlite3_free(err_msg);
        throw std::runtime_error("SQL error: " + error);
    }

}
