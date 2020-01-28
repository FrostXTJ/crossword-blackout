//
//  main.cpp
//  This CPP contains main function handling the crossword 
//  blackout problems in homework 6.
//
//  Created by Frost Tianjian Xu on 3/2/2019.
//  Copyright © 2019 Frost Tianjian Xu. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include "arrow.h"

// Pre-declaration of all the functions to be used.
void searchWords(const std::vector<std::vector<char> > &grid,
	const std::vector<std::string> &dictionary, const std::vector<int>& lengths,
	std::vector<Arrow>& words);
bool searchFromLoc(Arrow &path, const std::vector<std::vector<char> > &grid,
	std::vector<std::string> &dictionary, std::vector<int> lengths);
void permAndCombine(int n, int r, const std::vector<Arrow>& words,
	std::vector<std::vector<Arrow> >& result);
void splitByLength(const std::vector<Arrow> &words, 
	std::vector<std::vector<Arrow> > &result);
template <class T> void combine(int num, std::vector<T> elements, 
	std::vector<std::vector<T> > &result);
template <class T> void combine(int num, std::vector<T> chosen,
	std::vector<T> left, std::vector<std::vector<T> > &result);
template <class T> void simpleMerge(std::vector<T> &vecOne, 
	std::vector<T> &vecTwo);
void seperateCases(const std::vector<Arrow> &wordsFound,
const std::vector<int> &lengths, std::vector<std::vector<Arrow> > &result);
int generateGrids(const std::vector<std::vector<Arrow> > &allCases, 
	const std::vector<std::vector<char> > &originalGrid,
	std::vector<std::vector<std::vector<char> > > &allGrids,
	bool getOneSolution);
bool checkGrid(const std::vector<std::vector<char> > &blackoutBoard, 
	const std::vector<Arrow> &expectedWords);
bool checkGridFromLoc(Arrow &path, 
	const std::vector<std::vector<char> > &blackoutBoard, 
	const std::vector<Arrow> &expectedWords);


/** The program takes 4 command line arguments.
 * 1st argument: the dictionary filename
 * 2nd argument: the initial grid filename
 * 3rd argument: the solution mode
 * 				 (one_solution, all_solutions)
 * 4th argument: the output mode
 * 				 (count_only, print_boards)
 */
int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		std::cerr << "ERROR: TOO FEW ARGUMENTS ARE GIVEN."
		<< std::endl;;
		exit(1);
	}
	if (argc > 6)
	{
		std::cerr << "ERROR: TOO MANY ARGUMENTS ARE GIVEN."
		<< std::endl;;
		exit(1);
	}

	std::string dictFilename = argv[1];
	std::string gridFilename = argv[2];
	std::string solutionMode = argv[3];
	std::string outputMode = argv[4];
	std::string line;


	// Read the dictionary file and store the words in a vector.
	std::ifstream dictionaryFile(dictFilename);
	if (!dictionaryFile.good())
	{
		std::cerr << "ERROR: UNABLE TO OPEN THE DICTIONARY FILE."
		<< std::endl;
		exit(1);
	}

	std::vector<std::string> dictionary;
	while(dictionaryFile >> line)
		dictionary.push_back(line);

	dictionaryFile.close();
	
	// Read the grid file and store the length constraints in a 
	// vector and the grid line in another 2-D vector.
	std::ifstream gridFile (gridFilename);
	if (!gridFile.good())
	{
		std::cerr << "ERROR: UNABLE TO OPEN THE GRID FILE."
		<< std::endl;
		exit(1);
	}

	std::vector<int> lengths;
	std::vector<std::vector<char> > grid;

	while(gridFile >> line)
	{
		if (line[0] == '!')
			continue;
		if (line[0] == '+')
		{
			while (std::stoi(line) >= lengths.size())
				lengths.push_back(0);
			lengths[std::stoi(line)]++;
		}
		else
		{
			grid.push_back(std::vector<char> ());
			for (unsigned int i = 0; i < line.size(); i++)
				grid.back().push_back(line[i]);
		}
	}

	gridFile.close();
	
	std::vector<Arrow> wordsFound;
	searchWords(grid, dictionary, lengths, wordsFound);

	int numOfSolutions;
	std::vector<std::vector<std::vector<char> > > allGrids;

	if (solutionMode == "all_solutions" )
	{
		std::vector<std::vector<Arrow> > allCases;
		seperateCases(wordsFound, lengths, allCases);
		numOfSolutions = generateGrids(allCases, grid, allGrids, false);
	}
	else if (solutionMode == "one_solution")
	{
		std::vector<std::vector<Arrow> > allCases;
		seperateCases(wordsFound, lengths, allCases);
		numOfSolutions = generateGrids(allCases, grid, allGrids, true);
	}
	else
	{
		std::cerr << "ERROR: CANNOT RECOGNIZE THE SOLUTION MODE."
		<< std::endl;
		exit(1);
	}


	if (outputMode == "print_boards")
	{
		std::cout << "Number of solution(s): " << numOfSolutions 
		<< std::endl;
		for (unsigned int i = 0; i < allGrids.size(); i++)
		{
			std::cout << "Board:" << std::endl;
			for (unsigned int j = 0; j < allGrids[i].size(); j++)
			{
				for (unsigned int k = 0; k < allGrids[i][j].size(); k++)
					std::cout << allGrids[i][j][k];
				std::cout << std::endl;
			}
		}
	}
	
	else if (outputMode == "count_only")
		std::cout << "Number of solution(s): " << numOfSolutions 
		<< std::endl;

	else
	{
		std::cerr << "ERROR: CANNOT RECOGNIZE THE OUTPUT MODE."
		<< std::endl;
		exit(1);
	}

	return 0;
}

