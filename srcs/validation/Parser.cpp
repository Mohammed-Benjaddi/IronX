/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-maaz <ael-maaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:38:54 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/07/07 22:54:50 by ael-maaz         ###   ########.fr       */
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
	if (checkExtension(path) == 1)
		throw std::runtime_error("bad extension");
	std::string fullpath(path);
    this->infile.open(fullpath.c_str());
	// this->infile.open(fullpath);
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
			lines.push_back(_trim(line));
	}
	for(size_t i = 0; i < lines.size();i++)
	{
		size_t pos  = lines[i].find("#");
		if(pos != std::string::npos)
			lines[i].erase(pos);
	}
	return lines;
}



bool isWhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string _trim(const std::string& str) {
	size_t start = 0;
	while (start < str.length() && isWhitespace(str[start])) ++start;
	size_t end = str.length();
	while (end > start && isWhitespace(str[end - 1])) --end;
	return str.substr(start, end - start);
}


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

void commitRoute(Cluster* c, const std::string& path, const Route& r)
{
    if (!c)
        throw std::runtime_error("Route defined outside of [[servers]] block");
    if (path.empty())
        throw std::runtime_error("Invalid/Missing Path");

    c->addRoute(path, r);
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
	bool hasPathKey = false;

    // data that will be moved into WebServerConfig at the end
    std::vector<Cluster>        clusters;
    std::map<int, std::string>  errorPages;
    size_t                      maxBodySize = 1048576; // default 1 MB

	// hasPathKey = true;
	bool seenGlobalBlock = false;              // NEW
	bool seenDefaultErrorPagesBlock = false;
    //---------------------------------- --------------------------------
    // main loop
    //------------------------------------------------------------------
    while (std::getline(file, line))
    {
        line = _trim(line);
        if (line.empty() || line[0] == '#')
            continue;
		size_t pos  = line.find("#");
		if(pos != std::string::npos)
			line.erase(pos);
		line = _trim(line);
		if (line.empty())               // everything before # was whitespace
			continue;
        // --------- SECTION HEADERS ----------------------------------
        if (line[0] == '[')
        {
            //----------------------------------------------------------
            // 1) starting a new [[servers.routes]] block
            //----------------------------------------------------------
			if (line.compare(0, 2, "[[") == 0) {
				if (line.size() < 4 || line.substr(line.size() - 2) != "]]")
					throw std::runtime_error("Malformed section header: " + line);
				if (line.find("]]", 2) != line.size() - 2)
					throw std::runtime_error("Unexpected characters after closing ]] in: " + line);
			} else {
				if (line.size() < 3 || line[line.size() - 1] != ']')
					throw std::runtime_error("Malformed section header: back" + line);
				if (line.find(']', 1) != line.size() - 1)
					throw std::runtime_error("Unexpected characters after closing ] in: " + line);
				if (line.find('[', 1) != std::string::npos)
					throw std::runtime_error("Unexpected '[' inside single‑bracket section: " + line);
    		}
			
            // if (line.compare(0,2,"[[") == 0 && line.find("servers.routes") != std::string::npos)
            // {
			if (line.compare(0,2,"[[") == 0)
            {
				std::string section = _trim(line.substr(2, line.size() - 4)); // strip [[ ]]
                // commit the previous route (if we were already in one)
				if (section == "servers.routes")
    			{
					if (!currentCluster)
						throw std::runtime_error("Route block declared outside of [[servers]] block");
					// if (inRoute)
					//     commitRoute(currentCluster, currentRoutePath, currentRoute);
					if (inRoute) {
						if (!hasPathKey)
							throw std::runtime_error("Missing required `path` key in [[servers.routes]] block");
						if (currentRoutePath.empty())
							throw std::runtime_error("Empty `path` value in [[servers.routes]] block");
						commitRoute(currentCluster, currentRoutePath, currentRoute);
					}
					// start a fresh route
					currentRoute       = Route();
					currentRoutePath.clear();
					hasPathKey         = false; 
					inRoute            = true;
					currentSection.clear();      // keys now belong to the route
					continue;
					
				}
				else if(section == "servers")
				{
					if (inRoute)
						commitRoute(currentCluster, currentRoutePath, currentRoute);
					inRoute = false;

					if (currentCluster && currentCluster->getPorts().empty())
						throw std::runtime_error("Missing required `port` key in [[servers]] block");
					clusters.push_back(Cluster());        // create & select it
					currentCluster   = &clusters.back();
					currentSection.clear();               // keys now belong to server
					continue;
					
				}
				else
					throw std::runtime_error("Invalid section Header");
            // }
            // //----------------------------------------------------------
            // // 2) starting a new [[servers]] block
            // //----------------------------------------------------------
            // if (line.compare(0,2,"[[") == 0 &&
            //     line.find("servers") != std::string::npos)
            // {
                // commit any route that was still open
            }
            //----------------------------------------------------------
            // 3) single‑bracket section: [global] / [default_error_pages]
            //----------------------------------------------------------
            if (line[0] == '[' && line[1] != '[')
            {
                // commit any open route *before* switching section
                if (inRoute)
                    commitRoute(currentCluster, currentRoutePath, currentRoute);
                inRoute = false;

                currentSection = _trim(line.substr(1, line.size()-2)); // strip [ ]
				if (currentSection == "global") {
					if (seenGlobalBlock)
						throw std::runtime_error("Duplicate [global] block");
					seenGlobalBlock = true;
				} else if (currentSection == "default_error_pages") {
					if (seenDefaultErrorPagesBlock)
						throw std::runtime_error("Duplicate [default_error_pages] block");
					seenDefaultErrorPagesBlock = true;
				}else
				{
					throw std::runtime_error("Invalid Section header");
				}
					

                continue;
            }
        }

        // --------- KEY‑VALUE line -----------------------------------
        size_t eq = line.find('=');
        if (eq == std::string::npos) 
			throw std::runtime_error("Invalid line" + line);            // malformed / skip

        std::string key = _trim(line.substr(0, eq));
        std::string val = _trim(line.substr(eq + 1));

        //------------------------------------------------------------------
        // 1) GLOBAL / DEFAULT_ERROR_PAGES
        //------------------------------------------------------------------
        if      (currentSection == "global")
        {
            if (key == "max_body_size")
                maxBodySize = static_cast<size_t>(strtoul(val.c_str(), 0, 10));
			else
				throw std::runtime_error("Invalid Var for global label");
        }
        else if (currentSection == "default_error_pages")
        {
			bool isAllDigits = true;
			for (std::string::const_iterator it = key.begin(); it != key.end(); ++it) {
				if (!isdigit(static_cast<unsigned char>(*it))) {
					isAllDigits = false;
					break;
				}
			}
			if (!isAllDigits)
				throw std::runtime_error("Invalid error code in [default_error_pages]: " + key);
            int code = atoi(key.c_str());
			 if (code < 100 || code > 599)
        		throw std::runtime_error("Invalid HTTP status code: " + key);
            if (val.empty()) {
        		throw std::runtime_error("Empty string not permitted");
            }

			if (val[0] != '"' || val[val.size() - 1] != '"')
				throw std::runtime_error("Error page value must be enclosed in double quotes");

    		// Ensure there is only one quoted string and nothing after the closing quote
			size_t closingQuote = val.find('"', 1); // find second quote
			if (closingQuote != val.size() - 1)
				throw std::runtime_error("Unexpected content after closing quote in error page path");

			// Strip the quotes
			val = val.substr(1, val.size() - 2);

			errorPages[code] = val;
        }
        //------------------------------------------------------------------
        // 2) SERVER‑LEVEL keys  (host, port, hostnames)
        //------------------------------------------------------------------
        else if (currentCluster && !inRoute)
        {
            if      (key == "host")
			{
				if (val.empty())
        		throw std::runtime_error("Empty string not permitted");

			if (val[0] != '"' || val[val.size() - 1] != '"')
				throw std::runtime_error("Error page value must be enclosed in double quotes");

    		// Ensure there is only one quoted string and nothing after the closing quote
			size_t closingQuote = val.find('"', 1); // find second quote
			if (closingQuote != val.size() - 1)
				throw std::runtime_error("Unexpected content after closing quote in error page path");

			// Strip the quotes
				val = val.substr(1, val.size() - 2);
                currentCluster->setHost(val.substr(0, val.size())); // strip quotes
			}
            else if (key == "port")
                currentCluster->setPorts(parseIntList(val));
            else if (key == "hostnames")
                currentCluster->setHostnames(parseStringList(val));
			else
				throw std::runtime_error("Invalid key for [[servers]] block");
        }
        //------------------------------------------------------------------
        // 3) ROUTE‑LEVEL keys
        //------------------------------------------------------------------
        else if (currentCluster && inRoute)
        {
			if(key == "path" || key == "root" || key == "upload_dir"||key == "redirect"||key == "interpreter")
			{
				if (val.empty())
        		throw std::runtime_error("Empty string not permitted");

				if (val[0] != '"' || val[val.size() - 1] != '"')
					throw std::runtime_error("Error page value must be enclosed in double quotes");

    		// Ensure there is only one quoted string and nothing after the closing quote
				size_t closingQuote = val.find('"', 1); // find second quote
				if (closingQuote != val.size() - 1)
					throw std::runtime_error("Unexpected content after closing quote in error page path");

				// Strip the quotes
				val = val.substr(1, val.size() - 2);
			}
            if      (key == "path")
			{
                currentRoutePath = val;
				hasPathKey         = true; 
			}
            else if (key == "root")
                currentRoute.setRootDir(val);
            else if (key == "index") {
                std::vector<std::string> v = parseStringList(val);
                currentRoute.setIndexFiles(v);
            }
            else if (key == "methods") {
                std::vector<std::string> v = parseStringList(val);
                currentRoute.setAllowedMethods(std::set<std::string>(v.begin(), v.end()));
            }
            else if (key == "autoindex")
                currentRoute.setAutoindex(val == "true");
            else if (key == "upload_dir")
                currentRoute.setUploadDir(val);
            else if (key == "redirect")
                currentRoute.setRedirect(val);
            else if (key == "extensions") {
                CGIConfig cgi = currentRoute.getCGIConfig();
                cgi.setExtensions(parseStringList(val));
                currentRoute.setCGIConfig(cgi);
            }
            else if (key == "interpreter") {
                CGIConfig cgi = currentRoute.getCGIConfig();
                cgi.setInterpreter(val);
                currentRoute.setCGIConfig(cgi);
            }
			else
				throw std::runtime_error("Invalid key for [[server.routes]] block");	
        }
		else
		{
			throw std::runtime_error("Key-value pair found outside of any valid section: " + key);
		}
    }

    //------------------------------------------------------------------
    // commit the last open route / cluster after EOF
    //------------------------------------------------------------------
    if (inRoute) {
		if (currentCluster && currentCluster->getPorts().empty())
			throw std::runtime_error("Missing required `port` key in final [[servers]] block");
		if (!hasPathKey)
			throw std::runtime_error("Missing required `path` key in final [[servers.routes]] block");
		if (currentRoutePath.empty())
			throw std::runtime_error("Empty `path` value in final [[servers.routes]] block");
		commitRoute(currentCluster, currentRoutePath, currentRoute);
	}
	if (currentCluster && currentCluster->getPorts().empty())
		throw std::runtime_error("Missing required `port` key in final [[servers]] block");
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



bool isValidIPv4(const std::string& ip)
{
    std::istringstream iss(ip);
    std::string token;
    int parts = 0;

    while (std::getline(iss, token, '.'))
    {
        if (++parts > 4)
            return false;
        if (token.empty() || token.length() > 3)
            return false;
        for (size_t i = 0; i < token.length(); ++i)
            if (!isdigit(token[i]))
                return false;
        int num = std::atoi(token.c_str());
        if (num < 0 || num > 255)
            return false;
        if (token[0] == '0' && token.length() > 1)
            return false; // leading zero
    }
    return parts == 4;
}


void validateAndFixConfig(WebServerConfig& config)
{
    std::vector<Cluster> validClusters;
    std::set<std::string> seenHosts;

    const std::vector<Cluster>& originalClusters = config.getClusters();

    for (size_t i = 0; i < originalClusters.size(); ++i)
    {
        const Cluster& cluster = originalClusters[i];
        const std::string& host = cluster.getHost();

        // Test 1: Is the host a valid IPv4 address?
        if (!isValidIPv4(host))
        {
            std::cerr << "Invalid IP address in server #" << i << ": " << host << std::endl;
            continue; // skip this cluster
        }

        // Test 2: Check for duplicate host
        if (seenHosts.find(host) != seenHosts.end())
        {
            std::cerr << "Duplicate host detected: " << host << " — skipping server #" << i + 1 << std::endl;
            continue; // skip duplicate
        }

        seenHosts.insert(host);
        validClusters.push_back(cluster);
    }

    // Replace clusters with the validated ones
    config.setClusters(validClusters);
}



int Parser::MainParser(std::string path, WebServerConfig& conf)
{
	try
	{
		OpenTomlFile(path);
		parseTOML(path, conf);
		validateAndFixConfig(conf);
		// printConfigs(conf);
		

		// this->lines = ReadLines();
		// parseLines(conf);
	}
	catch(std::exception& e)
	{
        throw std::runtime_error(e.what());
	}
	return 0;
}