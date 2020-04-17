#pragma once

#define BOARDWIDTH 8ULL
#define BOARDHEIGHT 8ULL
#define BOARDSIZE (BOARDWIDTH*BOARDHEIGHT)

// start from bottom
#define H1 ((ONE<<BOARDWIDTH)-ONE)
#define H2 (H1<<BOARDWIDTH)
#define H3 (H2<<BOARDWIDTH)
#define H4 (H3<<BOARDWIDTH)
#define H5 (H4<<BOARDWIDTH)
#define H6 (H5<<BOARDWIDTH)
#define H7 (H6<<BOARDWIDTH)
#define H8 (H7<<BOARDWIDTH)

// Connect4 only defined up to H6, V6 and V1 did not contain the top two bits
// start from right (least significant side)
#define V1 ((ONE)+(ONE<<BOARDWIDTH)+(ONE<<(BOARDWIDTH*2ULL))+(ONE<<(BOARDWIDTH*3ULL))+(ONE<<(BOARDWIDTH*4ULL))+(ONE<<(BOARDWIDTH*5ULL))+(ONE<<(BOARDWIDTH*6ULL))+(ONE<<(BOARDWIDTH*7ULL)))
#define V2 (V1<<ONE)
#define V3 (V2<<ONE)
#define V4 (V3<<ONE)
#define V5 (V4<<ONE)
#define V6 (V5<<ONE)
#define V7 (V6<<ONE)
#define V8 (V7<<ONE)

#define NORTH BOARDWIDTH
#define SOUTH (-BOARDWIDTH)
#define EAST (-1)
#define WEST 1
#define NORTHWEST (NORTH+WEST)
#define NORTHEAST (NORTH+EAST)
#define SOUTHWEST (SOUTH+WEST)
#define SOUTHEAST (SOUTH+EAST)


#define RANK_1 0x00000000000000FFull
#define RANK_2 0x000000000000FF00ull
#define RANK_3 0x0000000000FF0000ull
#define RANK_4 0x00000000FF000000ull
#define RANK_5 0x000000FF00000000ull
#define RANK_6 0x0000FF0000000000ull
#define RANK_7 0x00FF000000000000ull
#define RANK_8 0xFF00000000000000ull

#define FILE_A 0x0101010101010101ull
#define FILE_B 0x0202020202020202ull
#define FILE_C 0x0404040404040404ull
#define FILE_D 0x0808080808080808ull
#define FILE_E 0x1010101010101010ull
#define FILE_F 0x2020202020202020ull
#define FILE_G 0x4040404040404040ull
#define FILE_H 0x8080808080808080ull


#define WHITE_SQUARES 0x55AA55AA55AA55AAull
#define BLACK_SQUARES 0xAA55AA55AA55AA55ull

#define LONG_DIAGONALS 0x8142241818244281ull
#define CENTER_SQUARES 0x0000001818000000ull

#define LEFT_FLANK (FILE_A | FILE_B | FILE_C | FILE_D)
#define RIGHT_FLANK (FILE_E | FILE_F | FILE_G | FILE_H)

#define PROMOTION_RANKS = (RANK_1 | RANK_8)



#define SQ_A1 0
#define SQ_B1 1
#define SQ_C1 2
#define SQ_D1 3
#define SQ_E1 4
#define SQ_F1 5
#define SQ_G1 6
#define SQ_H1 7

#define SQ_A8 (SQ_A1 + 7*8)
#define SQ_B8 (SQ_B1 + 7*8)
#define SQ_C8 (SQ_C1 + 7*8)
#define SQ_D8 (SQ_D1 + 7*8)
#define SQ_E8 (SQ_E1 + 7*8)
#define SQ_F8 (SQ_F1 + 7*8)
#define SQ_G8 (SQ_G1 + 7*8)
#define SQ_H8 (SQ_H1 + 7*8)

//

// diagonal and horizontal moves
inline u64 NW(u64 bb)
{
	return (bb&(~V8)) << 9;
}

inline u64 NE(u64 bb)
{
	return (bb & (~V1)) << 7;
}

inline u64 SW(u64 bb)
{
	return (bb & (~V8)) >> 7;
}

inline u64 SE(u64 bb)
{
	return (bb & (~V1)) >> 9;
}

inline u64 N(u64 bb)
{
	return bb << 8;
}

inline u64 S(u64 bb)
{
	return bb >> 8;
}

inline u64 W(u64 bb)
{
	return (bb & (~V8)) << 1;
}

inline u64 E(u64 bb)
{
	return (bb & (~V1)) >> 1;
}


