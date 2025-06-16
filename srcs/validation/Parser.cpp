/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-maaz <ael-maaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:38:54 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/06/16 22:04:45 by ael-maaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/Parser.hpp"



Parser::Parser()
{
	return ;
}

Parser::~Parser()
{
	return ;
}


int Parser::OpenTomlFile(std::string& path)
{
	if(checkExtension(path) == 1)
		throw std::runtime_error("bad extension");
	std::string fullpath = "./config/" + path;
	this->infile.open(fullpath);
	path = fullpath;
	if(!this->infile.is_open())
	{
		std::cerr << "Couldnt open config file, Please make sure the config file is present in config folder and read permission is set\n"; 
		throw std::runtime_error("lol");
	}
	return 0;
}

std::vector<std::string> Parser::ReadLines()
{
	std::string line;
	std::vector<std::string> lines;
	while(std::getline(this->infile,line))
	{
		if(line[0] != '#' && line.length() != 0)
			lines.push_back(trim(line));
	}
	for(size_t i = 0; i < lines.size();i++)
	{
		size_t pos  = lines[i].find("#");
		if(pos != std::string::npos)
			lines[i].erase(pos);
	}
	return lines;
}

// void Parser::parseLines(WebServerConfig& conf)
// {
// 	(void) conf;
// 	std::vector<std::string> data_lines;
// 	std::vector<std::string> labels;
// 	bool label_flag = false;
// 	std::string label;
// 	for(size_t i = 0; i < this->lines.size();i++)
// 	{
// 		std::cout <<this->lines[i] << std::endl;
// 		if(this->lines[i][0] == '[')
// 		{
// 			label_flag = true;
// 			if(this->lines[i][1] == '[')
// 			{
// 				size_t pos = this->lines[i].find(']');
// 				if(pos == std::string::npos || (this->lines[i][pos+1] != ']') || ((pos + 1) != this->lines[i].size() - 1))		
// 					throw std::runtime_error("Invalid syntax");
// 				else
// 				{
// 					label = this->lines[i].substr(2,pos - 2);
// 					label = trim(label);
// 					std::cout << "label line: " <<label << std::endl;
// 					if(checkBracketLabel(trim(label)) == "bad")
// 						throw std::runtime_error("Invalid label");
// 					// else
// 					// 	std::cout << "Something else\n";
// 						//fillConfFile
						
// 				}
// 			}
// 			else
// 			{
				
// 				size_t pos = this->lines[i].find(']');
// 				if(pos == std::string::npos || pos != this->lines[i].size() - 1)
// 				{					
// 					throw std::runtime_error("Invalid syntax");
// 				}
// 				else
// 				{
// 					label = this->lines[i].substr(1,pos - 1);
// 					label = trim(label);
// 					std::cout << "label line: " <<label << std::endl;
// 					if(checkLabel(trim(label)) == "bad")
// 						throw std::runtime_error("Invalid label");
// 					std::string target = trim(label);

// 					std::vector<std::string>::iterator it = std::find(labels.begin(), labels.end(), target);

// 					if (it != labels.end())
// 					{
// 						std::cout << labels.back() << std::endl;
// 						throw std::runtime_error("Duplicate labels");
// 					}
// 					labels.push_back(trim(label));
											
// 				}
// 			}
// 		}
// 		else
// 		{
// 			std::string line = this->lines[i];
// 			std::stringstream ss(line);
// 			std::string key, value;
// 			// std::cout << "data line: "<< this->lines[i] << std::endl;
			
// 			if (std::getline(ss, key, '=') && std::getline(ss, value))
// 			{
// 				std::cout << "Key: \"" << trim(key) << "\"\n";
// 				std::cout << "Value: " << trim(value) << "\n";
// 				testKey(key,value,label);
// 				std::cout << "here\n";
// 			}
// 			else
// 			{
// 				std::cerr << "Malformed line, missing '='\n";
// 				throw std::runtime_error("Invalid Syntax");
// 			}
			
// 		}
// 	}
// }