/** searchWords function will go through the grid and look for if 
 * there is any word in the dictionary. If there is one, it will 
 * record the words' path in an Arrow object, and all the Arrows will
 * be stored in a vector. The function searchFromLoc will be called.
 * @oaram {vector<vector <char> >} grid		the grid table
 * @param {vector<string>} dictionary	the dicionary vector
 * @param {vector<int>} length		the vector of length constraints
 * @param {vector<Arrow>} words		the Arrow vector of words found
 */

void searchWords(const std::vector<std::vector<char> > &grid,
	const std::vector<std::string> &dictionary, const std::vector<int> &lengths,
	std::vector<Arrow>& words)
{
	for (unsigned int i = 0; i < grid.size(); i++)
	{
		for (unsigned int j = 0; j < grid[0].size(); j++)
		{
			Arrow cross(i, j, i, j, true);
			Arrow down(i, j, i, j, false);
			std::vector<std::string> dictCopyOne = dictionary;
			std::vector<std::string> dictCopyTwo = dictionary;
			bool subWordCheck = false;
			// Call the helper function, examing across.
			if(searchFromLoc(cross, grid, dictCopyOne, lengths))
			{	// Check if the word is part of the other words
				// we have found.
				for (unsigned int k = 0; k < words.size(); k++)
					if (cross.isOverlapped(words[k]))
					{ subWordCheck = true; break; }
				if (!subWordCheck)
					words.push_back(cross);
			}
			subWordCheck = false;
			if(searchFromLoc(down, grid, dictCopyTwo, lengths))
			{
				for (unsigned int k = 0; k < words.size(); k++)
					if (down.isOverlapped(words[k]))
					{ subWordCheck = true; break; }
				if (!subWordCheck)
				words.push_back(down);
			}
		}
	}

}

/** searchFromLoc is a recursive function, it will examine whether
 * an arrow can form a word in the dictionary. The function will 
 * increment the arrow's size by one and go through the dictionary 
 * once. It will erase the words in the dictionary which has different
 * with the Arrow's representation. When the dictioary's 
 * first word is exactly the same with the arrow's represenation and fit 
 * the lenght constraints, return true; if the dictionary's size is 0, 
 * return false; otherwise the function will continue recursing.
 * @param {Arrow} path		an Arrow object recording the path of a word
 * @param {vector <vector<char> >} grid		the grid table
 * @param {vector <string>}	dictionary		the dictionary vector
 **(This argument will be modified during the function's recursiong and
 * should not be passed by reference of a copy of the original dictionary.)
 * @param {vector <int>} lengths 	the length constraints
 * @return {bool}	a boolean value showing whether the word has been found
 */

bool searchFromLoc(Arrow& path, const std::vector<std::vector<char> > &grid,
	std::vector<std::string> &dictionary, std::vector<int> lengths)
{
	// The dictionary is empty, return false.
	if(dictionary.size() == 0)
		return false;
	// The first element dictionary is is eactly the same
	// with path's representation. A word is found.
	if(path.representation(grid) == dictionary[0]
	&& lengths[path.length()] != 0)
		return true;

	path.increment();
	std::vector<std::string>::iterator itr = dictionary.begin();
	char lastChar = *(path.representation(grid).rbegin());
	while (itr != dictionary.end())
	{
		if((*itr)[path.length() - 1] != lastChar)
			itr = dictionary.erase(itr);
		else
			itr++;
		
	}

	return searchFromLoc(path, grid, dictionary, lengths);
}

