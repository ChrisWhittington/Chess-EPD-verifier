//
// EPD-to-u64s-and-flags.cpp : This file contains the 'main' function. 
// Program execution begins and ends there. Obviously you will need to modify.
//
// Designed to be a stand alone module, contains all necessary chess legality testing
//  routines without need for tables or lookup initialisations (like no magics and so on)
// Assumes 64 bit u64s
// Code is extremely pedantic and tries to check for everything possible that could go wrong
// 

#include <stdlib.h>  
#include <stdio.h>  
#include <stdint.h>
#include <string.h>
#include <intrin.h>
#include <assert.h>

#include "useful.h"
#include "structures.h"
#include "bitboards.h"
#include "epd.h"
#include "chess-functions.h"

int main()
{
	printf("FEN/EPD string reader\n");
	printf("(C) Chris Whittington 2020\n");
	printf("Do what you want with it, except ensure the author\n");
	printf("gets this credit documentation in the source code\n");
	printf("Up to you if you want to add credit in any binary\n");
	printf("But it is kind of expected that you do, helps to\n");
	printf(" spread the word to those engine programmers who still\n");
	printf(" don't error check their epd inputs!");
	printf("======================================================\n\n");

	printf("Some of the string handlers are Visual Studio compatible with _s extensions\n");
	printf(" probably these need minor syntax adjusts for other compilers\n");

    // scruffy internal data structure
	GAMEDATA gamedata[1];
	GAMEDATA* gd = gamedata;

	// test startpos initialiser ....
	InitBoard(gd);
	// should return with gamedata structure correctly set

	// test a bunch of EPDs, some of which are deliberately bad ...
	// pedantically reports to console window	
	TestEpdCode(gd);

	// test EPD loader for UCI position format <FEN moves e2e4 g8f6>
	TestUCI_PositionFormatEPD(gd);

	while (1)
	{	// lazily sitting in a debugger loop rather than crashing back to windows
		int a = 1;
	}
	return 0;
}


