/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-maaz <ael-maaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:38:54 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/06/03 22:51:52 by ael-maaz         ###   ########.fr       */
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


int Parser::OpenTomlFile(std::string path)
{
	if(checkExtension(path) == 1)
		throw std::runtime_error("bad extension");
	std::string fullpath = "./config/" + path;
	this->infile.open(fullpath);
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



			// this->lines[i] = removeSpaces(this->lines[i]);
			// this->lines[i].erase(std::remove(this->lines[i].begin(), this->lines[i].end(), ' '), this->lines[i].end());
			// std::cout <<this->lines[i]<< std::endl;
void Parser::parseLines(WebServerConfig& conf)
{
	(void) conf;
	std::vector<std::string> data_lines;
	// bool label_flag = false;
	for(size_t i = 0; i < this->lines.size();i++)
	{
		std::string label;
		if(this->lines[i][0] == '[')
		{
			if(this->lines[i][1] == '[')
			{
				size_t pos = this->lines[i].find(']');
				if(pos == std::string::npos || (this->lines[i][pos+1] != ']') || ((pos + 1) != this->lines[i].size() - 1))		
					throw std::runtime_error("Invalid syntax");
				else
				{
					label = this->lines[i].substr(2,pos - 2);
					std::cout << "label line: " <<label << std::endl;
					if(checkBracketLabel(trim(label)) == "bad")
						throw std::runtime_error("Invalid label");
					else
						std::cout << "Something else\n";
						//fillConfFile
						
				}
			}
			else
			{
				size_t pos = this->lines[i].find(']');
				if(pos == std::string::npos || pos != this->lines[i].size() - 1)
				{					
					throw std::runtime_error("Invalid syntax");
				}
				else
				{
					label = this->lines[i].substr(1,pos - 1);
					std::cout << "label line: " <<label << std::endl;
					if(checkLabel(trim(label)) == "bad")
						throw std::runtime_error("Invalid label");
					else
						std::cout << "Something else\n";
						//fillConfFile
						
				}
			}
		}
		else
			std::cout << "data line: "<< this->lines[i] << std::endl;
	}
}

int Parser::MainParser(std::string path)
{
	WebServerConfig conf;
	try
	{
		OpenTomlFile(path);
		this->lines = ReadLines();
		parseLines(conf);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}