// void Parser::parseLines(WebServerConfig& conf)
// {
// 	(void) conf;
// 	std::vector<std::string> data_lines;
// 	std::vector<std::string> labels;
// 	bool label_flag = false;
// 	std::string label;
// 	for(size_t i = 0; i < this->lines.size();i++)
// 	{
// 		std::cout <<this->lines[i] << std::endl;
// 		if(this->lines[i][0] == '[')
// 		{
// 			label_flag = true;
// 			if(this->lines[i][1] == '[')
// 			{
// 				size_t pos = this->lines[i].find(']');
// 				if(pos == std::string::npos || (this->lines[i][pos+1] != ']') || ((pos + 1) != this->lines[i].size() - 1))		
// 					throw std::runtime_error("Invalid syntax");
// 				else
// 				{
// 					label = this->lines[i].substr(2,pos - 2);
// 					label = trim(label);
// 					std::cout << "label line: " <<label << std::endl;
// 					if(checkBracketLabel(trim(label)) == "bad")
// 						throw std::runtime_error("Invalid label");
// 					// else
// 					// 	std::cout << "Something else\n";
// 						//fillConfFile
						
// 				}
// 			}
// 			else
// 			{
				
// 				size_t pos = this->lines[i].find(']');
// 				if(pos == std::string::npos || pos != this->lines[i].size() - 1)
// 				{					
// 					throw std::runtime_error("Invalid syntax");
// 				}
// 				else
// 				{
// 					label = this->lines[i].substr(1,pos - 1);
// 					label = trim(label);
// 					std::cout << "label line: " <<label << std::endl;
// 					if(checkLabel(trim(label)) == "bad")
// 						throw std::runtime_error("Invalid label");
// 					std::string target = trim(label);

// 					std::vector<std::string>::iterator it = std::find(labels.begin(), labels.end(), target);

// 					if (it != labels.end())
// 					{
// 						std::cout << labels.back() << std::endl;
// 						throw std::runtime_error("Duplicate labels");
// 					}
// 					labels.push_back(trim(label));
											
// 				}
// 			}
// 		}
// 		else
// 		{
// 			std::string line = this->lines[i];
// 			std::stringstream ss(line);
// 			std::string key, value;
// 			// std::cout << "data line: "<< this->lines[i] << std::endl;
			
// 			if (std::getline(ss, key, '=') && std::getline(ss, value))
// 			{
// 				std::cout << "Key: \"" << trim(key) << "\"\n";
// 				std::cout << "Value: " << trim(value) << "\n";
// 				testKey(key,value,label);
// 				std::cout << "here\n";
// 			}
// 			else
// 			{
// 				std::cerr << "Malformed line, missing '='\n";
// 				throw std::runtime_error("Invalid Syntax");
// 			}
			
// 		}
// 	}
// }






bool isWhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string trim(const std::string& str) {
	size_t start = 0;
	while (start < str.length() && isWhitespace(str[start])) ++start;
	size_t end = str.length();
	while (end > start && isWhitespace(str[end - 1])) --end;
	return str.substr(start, end - start);
}

// std::vector<std::string> parseStringList(const std::string& value) {
// 	std::vector<std::string> result;
// 	std::string temp;
// 	bool inQuotes = false;
// 	for (size_t i = 0; i < value.length(); ++i) {
// 		char c = value[i];
// 		if (c == '"') {
// 			inQuotes = !inQuotes;
// 			if (!inQuotes && !temp.empty()) {
// 				result.push_back(temp);
// 				temp.clear();
// 			}
// 		} else if (inQuotes) {
// 			temp += c;
// 		}
// 	}
// 	return result;
// }



std::vector<std::string> parseStringList(const std::string& value) {
	std::vector<std::string> result;
	size_t i = 0;
	bool foundClosingBracket = false;

	// Skip leading whitespace
	while (i < value.size() && std::isspace(value[i])) ++i;
	if (i >= value.size() || value[i] != '[')
		throw std::runtime_error("Expected '[' at start of list");

	++i; // Skip '['

	while (i < value.size()) {
		// Skip whitespace
		while (i < value.size() && std::isspace(value[i])) ++i;
		if (i >= value.size())
			break;

		// End of list
		if (value[i] == ']') {
			foundClosingBracket = true;
			++i;
			break;
		}

		// Each item must be in quotes
		if (value[i] != '"')
			throw std::runtime_error("Expected opening '\"' for string item");

		++i; // Skip opening quote
		std::string temp;
		while (i < value.size() && value[i] != '"') {
			temp += value[i++];
		}

		if (i >= value.size() || value[i] != '"')
			throw std::runtime_error("Expected closing '\"' for string item");

		++i; // Skip closing quote
		result.push_back(temp);

		// Skip whitespace
		while (i < value.size() && std::isspace(value[i])) ++i;

		if (i < value.size() && value[i] == ',') {
			++i; // Skip comma
			continue;
		} else if (i < value.size() && value[i] != ']') {
			throw std::runtime_error("Expected ',' or ']' after item");
		}
	}

	// Must have closed the bracket
	if (!foundClosingBracket)
		throw std::runtime_error("Expected closing ']' at end of list");

	// Skip trailing whitespace
	while (i < value.size() && std::isspace(value[i])) ++i;

	// Should not be any other characters
	if (i != value.size())
		throw std::runtime_error("Unexpected characters after closing ']'");

	return result;
}

