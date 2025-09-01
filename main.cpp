#include "http_server.h"
#include <iostream>

int main() {

	try {

		Database db("tasks.db");
		db.initialize();

		TaskManager task_manager(db);

		boost::asio::io_context io_context;
		HttpServer server(io_context, 8081, task_manager);

		std::cout << "Server running on http://localhost:8081\n";
		std::cout << "Endpoints:\n";
		std::cout << "  GET    /tasks - List all tasks\n";
		std::cout << "  POST   /tasks - Create new task\n";

		io_context.run();

	}
	catch (const std::exception& e) {

		std::cerr << "Error" << e.what() << std::endl;
		return 1;

	}

	return 0;

}