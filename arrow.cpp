//
//  arrow.cpp
//  CPP file includes functions' implementation of the 
//  the Arrow class.
//
//  Created by Frost Tianjian Xu on 3/5/2019.
//  Copyright © 2019 Frost Tianjian Xu. All rights reserved.
//

#include "arrow.h"
#include <vector>
#include <string>
#include <cassert>

// The length function will calculate the length of the 
// arrow. 
int Arrow::length() const 
{
	if (isAcross_)
		return endColumn_ - startColumn_;
	else
		return endRow_ - startRow_;
	
}

// The isSubArrrow function will determine whether the current
// arrow object is part of other vector. 
// eg. If current vector is across and starts at 1,2 and ends at 1,3,
// and the other vector starts at 1,0 and ends at 1,3. The current
// vector will be determined as a overlapped arrow.
// If current vector is still across and starts at 1,2 and ends at 1,4,
// and the other vector starts at 1,0 and ends at 1,3. The current vector
// will also considerred to be overlapped.
bool Arrow::isOverlapped(const Arrow &other) const
{
	if (getIsAcross() != other.getIsAcross())
		return false;
	if (this->getIsAcross() && other.getIsAcross()
	&& (this->getStartRow() == other.getStartRow())
	&& (this->getStartColumn() >= other.getStartColumn())
	&& (this->getEndColumn() <= other.getEndColumn()))
		return true;
	if (this->getIsAcross() && other.getIsAcross() 
	&& (this->getStartRow() == other.getStartRow())
	&& (this->getStartColumn() <= other.getEndColumn()))
		return true;
	
	if (!(this->getIsAcross()) && !(other.getIsAcross())
	&& (this->getStartColumn() == other.getStartColumn())
	&& (this->getStartRow() >= other.getStartRow())
	&& (this->getEndRow() <= other.getEndRow()))
		return true;

	if (!(this->getIsAcross()) && !(other.getIsAcross()) 
	&& (this->getStartColumn() == other.getStartColumn())
	&& (this->getStartRow() <= other.getEndRow()))
		return true;
	return false;
}

// Increase the arrow by one row/column, depending on the 
// arrow's direction.
void Arrow::increment()
{
	if (isAcross_)
		endColumn_++;
	else
		endRow_++;
}

// Decide whether two Arrow obejects are exactly the same.
bool Arrow::operator==(const Arrow& other) const
{
	return ((this->getIsAcross() == other.getIsAcross())
	&& (this->getStartRow() == other.getStartRow())
	&& (this->getStartColumn() == other.getStartColumn())
	&& (this->getEndRow() == other.getEndRow())
	&& (this->getEndColumn() == other.getEndColumn()));
}


// This function will return the string the arrow represents
// on a given grid.
std::string Arrow::representation(
	const std::vector<std::vector<char> > &grid ) const
{
	// Assume the grid is valid.
	assert(grid.size() >= 2);
	for (unsigned int i = 0; i < grid.size(); i++)
		assert(grid[i].size() >= 2);
	
	

	// Add the characters into a c-style string.
	char word[this->length()];
	if (isAcross_)
	{
		// The wordLength variable is used to ensure the arrow
		// does not goes out of the boundary of the grid.
		int wordLength = grid[0].size() - startColumn_;
		wordLength = std::min(this->length(), wordLength);
		for (unsigned int i = 0; i < wordLength; i++)
			word[i] = grid[startRow_][startColumn_ + i];
		word[wordLength] = '\0';
	}
	else
	{
		int wordLength = grid.size() - startRow_;
		wordLength = std::min(this->length(), wordLength);
		for (unsigned int i = 0; i < wordLength; i++)
			word[i] = grid[startRow_ + i][startColumn_];
		word[wordLength] = '\0';
	}
	// Convert c-style into c++ string and return it.
	return std::string(word);

}