/** splitByLength function will read a vector of Arrow,. It will split 
 * the Arrows into different vectors, and Arrows of same length will be 
 * appended in the same vector. All the splited vectors will be stored into
 * a 2D vector called result. Order does not matter in this function.
 * @param {vector<Arrow>} words		a plain vector of Arrows
 * @param {vector<vector<Arrow> >} result
 * 		a 2D vector of Arrows splitted by length
 * 
 */
void splitByLength(const std::vector<Arrow> &words, 
	std::vector<std::vector<Arrow> > &result)
{
	for (unsigned int i = 0; i < words.size(); i++)
	{
		// A bool representing if other Arrows of same length 
		// have already been appended.
		bool existSameLength = false;
		for (unsigned int k = 0; k < result.size(); k++)
		{
			if (result[k][0].length() == words[i].length())
			{
				existSameLength = true;
				result[k].push_back(words[i]);
				break;
			}
		}
		if(!existSameLength)
		{
			std::vector <Arrow> tmp;
			tmp.push_back(words[i]);
			result.push_back(tmp);
		}
	}
}

/** The driver funtion of combine, amd it will call its helper recursive
 * fucntion. The function will read a vector of all elements and
 * a number showing how many elements needed to be selected to form a 
 * combination it will append all the possible selections into a 
 * double vector called result. The function is made to be a template 
 * function so that it can be reused in the future program.
 * 
 * @param {int} num		the number of elements to be picked
 * @param {vector<T>} elements		the vector of all elements
 * @param {vector<vector<T> >} result		
 * 		the result vector of all combinations
 **(The arguement of the function will be copied and reset for many times,
 * so except for the result vector, all other arguments are passed via copy,
 * not by reference. It will use many memory spaces and should be 
 * carefully used for long elements vector.) 
 */
template <class T>
void combine(int num, std::vector<T> elements, 
	std::vector<std::vector<T> > &result)
{
	// The elements to be picked should not be greater than the
	// the number of total elements.
	assert(num <= elements.size());
	combine(num, std::vector<T>(), elements, result);
}

/** The helper function of combine. This function will recursively call 
 * itself and appending elements from vector left into vector chosen. 
 * Each time when a possible combination is found, it will append it into the 
 * result vector. The function is a combination of recursion and loop.
 * The terminal case for the the recursion is 1) a combination is found,
 * 2) the left vector is empty.
 * @param {int} num		the number of elements to be picked
 * @param {vector<T>} chosen		the vector of elements already picked
 * @param {vector<T>} left			the vector of elements to be picked
 * @param {vector<vector<T> >} result		
 * 		the result vector of all combinations
 **(The arguement of the function will be copied and reset for many times,
 * so except for the result vector, all other arguments are passed via copy
 * instead of by reference. It will use many memory spaces and should be 
 * carefully used for long elements vector.) 
 */
template <class T>
void combine(int num, std::vector<T> chosen,
	std::vector<T> left, std::vector<std::vector<T> > &result)
{
	// A combination is found, append the combination into result.
    if (chosen.size() == num)
    {
        result.push_back(chosen);
        return;
    }
    // The left vector is empty, termminate the recursion. 
    if(left.empty())
        return;

    typename std::vector<T>::iterator itr = left.begin();
    std::vector<T> chosenCopy (chosen);
	
	// Go through the left vecor and append each element into chosen.
	// and call the recursion function.
    while(itr != left.end())
    {
        chosen = chosenCopy;
        chosen.push_back(*itr);
        itr = left.erase(itr);
        combine(num, chosen, left, result);

    }
}

// The simpleMerge function will append each item in vecTwo into vecOne,
// and the order does not matter for this function.
template <class T>
void simpleMerge(std::vector<T> &vecOne, std::vector<T> &vecTwo)
{
    for (unsigned int i = 0; i < vecTwo.size(); i++)
        vecOne.push_back(vecTwo[i]);
}