// std::vector<uint16_t> parseIntList(const std::string& value) {
// 	std::vector<uint16_t> result;
// 	std::string num;
// 	for (size_t i = 0; i < value.length(); ++i) {
// 		if (isdigit(value[i])) num += value[i];
// 		else if (!num.empty()) {
// 			result.push_back(static_cast<uint16_t>(atoi(num.c_str())));
// 			num.clear();
// 		}
// 	}
// 	if (!num.empty())
// 		result.push_back(static_cast<uint16_t>(atoi(num.c_str())));
// 	return result;
// }





std::vector<uint16_t> parseIntList(const std::string& value) {
	std::vector<uint16_t> result;
	size_t i = 0;

	// Skip whitespace
	while (i < value.size() && std::isspace(value[i])) ++i;

	// CASE 1: Bracketed list
	if (i < value.size() && value[i] == '[') {
		++i;
		bool expectValue = true;
		bool foundClosing = false;

		while (i < value.size()) {
			while (i < value.size() && std::isspace(value[i])) ++i;

			if (i < value.size() && value[i] == ']') {
				foundClosing = true;
				++i;
				break;
			}

			if (!expectValue)
				throw std::runtime_error("Expected ',' or closing ']'");

			std::string num;
			while (i < value.size() && std::isdigit(value[i])) {
				num += value[i++];
			}
			if (num.empty())
				throw std::runtime_error("Expected an integer");

			long val = std::strtol(num.c_str(), NULL, 10);
			if (val < 0 || val > 65535)
				throw std::runtime_error("Integer out of uint16_t range");

			result.push_back(static_cast<uint16_t>(val));

			while (i < value.size() && std::isspace(value[i])) ++i;

			if (i < value.size() && value[i] == ',') {
				++i;
				expectValue = true;
			} else {
				expectValue = false;
			}
		}

		if (!foundClosing)
			throw std::runtime_error("Expected closing ']'");

		// No trailing garbage
		while (i < value.size() && std::isspace(value[i])) ++i;
		if (i != value.size())
			throw std::runtime_error("Unexpected trailing characters after ']'");
	}
	// CASE 2: Single integer
	else {
		std::string num;
		while (i < value.size() && std::isdigit(value[i])) {
			num += value[i++];
		}
		while (i < value.size() && std::isspace(value[i])) ++i;
		if (i != value.size())
			throw std::runtime_error("Unexpected characters after single integer");

		if (num.empty())
			throw std::runtime_error("Expected an integer");

		long val = std::strtol(num.c_str(), NULL, 10);
		if (val < 0 || val > 65535)
			throw std::runtime_error("Integer out of uint16_t range");

		result.push_back(static_cast<uint16_t>(val));
	}

	return result;
}


