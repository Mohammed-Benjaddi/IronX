/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-maaz <ael-maaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:38:56 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/06/02 22:34:38 by ael-maaz         ###   ########.fr       */
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
		int OpenTomlFile();
		int MainParser();
		std::ifstream infile;
		std::vector<std::string> lines;
		std::vector<std::string> ReadLines();
		void parseLines(WebServerConfig& conf);
};
