#include <iostream>
using namespace std;

/* Towers of Hanoi */

/* Print a move. */
void print(int from, int to)
{
    cout << "Move from peg " << from << " to peg " << to << endl; 
}

/* Recursively solve the Towers of Hanoi puzzle. */
int towers(int numDiscs, int start, int goal) 
{ 
    int steps, peg; 

    if(numDiscs < 2)
    { 
	print(start, goal);
	return 1; 
    } 
    else
    { 
	peg = 6 - start - goal;		// Calculate intermediate peg.
	steps  = towers(numDiscs-1, start, peg); 
	steps += towers(1, start, goal); 
	steps += towers(numDiscs-1, peg, goal); 
	return steps; 
    } 
} 


int main() 
{ 
    int numDiscs, start, goal, steps; 

    cout << "Enter the number of disks to be moved: "; 
    cin >> numDiscs; 

    start = 1; 
    goal  = 3; 

    steps = towers(numDiscs, start, goal); 

    cout << endl << numDiscs << " discs moved from peg "
	 << start << " to peg " << goal << " in "
	 << steps << " steps." << endl; 

    return 0; 
} 

// $Id: towers.cpp,v 1.3 2006/05/25 13:57:08 parks Exp $
// Authors: Chris Nevison and Thomas M. Parks
