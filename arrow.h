//
//  arrow.h
//  Header file includes declaration of functions of the 
//  the Arrow class.
//
//  Created by Frost Tianjian Xu on 3/5/2019.
//  Copyright © 2019 Frost Tianjian Xu. All rights reserved.
//

#ifndef __ARROW_H
#define __ARROW_H

#include <vector>
#include <string>
#include <cassert>

/** The Arrow class is very similar with the mathematical
 * concept of "vector", which has a length and direction.
 * This arrow class is specifically designed for the 
 * crossword problems and has the following member 
 * variables:
 * 
 *  startRow_, startColumn_, endRow_, endColumn_:
 * 		int representing the starting and ending position
 * 		of the arrow at the grid
 * 
 * 	isAcross_: 	a boolean deciding the direction of 
 * 		the arrow. For simplicity, an arrow either goes 
 * 		down or goes across
 * 
 */
class Arrow
{
	private:
		// MEMBER VARIABLE
		int startRow_;
		int startColumn_;
		int endRow_;
		int endColumn_;
		bool isAcross_;
	
	public:
		// CONSTRUCTOR
		Arrow(int startRow, int startColumn, int endRow, 
		int endColumn, bool isAcross): 
			startRow_(startRow), startColumn_(startColumn),
			endRow_(endRow), endColumn_(endColumn), 
			isAcross_(isAcross) {}
		
		// ACCESOR
		int getStartRow() const { return startRow_; }
		int getStartColumn() const { return startColumn_; }
		int getEndRow() const { return endRow_; }
		int getEndColumn() const { return endColumn_; }
		bool getIsAcross() const { return isAcross_; }

		int length() const;
		bool isOverlapped(const Arrow &other) const;
		void increment();
		bool operator==(const Arrow &other) const;
		std::string representation(const std::vector<std::vector<char> > &grid ) const;

};



#endif