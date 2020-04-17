#pragma once


//
// these are some very slow and inefficient routines built just to get the perft tests going
// returns 1 if true
// is the square attacked by us
inline bool IsSquareAttacked(GAMEDATA* gd, int s1, int us)
{
	u64 bb = (ONE << s1);
	u64 all_us_bb = gd->pieces[us];
	if (KINGSMEAR(bb) & (ONE << gd->king_sq[us])) return 1;
	if (NITESMEAR(bb) & gd->nites & all_us_bb) return 1;
	if (PAWNSCAPTURE(bb, us) & gd->nites & all_us_bb) return 1;
	u64 occupied = all_us_bb | gd->pieces[us^1];
	if (bitboard_bishop_attack_smear(bb, occupied) & gd->diagonals & all_us_bb) return 1;
	if (bitboard_rook_attack_smear(bb, occupied) & gd->manhattans & all_us_bb) return 1;
	return 0;
}

//
// returns 1 if true
// works on board as is, post move
inline bool IsGiveCheck(GAMEDATA* gd, int us)
{
	return (IsSquareAttacked(gd, gd->king_sq[us], us^1));
}
//
//
inline int rank_file_to_square(int rank, int file)
{
	return (rank << 3) + file;
}
//