/** This function will seperate different crossword board cases according
 * to length constraints. 
 * 
 * eg. If the representation of each Arrow in the wordsFound vector is 
 * {BLUES, FLEES, USE, EKE, SIS} and the lengths vector is {0, 0, 0, 1, 0, 2}
 * (One three-character word and two five-character words), the result 
 * vector should be {{BLUES, FLEES, USE}, {BLUES, FLEES, EKE}, 
 * {BLUES, FLEES, SIS}}.
 * 
 * @param {vector<Arrow>} wordsFound		
 * 		a vector containing all the word found
 * @param {vector<int>}	lengths		the vector of length constraints
 * @param {vector<vector<Arrow> >} result
 * 		the result vector containing all the possible cases
 * 
 */
void seperateCases(const std::vector<Arrow> &wordsFound,
    const std::vector<int> &lengths, std::vector<std::vector<Arrow> > &result)
{
	std::vector<std::vector<Arrow> > splitted;
	splitByLength(wordsFound, splitted);
	for(unsigned int i = 0; i < splitted.size(); i++)
    {
		int wordLength = splitted[i][0].length();
		// Ensure the lenghts constraints cover the situation for
		// the Arrows of wordLength.
		assert(lengths.size() > wordLength);
		// If the total number of Arrows of  wordLengths is less 
		// than the number to be picked for Arrows of that wordLength.
		// There should be no solutions. The result will be cleared
		// and the function will be terminated.
		if (splitted[i].size() < lengths[wordLength])
		{
			result.clear(); return;
		}

		int num = lengths[wordLength];
		std::vector<std::vector<Arrow> > temp;
		combine(num, splitted[i], temp);

		// Append all the combinations in the temp into result if 
		// result is previously empty.
        if(result.empty())
			simpleMerge(result, temp);
		
		// Distribute each combination cases with previous ones.
		// Every combination in the old result should be copied n times, 
		// and n is the size of the new combination.
        else
        {
			std::vector<std::vector<Arrow> > newResult;
            for(unsigned int j = 0; j < result.size(); j++)
			{
                for (unsigned int k = 0; k < temp.size(); k++)
                    newResult.push_back(result[j]);
			}

			assert(newResult.size() == (result.size() * temp.size()));
			int j = 0; int k = 0;
			while (j < newResult.size())
            {
                if (k == temp.size()) k = 0;
                simpleMerge(newResult[j], temp[k]);
                j++; k++;
                
            }
            result.clear();
            result = newResult;
		}
	}
}

/** The generateGrids function will take in the allCases vector and the 
 * original grid. It will generate all possible blackout grids. The function
 * itself will return the number of the posiible solutions.
 * *************************************************************************
 * To be noted that, not every case in the allCases can form a legal grid.
 * For example, the combination {BLUES, FLEES, SKI} in "blackout1.txt"
 * will not form a valid grid. The grid it generats is 
 * 
 *   B#F
 *   L#L
 *   USE	USE, EKE, SIS are also words existing in the dictionary,
 *   EKE	so this grid is invalid.
 *   SIS
 * 
 * So there will be code checking if the grid generated is valid,
 * if not, it will not be appeded into the allGrids.
 * *************************************************************************
 * 
 * @param {vector<vector<Arrow> >} allCases
 * 		all possible combinations of the Arrow
 * @param {vector<vector<char> >} originalGrid		
 * 		the orignal grid
 * @param {vector<vector<vector<char> > >} allGrids
 * 		a vector of all possible valid grids
 * @param {bool} getOneSolution
 * 		the function will terminate once it gets one valid board
 * 		if getOnesolution is set to true
 */
int generateGrids(const std::vector<std::vector<Arrow> > &allCases, 
	const std::vector<std::vector<char> > &originalGrid,
	std::vector<std::vector<std::vector<char> > > &allGrids,
	bool getOneSolution)
{
	for (unsigned int i = 0; i < allCases.size(); i++)
	{
		// If only one solution is needed,
		// terminate the function once a solution is found
		if (getOneSolution && (allGrids.size() == 1))
			return allGrids.size();

		// Generate a all # board.
		std::vector<std::vector<char> > blackoutBoard;
		for (unsigned int i = 0; i < originalGrid.size(); i++)
			blackoutBoard.push_back(
				std::vector<char> (originalGrid[0].size(), '#'));
		
		// Substitute # with the character
		for (unsigned int j = 0; j < allCases[i].size(); j++)
		{
			std::string str = allCases[i][j].representation(originalGrid);
			if (allCases[i][j].getIsAcross())
			{
				for (unsigned int k = 0; k < str.size(); k++)
					blackoutBoard[allCases[i][j].getStartRow()]
					[allCases[i][j].getStartColumn() + k] = str[k];
			}
			else
			{
				for (unsigned int k = 0; k < str.size(); k++)
					blackoutBoard[allCases[i][j].getStartRow() + k]
					[allCases[i][j].getStartColumn()] = str[k];
			}
		}

		// Check the validity of the board.
		if (checkGrid(blackoutBoard, allCases[i]))
			allGrids.push_back(blackoutBoard);
	}
	return allGrids.size();
	
}

