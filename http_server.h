#pragma once
#include "task_manager.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class HttpServer {
public:

	HttpServer(asio::io_context& io_context, unsigned short port, TaskManager& task_manager);

private:

	void start_accept();
	void handle_request(beast::tcp_stream stream, beast::flat_buffer buffer);
	http::response<http::string_body> handle_api_request(const http::request<http::string_body>& req);

	tcp::acceptor acceptor_;
	TaskManager& task_manager_;

};