#include "WebServerConfig.hpp"

// CGIConfig methods
//setters
void CGIConfig::setExtensions(const std::vector<std::string>& extensions) {
    this->extensions = extensions;
}

void CGIConfig::setInterpreter(const std::string& interpreter) {
    this->interpreter = interpreter;
}

//getters
const std::vector<std::string>& CGIConfig::getExtensions() const {
    return extensions;
}

const std::string& CGIConfig::getInterpreter() const {
    return interpreter;
}

// Route methods
// setters
void Route::setRootDir(const std::string& root_dir) {
    this->root_dir = root_dir;
}

void Route::setIndexFiles(const std::vector<std::string>& index_files) {
    this->index_files = index_files;
}

void Route::setAllowedMethods(const std::set<std::string>& allowed_methods) {
    this->allowed_methods = allowed_methods;
}

void Route::setAutoindex(bool autoindex) {
    this->autoindex = autoindex;
}

void Route::setRedirect(const std::string& redirect) {
    this->redirect = redirect;
}

void Route::setUploadDir(const std::string& upload_dir) {
    this->upload_dir = upload_dir;
}

void Route::setCGIConfig(const CGIConfig& cgi) {
    this->cgi = cgi;
}

// getters
const std::string& Route::getRootDir() const {
    return root_dir;
}

const std::vector<std::string>& Route::getIndexFiles() const {
    return index_files;
}

const std::set<std::string>& Route::getAllowedMethods() const {
    return allowed_methods;
}

bool Route::isAutoindex() const {
    return autoindex;
}

const std::string& Route::getRedirect() const {
    return redirect;
}

const std::string& Route::getUploadDir() const {
    return upload_dir;
}

const CGIConfig& Route::getCGIConfig() const {
    return cgi;
}

// Cluster methods
// setters
void Cluster::setHost(const std::string& host) {
    this->host = host;
}

void Cluster::setPorts(const std::vector<uint16_t>& ports) {
    this->ports = ports;
}

void Cluster::setHostnames(const std::vector<std::string>& hostnames) {
    this->hostnames = hostnames;
}

void Cluster::setRoutes(const std::map<std::string, Route>& routes) {
    this->routes = routes;
}

// getters
const std::string& Cluster::getHost() const {
    return host;
}

const std::vector<uint16_t>& Cluster::getPorts() const {
    return ports;
}

const std::vector<std::string>& Cluster::getHostnames() const {
    return hostnames;
}

const std::map<std::string, Route>& Cluster::getRoutes() const {
    return routes;
}

std::map<std::string, Route>& Cluster::getRoutes() {
    return this->routes;
}

// WebServerConfig methods
// setters
void WebServerConfig::setClusters(const std::vector<Cluster>& clusters) {
    this->clusters = clusters;
}

void WebServerConfig::setMaxBodySize(size_t max_body_size) {
    this->max_body_size = max_body_size;
}

void WebServerConfig::setErrorPages(const std::map<int, std::string>& error_pages) {
    this->error_pages = error_pages;
}

// getters
size_t WebServerConfig::getMaxBodySize() const {
    return max_body_size;
}

const std::vector<Cluster>& WebServerConfig::getClusters() const {
    return clusters;
}

const std::map<int, std::string>& WebServerConfig::getErrorPages() const {
    return error_pages;
}
