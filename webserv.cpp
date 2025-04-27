#include "headers/WebServerConfig.hpp"
#include "headers/HTTPRequest.hpp"
#include "mocker.hpp"

int main() {
    WebServerConfig config;
    

    //!   Mock Requests File ==== to write a full request to the test file 
        // * nc -l -p nof.PORT > path to test file
        // * postman to make req to localhost

    const std::string request_test_file = "./tests/request_mock.txt";
    const std::string response_test_file = "./tests/response_mock.txt";

    //! Load Configuration
    mocker(config);
    printConfig(config);

    // ? Khay Simo Starts reading from the file raw data the process the HTTP Request
        // HTTPRequest req = HTTPRequest::from_file(test_file);
        // HTTPRequest req =::from_file(request_test_file);
    
    // ? Meanwhile my goal would be to always write binary data to a file
        // 

    //! Run Clusters (using WebServerConfig)
        //! Write To said file Raw data of request 
        //! Process Request once its marked ready for reading
    //! Test reading  raw requests

    return (0);
}


// HLDC
/*

    class RequestQueue {
        public:
            void        push(int client_fd, std::vector<char> data);
            
            std::pair   <int, std::vector<char>> get_next()
        private:
            std::vector <std::pair<int, std::vector<char>>> requests;
    }

*/