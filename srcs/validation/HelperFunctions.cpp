/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-maaz <ael-maaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 17:32:43 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/06/03 22:24:17 by ael-maaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../headers/Parser.hpp"

std::string checkLabel(std::string label)
{
	if(label != "global" && label != "default_error_pages" && label != "servers" && label != "servers.routes")
		return "bad";
	else
		return label;
}

std::string checkBracketLabel(std::string label)
{
	if(label != "servers" && label != "servers.routes")
		return "bad";
	else
		return label;
}

std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string removeSpaces(const std::string& s)
{
	size_t start = s.find_first_not_of(" ",1);
	std::cout <<start << std::endl;
    if (start == std::string::npos)
        return "";
    // size_t end = s.find_last_not_of(" \t\r\n");
	return "";
}

int checkExtension(std::string path)
{
	size_t i = path.length() - 1;
	if(path[i] == 'l')
	{
		i--;
		if(path[i] == 'm')
		{
			i--;
			if(path[i] == 'o')
			{
				i--;
				if(path[i] == 't')
				{
					i--;
					if(path[i] == '.')
						return 0;
				}
			}
		}
	}
	return 1;
	
}