void parseTOML(const std::string& filepath, WebServerConfig& config)
{
    std::ifstream file(filepath.c_str());
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file");

    std::string line;
    std::string currentSection;          // "global", "default_error_pages", ""
    Cluster   *currentCluster = NULL;    // the server we are filling
    Route      currentRoute;             // the route we are filling
    std::string currentRoutePath;        // its eventual key in the map
    bool        inRoute = false;         // true when inside [[servers.routes]]

    // data that will be moved into WebServerConfig at the end
    std::vector<Cluster>        clusters;
    std::map<int, std::string>  errorPages;
    size_t                      maxBodySize = 1048576; // default 1 MB

    //------------------------------------------------------------------
    // helper lambda to commit the buffered route (if any) to its cluster
    //------------------------------------------------------------------

    const struct CommitRoute {
        static void exec(Cluster* c, const std::string& path, const Route& r)
        {
            if (!c)
			    throw std::runtime_error("Route block is missing a \"path\" key");
            if (path.empty())
			    throw std::runtime_error("Route defined outside of [[servers]] block");
            c->getRoutes()[path] = r;    // or c->addRoute(path,r);
        }
    } commitRoute;
	bool seenGlobalBlock = false;              // NEW
	bool seenDefaultErrorPagesBlock = false;
    //---------------------------------- --------------------------------
    // main loop
    //------------------------------------------------------------------
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;
		size_t pos  = line.find("#");
		if(pos != std::string::npos)
			line.erase(pos);
        // --------- SECTION HEADERS ----------------------------------
        if (line[0] == '[')
        {
            //----------------------------------------------------------
            // 1) starting a new [[servers.routes]] block
            //----------------------------------------------------------
            if (line.compare(0,2,"[[") == 0 &&
                line.find("servers.routes") != std::string::npos)
            {
                // commit the previous route (if we were already in one)
                if (inRoute)
                    commitRoute.exec(currentCluster, currentRoutePath, currentRoute);

                // start a fresh route
                currentRoute       = Route();
                currentRoutePath.clear();
                inRoute            = true;
                currentSection.clear();      // keys now belong to the route
                continue;
            }
            //----------------------------------------------------------
            // 2) starting a new [[servers]] block
            //----------------------------------------------------------
            if (line.compare(0,2,"[[") == 0 &&
                line.find("servers") != std::string::npos)
            {
                // commit any route that was still open
                if (inRoute)
                    commitRoute.exec(currentCluster, currentRoutePath, currentRoute);
                inRoute = false;

                clusters.push_back(Cluster());        // create & select it
                currentCluster   = &clusters.back();
                currentSection.clear();               // keys now belong to server
                continue;
            }
            //----------------------------------------------------------
            // 3) single‑bracket section: [global] / [default_error_pages]
            //----------------------------------------------------------
            if (line[0] == '[' && line[1] != '[')
            {
                // commit any open route *before* switching section
                if (inRoute)
                    commitRoute.exec(currentCluster, currentRoutePath, currentRoute);
                inRoute = false;

                currentSection = trim(line.substr(1, line.size()-2)); // strip [ ]
                continue;
            }
        }

        // --------- KEY‑VALUE line -----------------------------------
        size_t eq = line.find('=');
        if (eq == std::string::npos) 
			throw std::runtime_error("Invalid line");            // malformed / skip

        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));

        //------------------------------------------------------------------
        // 1) GLOBAL / DEFAULT_ERROR_PAGES
        //------------------------------------------------------------------
        if      (currentSection == "global")
        {
			if (seenGlobalBlock)
            	throw std::runtime_error("Duplicate [global] block");
			seenGlobalBlock = true;
            if (key == "max_body_size")
                maxBodySize = static_cast<size_t>(strtoul(val.c_str(), 0, 10));
			else
				throw std::runtime_error("Invalid Var for global label");
        }
        else if (currentSection == "default_error_pages")
        {
			if (seenDefaultErrorPagesBlock)
            	throw std::runtime_error("Duplicate [default_error_pages] block");
			seenDefaultErrorPagesBlock = true;
            int code = atoi(key.c_str());
            if (!val.empty() && val[0] == '"')
                val = val.substr(1, val.size()-2);        // strip quotes
            errorPages[code] = val;
        }
        //------------------------------------------------------------------
        // 2) SERVER‑LEVEL keys  (host, port, hostnames)
        //------------------------------------------------------------------
        else if (currentCluster && !inRoute)
        {
            if      (key == "host")
                currentCluster->setHost(val.substr(0, val.size())); // strip quotes
            else if (key == "port")
                currentCluster->setPorts(parseIntList(val));
            else if (key == "hostnames")
                currentCluster->setHostnames(parseStringList(val));
        }
        //------------------------------------------------------------------
        // 3) ROUTE‑LEVEL keys
        //------------------------------------------------------------------
        else if (currentCluster && inRoute)
        {
            if      (key == "path")
                currentRoutePath = val.substr(0, val.size());   // strip quotes
            else if (key == "root")
                currentRoute.setRootDir(val.substr(0, val.size()));
            else if (key == "index") {
                std::vector<std::string> tmp(1, val.substr(0, val.size()));
                currentRoute.setIndexFiles(tmp);
            }
            else if (key == "methods") {
                std::vector<std::string> v = parseStringList(val);
                currentRoute.setAllowedMethods(std::set<std::string>(v.begin(), v.end()));
            }
            else if (key == "autoindex")
                currentRoute.setAutoindex(val == "true");
            else if (key == "upload_dir")
                currentRoute.setUploadDir(val.substr(0, val.size()));
            else if (key == "redirect")
                currentRoute.setRedirect(val.substr(0, val.size()));
            else if (key == "extensions") {
                CGIConfig cgi = currentRoute.getCGIConfig();
                cgi.setExtensions(parseStringList(val));
                currentRoute.setCGIConfig(cgi);
            }
            else if (key == "interpreter") {
                CGIConfig cgi = currentRoute.getCGIConfig();
                cgi.setInterpreter(val.substr(0, val.size()));
                currentRoute.setCGIConfig(cgi);
            }
        }
    }

    //------------------------------------------------------------------
    // commit the last open route / cluster after EOF
    //------------------------------------------------------------------
    if (inRoute)
        commitRoute.exec(currentCluster, currentRoutePath, currentRoute);

    // move data into WebServerConfig
    config.setMaxBodySize(maxBodySize);
    config.setErrorPages(errorPages);
    config.setClusters(clusters);
}