// knight moves
inline u64 NNW(u64 bb)
{
	return (bb&(~V8)) << 17;
}

inline u64 NNE(u64 bb)
{
	return (bb&(~V1)) << 15;
}

inline u64 SSW(u64 bb)
{
	return (bb&(~V8)) >> 15;
}

inline u64 SSE(u64 bb)
{
	return (bb&(~V1)) >> 17;
}

//
inline u64 WWN(u64 bb)
{
	return (bb&(~(V8 | V7))) << 10;
}

inline u64 WWS(u64 bb)
{
	return (bb&(~(V8 | V7))) >> 6;
}

inline u64 EEN(u64 bb)
{
	return (bb&(~(V1 | V2))) << 6;
}

inline u64 EES(u64 bb)
{
	return (bb&(~(V1 | V2))) >> 10;
}


inline u64 NWsmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = NW(bb);
		smear |= bb;
	}
	return smear;
}

inline u64 NEsmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = NE(bb);
		smear |= bb;
	}
	return smear;
}
inline u64 SWsmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = SW(bb);
		smear |= bb;
	}
	return smear;
}
inline u64 SEsmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = SE(bb);
		smear |= bb;
	}
	return smear;
}
inline u64 Nsmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = N(bb);
		smear |= bb;
	}
	return smear;
}
inline u64 Ssmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = S(bb);
		smear |= bb;
	}
	return smear;
}
inline u64 Esmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = E(bb);
		smear |= bb;
	}
	return smear;
}
inline u64 Wsmear(u64 bb)
{
	int i;
	u64 smear = 0;
	for (i = 0; i < 7; i++)
	{
		bb = W(bb);
		smear |= bb;
	}
	return smear;
}

inline u64 Wsmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = W(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}

inline u64 Esmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = E(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}
inline u64 Nsmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = N(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}
inline u64 Ssmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = S(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}
inline u64 NWsmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = NW(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}
inline u64 SWsmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = SW(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}
inline u64 NEsmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = NE(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}
inline u64 SEsmearuntilnotempty(u64 bb, u64 occupiers)
{
	u64 smear = 0;
	do
	{
		bb = SE(bb);
		smear |= bb;
		bb &= (~occupiers);
	} while (bb);
	return smear;
}


inline u64 SEsmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = SE(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 SWsmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = SW(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 NEsmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = NE(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 NWsmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = NW(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 Nsmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = N(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 Ssmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = S(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 Esmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = E(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


inline u64 Wsmearmove(u64 bb, u64 all_us, u64 all_them)
{
	u64 moves = 0;
	do
	{
		bb = W(bb);
		moves |= bb & (~all_us);
		bb &= (~(all_us | all_them));
	} while (bb);
	return moves;
}


#define PAWNSPUSH(x,y) ((y) ? (S(x)) : (N(x)))
#define NITESMEAR(x) (NNW(x) | NNE(x) | EEN(x) | EES(x) | WWN(x) | WWS(x) | SSE(x) | SSW(x))
#define KINGSMEAR(x) (NW(x) | NE(x) | SW(x) | SE(x) | N(x) | S(x) | E(x) | W(x))
#define PAWNSCAPTURE(x,y) ((y) ? (SW(x) | SE(x)) : (NW(x) | NE(x)))


inline u64 bitboard_bishop_attack_smear(u64 us, u64 occ)
{
	u64 bb = NWsmearuntilnotempty(us, occ);
	bb |= NEsmearuntilnotempty(us, occ);
	bb |= SWsmearuntilnotempty(us, occ);
	bb |= SEsmearuntilnotempty(us, occ);
	return bb;
}

inline u64 bitboard_rook_attack_smear(u64 us, u64 occ)
{
	u64 bb = Nsmearuntilnotempty(us, occ);
	bb |= Ssmearuntilnotempty(us, occ);
	bb |= Esmearuntilnotempty(us, occ);
	bb |= Wsmearuntilnotempty(us, occ);
	return bb;
}

inline u64 bitboard_queen_attack_smear(u64 us, u64 occ)
{
	u64 bb = NWsmearuntilnotempty(us, occ);
	bb |= NEsmearuntilnotempty(us, occ);
	bb |= SWsmearuntilnotempty(us, occ);
	bb |= SEsmearuntilnotempty(us, occ);
	bb |= Nsmearuntilnotempty(us, occ);
	bb |= Ssmearuntilnotempty(us, occ);
	bb |= Esmearuntilnotempty(us, occ);
	bb |= Wsmearuntilnotempty(us, occ);
	return bb;
}

