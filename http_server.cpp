#include <iostream>
#include "http_server.h"
#include <boost/json.hpp>

namespace json = boost::json;

/**
 * HttpServer class constructor.
 * Initializes the HTTP server with the specified port and task manager.
 * @param io_context ASIO I/O context for asynchronous operations
 * @param port Port number to listen on
 * @param task_manager Reference to TaskManager for task operations
 */
HttpServer::HttpServer(asio::io_context& io_context, unsigned short port, TaskManager& task_manager) : acceptor_(io_context, { tcp::v4(), port }), task_manager_(task_manager) {
    start_accept();
}

/**
 * Starts asynchronous acceptance of incoming connections.
 * Continuously listens for new client connections and accepts them.
 */
void HttpServer::start_accept() {

    acceptor_.async_accept(

        [this](beast::error_code ec, tcp::socket socket) {

            if (!ec) {

                auto stream = beast::tcp_stream(std::move(socket));
                auto buffer = beast::flat_buffer();

                handle_request(std::move(stream), std::move(buffer));

            }

            start_accept();
        
        }
    );
}

/**
 * Handles an incoming HTTP request.
 * Reads the request, processes it, and sends back a response.
 * @param stream TCP stream for communication with the client
 * @param buffer Buffer for storing incoming request data
 */
void HttpServer::handle_request(beast::tcp_stream stream, beast::flat_buffer buffer) {

    http::request<http::string_body> req;
    http::read(stream, buffer, req);

    auto res = handle_api_request(req);

    http::write(stream, res);
    stream.socket().shutdown(tcp::socket::shutdown_send);

}

/**
 * Processes API requests and generates appropriate HTTP responses.
 * Routes requests to the appropriate handler based on HTTP method and target.
 * @param req HTTP request object containing request details
 * @return http::response<http::string_body> HTTP response with JSON body
 */
http::response<http::string_body> HttpServer::handle_api_request(const http::request<http::string_body>& req) {

    http::response<http::string_body> res;
    res.version(req.version());
    res.set(http::field::server, "C++ Rest Server");
    res.set(http::field::content_type, "application/json");
    res.set(http::field::access_control_allow_origin, "*");

    try {

        if (req.method() == http::verb::get && req.target() == "/tasks") {

            auto tasks = task_manager_.get_all_tasks();
            json::array tasks_json;

            for (const auto task : tasks) { 
                tasks_json.push_back({
                    {"id", task.id},
                    {"title", task.title},
                    {"description", task.description},
                    {"completed", task.completed}
                });
            }

            res.result(http::status::ok);
            res.body() = json::serialize(tasks_json);

        }
        else if (req.method() == http::verb::post && req.target() == "/tasks") {

            json::value request_json = json::parse(req.body());

            if (!request_json.as_object().contains("title")) throw std::runtime_error("Field 'title' is required");

            // create task
            int id = task_manager_.create_task(
                request_json.at("title").as_string().c_str(),
                request_json.as_object().contains("description") ? request_json.at("description").as_string().c_str() : ""
            );

            res.result(http::status::created);
            res.body() = json::serialize(json::object{ {"id", id} });

        }
        else {

            res.result(http::status::not_found);
            res.body() = json::serialize(json::object{ {"error", "Not found"} });

        }

    }
    catch (const std::exception& e) {

        res.result(http::status::internal_server_error);
        res.body() = json::serialize(json::object{ {"error", e.what()} });

    }

    res.prepare_payload();
    return res;

}
