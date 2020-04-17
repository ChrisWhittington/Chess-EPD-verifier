#pragma once


//
#if 1
typedef struct GAMEDATA
{
	u64 pieces[2];	// whites, blacks
	u64 pawns;
	u64 nites;
	u64 diagonals;
	u64 manhattans;
	int king_sq[2];

	int castlestatus;
	int epsquare;
	int moveno;
	int rule50;
	int color;
} GAMEDATA;

#else
typedef struct GAMEDATA
{
	u64 pawns[2];
	u64 nites[2];
	u64 bishops[2];
	u64 rooks[2];
	u64 queens[2];
	u64 king[2];

	int castlestatus;
	int epsquare;
	int moveno;
	int rule50;
	int color;
} GAMEDATA;
#endif
//

// availability of castling flags (from FEN, or game status or whatever)
#define W_SHORTCASTLE 0b00000001
#define W_LONGCASTLE  0b00000010
#define B_SHORTCASTLE 0b00000100
#define B_LONGCASTLE  0b00001000

#define NORMALRULE50 100
