#include "../../headers/HTTPRequest.hpp"

// HTTPRequest HTTPRequest::from_file(const std::string& path) {
//     // Open the file in binary mode to read raw data
//     std::ifstream file(path, std::ios::binary);
//     if (!file) {
//         throw std::runtime_error("Failed to open request file: " + path);
//     }

//     // Read the entire file into a string or vector
//     std::vector<uint8_t> raw_data((std::istreambuf_iterator<char>(file)),
//                                   std::istreambuf_iterator<char>());

//     // For now, you can use raw data to initialize fields like method, path, query
//     HTTPRequest request;
//     std::string request_line(raw_data.begin(), raw_data.begin() + raw_data.find('\n'));

//     // Split the request_line into method, path, and query
//     std::istringstream stream(request_line);
//     stream >> request.method >> request.path;

//     // Check if there's a query string in the path
//     size_t query_pos = request.path.find('?');
//     if (query_pos != std::string::npos) {
//         request.query = request.path.substr(query_pos + 1);
//         request.path = request.path.substr(0, query_pos);
//     }

//     return request;
// }