/** checkGrid function will check the validity of the a blackoutBoard.
 * it is very similar to the searchWord function except that it does not 
 * take the dictionary the length constraints. It will first check if 
 * repetition exists in the on the board. Then it will go through the 
 * blackoutBoard and find all possible words. It will compare every word
 * it found with a vector of words expected to exist in the board, and 
 * there is any word found to be not existed in the expected words,
 * the board is invalid. This function will call the recursive function
 * checkGridFromLoc.
 * @param {vector<vector<char> >} blackoutBoard
 * 		a board which has already been blacked out
 * @param {vector<Arrow>} expectedWords
 * 		words expected to exist on this board
 * @return {bool} 	whether or not the blackoutBoard is valid
 */
bool checkGrid (const std::vector<std::vector<char> > &blackoutBoard, 
	const std::vector<Arrow> &expectedWords)
{
	// Check repetition.
	for (unsigned int i = 0; i < expectedWords.size(); i++)
	{
		for (unsigned int j = i + 1; j <expectedWords.size(); j++)
		{
			if (expectedWords[i].representation(blackoutBoard)
			== expectedWords[j].representation(blackoutBoard))
				return false;
		}
	}

	// Check all words.
	for (unsigned int i = 0; i < blackoutBoard.size(); i++)
	{
		for (unsigned int j = 0; j < blackoutBoard[0].size(); j++)
		{
			if (blackoutBoard[i][j] == '#') 
				continue;

			// Check a downside word.
			if (i == 0 || blackoutBoard[i - 1][j] == '#')
			{
				Arrow down(i, j, i, j, false);
				if (!checkGridFromLoc(down, blackoutBoard, expectedWords))
					return false;
			}

			// Check a across word.
			if (j == 0 || blackoutBoard[i][j - 1] == '#')
			{
				Arrow cross(i, j, i, j, true);
				if (!checkGridFromLoc(cross, blackoutBoard, expectedWords))
					return false;
			}
		}
	}
	return true;
}

/** It is a recursive function and will check whether an word on the 
 * blackout is part of the expectedWords. The recursion will keep
 * incremeneting the Arrow path and will terminate in two situations: 
 * 1) the Arrow path ends at a '#'  2) the Arrow  path reaches to the 
 * edge of the board. 
 * @param {Arrow} path		an Arrow object recording the path of a word
 * @param {vector<vector<char> >} blackoutBoard
 * 		a board which has already been blacked out
 * @param {vector<Arrow>} expectedWords
 * 		words expected to exist on this board
 * @return {bool} 	whether or not the path Arrow is one of the expectedWords
 */
bool checkGridFromLoc(Arrow &path, 
	const std::vector<std::vector<char> > &blackoutBoard, 
	const std::vector<Arrow> &expectedWords)
{
	path.increment();

	// The path reaches the edge of the board, examie if the path is one
	// of the expected words.
	if (((path.getIsAcross()) && (path.getEndColumn() == blackoutBoard[0].size()))
	|| ((!path.getIsAcross()) && (path.getEndRow() == blackoutBoard.size())))
	{
		// If it is one character, everything is fine.
		if (path.length() == 1) return true;

		// The length of the word cannot be 2.
		if (path.length() == 2) return false;

		for (unsigned int i = 0; i < expectedWords.size(); i++)
		{
			if (path == expectedWords[i])	return true;
		}
		return false;
	}
	// If the path reaches an "#", examine if it is a valid word in the
	// expected words.
	if (blackoutBoard[path.getEndRow()][path.getEndColumn()] == '#')
	{
		if (path.length() == 1) return true;
		if (path.length() == 2) return false;

		// Compare the path with the expectedWords.
		for (unsigned int i = 0; i < expectedWords.size(); i++)
		{
			if (path == expectedWords[i])	return true;
		}
		return false;
	}

	return checkGridFromLoc(path, blackoutBoard, expectedWords);
}