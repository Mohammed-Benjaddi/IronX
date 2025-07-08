/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HelperFunctions.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nhayoun <nhayoun@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 17:32:43 by ael-maaz          #+#    #+#             */
/*   Updated: 2025/07/08 19:04:01 by nhayoun          ###   ########.fr       */
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


std::string removeSpaces(const std::string& s)
{
	size_t start = s.find_first_not_of(" ",1);
	// std::cout <<start << std::endl;
    if (start == std::string::npos) {
        return "";
	}
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