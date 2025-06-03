/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-maaz <ael-maaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:38:56 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/06/03 22:24:28 by ael-maaz         ###   ########.fr       */
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

#include "WebServerConfig.hpp"


class Parser {
	public:
		Parser();
		~Parser();
		int OpenTomlFile(std::string path);
		int MainParser(std::string path);
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