void printConfigs(const WebServerConfig& conf)
{
    std::cout << "=== GLOBAL CONFIGURATION ===" << std::endl;
    std::cout << "Max Body Size: " << conf.getMaxBodySize() << std::endl;

    std::cout << "\nError Pages:" << std::endl;
    std::map<int, std::string> error_pages = conf.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
        std::cout << "  " << it->first << " => " << it->second << std::endl;
    }

    std::cout << "\n=== SERVER BLOCKS ===" << std::endl;
    const std::vector<Cluster>& clusters = conf.getClusters();
    for (size_t i = 0; i < clusters.size(); ++i) {
        const Cluster& cluster = clusters[i];
        std::cout << "\n--- Server " << (i + 1) << " ---" << std::endl;
        std::cout << "Host: " << cluster.getHost() << std::endl;

        std::cout << "Ports: ";
        const std::vector<uint16_t>& ports = cluster.getPorts();
        for (size_t j = 0; j < ports.size(); ++j)
            std::cout << ports[j] << (j + 1 < ports.size() ? ", " : "");
        std::cout << std::endl;

        std::cout << "Hostnames: ";
        const std::vector<std::string>& hostnames = cluster.getHostnames();
        for (size_t j = 0; j < hostnames.size(); ++j)
            std::cout << hostnames[j] << (j + 1 < hostnames.size() ? ", " : "");
        std::cout << std::endl;

        const std::map<std::string, Route>& routes = cluster.getRoutes();
        for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
            const std::string& path = it->first;
            const Route& route = it->second;
            std::cout << "\n  Route Path: " << path << std::endl;
            std::cout << "    Root: " << route.getRootDir() << std::endl;

            const std::vector<std::string>& index_files = route.getIndexFiles();
            std::cout << "    Index Files: ";
            for (size_t k = 0; k < index_files.size(); ++k)
                std::cout << index_files[k] << (k + 1 < index_files.size() ? ", " : "");
            std::cout << std::endl;

            const std::set<std::string>& methods = route.getAllowedMethods();
            std::cout << "    Allowed Methods: ";
            for (std::set<std::string>::const_iterator m = methods.begin(); m != methods.end(); ) {
    			std::cout << *m;
    			if (++m != methods.end())
       		 		std::cout << ", ";
			}

            std::cout << std::endl;

            std::cout << "    Autoindex: " << (route.isAutoindex() ? "on" : "off") << std::endl;

            if (!route.getRedirect().empty())
                std::cout << "    Redirect: " << route.getRedirect() << std::endl;
            if (!route.getUploadDir().empty())
                std::cout << "    Upload Dir: " << route.getUploadDir() << std::endl;

            const CGIConfig& cgi = route.getCGIConfig();
            const std::vector<std::string>& exts = cgi.getExtensions();
            if (!exts.empty()) {
                std::cout << "    CGI Extensions: ";
                for (size_t k = 0; k < exts.size(); ++k)
                    std::cout << exts[k] << (k + 1 < exts.size() ? ", " : "");
                std::cout << std::endl;
                std::cout << "    CGI Interpreter: " << cgi.getInterpreter() << std::endl;
            }
        }
    }
}

int Parser::MainParser(std::string path)
{
	WebServerConfig conf;
	try
	{
		OpenTomlFile(path);
		parseTOML(path, conf);
		// std::cout << conf.getMaxBodySize() <<std::endl;
		// std::map<int, std::string> error_pages = conf.getErrorPages();
		// for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
		// 	std::cout << "Error " << it->first << " -> " << it->second << std::endl;
		// }
		printConfigs(conf);

		// this->lines = ReadLines();
		// parseLines(conf);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}