/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nhayoun <nhayoun@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:38:56 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/06/29 15:28:38 by nhayoun          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
// #include <cstdint>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "WebServerConfig.hpp"


class Parser {
	public:
		Parser();
		~Parser();
		int OpenTomlFile(std::string& path);
		int MainParser(std::string path, WebServerConfig &);
		std::ifstream infile;
		std::vector<std::string> lines;
		std::vector<std::string> ReadLines();
		void parseLines(WebServerConfig& conf);
};

std::string checkLabel(std::string label);
std::string checkBracketLabel(std::string label);
std::string trim(const std::string& s);
std::string removeSpaces(const std::string& s);
int checkExtension(std::string path);
int testKey(std::string key, std::string value, std::string label);