#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include "arrow.h"

int main(int argc, char* argv[])
{
	std::string line;
	std::ifstream gridFile (argv[1]);
	std::vector<int> lengths;
	std::vector<std::vector<char> > grid;

	while(gridFile >> line)
	{
		if (line[0] == '!')
			continue;
		if (line[0] == '+')
			lengths.push_back(std::stoi(line));
		else
		{
			grid.push_back(std::vector<char> ());
			for (unsigned int i = 0; i < line.size(); i++)
				grid.back().push_back(line[i]);
		}
		
	}

	Arrow testArrow(2,0,2,3,true);
	std::cout << testArrow.representation(grid) << std::endl;

	gridFile.close();
}