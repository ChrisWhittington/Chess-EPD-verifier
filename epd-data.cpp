

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

//
// masses of test epds, cobbled together from all over the internet
// these should all pass as legal EPDs
// junk them when happy with the code
// credited where the author is known
char destruction_test[][MAXEPD_LENGTH] =
{
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; startpos; should be OK, all EPDs below assume normal chess(not 960)",
	"rnbqkbnr/nnnnnnnn/8/8/8/8/NNNNNNNN/RNBQKBNR w KQkq - 0 1; 18 knights; should be OK(but see HGM on max possible promotions if no pieces captured)",
	"rnbqkbnr/bbbbbbbb/8/8/8/8/BBBBBBBB/RNBQKBNR w KQkq - 0 1; 18 bishops; should be OK(technically these are not allowed under normal chess rules)",
	"rnbqkbnr/rrrrrrrr/8/8/8/8/RRRRRRRR/RNBQKBNR w KQkq - 0 1; 18 rooks; should be OK(ditto)",
	"rnbqkbnr/qqqqqqqq/8/8/8/8/QQQQQQQQ/RNBQKBNR w KQkq - 0 1; 18 queens; should be OK(ditto)",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR  w  KQkq - 0  1; extra white space; should be not a problem",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; leading spaces; should be not a problem",
	"nrbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; castle status; should be overridable",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 400 500; movenums; should be overridable",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 10 5; movenums; should be overridable",
	"krr5/8/8/8/8/8/8/4K3 w kq - 0 1; two castling rooks on same side of black king; ; should be overridable",
	"rnbqkbnr/ppppNNpp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; white 18 pieces; well if engines can handle it, leave it ok, else cull",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPnPP/RNBQKBNR w KQkq - 0 1; black 17 pieces; well if engines can handle it, leave it ok, else cull",
	"nnnnknnn/nnnnnnnn/8/8/8/8/NNNNNNNN/NNNNKNNN w - -0 1; 30 knights; well, if engine can handle impossible promoted piece counts, leave it as ok else cull",
	"rnbnkbnn/bbbbnnnn/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; too many promoted pieces; cull, or if engine can handle it, leave as OK",
	"qqqqkqqq/qqqqqqqq/8/8/8/8/QQQQQQQQ/QQQQKQQQ w - -0 1; 30 queens; well, if engine can handle impossible promoted piece counts, leave it as ok else cull",
	"rqrkbn2/1pBppppp/8/8/8/8/8/4K2R w K - 0 1 moves e1g1 a8a7 g1f2 a7a6 f1h1 f2e1 a6a8; f2e1 NOT a repetitio ** EPD WIP **",
	"rnbqkbn1/ppppppppr/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; extra piece on rank, cull",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR K KQkq - 0 1; colour, cull""",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQAb - 0 1; castle status, cull(or override)",
	"rnbqkbnr.pppppppp.8.8.8.8.PPPPPPPP.RNBQKBNR w KQkq - 0 1; slash chars; cull EPD",
	"rnbqkbnr/ppppippp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; bad character; cull",
	"rnbqk0nr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; bad numeric; cull",
	"rnbqkbnr/pppppppp/8/6/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; square count too low; cull",
	"N7/8/2KQ2rp/6k1/4p3p/2p4P/4PP2/5N2 w - -0 1; square count too high; cull",
	"rnkqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; 3 kings; cull",
	"rnbq1bnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; 1 king; cull",
	"pnbqkbnr/ppprpppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; pawn back rank; cull",
	"blablabl/blablabl/blablabl/blablabl/blablabl/blablabl/blablabl/blablabl w - -0 1; garbage",
	"hello have a nice day w - -0 1; garbage",
	"1B6/prpb2p1/2KPp3/qp1p4/Q1k5/nRP3p1/BRPP2Pp/BN6 w - -; bad EPD, Dann Corbitt",
	"1N4B1/2p2p2/2Q3k1/p2p1RPp/P4P1b/7P/2PPPrrK/8 w - -; bad EPD, Dann Corbitt",
	"1N6/p7/5R2/nkR1r3/rp1QB3/qNpp4/1p1n1p2/1K2b3 w - -; bad EPD, Dann Corbitt",
	"1Q2Rb2/1NB2kp1/4P2R/2K2P2/2p1P3/P7/n1bnPPP1/r2q4 w - -; bad EPD, Dann Corbitt",
	"1Q6/3P1R2/2N1kPpp/5BPK/1N1pp1pb/1n1b1Pp1/4P3/8 w - -; bad EPD, Dann Corbitt",
	"1b1knR1r/2pR3r/2b5/7K/4QBp1/1p1p3B/pn5p/7q w - -; bad EPD, Dann Corbitt",
	"1b6/pp6/1pN5/np4R1/1QNk2b1/R4K2/3r4/1n1r4 w - -; bad EPD, Dann Corbitt",
	"1bk1N3/1p1R2p1/P6p/r6p/r6p/2R1p2B/4Pp2/5Kqb w - -; bad EPD, Dann Corbitt",
	"1r1K1R2/2P1P3/2N4q/Bp4p1/6np/4k3/1bP1p1p1/1r1n1N1b w - -; bad EPD, Dann Corbitt",
	"2K5/3bBp1Q/5p2/5n2/2pN2q1/k5R1/n2rp3/8 w - -; bad EPD, Dann Corbitt",
	"2Q1NR2/5B2/3p2p1/p2RPk2/1p1p4/4BP1N/4pPpP/1rq3Kb w - -; bad EPD, Dann Corbitt",
	"2Q3k1/4b3/2K2q2/8/2B3r1/8/8/8 w - -; bad EPD, Dann Corbitt",
	"3N4/2p3p1/p1R4n/Rq2BkPb/2N1rPP1/2pQ3B/3nK3/2r1b3 w - -; bad EPD, Dann Corbitt",
	"3R4/Pp6/qK6/rp6/1R2p3/k3p2P/PQ2N3/BBN5 w - -; bad EPD, Dann Corbitt",
	"3RB3/1b4Bp/1b2p2p/r3kpK1/p2R2p1/n1qP2P1/1PPN2P1/2r5 w - -; bad EPD, Dann Corbitt",
	"3b3b/2ppRp1p/2p2KpQ/1P5P/5k1R/2P2PBq/4pPP1/6NN w - -; bad EPD, Dann Corbitt",
	"3n4/2Q1p3/p3p3/p2k1NP1/p7/2KP1B2/8/4b3 w - -; bad EPD, Dann Corbitt",
	"3nn3/2q1P3/3P2P1/B3P1b1/2Q2nP1/2q5/kn1P1qr1/2K5 w - -; bad EPD, Dann Corbitt",
	"4B1N1/1p3N1p/1Rn1k1pr/1K4pr/6Pp/1Q5P/1Pp1b3/4R3 w - -; bad EPD, Dann Corbitt",
	"4R3/1N3K2/pb4p1/1Pp2NP1/B1q1k3/1P5Q/bBP3P1/4n3 w - -; bad EPD, Dann Corbitt",
	"4q3/2p2K2/2N2N1B/r2R1kRr/3p4/2P3P1/p7/4Q3 w - -; bad EPD, Dann Corbitt",
	"5Q2/Q5P1/1q4KP/1P6/p3Q3/P4NpB/1P3ppp/RQ3kqn w - -; bad EPD, Dann Corbitt",
	"5b2/2Q5/2pBp3/1N1k1p2/5P2/2K1Pb2/2P1P1R1/1n1R3B w - -; bad EPD, Dann Corbitt",
	"5b2/2pQ3n/3kP2p/2pP4/b1pKP3/8/8/8 w - -; bad EPD, Dann Corbitt",
	"5b2/8/2p4q/1rB1nn2/pkR3r1/1NR5/1p4K1/1N4Q1 w - -; bad EPD, Dann Corbitt",
	"6B1/Q3n1n1/P2B1P1p/Rpp3pP/1p1p1rKp/1k1B3P/3p4/BrN1R2q w - -; bad EPD, Dann Corbitt",
	"6bn/1Np1Np2/4pPp1/Qp2P1Pp/kq2P2R/bp6/1p6/1Kr5 w - -; bad EPD, Dann Corbitt",
	"7Q/8/8/6p1/2N3Bk/pp3n1P/r2B1RRP/qK3n2 w - -; bad EPD, Dann Corbitt",
	"8/1K4Q1/3k3b/p6B/2N1bn2/3N3p/7p/6R1 w - -; bad EPD, Dann Corbitt",
	"8/1p6/5p2/2pP1ppn/P1R4k/2pK2rp/n1N1P3/8 w - -; bad EPD, Dann Corbitt",
	"8/1p6/K7/8/8/2B5/1k6/8 w - -; bad EPD, Dann Corbitt",
	"8/1qn5/5B2/8/2R5/k5n1/2R1P1pp/1KQ3rr w - -; bad EPD, Dann Corbitt",
	"8/2N5/1p6/bkp1p3/8/3pP3/Q1pK4/2N5 w - -; bad EPD, Dann Corbitt",
	"8/3N4/8/p7/1B6/k1Q5/1R2p1p1/rnN1K1rb w - -; bad EPD, Dann Corbitt",
	"8/3k1p2/4P3/8/8/6p1/3P1K2/4B3 w - -; bad EPD, Dann Corbitt",
	"8/3p1pq1/2p2b2/8/8/1B2B3/4pk1p/K1Rr1r2 w - -; bad EPD, Dann Corbitt",
	"8/3pp1bp/6pk/6Pp/1pp3rP/qK6/8/r7 w - -; bad EPD, Dann Corbitt",
	"8/4q1p1/4p3/5rKP/4kn1R/4bRNQ/4P1PB/5rNn w - -; bad EPD, Dann Corbitt",
	"8/8/1N2P2n/Q1NpB1p1/ppPk4/Kp2pPP1/1R2P3/8 w - -; bad EPD, Dann Corbitt",
	"8/8/4P1B1/8/4Qp2/3K1P2/1P3P2/3rk1R1 w - -; bad EPD, Dann Corbitt",
	"8/8/8/1k3q2/8/3K4/8/1Q6 w - -; bad EPD, Dann Corbitt",
	"8/8/8/5R1p/7p/6pK/5Ppq/5rQk w - -; bad EPD, Dann Corbitt",
	"8/8/8/8/RP5P/p6r/P5Qp/K5qk w - -; bad EPD, Dann Corbitt",
	"8/B2p1P1r/B7/7q/8/8/1pN1PPP1/2RQ1k1K w - -; bad EPD, Dann Corbitt",
	"8/P3p1Q1/4p2B/2bpPk2/3p2p1/R2b2N1/r1P1P3/KB1N4 w - -; bad EPD, Dann Corbitt",
	"8/R2k4/8/8/8/6Q1/K3r3/5q2 w - -; bad EPD, Dann Corbitt",
	"8/kq6/8/8/8/8/8/1K4Q1 w - -; bad EPD, Dann Corbitt",
	"Kq5r/5BPn/3p2R1/5k2/2pP1p1R/5P2/5N2/1Q6 w - -; bad EPD, Dann Corbitt",
	"Q4Q2/qk2P2n/NN4P1/1K4pb/Q5q1/qq6/4p3/2n5 w - -; bad EPD, Dann Corbitt",
	"Q4k2/2N1Bp1P/4Kp1P/5P2/8/1P6/8/n1Rq4 w - -; bad EPD, Dann Corbitt",
	"RBRqBb2/nQr2pp1/2pK1p2/1k6/bPpp2p1/1P1P2P1/3P4/8 w - -; bad EPD, Dann Corbitt",
	"Rb3b2/p1Bp1Rp1/P1pK1p2/nb6/p1k4P/2PP2P1/B1P1P3/rN6 w - -; bad EPD, Dann Corbitt",
	"bn2rb2/1rP4R/1KBkQP2/3P1n2/1N6/3NR3/8/6q1 w - -; bad EPD, Dann Corbitt",
	"k5Rq/4RNP1/5PPK/6N1/5B1r/7B/8/8 w - -; bad EPD, Dann Corbitt",
	"n5K1/7R/1pk2PN1/4N2R/PP2B3/1bP1P3/7B/8 w - -; bad EPD, Dann Corbitt",
	"q1KNNn1n/8/8/8/8/2brrb2/1RR5/3QQkB1 w - -; bad EPD, Dann Corbitt",
	"r2R1kb1/2R5/4P1B1/3pP3/1p1N4/1p2K1pN/1PP3Q1/n5br w - -; bad EPD, Dann Corbitt",
	"r6k/1K2q1Q1/8/8/8/8/8/8 w - -; bad EPD, Dann Corbitt",
	"r7/2N5/1bk5/p2P4/p1np4/B7/2K4r/8 w - -; bad EPD, Dann Corbitt",
	"rnb1k2r/pppp1ppp/8/4Q3/4P3/4q3/P1P2PPP/4KB1R w - -; bad EPD, Dann Corbitt",
	"1R6/1brk2p1/4p2p/p1P1Pp2/P7/6P1/1P4P1/2R3K1 w - -0 1 bm b8b7; epd should be OK",
	"4r1k1/p1qr1p2/2pb1Bp1/1p5p/3P1n1R/1B3P2/PP3PK1/2Q4R w - -0 1 bm c1f4; epd should be OK",
	"r1b2rk1/ppq1bppp/2p1pn2/8/2NP4/2N1P3/PP2BPPP/2RQK2R w K - 0 1 bm e1g1; epd should be OK",
	"r3r1k1/1bppq2p/1p1n1bp1/3P1p2/p4N1P/P1PB1P2/1P1Q1BP1/1K1R3R w - -0 1 bm g2g4; epd should be OK",
	"r1bqrk2/pp1n1n1p/3p1p2/P1pP1P1Q/2PpP1NP/6R1/2PB4/4RBK1 w - -0 1 bm h5f7; epd should be OK",
	"rn1q1rk1/p2p1pb1/bp3np1/2pP2Bp/7P/P1N2N2/1PQ1PPP1/R3KB1R w KQ - 0 1 bm g2g4; epd should be OK",
	"N7/8/2KQ2rp/6k1/3p3p/2p4P/4PP2/5N2 w - -0 1 bm f2f4; epd should be OK",
	"r1bqk2r/ppn1bppp/2n5/2p1p3/8/2NP1NP1/PP1BPPBP/R2Q1RK1 b kq - 0 1 bm e8g8; epd should be OK",
	"8/2p2kb1/np1p2p1/pN1Pp1Pp/2P1P2P/1P6/P4BK1/8 w - -0 1 bm b5c7; epd should be OK",
	"4r1rk/p3qpp1/1pnp1n1p/5P2/P1PPP3/4Q2P/2BB2R1/6RK w - -0 1 bm g2g7; epd should be OK",
	"rn1q1rk1/pp2bppp/1n2p1b1/8/2pPP3/1BN1BP2/PP2N1PP/R2Q1RK1 w - -0 1 bm b3c2; epd should be OK",
	"r4r1k/1p4qp/b1pNRbp1/2P5/3p1P1P/PQ4P1/3R2BK/8 w - -0 1 bm d2d1; epd should be OK",
	"2b2r1k/4q2p/3p2pQ/2pBp3/8/6P1/1PP2P1P/R5K1 w - -0 1 bm a1a7; epd should be OK",
	"r2q1r2/1b2bpkp/n3p1p1/2ppP1P1/p6R/1PN1BQR1/NPP2P1P/4K3 w - -0 1 bm f3f6; epd should be OK",
	"r1b1k2r/ppppqppp/8/2bP4/3p4/6P1/PPQPPPBP/R1B2RK1 b kq - 0 1 bm e8g8; epd should be OK",
	"rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq - 0 1 bm e5e6; epd should be OK",
	"X"
};

char rybkaforum[][MAXEPD_LENGTH] =
{
	"kqq5/qq6/q7/8/8/5RRR/5RRR/5RRK w - - 0 143;",
	"6N1/7b/8/7p/p3k3/6KN/8/8 w - - ; bm Ng5 + ;",
	"8/8/8/8/2RK3R/6k1/1Pb2pp1/8 w - - ; bm Ke3;",
	"4rk2/7p/p1nq2p1/7r/Q1P2p2/2B2P2/P3P2P/1N2R2K w - - ; bm c5;",
	"6rk/p1r2qp1/1n3p1p/3pp3/p7/B1PP1Q2/B1P4R/6RK b - - ; bm e4;",
	"2Q5/8/8/8/3kbP1N/2pq3P/6PK/8 w - - ; bm f5;",
	"6k1/p4p1p/4pbp1/1p1r4/q1rP1P2/2P2R1P/PB4P1/R2Q3K w - - ; bm Qxa4;",
	"3q1r1k/4n3/1R3p1p/3p1b2/3P4/Q3RBPp/4PP1K/2r2N2 b - - ; bm Rxf1;",
	"3B4/1r2p3/r2p1p2/bkp1P1p1/1p1P1PPp/p1P1K2P/PPB5/8 w - - ; bm Ba4 + ;",
	"q2r2k1/1bpn1ppp/1p2p3/4P1N1/r2P4/b5P1/1BQ2PBP/3RR1K1 b - - ; bm Nf8;",
	"5r2/1b2k2p/3bp3/5PP1/Bn1Q3P/1N6/1Pr5/1K3R2 b - - ; bm Rxb2 + ;",
	"8/5p2/8/4k3/4p2R/2pp2P1/2p5/4KB1R w K -  ; bm Rxe4 + ;",
	"5k2/7R/8/4Kp2/5Pp1/P5rp/1P6/8 w - - ; bm Kf6;",
	"r4rk1/pbp2ppp/1p3Q2/8/2P5/2BR3P/PP2qPP1/5RK1 b - - ; bm Qg4;",
	"8/3k4/1p4b1/3p1p1p/K2P1PpP/3B2P1/1P6/8 w - - ; bm Kb5;",
	"8/2B2k2/6p1/5P1p/6p1/p1p3PP/6P1/1n2K2R w K -  ; bm fxg6 + ;",
	"8/3r2p1/pR5p/5P1K/P4kP1/8/8/8 b - - ; bm a5;",
	"8/8/8/2k5/p1b5/8/1P1K4/8 w - - ; bm Kc2;",
	"r1b1k3/1p1p1pp1/1p1P3p/pP6/P7/8/5PPP/4R1K1 b q -  ; bm Kf8;",
	"5k2/1p2pPp1/3pP1P1/2p1pBp1/2K1P1Pp/8/8/N7 w - - ; bm Nb3;",
	"7k/6np/3B3N/6P1/8/8/K7/5q2 w - - ; bm Kb2;",
	"8/8/6b1/1p3R1p/5b2/k7/8/7K w - - ; bm Rxb5;",
	"8/1p2b3/6k1/5p2/p2NbP1p/P1B1P1p1/1P5P/6K1 w - -  ; bm h3; Be1;",
	"8/p7/5k2/pPp1p3/2P3Kb/8/8/3B4 w - - ; bm Kf3;",
	"4K3/2k1Bp1N/6p1/5PP1/8/7p/b7/8 w - - ; bm Bf6;",
	"r7/1b1r4/k1p1p1p1/1p1pPpPp/p1PP1P1P/PP1K4/8/4Q3 w - - ; bm Qa5 + ;",
	"8/8/8/8/4kp2/1R6/P2q1PPK/8 w - - ; bm a3;",
	"8/8/8/5Bp1/7k/4p3/5PKP/8 b - - ; bm e2;",
	"k1N5/pp6/1P1K4/8/6p1/8/8/8 w - - ; bm Nxa7;",
	"q7/8/2p5/B2p2pp/5pp1/2N3k1/6P1/7K w - - ; bm Ne4 + ;",
	"1K3kB1/8/6p1/7R/8/4b3/8/6n1 w - - ; bm Rh8;",
	"6k1/8/8/8/q5p1/3p2P1/3N1R2/6K1 b - - ; bm Kg7;",
	"8/4B3/8/1p1b1k1p/p5pP/P5K1/8/8 w - - ; bm Bc5;",
	"8/3p4/6B1/1k6/8/1p2p3/3b4/3K4 w - - ; bm Be8;",
	"X"
};


char grief_causers[][MAXEPD_LENGTH] =
{
	//	"brkrqbqn/qnpNqqbr/bnNqRbrn/qNrrnRrb/BrqnbqRr/bqRNnprR/QQbNNrNn/bqrnRbKb w - - ; bm Nbxa7;",
	"1r4Q1/7p/2N1B2k/7P/3Pp1P1/K1N1P2R/PP2PR2/8 b - -; +- no repet - StackOverflow",
	"2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk -  ; bm Nc7 + ; id WAC.267 late mate finder",
	"r3r1k1/pp1q1pp1/4b1p1/3p2B1/3Q1R2/8/PPP3PP/4R1K1 w - - ; bm Qxg7 + ; id WAC.055 hit breakpoint",
	"8/2k5/1p6/p4p1p/P1Pb1P2/3N3P/5P2/3K4 w - -2 44; high eval for white (now fixed)",
	"k1b5/1p1p1p1p/pPpPpPpP/P1P1P1P1/8/8/8/K1B5 w - - 0 1; draw",
	"k7/1p1p1p1p/pPpPpPpP/P1P1P1P1/1B1B1B1B/B1B1B1B1/1B1B1B1B/K1B1B1B1 w - - 0 1; draw",
	"k7/1p1p1p1p/pPpPpPpP/B1B1B1B1/1BRBRBRB/B1BQBQB1/KBRBRBRB/B1B1B1B1 w - - 0 1; draw",
	"k7/1p1p1p1p/pPpPpPpP/P1P1P1P1/2RRRR2/BRBRBRBR/KBRBRBRB/B1BRBRBR w - - 0 1; draw",
	"X"
};


char winatchess[][MAXEPD_LENGTH] =
{
	"r3k2r/2p2p2/p2p1n2/1p2p3/4P2p/1PPPPp1q/1P5P/R1N2QRK b kq -  ; bm Ng4; id WAC.181 bad proposal",
	"2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk -  ; bm Nc7 + ; id WAC.267 slow M7",
	"6k1/8/8/8/q5p1/3p2P1/3N1R2/6K1 b - -1 45; eval = draw by fortress stack overflow",
	"3r2kq/p2prp1p/1p4pP/2nR4/1Q6/1B3RP1/P4PK1/8 w - - ; bm Rxc5 ; Qh4; breakpoint hit",

	"r1bq3r/ppppR1p1/5n1k/3P4/6pP/3Q4/PP1N1PP1/5K1R w - - ; bm h5; id WAC.138",
	"r2qkb1r/1ppb1ppp/p7/4p3/P1Q1P3/2P5/5PPP/R1B2KNR b kq -  ; bm Bb5; id WAC.020",
	"r1bqk2r/ppp1nppp/4p3/n5N1/2BPp3/P1P5/2P2PPP/R1BQK2R w KQkq -  ; bm Nxf7; Ba2; id WAC.022",
	"rb3qk1/pQ3ppp/4p3/3P4/8/1P3N2/1P3PPP/3R2K1 w - - ; bm dxe6; d6; Qxa8; g3; id WAC.031",
	"4k3/p4prp/1p6/2b5/8/2Q3P1/P2R1PKP/4q3 w - - ; bm Rd8 +; Qd3 ; id WAC.038",
	"1k6/5RP1/1P6/1K6/6r1/8/8/8 w - - ; bm Ka5; Kc5 b7; id WAC.041",
	"r2q3k/p2P3p/1p3p2/3QP1r1/8/B7/P5PP/2R3K1 w - - ; bm Be7; Qxa8; id WAC.043",
	"r1k5/1p3q2/1Qpb4/3N1p2/5Pp1/3P2Pp/PPPK3P/4R3 w - - ; bm Re7; c4; id WAC.052",
	"2k5/pppr4/4R3/4Q3/2pp2q1/8/PPP2PPP/6K1 w - - ; bm f3; h3; id WAC.069",
	"r1b4k/ppp2Bb1/6Pp/3pP3/1qnP1p1Q/8/PPP3P1/1K1R3R w - - ; bm b3; Qd8 +; id WAC.096",
	"8/k1b5/P4p2/1Pp2p1p/K1P2P1P/8/3B4/8 w - - ; bm Be3; b6 + ; id WAC.100",
	"r2r2k1/pb3ppp/1p1bp3/7q/3n2nP/PP1B2P1/1B1N1P2/RQ2NRK1 b - - ; bm Qxh4; Bxg3; id WAC.105",
	"r4rk1/1bn2qnp/3p1B1Q/p2P1pP1/1pp5/5N1P/PPB2P2/2KR3R w - - ; bm g6; Rhg1; id WAC.120",
	"6k1/1b2rp2/1p4p1/3P4/PQ4P1/2N2q2/5P2/3R2K1 b - - ; bm Rc7; Bxd5; Re6; id WAC.123",
	"2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - ; bm Qg6; id WAC.001",
	"8/7p/5k2/5p2/p1p2P2/Pr1pPK2/1P1R3P/8 b - - ; bm Rxb2; id WAC.002",
	"5rk1/1ppb3p/p1pb4/6q1/3P1p1r/2P1R2P/PP1BQ1P1/5RKN w - - ; bm Rg3; id WAC.003",
	"r1bq2rk/pp3pbp/2p1p1pQ/7P/3P4/2PB1N2/PP3PPR/2KR4 w - - ; bm Qxh7 + ; id WAC.004",
	"5k2/6pp/p1qN4/1p1p4/3P4/2PKP2Q/PP3r2/3R4 b - - ; bm Qc4 + ; id WAC.005",
	"7k/p7/1R5K/6r1/6p1/6P1/8/8 w - - ; bm Rb7; id WAC.006",
	"rnbqkb1r/pppp1ppp/8/4P3/6n1/7P/PPPNPPP1/R1BQKBNR b KQkq -  ; bm Ne3; id WAC.007",
	"r4q1k/p2bR1rp/2p2Q1N/5p2/5p2/2P5/PP3PPP/R5K1 w - - ; bm Rf7; id WAC.008",
	"3q1rk1/p4pp1/2pb3p/3p4/6Pr/1PNQ4/P1PB1PP1/4RRK1 b - - ; bm Bh2 + ; id WAC.009",
	"2br2k1/2q3rn/p2NppQ1/2p1P3/Pp5R/4P3/1P3PPP/3R2K1 w - - ; bm Rxh7; id WAC.010",
	"r1b1kb1r/3q1ppp/pBp1pn2/8/Np3P2/5B2/PPP3PP/R2Q1RK1 w kq -  ; bm Bxc6; id WAC.011",
	"4k1r1/2p3r1/1pR1p3/3pP2p/3P2qP/P4N2/1PQ4P/5R1K b - - ; bm Qxf3 + ; id WAC.012",
	"5rk1/pp4p1/2n1p2p/2Npq3/2p5/6P1/P3P1BP/R4Q1K w - - ; bm Qxf8 + ; id WAC.013",
	"r2rb1k1/pp1q1p1p/2n1p1p1/2bp4/5P2/PP1BPR1Q/1BPN2PP/R5K1 w - - ; bm Qxh7 + ; id WAC.014",
	"1R6/1brk2p1/4p2p/p1P1Pp2/P7/6P1/1P4P1/2R3K1 w - - ; bm Rxb7; id WAC.015",
	"r4rk1/ppp2ppp/2n5/2bqp3/8/P2PB3/1PP1NPPP/R2Q1RK1 w - - ; bm Nc3; id WAC.016",
	"1k5r/pppbn1pp/4q1r1/1P3p2/2NPp3/1QP5/P4PPP/R1B1R1K1 w - - ; bm Ne5; id WAC.017",
	"R7/P4k2/8/8/8/8/r7/6K1 w - - ; bm Rh8; id WAC.018",
	"r1b2rk1/ppbn1ppp/4p3/1QP4q/3P4/N4N2/5PPP/R1B2RK1 w - - ; bm c6; id WAC.019",
	"5rk1/1b3p1p/pp3p2/3n1N2/1P6/P1qB1PP1/3Q3P/4R1K1 w - - ; bm Qh6; id WAC.021",
	"r3nrk1/2p2p1p/p1p1b1p1/2NpPq2/3R4/P1N1Q3/1PP2PPP/4R1K1 w - - ; bm g4; id WAC.023",
	"6k1/1b1nqpbp/pp4p1/5P2/1PN5/4Q3/P5PP/1B2B1K1 b - - ; bm Bd4; id WAC.024",
	"3R1rk1/8/5Qpp/2p5/2P1p1q1/P3P3/1P2PK2/8 b - - ; bm Qh4 + ; id WAC.025",
	"3r2k1/1p1b1pp1/pq5p/8/3NR3/2PQ3P/PP3PP1/6K1 b - - ; bm Bf5; id WAC.026",
	"7k/pp4np/2p3p1/3pN1q1/3P4/Q7/1r3rPP/2R2RK1 w - - ; bm Qf8 + ; id WAC.027",
	"1r1r2k1/4pp1p/2p1b1p1/p3R3/RqBP4/4P3/1PQ2PPP/6K1 b - - ; bm Qe1 + ; id WAC.028",
	"r2q2k1/pp1rbppp/4pn2/2P5/1P3B2/6P1/P3QPBP/1R3RK1 w - - ; bm c6; id WAC.029",
	"1r3r2/4q1kp/b1pp2p1/5p2/pPn1N3/6P1/P3PPBP/2QRR1K1 w - - ; bm Nxd6; id WAC.030",
	"6k1/p4p1p/1p3np1/2q5/4p3/4P1N1/PP3PPP/3Q2K1 w - - ; bm Qd8 + ; id WAC.032",
	"8/p1q2pkp/2Pr2p1/8/P3Q3/6P1/5P1P/2R3K1 w - - ; bm Qe5 + ;Qf4; id WAC.033",
	"7k/1b1r2p1/p6p/1p2qN2/3bP3/3Q4/P5PP/1B1R3K b - - ; bm Bg1; id WAC.034",
	"r3r2k/2R3pp/pp1q1p2/8/3P3R/7P/PP3PP1/3Q2K1 w - - ; bm Rxh7 + ; id WAC.035",
	"3r4/2p1rk2/1pQq1pp1/7p/1P1P4/P4P2/6PP/R1R3K1 b - - ; bm Re1 + ; id WAC.036",
	"2r5/2rk2pp/1pn1pb2/pN1p4/P2P4/1N2B3/nPR1KPPP/3R4 b - - ; bm Nxd4 + ; id WAC.037",
	"r1br2k1/pp2bppp/2nppn2/8/2P1PB2/2N2P2/PqN1B1PP/R2Q1R1K w - - ; bm Na4; id WAC.039",
	"3r1r1k/1p4pp/p4p2/8/1PQR4/6Pq/P3PP2/2R3K1 b - - ; bm Rc8; id WAC.040",
	"r1b1r1k1/pp1n1pbp/1qp3p1/3p4/1B1P4/Q3PN2/PP2BPPP/R4RK1 w - - ; bm Ba5; id WAC.042",
	"3rb1k1/pq3pbp/4n1p1/3p4/2N5/2P2QB1/PP3PPP/1B1R2K1 b - - ; bm dxc4; id WAC.044",
	"7k/2p1b1pp/8/1p2P3/1P3r2/2P3Q1/1P5P/R4qBK b - - ; bm Qxa1; id WAC.045",
	"r1bqr1k1/pp1nb1p1/4p2p/3p1p2/3P4/P1N1PNP1/1PQ2PP1/3RKB1R w K -  ; bm Nb5; id WAC.046",
	"r1b2rk1/pp2bppp/2n1pn2/q5B1/2BP4/2N2N2/PP2QPPP/2R2RK1 b - - ; bm Nxd4; id WAC.047",
	"1rbq1rk1/p1p1bppp/2p2n2/8/Q1BP4/2N5/PP3PPP/R1B2RK1 b - - ; bm Rb4; id WAC.048",
	"2b3k1/4rrpp/p2p4/2pP2RQ/1pP1Pp1N/1P3P1P/1q6/6RK w - - ; bm Qxh7 + ; id WAC.049",
	"k4r2/1R4pb/1pQp1n1p/3P4/5p1P/3P2P1/r1q1R2K/8 w - - ; bm Rxb6 + ; id WAC.050",
	"r1bq1r2/pp4k1/4p2p/3pPp1Q/3N1R1P/2PB4/6P1/6K1 w - - ; bm Rg4 + ; id WAC.051",
	"6k1/6p1/p7/3Pn3/5p2/4rBqP/P4RP1/5QK1 b - - ; bm Re1; id WAC.053",
	"r3kr2/1pp4p/1p1p4/7q/4P1n1/2PP2Q1/PP4P1/R1BB2K1 b q -  ; bm Qh1 + ; id WAC.054",
	"r3r1k1/pp1q1pp1/4b1p1/3p2B1/3Q1R2/8/PPP3PP/4R1K1 w - - ; bm Qxg7 + ; id WAC.055",
	"r1bqk2r/pppp1ppp/5n2/2b1n3/4P3/1BP3Q1/PP3PPP/RNB1K1NR b KQkq -  ; bm Bxf2 + ; id WAC.056",
	"r3q1kr/ppp5/3p2pQ/8/3PP1b1/5R2/PPP3P1/5RK1 w - - ; bm Rf8 + ; id WAC.057",
	"8/8/2R5/1p2qp1k/1P2r3/2PQ2P1/5K2/8 w - - ; bm Qd1 + ; id WAC.058",
	"r1b2rk1/2p1qnbp/p1pp2p1/5p2/2PQP3/1PN2N1P/PB3PP1/3R1RK1 w - - ; bm Nd5; id WAC.059",
	"rn1qr1k1/1p2np2/2p3p1/8/1pPb4/7Q/PB1P1PP1/2KR1B1R w - - ; bm Qh8 + ; id WAC.060",
	"3qrbk1/ppp1r2n/3pP2p/3P4/2P4P/1P3Q2/PB6/R4R1K w - - ; bm Qf7 + ; id WAC.061",
	"6r1/3Pn1qk/p1p1P1rp/2Q2p2/2P5/1P4P1/P3R2P/5RK1 b - - ; bm Rxg3 + ; id WAC.062",
	"r1brnbk1/ppq2pp1/4p2p/4N3/3P4/P1PB1Q2/3B1PPP/R3R1K1 w - - ; bm Nxf7; id WAC.063",
	"8/6pp/3q1p2/3n1k2/1P6/3NQ2P/5PP1/6K1 w - - ; bm g4 + ; id WAC.064",
	"1r1r1qk1/p2n1p1p/bp1Pn1pQ/2pNp3/2P2P1N/1P5B/P6P/3R1RK1 w - - ; bm Ne7 + ; id WAC.065",
	"1k1r2r1/ppq5/1bp4p/3pQ3/8/2P2N2/PP4P1/R4R1K b - - ; bm Qxe5; id WAC.066",
	"3r2k1/p2q4/1p4p1/3rRp1p/5P1P/6PK/P3R3/3Q4 w - - ; bm Rxd5; id WAC.067",
	"6k1/5ppp/1q6/2b5/8/2R1pPP1/1P2Q2P/7K w - - ; bm Qxe3; id WAC.068",
	"2kr3r/pppq1ppp/3p1n2/bQ2p3/1n1PP3/1PN1BN1P/1PP2PP1/2KR3R b - - ; bm Na2 + ; id WAC.070",
	"2kr3r/pp1q1ppp/5n2/1Nb5/2Pp1B2/7Q/P4PPP/1R3RK1 w - - ; bm Nxa7 + ; id WAC.071",
	"r3r1k1/pp1n1ppp/2p5/4Pb2/2B2P2/B1P5/P5PP/R2R2K1 w - - ; bm e6; id WAC.072",
	"r1q3rk/1ppbb1p1/4Np1p/p3pP2/P3P3/2N4R/1PP1Q1PP/3R2K1 w - - ; bm Qd2; id WAC.073",
	"5r1k/pp4pp/2p5/2b1P3/4Pq2/1PB1p3/P3Q1PP/3N2K1 b - - ; bm Qf1 + ; id WAC.074",
	"r3r1k1/pppq1ppp/8/8/1Q4n1/7P/PPP2PP1/RNB1R1K1 b - - ; bm Qd6; id WAC.075",
	"r1b1qrk1/2p2ppp/pb1pnn2/1p2pNB1/3PP3/1BP5/PP2QPPP/RN1R2K1 w - - ; bm Bxf6; id WAC.076",
	"3r2k1/ppp2ppp/6q1/b4n2/3nQB2/2p5/P4PPP/RN3RK1 b - - ; bm Ng3; id WAC.077",
	"r2q3r/ppp2k2/4nbp1/5Q1p/2P1NB2/8/PP3P1P/3RR1K1 w - - ; bm Ng5 + ; id WAC.078",
	"r3k2r/pbp2pp1/3b1n2/1p6/3P3p/1B2N1Pq/PP1PQP1P/R1B2RK1 b kq -  ; bm Qxh2 + ; id WAC.079",
	"r4rk1/p1B1bpp1/1p2pn1p/8/2PP4/3B1P2/qP2QP1P/3R1RK1 w - - ; bm Ra1; id WAC.080",
	"r4rk1/1bR1bppp/4pn2/1p2N3/1P6/P3P3/4BPPP/3R2K1 b - - ; bm Bd6; id WAC.081",
	"3rr1k1/pp3pp1/4b3/8/2P1B2R/6QP/P3q1P1/5R1K w - - ; bm Bh7 + ; id WAC.082",
	"3rr1k1/ppqbRppp/2p5/8/3Q1n2/2P3N1/PPB2PPP/3R2K1 w - - ; bm Qxd7; id WAC.083",
	"r2q1r1k/2p1b1pp/p1n5/1p1Q1bN1/4n3/1BP1B3/PP3PPP/R4RK1 w - - ; bm Qg8 + ; id WAC.084",
	"kr2R3/p4r2/2pq4/2N2p1p/3P2p1/Q5P1/5P1P/5BK1 w - - ; bm Na6; id WAC.085",
	"8/p7/1ppk1n2/5ppp/P1PP4/2P1K1P1/5N1P/8 b - - ; bm Ng4 + ; id WAC.086",
	"8/p3k1p1/4r3/2ppNpp1/PP1P4/2P3KP/5P2/8 b - - ; bm Rxe5; id WAC.087",
	"r6k/p1Q4p/2p1b1rq/4p3/B3P3/4P3/PPP3P1/4RRK1 b - - ; bm Rxg2 + ; id WAC.088",
	"1r3b1k/p4rpp/4pp2/3q4/2ppbPPQ/6RK/PP5P/2B1NR2 b - - ; bm g5; id WAC.089",
	"3qrrk1/1pp2pp1/1p2bn1p/5N2/2P5/P1P3B1/1P4PP/2Q1RRK1 w - - ; bm Nxg7; id WAC.090",
	"2qr2k1/4b1p1/2p2p1p/1pP1p3/p2nP3/PbQNB1PP/1P3PK1/4RB2 b - - ; bm Be6; id WAC.091",
	"r4rk1/1p2ppbp/p2pbnp1/q7/3BPPP1/2N2B2/PPP4P/R2Q1RK1 b - - ; bm Bxg4; id WAC.092",
	"r1b1k1nr/pp3pQp/4pq2/3pn3/8/P1P5/2P2PPP/R1B1KBNR w KQkq -  ; bm Bh6; id WAC.093",
	"8/k7/p7/3Qp2P/n1P5/3KP3/1q6/8 b - - ; bm e4 + ; id WAC.094",
	"2r5/1r6/4pNpk/3pP1qp/8/2P1QP2/5PK1/R7 w - - ; bm Ng4 + ; id WAC.095",
	"6k1/5p2/p5np/4B3/3P4/1PP1q3/P3r1QP/6RK w - - ; bm Qa8 + ; id WAC.097",
	"1r3rk1/5pb1/p2p2p1/Q1n1q2p/1NP1P3/3p1P1B/PP1R3P/1K2R3 b - - ; bm Nxe4; id WAC.098",
	"r1bq1r1k/1pp1Np1p/p2p2pQ/4R3/n7/8/PPPP1PPP/R1B3K1 w - - ; bm Rh5; id WAC.099",
	"5rk1/p5pp/8/8/2Pbp3/1P4P1/7P/4RN1K b - - ; bm Bc3; id WAC.101",
	"2Q2n2/2R4p/1p1qpp1k/8/3P3P/3B2P1/5PK1/r7 w - - ; bm Qxf8 + ; id WAC.102",
	"6k1/2pb1r1p/3p1PpQ/p1nPp3/1q2P3/2N2P2/PrB5/2K3RR w - - ; bm Qxg6 + ; id WAC.103",
	"b4r1k/pq2rp2/1p1bpn1p/3PN2n/2P2P2/P2B3K/1B2Q2N/3R2R1 w - - ; bm Qxh5; id WAC.104",
	"4rrk1/pppb4/7p/3P2pq/3Qn3/P5P1/1PP4P/R3RNNK b - - ; bm Nf2 + ; id WAC.106",
	"5n2/pRrk2p1/P4p1p/4p3/3N4/5P2/6PP/6K1 w - - ; bm Nb5; id WAC.107",
	"r5k1/1q4pp/2p5/p1Q5/2P5/5R2/4RKPP/r7 w - - ; bm Qe5; id WAC.108",
	"rn2k1nr/pbp2ppp/3q4/1p2N3/2p5/QP6/PB1PPPPP/R3KB1R b KQkq -  ; bm c3; id WAC.109",
	"2kr4/bp3p2/p2p2b1/P7/2q5/1N4B1/1PPQ2P1/2KR4 b - - ; bm Be3; id WAC.110",
	"6k1/p5p1/5p2/2P2Q2/3pN2p/3PbK1P/7P/6q1 b - - ; bm Qf1 + ; id WAC.111",
	"r4kr1/ppp5/4bq1b/7B/2PR1Q1p/2N3P1/PP3P1P/2K1R3 w - - ; bm Rxe6; id WAC.112",
	"rnbqkb1r/1p3ppp/5N2/1p2p1B1/2P5/8/PP2PPPP/R2QKB1R b KQkq -  ; bm Qxf6; id WAC.113",
	"r1b1rnk1/1p4pp/p1p2p2/3pN2n/3P1PPq/2NBPR1P/PPQ5/2R3K1 w - - ; bm Bxh7 + ; id WAC.114",
	"4N2k/5rpp/1Q6/p3q3/8/P5P1/1P3P1P/5K2 w - - ; bm Nd6; id WAC.115",
	"r2r2k1/2p2ppp/p7/1p2P1n1/P6q/5P2/1PB1QP1P/R5RK b - - ; bm Rd2; id WAC.116",
	"3r1rk1/q4ppp/p1Rnp3/8/1p6/1N3P2/PP3QPP/3R2K1 b - - ; bm Ne4; id WAC.117",
	"r5k1/pb2rpp1/1p6/2p4q/5R2/2PB2Q1/P1P3PP/5R1K w - - ; bm Rh4; id WAC.118",
	"r2qr1k1/p1p2ppp/2p5/2b5/4nPQ1/3B4/PPP3PP/R1B2R1K b - - ; bm Qxd3; id WAC.119",
	"6k1/5p1p/2bP2pb/4p3/2P5/1p1pNPPP/1P1Q1BK1/1q6 b - - ; bm Bxf3 + ; id WAC.121",
	"1k6/ppp4p/1n2pq2/1N2Rb2/2P2Q2/8/P4KPP/3r1B2 b - - ; bm Rxf1 + ; id WAC.122",
	"6k1/3r4/2R5/P5P1/1P4p1/8/4rB2/6K1 b - - ; bm g3; id WAC.124",
	"r1bqr1k1/pp3ppp/1bp5/3n4/3B4/2N2P1P/PPP1B1P1/R2Q1RK1 b - - ; bm Bxd4 + ; id WAC.125",
	"r5r1/pQ5p/1qp2R2/2k1p3/4P3/2PP4/P1P3PP/6K1 w - - ; bm Rxc6 + ; id WAC.126",
	"2k4r/1pr1n3/p1p1q2p/5pp1/3P1P2/P1P1P3/1R2Q1PP/1RB3K1 w - - ; bm Rxb7; id WAC.127",
	"6rk/1pp2Qrp/3p1B2/1pb1p2R/3n1q2/3P4/PPP3PP/R6K w - - ; bm Qg6; id WAC.128",
	"3r1r1k/1b2b1p1/1p5p/2p1Pp2/q1B2P2/4P2P/1BR1Q2K/6R1 b - - ; bm Bf3; id WAC.129",
	"6k1/1pp3q1/5r2/1PPp4/3P1pP1/3Qn2P/3B4/4R1K1 b - - ; bm Qh6; Qh8; id WAC.130",
	"2rq1bk1/p4p1p/1p4p1/3b4/3B1Q2/8/P4PpP/3RR1K1 w - - ; bm Re8; id WAC.131",
	"4r1k1/5bpp/2p5/3pr3/8/1B3pPq/PPR2P2/2R2QK1 b - - ; bm Re1; id WAC.132",
	"r1b1k2r/1pp1q2p/p1n3p1/3QPp2/8/1BP3B1/P5PP/3R1RK1 w kq -  ; bm Bh4; id WAC.133",
	"3r2k1/p6p/2Q3p1/4q3/2P1p3/P3Pb2/1P3P1P/2K2BR1 b - - ; bm Rd1 + ; id WAC.134",
	"3r1r1k/N2qn1pp/1p2np2/2p5/2Q1P2N/3P4/PP4PP/3R1RK1 b - - ; bm Nd4; id WAC.135",
	"6kr/1q2r1p1/1p2N1Q1/5p2/1P1p4/6R1/7P/2R3K1 w - - ; bm Rc8 + ; id WAC.136",
	"3b1rk1/1bq3pp/5pn1/1p2rN2/2p1p3/2P1B2Q/1PB2PPP/R2R2K1 w - - ; bm Rd7; id WAC.137",
	"rnb3kr/ppp2ppp/1b6/3q4/3pN3/Q4N2/PPP2KPP/R1B1R3 w - - ; bm Nf6 + ; id WAC.139",
	"r2b1rk1/pq4p1/4ppQP/3pB1p1/3P4/2R5/PP3PP1/5RK1 w - - ; bm Bc7; Rc7; id WAC.140",
	"4r1k1/p1qr1p2/2pb1Bp1/1p5p/3P1n1R/1B3P2/PP3PK1/2Q4R w - - ; bm Qxf4; id WAC.141",
	"r2q3n/ppp2pk1/3p4/5Pr1/2NP1Qp1/2P2pP1/PP3K2/4R2R w - - ; bm Re8; f6 + ; id WAC.142",
	"5b2/pp2r1pk/2pp1pRp/4rP1N/2P1P3/1P4QP/P3q1P1/5R1K w - - ; bm Rxh6 + ; id WAC.143",
	"r2q1rk1/pp3ppp/2p2b2/8/B2pPPb1/7P/PPP1N1P1/R2Q1RK1 b - - ; bm d3; id WAC.144",
	"r1bq4/1p4kp/3p1n2/p4pB1/2pQ4/8/1P4PP/4RRK1 w - - ; bm Re8; id WAC.145",
	"8/8/2Kp4/3P1B2/2P2k2/5p2/8/8 w - - ; bm Bc8; Bd3; Bh3; id WAC.146",
	"r2r2k1/ppqbppbp/2n2np1/2pp4/6P1/1P1PPNNP/PBP2PB1/R2QK2R b KQ -  ; bm Nxg4; id WAC.147",
	"2r1k3/6pr/p1nBP3/1p3p1p/2q5/2P5/P1R4P/K2Q2R1 w - - ; bm Rxg7; id WAC.148",
	"6k1/6p1/2p4p/4Pp2/4b1qP/2Br4/1P2RQPK/8 b - - ; bm Bxg2; id WAC.149",
	"r3r1k1/5p2/pQ1b2pB/1p6/4p3/6P1/Pq2BP1P/2R3K1 b - - ; bm e3; Ba3; Be5; Bf8; e3; e3 is best id WAC.150",
	"8/3b2kp/4p1p1/pr1n4/N1N4P/1P4P1/1K3P2/3R4 w - - ; bm Nc3; id WAC.151",
	"1br2rk1/1pqb1ppp/p3pn2/8/1P6/P1N1PN1P/1B3PP1/1QRR2K1 w - - ; bm Ne4; id WAC.152",
	"2r3k1/q4ppp/p3p3/pnNp4/2rP4/2P2P2/4R1PP/2R1Q1K1 b - - ; bm Nxd4; id WAC.153",
	"r1b2rk1/2p2ppp/p7/1p6/3P3q/1BP3bP/PP3QP1/RNB1R1K1 w - - ; bm Qxf7 + ; id WAC.154",
	"5bk1/1rQ4p/5pp1/2pP4/3n1PP1/7P/1q3BB1/4R1K1 w - - ; bm d6; id WAC.155",
	"r1b1qN1k/1pp3p1/p2p3n/4p1B1/8/1BP4Q/PP3KPP/8 w - - ; bm Qxh6 + ; id WAC.156",
	"5rk1/p4ppp/2p1b3/3Nq3/4P1n1/1p1B2QP/1PPr2P1/1K2R2R w - - ; bm Ne7 + ; id WAC.157",
	"5rk1/n1p1R1bp/p2p4/1qpP1QB1/7P/2P3P1/PP3P2/6K1 w - - ; bm Rxg7 + ; id WAC.158",
	"r1b2r2/5P1p/ppn3pk/2p1p1Nq/1bP1PQ2/3P4/PB4BP/1R3RK1 w - - ; bm Ne6 + ; id WAC.159",
	"qn1kr2r/1pRbb3/pP5p/P2pP1pP/3N1pQ1/3B4/3B1PP1/R5K1 w - - ; bm Qxd7 + ; id WAC.160",
	"3r3k/3r1P1p/pp1Nn3/2pp4/7Q/6R1/Pq4PP/5RK1 w - - ; bm Qxd8 + ; id WAC.161",
	"r3kbnr/p4ppp/2p1p3/8/Q1B3b1/2N1B3/PP3PqP/R3K2R w KQkq -  ; bm Bd5; id WAC.162",
	"5rk1/2p4p/2p4r/3P4/4p1b1/1Q2NqPp/PP3P1K/R4R2 b - - ; bm Qg2 + ; id WAC.163",
	"8/6pp/4p3/1p1n4/1NbkN1P1/P4P1P/1PR3K1/r7 w - - ; bm Rxc4 + ; id WAC.164",
	"1r5k/p1p3pp/8/8/4p3/P1P1R3/1P1Q1qr1/2KR4 w - - ; bm Re2; id WAC.165",
	"r3r1k1/5pp1/p1p4p/2Pp4/8/q1NQP1BP/5PP1/4K2R b K -  ; bm d4; id WAC.166",
	"7Q/ppp2q2/3p2k1/P2Ppr1N/1PP5/7R/5rP1/6K1 b - - ; bm Rxg2 + ; id WAC.167",
	"r3k2r/pb1q1p2/8/2p1pP2/4p1p1/B1P1Q1P1/P1P3K1/R4R2 b kq -  ; bm Qd2 + ; id WAC.168",
	"5rk1/1pp3bp/3p2p1/2PPp3/1P2P3/2Q1B3/4q1PP/R5K1 b - - ; bm Bh6; id WAC.169",
	"5r1k/6Rp/1p2p3/p2pBp2/1qnP4/4P3/Q4PPP/6K1 w - - ; bm Qxc4; id WAC.170",
	"2rq4/1b2b1kp/p3p1p1/1p1nNp2/7P/1B2B1Q1/PP3PP1/3R2K1 w - - ; bm Bh6 + ; id WAC.171",
	"5r1k/p5pp/8/1P1pq3/P1p2nR1/Q7/5BPP/6K1 b - - ; bm Qe1 + ; id WAC.172",
	"2r1b3/1pp1qrk1/p1n1P1p1/7R/2B1p3/4Q1P1/PP3PP1/3R2K1 w - - ; bm Qh6 + ; id WAC.173",
	"2r2rk1/6p1/p3pq1p/1p1b1p2/3P1n2/PP3N2/3N1PPP/1Q2RR1K b - - ; bm Nxg2; id WAC.174",
	"r5k1/pppb3p/2np1n2/8/3PqNpP/3Q2P1/PPP5/R4RK1 w - - ; bm Nh5; id WAC.175",
	"r1bq3r/ppp2pk1/3p1pp1/8/2BbPQ2/2NP2P1/PPP4P/R4R1K b - - ; bm Rxh2 + ; id WAC.176",
	"r1b3r1/4qk2/1nn1p1p1/3pPp1P/p4P2/1p3BQN/PKPBN3/3R3R b - - ; bm Qa3 + ; id WAC.177",
	"3r2k1/p1rn1p1p/1p2pp2/6q1/3PQNP1/5P2/P1P4R/R5K1 w - - ; bm Nxe6; id WAC.178",
	"r1b2r1k/pp4pp/3p4/3B4/8/1QN3Pn/PP3q1P/R3R2K b - - ; bm Qg1 + ; id WAC.179",
	"r1q2rk1/p3bppb/3p1n1p/2nPp3/1p2P1P1/6NP/PP2QPB1/R1BNK2R b KQ -  ; bm Nxd5; id WAC.180",
	"r3k2r/2p2p2/p2p1n2/1p2p3/4P2p/1PPPPp1q/1P5P/R1N2QRK b kq -  ; bm Ng4; id WAC.181",
	"r1b2rk1/ppqn1p1p/2n1p1p1/2b3N1/2N5/PP1BP3/1B3PPP/R2QK2R w KQ -  ; bm Qh5; id WAC.182",
	"1r2k1r1/5p2/b3p3/1p2b1B1/3p3P/3B4/PP2KP2/2R3R1 w - - ; bm Bf6; id WAC.183",
	"4kn2/r4p1r/p3bQ2/q1nNP1Np/1p5P/8/PPP3P1/2KR3R w - - ; bm Qe7 + ; id WAC.184",
	"1r1rb1k1/2p3pp/p2q1p2/3PpP1Q/Pp1bP2N/1B5R/1P4PP/2B4K w - - ; bm Qxh7 + ; id WAC.185",
	"r5r1/p1q2p1k/1p1R2pB/3pP3/6bQ/2p5/P1P1NPPP/6K1 w - - ; bm Bf8 + ; id WAC.186",
	"6k1/5p2/p3p3/1p3qp1/2p1Qn2/2P1R3/PP1r1PPP/4R1K1 b - - ; bm Nh3 + ; id WAC.187",
	"3RNbk1/pp3p2/4rQpp/8/1qr5/7P/P4P2/3R2K1 w - - ; bm Qg7 + ; id WAC.188",
	"3r1k2/1ppPR1n1/p2p1rP1/3P3p/4Rp1N/5K2/P1P2P2/8 w - - ; bm Re8 + ; id WAC.189",
	"8/p2b2kp/1q1p2p1/1P1Pp3/4P3/3B2P1/P2Q3P/2Nn3K b - - ; bm Bh3; id WAC.190",
	"2r1Rn1k/1p1q2pp/p7/5p2/3P4/1B4P1/P1P1QP1P/6K1 w - - ; bm Qc4; id WAC.191",
	"r3k3/ppp2Npp/4Bn2/2b5/1n1pp3/N4P2/PPP3qP/R2QKR2 b Qq -  ; bm Nd3 + ; id WAC.192",
	"5bk1/p4ppp/Qp6/4B3/1P6/Pq2P1P1/2rr1P1P/R4RK1 b - - ; bm Qxe3; id WAC.193",
	"5rk1/ppq2ppp/2p5/4bN2/4P3/6Q1/PPP2PPP/3R2K1 w - - ; bm Nh6 + ; id WAC.194",
	"3r1rk1/1p3p2/p3pnnp/2p3p1/2P2q2/1P5P/PB2QPPN/3RR1K1 w - - ; bm g3; id WAC.195",
	"rr4k1/p1pq2pp/Q1n1pn2/2bpp3/4P3/2PP1NN1/PP3PPP/R1B1K2R b KQ -  ; bm Nb4; id WAC.196",
	"7k/1p4p1/7p/3P1n2/4Q3/2P2P2/PP3qRP/7K b - - ; bm Qf1 + ; id WAC.197",
	"2br2k1/ppp2p1p/4p1p1/4P2q/2P1Bn2/2Q5/PP3P1P/4R1RK b - - ; bm Rd3; id WAC.198",
	"r1br2k1/pp2nppp/2n5/1B1q4/Q7/4BN2/PP3PPP/2R2RK1 w - - ; bm Bxc6; Rcd1; Rfd1; id WAC.199",
	"2rqrn1k/pb4pp/1p2pp2/n2P4/2P3N1/P2B2Q1/1B3PPP/2R1R1K1 w - - ; bm Bxf6; id WAC.200",
	"2b2r1k/4q2p/3p2pQ/2pBp3/8/6P1/1PP2P1P/R5K1 w - - ; bm Ra7; id WAC.201",
	"QR2rq1k/2p3p1/3p1pPp/8/4P3/8/P1r3PP/1R4K1 b - - ; bm Rxa2; id WAC.202",
	"r4rk1/5ppp/p3q1n1/2p2NQ1/4n3/P3P3/1B3PPP/1R3RK1 w - - ; bm Qh6; id WAC.203",
	"r1b1qrk1/1p3ppp/p1p5/3Nb3/5N2/P7/1P4PQ/K1R1R3 w - - ; bm Rxe5; id WAC.204",
	"r3rnk1/1pq2bb1/p4p2/3p1Pp1/3B2P1/1NP4R/P1PQB3/2K4R w - - ; bm Qxg5; id WAC.205",
	"1Qq5/2P1p1kp/3r1pp1/8/8/7P/p4PP1/2R3K1 b - - ; bm Rc6; id WAC.206",
	"r1bq2kr/p1pp1ppp/1pn1p3/4P3/2Pb2Q1/BR6/P4PPP/3K1BNR w - - ; bm Qxg7 + ; id WAC.207",
	"3r1bk1/ppq3pp/2p5/2P2Q1B/8/1P4P1/P6P/5RK1 w - - ; bm Bf7 + ; id WAC.208",
	"4kb1r/2q2p2/r2p4/pppBn1B1/P6P/6Q1/1PP5/2KRR3 w k -  ; bm Rxe5 + ; id WAC.209",
	"3r1rk1/pp1q1ppp/3pn3/2pN4/5PP1/P5PQ/1PP1B3/1K1R4 w - - ; bm Rh1; id WAC.210",
	"r1bqrk2/pp1n1n1p/3p1p2/P1pP1P1Q/2PpP1NP/6R1/2PB4/4RBK1 w - - ; bm Qxf7 + ; id WAC.211",
	"rn1qr2Q/pbppk1p1/1p2pb2/4N3/3P4/2N5/PPP3PP/R4RK1 w - - ; bm Qxg7 + ; id WAC.212",
	"3r1r1k/1b4pp/ppn1p3/4Pp1R/Pn5P/3P4/4QP2/1qB1NKR1 w - - ; bm Rxh7 + ; id WAC.213",
	"r2r2k1/1p2qpp1/1np1p1p1/p3N3/2PPN3/bP5R/4QPPP/4R1K1 w - - ; bm Ng5; id WAC.214",
	"3r2k1/pb1q1pp1/1p2pb1p/8/3N4/P2QB3/1P3PPP/1Br1R1K1 w - - ; bm Qh7 + ; id WAC.215",
	"r2qr1k1/1b1nbppp/p3pn2/1p1pN3/3P1B2/2PB1N2/PP2QPPP/R4RK1 w - - ; bm Nxf7; a4; id WAC.216",
	"r3kb1r/1pp3p1/p3bp1p/5q2/3QN3/1P6/PBP3P1/3RR1K1 w kq -  ; bm Qd7 + ; id WAC.217",
	"6k1/pp5p/2p3q1/6BP/2nPr1Q1/8/PP3R1K/8 w - - ; bm Bh6; id WAC.218",
	"7k/p4q1p/1pb5/2p5/4B2Q/2P1B3/P6P/7K b - - ; bm Qf1 + ; id WAC.219",
	"3rr1k1/ppp2ppp/8/5Q2/4n3/1B5R/PPP1qPP1/5RK1 b - - ; bm Qxf1 + ; id WAC.220",
	"r3k3/P5bp/2N1bp2/4p3/2p5/6NP/1PP2PP1/3R2K1 w q -  ; bm Rd8 + ; id WAC.221",
	"2r1r2k/1q3ppp/p2Rp3/2p1P3/6QB/p3P3/bP3PPP/3R2K1 w - - ; bm Bf6; id WAC.222",
	"r1bqk2r/pp3ppp/5n2/8/1b1npB2/2N5/PP1Q2PP/1K2RBNR w kq -  ; bm Nxe4; id WAC.223",
	"5rk1/p1q3pp/1p1r4/2p1pp1Q/1PPn1P2/3B3P/P2R2P1/3R2K1 b - - ; bm Rh6; e4; id WAC.224",
	"4R3/4q1kp/6p1/1Q3b2/1P1b1P2/6KP/8/8 b - - ; bm Qh4 + ; id WAC.225",
	"2b2rk1/p1p4p/2p1p1p1/br2N1Q1/1p2q3/8/PB3PPP/3R1RK1 w - - ; bm Nf7; id WAC.226",
	"2k1rb1r/ppp3pp/2np1q2/5b2/2B2P2/2P1BQ2/PP1N1P1P/2KR3R b - - ; bm d5; id WAC.227",
	"r4rk1/1bq1bp1p/4p1p1/p2p4/3BnP2/1N1B3R/PPP3PP/R2Q2K1 w - - ; bm Bxe4; id WAC.228",
	"8/8/8/1p5r/p1p1k1pN/P2pBpP1/1P1K1P2/8 b - - ; bm Rxh4; b4; id WAC.229",
	"2b5/1r6/2kBp1p1/p2pP1P1/2pP4/1pP3K1/1R3P2/8 b - - ; bm Rb4; id WAC.230",
	"r4rk1/1b1nqp1p/p5p1/1p2PQ2/2p5/5N2/PP3PPP/R1BR2K1 w - - ; bm Bg5; id WAC.231",
	"1R2rq1k/2p3p1/Q2p1pPp/8/4P3/8/P1r3PP/1R4K1 w - - ; bm Qb5; Rxe8; id WAC.232",
	"5rk1/p1p2r1p/2pp2p1/4p3/PPPnP3/3Pq1P1/1Q1R1R1P/4NK2 b - - ; bm Nb3; id WAC.233",
	"2kr1r2/p6p/5Pp1/2p5/1qp2Q1P/7R/PP6/1KR5 w - - ; bm Rb3; id WAC.234",
	"5r2/1p1RRrk1/4Qq1p/1PP3p1/8/4B3/1b3P1P/6K1 w - - ; bm Qe4; Qxf7 +; Rxf7 + ; id WAC.235",
	"1R6/p5pk/4p2p/4P3/8/2r3qP/P3R1b1/4Q1K1 b - - ; bm Rc1; id WAC.236",
	"r5k1/pQp2qpp/8/4pbN1/3P4/6P1/PPr4P/1K1R3R b - - ; bm Rc1 + ; id WAC.237",
	"1k1r4/pp1r1pp1/4n1p1/2R5/2Pp1qP1/3P2QP/P4PB1/1R4K1 w - - ; bm Bxb7; id WAC.238",
	"8/6k1/5pp1/Q6p/5P2/6PK/P4q1P/8 b - - ; bm Qf1 + ; id WAC.239",
	"2b4k/p1b2p2/2p2q2/3p1PNp/3P2R1/3B4/P1Q2PKP/4r3 w - - ; bm Qxc6; id WAC.240",
	"2rq1rk1/pp3ppp/2n2b2/4NR2/3P4/PB5Q/1P4PP/3R2K1 w - - ; bm Qxh7 + ; id WAC.241",
	"r1b1r1k1/pp1nqp2/2p1p1pp/8/4N3/P1Q1P3/1P3PPP/1BRR2K1 w - - ; bm Rxd7; id WAC.242",
	"1r3r1k/3p4/1p1Nn1R1/4Pp1q/pP3P1p/P7/5Q1P/6RK w - - ; bm Qe2; id WAC.243",
	"r6r/pp3ppp/3k1b2/2pb4/B4Pq1/2P1Q3/P5PP/1RBR2K1 w - - ; bm Qxc5 + ; id WAC.244",
	"4rrn1/ppq3bk/3pPnpp/2p5/2PB4/2NQ1RPB/PP5P/5R1K w - - ; bm Qxg6 + ; id WAC.245",
	"6R1/4qp1p/ppr1n1pk/8/1P2P1QP/6N1/P4PP1/6K1 w - - ; bm Qh5 + ; id WAC.246",
	"2k1r3/1p2Bq2/p2Qp3/Pb1p1p1P/2pP1P2/2P5/2P2KP1/1R6 w - - ; bm Rxb5; id WAC.247",
	"5r1k/1p4pp/3q4/3Pp1R1/8/8/PP4PP/4Q1K1 b - - ; bm Qc5 + ; id WAC.248",
	"r4rk1/pbq2pp1/1ppbpn1p/8/2PP4/1P1Q1N2/PBB2PPP/R3R1K1 w - - ; bm c5; d5; id WAC.249",
	"1b5k/7P/p1p2np1/2P2p2/PP3P2/4RQ1R/q2r3P/6K1 w - - ; bm Re8 + ; id WAC.250",
	"k7/p4p2/P1q1b1p1/3p3p/3Q4/7P/5PP1/1R4K1 w - - ; bm Qe5; Qf4; id WAC.251",
	"1rb1r1k1/p1p2ppp/5n2/2pP4/5P2/2QB4/qNP3PP/2KRB2R b - - ; bm Re2; Bg4; id WAC.252",
	"k5r1/p4b2/2P5/5p2/3P1P2/4QBrq/P5P1/4R1K1 w - - ; bm Qe8 + ; id WAC.253",
	"r6k/pp3p1p/2p1bp1q/b3p3/4Pnr1/2PP2NP/PP1Q1PPN/R2B2RK b - - ; bm Nxh3; id WAC.254",
	"3r3r/p4pk1/5Rp1/3q4/1p1P2RQ/5N2/P1P4P/2b4K w - - ; bm Rfxg6 + ; id WAC.255",
	"3r1rk1/1pb1qp1p/2p3p1/p7/P2Np2R/1P5P/1BP2PP1/3Q1BK1 w - - ; bm Nf5; id WAC.256",
	"4r1k1/pq3p1p/2p1r1p1/2Q1p3/3nN1P1/1P6/P1P2P1P/3RR1K1 w - - ; bm Rxd4; id WAC.257",
	"r3brkn/1p5p/2p2Ppq/2Pp3B/3Pp2Q/4P1R1/6PP/5R1K w - - ; bm Bxg6; id WAC.258",
	"r1bq1rk1/ppp2ppp/2np4/2bN1PN1/2B1P3/3p4/PPP2nPP/R1BQ1K1R w - - ; bm Qh5; id WAC.259",
	"2r2b1r/p1Nk2pp/3p1p2/N2Qn3/4P3/q6P/P4PP1/1R3K1R w - - ; bm Qe6 + ; id WAC.260",
	"r5k1/1bp3pp/p2p4/1p6/5p2/1PBP1nqP/1PP3Q1/R4R1K b - - ; bm Nd4; id WAC.261",
	"6k1/p1B1b2p/2b3r1/2p5/4p3/1PP1N1Pq/P2R1P2/3Q2K1 b - - ; bm Rh6; id WAC.262",
	"rnbqr2k/pppp1Qpp/8/b2NN3/2B1n3/8/PPPP1PPP/R1B1K2R w KQ -  ; bm Qg8 + ; id WAC.263",
	"r2r2k1/1R2qp2/p5pp/2P5/b1PN1b2/P7/1Q3PPP/1B1R2K1 b - - ; bm Qe5; Rab8; id WAC.264",
	"2r1k2r/2pn1pp1/1p3n1p/p3PP2/4q2B/P1P5/2Q1N1PP/R4RK1 w k -  ; bm exf6; id WAC.265",
	"r3q2r/2p1k1p1/p5p1/1p2Nb2/1P2nB2/P7/2PNQbPP/R2R3K b - - ; bm Rxh2 + ; id WAC.266",
	"2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk -  ; bm Nc7 + ; id WAC.267",
	"2r3kr/ppp2n1p/7B/5q1N/1bp5/2Pp4/PP2RPPP/R2Q2K1 w - - ; bm Re8 + ; id WAC.268",
	"2kr2nr/pp1n1ppp/2p1p3/q7/1b1P1B2/P1N2Q1P/1PP1BPP1/R3K2R w KQ -  ; bm axb4; id WAC.269",
	"2r1r1k1/pp1q1ppp/3p1b2/3P4/3Q4/5N2/PP2RPPP/4R1K1 w - - ; bm Qg4; id WAC.270",
	"2kr4/ppp3Pp/4RP1B/2r5/5P2/1P6/P2p4/3K4 w - - ; bm Rd6; id WAC.271",
	"nrq4r/2k1p3/1p1pPnp1/pRpP1p2/P1P2P2/2P1BB2/1R2Q1P1/6K1 w - - ; bm Bxc5; id WAC.272",
	"2k4B/bpp1qp2/p1b5/7p/1PN1n1p1/2Pr4/P5PP/R3QR1K b - - ; bm Ng3 + g3; id WAC.273",
	"8/1p6/p5R1/k7/Prpp4/K7/1NP5/8 w - -; am Rd6;  ; bm Rb6; Rg5 + ; id WAC.274",
	"r1b2rk1/1p1n1ppp/p1p2q2/4p3/P1B1Pn2/1QN2N2/1P3PPP/3R1RK1 b - - ; bm Nc5: Nxg2: b5; id WAC.275",
	"r5k1/pp1RR1pp/1b6/6r1/2p5/B6P/P4qPK/3Q4 w - - ; bm Qd5 + ; id WAC.276",
	"1r4r1/p2kb2p/bq2p3/3p1p2/5P2/2BB3Q/PP4PP/3RKR2 b - - ; bm Rg3; Rxg2; id WAC.277",
	"r2qkb1r/pppb2pp/2np1n2/5pN1/2BQP3/2N5/PPP2PPP/R1B1K2R w KQkq -  ; bm Bf7 + ; id WAC.278",
	"r7/4b3/2p1r1k1/1p1pPp1q/1P1P1P1p/PR2NRpP/2Q3K1/8 w - - ; bm Nxf5; Rc3; id WAC.279",
	"r1r2bk1/5p1p/pn4p1/N2b4/3Pp3/B3P3/2q1BPPP/RQ3RK1 b - - ; bm Bxa3; id WAC.280",
	"2R5/2R4p/5p1k/6n1/8/1P2QPPq/r7/6K1 w - - ; bm Rxh7 + ; id WAC.281",
	"6k1/2p3p1/1p1p1nN1/1B1P4/4PK2/8/2r3b1/7R w - - ; bm Rh8 + ; id WAC.282",
	"3q1rk1/4bp1p/1n2P2Q/3p1p2/6r1/Pp2R2N/1B4PP/7K w - - ; bm Ng5; id WAC.283",
	"3r3k/pp4pp/8/1P6/3N4/Pn2P1qb/1B1Q2B1/2R3K1 w - - ; bm Nf5; id WAC.284",
	"2rr3k/1b2bppP/p2p1n2/R7/3P4/1qB2P2/1P4Q1/1K5R w - - ; bm Qxg7 + ; id WAC.285",
	"3r1k2/1p6/p4P2/2pP2Qb/8/1P1KB3/P6r/8 b - - ; bm Rxd5 + ; id WAC.286",
	"rn3k1r/pp2bBpp/2p2n2/q5N1/3P4/1P6/P1P3PP/R1BQ1RK1 w - - ; bm Qg4; Qh5; id WAC.287",
	"r1b2rk1/p4ppp/1p1Qp3/4P2N/1P6/8/P3qPPP/3R1RK1 w - - ; bm Nf6 + ; id WAC.288",
	"2r3k1/5p1p/p3q1p1/2n3P1/1p1QP2P/1P4N1/PK6/2R5 b - - ; bm Qe5; id WAC.289",
	"2k2r2/2p5/1pq5/p1p1n3/P1P2n1B/1R4Pp/2QR4/6K1 b - - ; bm Ne2 + ; id WAC.290",
	"5r1k/3b2p1/p6p/1pRpR3/1P1P2q1/P4pP1/5QnP/1B4K1 w - - ; bm h3; id WAC.291",
	"4r3/1Q1qk2p/p4pp1/3Pb3/P7/6PP/5P2/4R1K1 w - - ; bm d6 + ; id WAC.292",
	"1nbq1r1k/3rbp1p/p1p1pp1Q/1p6/P1pPN3/5NP1/1P2PPBP/R4RK1 w - - ; bm Nfg5; id WAC.293",
	"3r3k/1r3p1p/p1pB1p2/8/p1qNP1Q1/P6P/1P4P1/3R3K w - - ; bm Bf8; Nf5; Qf4; id WAC.294",
	"4r3/p4r1p/R1p2pp1/1p1bk3/4pNPP/2P1K3/2P2P2/3R4 w - - ; bm Rxd5 + ; id WAC.295",
	"3r4/1p2k2p/p1b1p1p1/4Q1Pn/2B3KP/4pP2/PP2R1N1/6q1 b - - ; bm Rd4; + Rf8; id WAC.296",
	"3r1rk1/p3qp1p/2bb2p1/2p5/3P4/1P6/PBQN1PPP/2R2RK1 b - - ; bm Bxg2; Bxh2 + ; id WAC.297",
	"3Q4/p3b1k1/2p2rPp/2q5/4B3/P2P4/7P/6RK w - - ; bm Qh8 + ; id WAC.298",
	"1n2rr2/1pk3pp/pNn2p2/2N1p3/8/6P1/PP2PPKP/2RR4 w - - ; bm Nca4; id WAC.299",
	"b2b1r1k/3R1ppp/4qP2/4p1PQ/4P3/5B2/4N1K1/8 w - - ; bm g6; id WAC.300",
	"X"
};


char lct2[][MAXEPD_LENGTH] =
{
	"r3kb1r/3n1pp1/p6p/2pPp2q/Pp2N3/3B2PP/1PQ2P2/R3K2R w KQkq -  ; bm d6; id LCTII.POS.01",
	"1k1r3r/pp2qpp1/3b1n1p/3pNQ2/2pP1P2/2N1P3/PP4PP/1K1RR3 b - - ; bm Bb4; id LCTII.POS.02",
	"r6k/pp4p1/2p1b3/3pP3/7q/P2B3r/1PP2Q1P/2K1R1R1 w - - ; bm Qc5; id LCTII.POS.03",
	"1nr5/2rbkppp/p3p3/Np6/2PRPP2/8/PKP1B1PP/3R4 b - - ; bm e5; id LCTII.POS.04",
	"2r2rk1/1p1bq3/p3p2p/3pPpp1/1P1Q4/P7/2P2PPP/2R1RBK1 b - - ; bm Bb5; id LCTII.POS.05",
	"3r1bk1/p4ppp/Qp2p3/8/1P1B4/Pq2P1P1/2r2P1P/R3R1K1 b - - ; bm e5; id LCTII.POS.06",
	"r1b2r1k/pp2q1pp/2p2p2/2p1n2N/4P3/1PNP2QP/1PP2RP1/5RK1 w - - ; bm Nd1; id LCTII.POS.07",
	"r2qrnk1/pp3ppb/3b1n1p/1Pp1p3/2P1P2N/P5P1/1B1NQPBP/R4RK1 w - - ; bm Bh3; id LCTII.POS.08",
	"5nk1/Q4bpp/5p2/8/P1n1PN2/q4P2/6PP/1R4K1 w - - ; bm Qd4; id LCTII.POS.09 c0 Boissel - Del Gobbo, corr. 1994",
	"r3k2r/3bbp1p/p1nppp2/5P2/1p1NP3/5NP1/PPPK3P/3R1B1R b kq -  ; bm Bf8; id LCTII.POS.10",
	"bn6/1q4n1/1p1p1kp1/2pPp1pp/1PP1P1P1/3N1P1P/4B1K1/2Q2N2 w - - ; bm h4; id LCTII.POS.11",
	"3r2k1/pp2npp1/2rqp2p/8/3PQ3/1BR3P1/PP3P1P/3R2K1 b - - ; bm Rb6; id LCTII.POS.12",
	"1r2r1k1/4ppbp/B5p1/3P4/pp1qPB2/2n2Q1P/P4PP1/4RRK1 b - - ; bm Nxa2; id LCTII.POS.13",
	"r2qkb1r/1b3ppp/p3pn2/1p6/1n1P4/1BN2N2/PP2QPPP/R1BR2K1 w kq -  ; bm d5; id LCTII.POS.14",
	"1r4k1/1q2bp2/3p2p1/2pP4/p1N4R/2P2QP1/1P3PK1/8 w - - ; bm Nxd6; id LCTII.CMB.01",
	"rn3rk1/pbppq1pp/1p2pb2/4N2Q/3PN3/3B4/PPP2PPP/R3K2R w KQ -  ; bm Qxh7 + ; id LCTII.CMB.02",
	"4r1k1/3b1p2/5qp1/1BPpn2p/7n/r3P1N1/2Q1RPPP/1R3NK1 b - - ; bm Qf3; id LCTII.CMB.03",
	"2k2b1r/1pq3p1/2p1pp2/p1n1PnNp/2P2B2/2N4P/PP2QPP1/3R2K1 w - - ; bm exf6; id LCTII.CMB.04",
	"2r2r2/3qbpkp/p3n1p1/2ppP3/6Q1/1P1B3R/PBP3PP/5R1K w - - ; bm Rxh7 + ; id LCTII.CMB.05",
	"2r1k2r/2pn1pp1/1p3n1p/p3PP2/4q2B/P1P5/2Q1N1PP/R4RK1 w q -  ; bm exf6; id LCTII.CMB.06",
	"2rr2k1/1b3ppp/pb2p3/1p2P3/1P2BPnq/P1N3P1/1B2Q2P/R4R1K b - - ; bm Rxc3; id LCTII.CMB.07",
	"2b1r1k1/r4ppp/p7/2pNP3/4Q3/q6P/2P2PP1/3RR1K1 w - - ; bm Nf6 + ; id LCTII.CMB.08",
	"6k1/5p2/3P2p1/7n/3QPP2/7q/r2N3P/6RK b - - ; bm Rxd2; id LCTII.CMB.09",
	"rq2rbk1/6p1/p2p2Pp/1p1Rn3/4PB2/6Q1/PPP1B3/2K3R1 w - - ; bm Bxh6; id LCTII.CMB.10",
	"rnbq2k1/p1r2p1p/1p1p1Pp1/1BpPn1N1/P7/2P5/6PP/R1B1QRK1 w - - ; bm Nxh7; id LCTII.CMB.11",
	"r2qrb1k/1p1b2p1/p2ppn1p/8/3NP3/1BN5/PPP3QP/1K3RR1 w - - ; bm e5; id LCTII.CMB.12",
	"8/1p3pp1/7p/5P1P/2k3P1/8/2K2P2/8 w - - ; bm f6; id LCTII.FIN.01",
	"8/pp2r1k1/2p1p3/3pP2p/1P1P1P1P/P5KR/8/8 w - - ; bm f5; id LCTII.FIN.02",
	"8/3p4/p1bk3p/Pp6/1Kp1PpPp/2P2P1P/2P5/5B2 b - - ; bm Bxe4; id LCTII.FIN.03",
	"5k2/7R/4P2p/5K2/p1r2P1p/8/8/8 b - - ; bm h3; am h5; id LCTII.FIN.04",
	"6k1/6p1/7p/P1N5/1r3p2/7P/1b3PP1/3bR1K1 w - - ; bm a6; id LCTII.FIN.05",
	"8/3b4/5k2/2pPnp2/1pP4N/pP1B2P1/P3K3/8 b - - ; bm f4; id LCTII.FIN.06",
	"6k1/4pp1p/3p2p1/P1pPb3/R7/1r2P1PP/3B1P2/6K1 w - - ; bm Bb4; id LCTII.FIN.07",
	"2k5/p7/Pp1p1b2/1P1P1p2/2P2P1p/3K3P/5B2/8 w - - ; bm c5; id LCTII.FIN.08",
	"8/5Bp1/4P3/6pP/1b1k1P2/5K2/8/8 w - - ; bm Kg4; id LCTII.FIN.09",
	"X"
};

char nolot[][MAXEPD_LENGTH] =
{
	"r3qb1k/1b4p1/p2pr2p/3n4/Pnp1N1N1/6RP/1B3PP1/1B1QR1K1 w - - ; bm Nxh6; id Position 1",
	"r4rk1/pp1n1p1p/1nqP2p1/2b1P1B1/4NQ2/1B3P2/PP2K2P/2R5 w - - ; bm Rxc5; id Position 2",
	"r2qk2r/ppp1b1pp/2n1p3/3pP1n1/3P2b1/2PB1NN1/PP4PP/R1BQK2R w KQkq -  ; bm Nxg5; id Position 3",
	"r1b1kb1r/1p1n1ppp/p2ppn2/6BB/2qNP3/2N5/PPP2PPP/R2Q1RK1 w kq -  ; bm Nxe6; id Position 4",
	"r2qrb1k/1p1b2p1/p2ppn1p/8/3NP3/1BN5/PPP3QP/1K3RR1 w - - ; bm e5; id Position 5",
	"rnbqk2r/1p3ppp/p7/1NpPp3/QPP1P1n1/P4N2/4KbPP/R1B2B1R b kq -  ; bm axb5; id Position 6",
	"1r1bk2r/2R2ppp/p3p3/1b2P2q/4QP2/4N3/1B4PP/3R2K1 w k -  ; bm Rxd8 + ; id Position 7",
	"r3rbk1/ppq2ppp/2b1pB2/8/6Q1/1P1B3P/P1P2PP1/R2R2K1 w - - ; bm Bxh7 + ; id Position 8",
	"r4r1k/4bppb/2n1p2p/p1n1P3/1p1p1BNP/3P1NP1/qP2QPB1/2RR2K1 w - - ; bm Ng5; id Position 9",
	"r1b2rk1/1p1nbppp/pq1p4/3B4/P2NP3/2N1p3/1PP3PP/R2Q1R1K w - - ; bm Rxf7; id Position 10",
	"r1b3k1/p2p1nP1/2pqr1Rp/1p2p2P/2B1PnQ1/1P6/P1PP4/1K4R1 w - - ; bm Rxh6; id Position 11",
	"X"
};

char kaufman[][MAXEPD_LENGTH] =
{
	"rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/2N1P3/PP3PPP/R1BQKBNR w KQkq - 0 1 ; bm Qb3",
	"rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/1QN1P3/PP3PPP/R1B1KBNR b KQkq - 1 1 ; bm Bc8",
	"r1bqk2r/ppp2ppp/2n5/4P3/2Bp2n1/5N1P/PP1N1PP1/R2Q1RK1 b kq - 1 10 ; bm Nh6; am Ne5",
	"r1bqrnk1/pp2bp1p/2p2np1/3p2B1/3P4/2NBPN2/PPQ2PPP/1R3RK1 w - -1 12 ; bm b4",
	"rnbqkb1r/ppp1pppp/5n2/8/3PP3/2N5/PP3PPP/R1BQKBNR b KQkq - 3 5 ; bm e5",
	"rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/P1N5/1PQ1PPPP/R1B1KBNR b KQ - 1 5 ; bm Bcx3 + ",
	"r4rk1/3nppbp/bq1p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - -1 12 ; bm Rfb8",
	"rn1qkb1r/pb1p1ppp/1p2pn2/2p5/2PP4/5NP1/PP2PPBP/RNBQK2R w KQkq c6 1 6; bm d5",
	"r1bq1rk1/1pp2pbp/p1np1np1/3Pp3/2P1P3/2N1BP2/PP4PP/R1NQKB1R b KQ - 1 9; bm Nd4",
	"rnbqr1k1/1p3pbp/p2p1np1/2pP4/4P3/2N5/PP1NBPPP/R1BQ1RK1 w - -1 11 ; bm a4",
	"rnbqkb1r/pppp1ppp/5n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR b KQkq f3 1 3 ; bm d5",
	"r1bqk1nr/pppnbppp/3p4/8/2BNP3/8/PPP2PPP/RNBQK2R w KQkq - 2 6 ; bm Bxf7 + ",
	"rnbq1b1r/ppp2kpp/3p1n2/8/3PP3/8/PPP2PPP/RNBQKB1R b KQ d3 1 5 ; am Ne4",
	"rnbqkb1r/pppp1ppp/3n4/8/2BQ4/5N2/PPP2PPP/RNB2RK1 b kq - 1 6 ; am Nxc4",
	"r2q1rk1/2p1bppp/p2p1n2/1p2P3/4P1b1/1nP1BN2/PP3PPP/RN1QR1K1 w - -1 12 ; bm exf6",
	"r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - 2 7 ; bm d5",
	"r2qkbnr/2p2pp1/p1pp4/4p2p/4P1b1/5N1P/PPPP1PP1/RNBQ1RK1 w kq - 1 8 ; am hxg4",
	"r1bqkb1r/pp3ppp/2np1n2/4p1B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq e6 1 7 ; bm Bxf6 + ",
	"rn1qk2r/1b2bppp/p2ppn2/1p6/3NP3/1BN5/PPP2PPP/R1BQR1K1 w kq - 5 10 ; am Bxe6",
	"r1b1kb1r/1pqpnppp/p1n1p3/8/3NP3/2N1B3/PPP1BPPP/R2QK2R w KQkq - 3 8 ; am Ndb5",
	"r1bqnr2/pp1ppkbp/4N1p1/n3P3/8/2N1B3/PPP2PPP/R2QK2R b KQ - 2 11 ; am Kxe6",
	"r3kb1r/pp1n1ppp/1q2p3/n2p4/3P1Bb1/2PB1N2/PPQ2PPP/RN2K2R w KQkq - 3 11 ; bm a4",
	"r1bq1rk1/pppnnppp/4p3/3pP3/1b1P4/2NB3N/PPP2PPP/R1BQK2R w KQ - 3 7 ; bm Bxh7 + ",
	"r2qkbnr/ppp1pp1p/3p2p1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R w KQkq - 2 6 ; bm Nxe5",
	"rn2kb1r/pp2pppp/1qP2n2/8/6b1/1Q6/PP1PPPBP/RNB1K1NR b KQkq - 1 6 ; am Qxb3",
	"X"
};


// some of these are fortress positions, but not all
char fortresses[][MAXEPD_LENGTH] =
{
	"2k5/2p5/1q1p4/pPpPp1p1/N1P1Pp1p/P4PbP/KQ4P1/8 w - -; am Nxb6 + ? ; c1 diff=10",
	"2k5/2p5/1q1p4/pPpPp1pp/N1P1Pp2/P4PbP/KQ4P1/8 w - -; am Nxb6 + ? ; c1 diff=10",
	"8/4nk2/1p3p2/2rp2pp/1P1R1N1P/6P1/3KPP2/8 b - - ; am Rb5 ? ; c1 diff=7",
	"5rkq/3r1p1p/5RpP/2p3P1/2B2Q2/p7/P4PK1/8 w - - 0 1; draw",
	"5rkq/3r1p1p/5RpP/p1p3P1/2B2Q2/8/P4PK1/8 w - - 0 1; draw",
	"5rkq/3r1p1p/5RpP/p1p5/2B2QP1/8/P4PK1/8 b - - 0 1; draw",
	"k1b5/1p1p1p1p/pPpPpPpP/P1P1P1P1/8/8/8/K1B5 w - - 0 1; draw",
	"k7/1p1p1p1p/pPpPpPpP/P1P1P1P1/1B1B1B1B/B1B1B1B1/1B1B1B1B/K1B1B1B1 w - - 0 1; draw",
	"k7/1p1p1p1p/pPpPpPpP/B1B1B1B1/1BRBRBRB/B1BQBQB1/KBRBRBRB/B1B1B1B1 w - - 0 1; draw",
	"k7/1p1p1p1p/pPpPpPpP/P1P1P1P1/2RRRR2/BRBRBRBR/KBRBRBRB/B1BRBRBR w - - 0 1; draw",
	"1b6/2p5/1kB5/1PN5/8/3K4/8/q7 w - -; bm Nb3;",
	"8/1p2b3/6k1/5p2/p2NbP1p/P1B1P1p1/1P5P/6K1 w - -; bm h3; c0 fortress From Smirin-HIARCS",
	"8/1p6/1p6/kPp2P1K/2P5/N1Pp4/q2P4/1N6 w - -; draw; c0 eval = draw, c1 diff=9",
	"8/8/5k2/8/2B4p/5RbP/5pP1/7K b - -; draw; c0 eval = draw, c1 diff=7",
	"8/8/8/5Bp1/7k/8/4pPKP/8 w - -; bm Bg4;  eval = draw;",
	"r1b1k3/1p1p1pp1/1p1P3p/pP6/P7/8/5PPP/4R1K1 b q - ; c0 eval +/-; c1 diff=6",
	"1k3b1q/pP2p1p1/P1K1P1Pp/7P/2B5/8/8/8 w - -; bm Kd5; Bd5; c0 eval +/-; c1 diff=8",
	"8/2b2k1K/1pPp1p2/1P1P1P2/5B2/8/8/8 w - -; bm Be3; Bd2; Bc1; Bh6;",
	"1k1n4/1p1b1p2/1Bp1pPp1/1PP1P3/8/6P1/8/1K6 w - - ; bm Bc7 + ;",
	"1nrqr3/1b4bk/pp3pp1/3pPp1p/P1pP1P1P/2P1BN2/1PB2QP1/R4RK1 w - - ; bm Nh2; g4;",
	"1q1r3k/3P1pp1/ppBR1n1p/4Q2P/P4P2/8/5PK1/8 w - -; bm Rxf6; c1 diff=6",
	"1qrrbbk1/1p1nnppp/p3p3/4P3/2P5/1PN1N3/PB2Q1PP/1B2RR1K w - -; bm Bxh7 + ; c1 diff=5",
	"1r1rb1k1/5ppp/4p3/1p1p3P/1q2P2Q/pN3P2/PPP4P/1K1R2R1 w - -; bm Rxg7; c1 diff=6",
	"1r3rk1/3b1p1p/pp1p1p1Q/n1q1p3/2P1P3/P1PB1N2/6PP/1R3RK1 w - -; bm Nd4; c1 diff=6",
	"1r3rk1/3bbppp/1qn2P2/p2pP1P1/3P4/2PB1N2/6K1/qNBQ1R2 w - -; bm Bxh7 + ;",
	"1r3rk1/5p1p/p2pb1p1/3Np1P1/2pRPR2/qP2Q1N1/P1PK3P/8 w - -; bm Nf5; c1 diff=5",
	"1r6/4k3/r2p2p1/2pR1p1p/2P1pP1P/pPK1P1P1/P7/1B6 b - -; bm Rxb3; c1 diff=9",
	"1rb1nrk1/1p1qp2p/p1np1p1Q/3NPp2/2P5/3B1N1P/P2B2P1/R6K w - -; bm g4; c1 diff=6",
	"1rb1r1k1/2qn1pbp/pp1p2p1/2nP4/P1p1PP2/2N1BQNP/1PB3P1/R4R1K w - -; bm e5;",
	"1rb2r1k/3p1pp1/p3p2p/1p2P3/4q3/1N2B1R1/PPP1N1P1/2KR1B2 w - - ; bm Nf4;",
	"1rb4k/p5np/3p1rp1/1ppB4/2N2P2/1P2R1P1/P1P4P/4R1K1 w - -; bm Re8; c1 diff=5",
	"1rbr2k1/4q1p1/p2bp2p/8/R6P/3QB1P1/4PPB1/2R3K1 w - -; bm Bxh6;",
	"1rr1b1k1/1pq1bp2/p2p1np1/4p3/P2BP3/2NB2Q1/1PP3PP/4RR1K w - -; bm Rxf6;",
	"2R5/k1p1p1K1/P1p1Pp2/5P2/7P/pB6/2p5/2r5 w - -; bm Rxc7; c1 diff=5",
	"2b1Bbk1/5ppp/p3p3/8/1r3P2/4P3/P4P1P/3Q2K1 w - -; bm e4; c1 diff=5",
	"2b1r3/r2ppN2/8/1p1p1k2/pP1P4/2P3R1/PP3PP1/2K5 w - -; bm Nd6; c1 diff=10",
	"2b1rk2/5p2/p1P5/2p2P2/2p5/7B/P7/2KR4 w - -; bm f6; id arasan19.41",
	"2b2r2/2r2pbk/4n1pp/pp1pP3/2pP3q/P1P1B2P/1P1QNRPK/1B3R2 w - -; bm Rf6;",
	"2bq1b2/1p3k2/1r1p1p1p/pNpP1PpP/P1P3P1/6B1/1P5Q/2K1R3 w - -; bm Re6;",
	"2k2Br1/p6b/Pq1r4/1p2p1b1/1Ppp2p1/Q1P3N1/5RPP/R3N1K1 b - -; bm Rf6;",
	"2k5/1pP4p/3p3P/3p1N2/1P3P2/7K/5b2/8 w - -; bm Kg2; c1 diff=4",
	"2k5/8/1p1p4/pPpPp2p/2P1Pp2/P4PbP/K2Q4/8 b - -; bm h4; c1 diff=7",
	"2k5/p7/Pp1p1b2/1P1P1p2/2P2P1p/3K3P/5B2/8 w - -; bm c5; c1 diff=8",
	"2q2r1k/p1p1b1pp/br1p1n2/2p1N1BP/Q1P1P3/8/PP1N1PP1/2KR3R w - -;",
	"2r1r1k1/2P2bp1/p1Q2pq1/2R3b1/1P6/4p1Bp/4B2P/3R3K w - - ; bm Qf3;",
	"2r1r1k1/4bppp/1q4n1/3pP3/p1pP4/PnR2NBP/1P4P1/1B1QR2K w - -; bm e6; c1 diff=8",
	"2r2rk1/1b3ppp/pn2p3/2qpP3/1p1Q1PP1/3B2R1/PPP1N2P/1K1R4 w - -; bm Bxh7 + ;",
	"2r2rk1/1bpR1p2/1pq1pQp1/p3P2p/P1PR3P/5N2/2P2PPK/8 w - -; bm Kg3; c1 diff=5",
	"2r2rk1/2q2p1p/p1p1nPpQ/3p4/4p1B1/8/PPP3PP/R4R1K w - -; bm Rf5;",
	"2r2rk1/2qp1p1p/p1p2Pp1/2b1p3/4n3/4B3/PPP1B1PP/R3QR1K w - -; bm Bxc5;",
	"2r3k1/1r3p2/1qb1pPpR/3pP1N1/p1pP4/PpP5/1Pn1BB1P/3R3K w - -; bm Rg1;",
	"2r4r/3n1pk1/pq1p1bp1/3B4/1p2P1N1/7P/PP1Q1PK1/3RR3 w - -; bm Bxf7; c1 diff=6",
	"2r5/prkpR1p1/2p1ppK1/P1p1N1B1/P1P1P3/8/8/8 w - -; bm Bf4; c1 diff=9",
	"3B4/1r2p3/r2p1p2/bkp1P1p1/1p1P1PPp/p1P1K2P/PPB5/8 w - -; bm Ba4 + ; c1 diff=5",
	"3Q4/3p4/P2p4/N2b4/8/4P3/5p1p/5Kbk w - -; bm Qa8; c1 diff=6",
	"3k4/2pq3p/pp5R/4P3/P6P/2PN4/1PN3K1/8 w - -; bm Nd4; c1 diff=10",
	"3k4/4pp2/n2n4/5BN1/3N4/b7/4K3/8 w - -; bm Nxf7 + ; c1 diff=8",
	"3q3r/4kpb1/Q1ppp3/3b3p/PP1P1np1/6P1/5P1P/R1R1NBK1 b - -; bm Bxd4;",
	"3q4/4k3/1p1b1p1r/p2Q4/3B1p1p/7P/1P4P1/3R3K w - -; bm b4; id arasan19.120",
	"3r2k1/3N1ppp/p2Pp3/8/5q2/8/1PP3R1/1K1R1B2 w - -; bm c4; c1 diff=7",
	"3r2rk/5pp1/p2p1q1p/n1pPpP2/PpP1Q2P/1P3RR1/2B2PP1/6K1 w - -; bm Rg5;",
	"3rr1k1/pb3p1p/qp2p1pB/2p4n/3pN3/2P2B1Q/PP5P/R2R3K w - -; bm Qxh5;",
	"4K1k1/8/1p5p/1Pp3b1/8/1P3P2/P1B2P2/8 w - -; bm f4; id arasan19.189",
	"4R3/P7/2bB2p1/r3pk2/2p2p1P/2P2P1K/6P1/8 w - -; bm Rxe5 + ; c1 diff=7",
	"4bbk1/3nq2r/pr2p3/Np1p1p1p/1PpP1Pp1/2P1P1P1/PQ1NB1PR/2K4R b - -; bm e5; c1 diff=8",
	"4k1br/1K1p1n1r/2p2pN1/P2p1N2/2P3pP/5B2/P2P4/8 w - - ; bm Kc8;",
	"4kb1r/2q2ppb/p2pp2p/5P2/NrPB2P1/1P3Q2/1K1R3P/7R w k -  ; bm fxe6;",
	"4kr2/5p1K/3p1Q2/1p4P1/4P3/1PP5/7b/8 w - -; bm Kh6; c1 diff=9",
	"4q1kr/p6p/1prQPppB/4n3/4P3/2P5/PP2B2P/R5K1 w - -; bm Qxe5; c1 diff=10",
	"4r2r/pppkq1pp/2n1pn2/4p1B1/4N2Q/8/PPP3PP/4RRK1 w - -; bm Nxf6 + ;",
	"4rrk1/p1q1bpp1/b1n1p2p/1pppP1N1/n4B1P/3P2P1/PPP1QPBN/R3R1K1 w - - 0 1 ; bm Qh5;",
	"5N2/6N1/q2k3p/8/6p1/4K1PP/B5P1/8 w - -; bm Nf5;",
	"5k2/4bp2/2B3p1/1P4p1/3R4/3P2PP/2r2PK1/8 b - -; bm Rxf2 + ;",
	"5kN1/3p1q2/5P2/5PP1/2pK4/1p1R4/1P6/8 w - -; bm Rxb3; c1 diff=6",
	"5nr1/2Pp2pk/3Pp1p1/4P1P1/6P1/5K2/8/7n w - - ; bm c8N;",
	"5rk1/2p2Rpq/p2p2n1/2pPpQPp/4P2P/3P2N1/PP6/6K1 w - -; bm Qe6; c1 diff=7",
	"5rk1/8/pqPp1r1p/1p1Pp1bR/4B3/5PP1/PP2Q1K1/R7 w - -; bm Rxg5 + ;",
	"5rk1/p2qr1b1/1Pn3pp/2pn4/2Np1p2/1N1P2P1/1P1BPP1P/R2QR1K1 b - -; bm Qh3; c1 diff=5",
	"5rk1/pp3p1p/7b/2pR4/8/2P4P/P1PNr1P1/2K4R b - -; bm Rxd2;",
	"5rr1/1p1bk3/p3pp2/8/3PP1pp/1PN1P3/1P3RPP/5RK1 w - -; bm e5; c0 MD vs JL, Chess Thinkers Forum 2015",
	"6R1/8/2pB3k/2P4p/5p1q/5P2/4P1K1/8 w - -; bm Bf8 + ; c1 diff=8",
	"6b1/5p2/1p3Bp1/1P3pP1/5P2/pp1B2K1/pr4P1/k7 w - -; bm Kh2; c1 diff=10",
	"6br/1KNp1n1r/2p2p2/P1ppRP2/1kP3pP/3PBB2/PN1P4/8 w - -; bm Bxc5 + ;",
	"6k1/1pp2p2/3p4/p2P1Pp1/2P1n1Pp/4B1nP/PP4B1/4K3 w - -; bm Bxe4; c1 diff=8",
	"6k1/p7/8/4B3/Pp3p1P/qP5P/P1Rb2K1/8 w - -; bm Kf3; c1 diff=10",
	"6k1/ppn2p2/8/2P2P1p/1P2K3/2N5/P7/8 w - -; bm Nd5; c1 diff=7",
	"6qk/p4r1p/1n1p2pP/2p1p1Q1/2PnB1R1/1P6/P4PP1/5RK1 w - -; bm b4; c1 diff=8",
	"6r1/3q1npk/2b1pbnp/Rp1p4/1N1P3P/2PQ1pP1/1K3B2/5B1R w - -; bm Qxg6; c1 diff=7",
	"7k/3q1pp1/1p3r2/p1bP4/P1P2p2/1P2rNpP/2Q3P1/4RR1K b - -; bm Rxf3;",
	"7k/pp2Np1p/2p2P1N/5pP1/7K/P4n2/1q3Q2/5n2 w - -; bm Kh5; c1 diff=10",
	"7q/b1p5/1p1Npkb1/pPP2ppP/P1P5/3B2P1/5P1R/K3R3 w - -; bm hxg6; c1 diff=10",
	"7r/pp3pk1/3p1N2/5PBp/1Pq1P3/2P4P/rP3RP1/4R1K1 w - -; bm Kh2; c1 diff=9",
	"8/1p1r2R1/p5p1/5bp1/2PB1k2/1PK5/P7/8 b - -; bm Rxd4; c1 diff=8",
	"8/1p2K2k/8/2p3rr/8/PPR4B/2P3P1/8 w - -; bm Bf5 + ; c1 diff=8",
	"8/2N4r/1p3pkp/8/5K1p/2P4N/P3Bn2/8 w - - ; bm Bh5;",
	"8/2p1q3/p3P3/2P4p/1PBP2kP/2N3P1/7K/8 w - -; bm Be2; c1 diff=7",
	"8/2pPpP2/1P1qk1p1/1p4P1/1P4p1/1p1QK1P1/2PpPp2/8 w - -; bm f8 = Q; c1 diff=7",
	"8/3P3k/n2K3p/2p3n1/1b4N1/2p1p1P1/8/3B4 w - -; bm Nf6 + ; c0 study by Van Breukelen",
	"8/4K2p/6pr/5pk1/1pB2R2/p4PP1/8/8 w - -; bm Rg4; c1 diff=7",
	"8/4nk2/1p3p2/1r1p2pp/1P1R1N1P/6P1/3KPP2/8 w - -; bm Nd3;",
	"8/5p2/N1p3p1/3pP1P1/7P/2kP4/Pb6/7K w - -; bm h5; c1 diff=7",
	"8/6k1/2p2qp1/1p1n1p2/r2P4/p1P3P1/3Q1PK1/R6R w - -; bm c4;",
	"8/6pp/1K6/N5P1/3N4/8/npn1P3/k7 w - -; bm Nab3 + ; fmvn 1; hmvc 0;",
	"8/8/2p1p2b/1pPp1p1k/pP1P1P2/P2PB2K/8/8 w - -; bm Bc1; c1 diff=9",
	"8/8/4b1p1/2Bp3p/5P1P/1pK1Pk2/8/8 b - -; bm g5 d4 + ;",
	"8/8/4kpp1/3p1b2/p6P/2B5/6P1/6K1 b - -; bm Bh3; c1 diff=7",
	"8/8/8/2pK3p/8/5N2/1P4pq/5Rbk w - -; bm Rb1; c1 diff=8",
	"8/P5k1/P5p1/5pKp/4bP1P/6P1/5B2/8 w - -; bm g4;",
	"8/bB1p1p1k/Pr2nPpn/4P3/1PR3P1/2B4p/4K2P/8 w - -; bm g5;",
	"8/bp4Pk/4KB2/R6p/2pNp1P1/p7/3p1Pnq/2r5 w - - ; bm Nc6;",
	"8/p4pk1/r1p1p1p1/P1PpRnPp/1P3P2/2P5/4RK2/8 w - -; bm Rxf5;",
	"N7/8/2KQ2rp/6k1/3p3p/2p4P/4PP2/5N2 w - -; bm f4 + ; c1 diff=8",
	"k5r1/5p1p/p1b1B3/2N1n1r1/1P1q4/3p1P1Q/PP3RPP/5R1K b - -; bm d2; c1 diff=7",
	"k7/p1pR4/1p3p2/2P2rpB/PbP4p/2N1p2p/KP6/8 w - -; bm Bf3; c1 diff=7",
	"kr1b4/p1p2B2/2p5/2p1p3/KpP1P3/1P4N1/8/7R w - - ; bm Rh6;",
	"n1QBq1k1/5p1p/5KP1/p7/8/8/8/8 w - -; bm Bc7; c1 diff=9",
	"q1r5/k3ppp1/2R5/PP1KP2r/2p4Q/2p1p3/5pP1/3R4 w - - ; bm Qd4 + ;",
	"q3nrk1/4bppp/3p4/r3nPP1/4P2P/NpQ1B3/1P4B1/1K1R3R b - -; bm Nc7;",
	"q7/b1p5/kp1p4/p2PN3/PPP5/1K6/8/5B2 w - -; bm c5 + ; c1 diff=7",
	"r1b1k1r1/1p2np1p/p1n1pQp1/3p4/3NPP2/P2RB3/2PK2PP/q4B1R w q d6 ; bm Be2; c1 diff=6",
	"r1b1r1k1/p2q1ppp/np2n3/3p1NBQ/8/6PB/P4P1P/1R2R1K1 w - -; bm Bf6; c1 diff=7",
	"r1b1r2k/pp1n1pp1/1qpb3p/8/3PB2B/5N2/PPQ2PPP/R3R1K1 w - -; bm Bg6; c1 diff=9",
	"r1b2rk1/1pq2ppp/p1p1p3/2n1P3/2N2P2/3B4/PPP3PP/R2Q1RK1 w - -; bm Bxh7; c1 diff=7",
	"r1b2rk1/2q1bppp/p2pp3/2n3PQ/1p1BP3/1BN5/PPP2P1P/2KR2R1 w - -; bm Bf6; c1 diff=7",
	"r1b3k1/ppp1q1pp/2n1p3/3pP2P/8/2NB2Q1/PPP3P1/2K2R2 w - -; bm Bg6;",
	"r1b3kr/pp1n2Bp/2pb2q1/3p3N/3P4/2P2Q2/P1P3PP/4RRK1 w - -; bm Re5; id arasan19.124",
	"r1b3r1/5p1k/p1n2P1p/P1qpp1P1/1p1p4/3P2Q1/BPPB2P1/R4RK1 w - -; bm Kf2; id arasan19.179;",
	"r1bq1rk1/1p1nbppp/p3p3/2ppP3/5B1P/2PBP3/PP1N1PP1/R2QK2R w - -; bm Bxh7 + ;",
	"r1bq1rk1/p4p1p/1np3p1/3p2Pn/1p2PP2/1PbQB3/P1P1BN1P/2KR2NR b - -; bm Nc4; c1 diff=6",
	"r1bq1rk1/pp1n1pp1/4pn1p/2p3B1/3P4/P1PB1N2/2P3PP/R3QRK1 w - -; bm Qh4; c1 diff=6",
	"r1bq1rk1/pp2b2p/2p1pppQ/8/1n1PN2P/3B1N2/PPP2PP1/2KR3R w - -; bm Neg5;",
	"r1bq1rk1/pp2bppp/1n2p3/3pP3/8/2RBBN2/PP2QPPP/2R3K1 w - -; bm Bxh7 + ; c1 diff=7",
	"r1bq1rk1/ppp3b1/3p1nn1/P1PPp1Bp/1P2P3/1QNN1Pp1/4B1P1/R4RK1 b - -; bm Bh6; c1 diff=8",
	"r1bqkb1r/ppp3pp/n3pn2/1N1p1p2/3P1B2/4P3/PPP2PPP/R2QKBNR w KQkq - ; bm c4;",
	"r1bqn1nk/ppp4p/3p4/3Pp1r1/2P1Pp2/2NB1P2/PP2N3/2K1Q1RR w - -; bm Rxh7; c1 diff=5",
	"r1bqr1k1/3n1ppp/p2p1b2/3N1PP1/1p1B1P2/1P6/1PP1Q2P/2KR2R1 w - -; bm Qxe8 + ;",
	"r1q1rbk1/2pp1ppp/p1b5/1p1NP3/3Q4/3R2B1/PPP2PPP/5RK1 w - -; bm Nf6; c1 diff=8",
	"r1q2rk1/ppp1bp2/6p1/3pNPn1/7p/2PP3P/P6B/1R1Q1RK1 w - -; bm f6; c1 diff=6",
	"r2br1k1/1b3q1p/pp1p1ppB/3Nn3/n1PNP2R/4Q3/1P2B1PP/5R1K w - -; bm Nf5;",
	"r2q1r2/1b2bpkp/n3p1p1/2ppP1P1/p6R/1PN1BQR1/NPP2P1P/4K3 w - -; bm Qf6 + ;",
	"r2q1r2/1b2bpkp/p3p1p1/2ppP1P1/7R/1PN1BQR1/1PP2P1P/4K3 w - -; bm Qf6 + ;",
	"r2q1rk1/ppp2p2/3p1np1/4pNQ1/4P1pP/1PPP4/1P3P2/R3K1R1 w Q - ; bm Qh6; c1 diff=8",
	"r2qk2r/ppp1b1pp/2n1p3/3pP1n1/3P2b1/2PB1NN1/PP4PP/R1BQK2R w KQkq - ; bm Nxg5; c1 diff=8",
	"r2qkb1r/3bpppp/p4nn1/3P4/1p1N4/1QN3B1/PP3PPP/R3KB1R w KQkq - ; bm Nc6; c1 diff=6",
	"r2qnrk1/p4pp1/1p2b2p/2p1Pp2/2P5/PnPB1QN1/6PP/R1B2RK1 w - -; bm Bxh6;",
	"r2qrbk1/1b3p2/3p2pB/1p1P1N2/1np1P3/5NRP/1P3PP1/1B1Qn1K1 w - - ; bm N3h4;",
	"r2r2k1/pqp1bppp/1pn1pn2/8/2NP1B2/2N1P1P1/PPQ2P1P/R4RK1 b - -; bm Rxd4;",
	"r3nrk1/5pB1/p2q1P1b/1p2P3/3p4/3B1N2/PPP5/5R1K w - -; bm Bxh6;",
	"r3r1k1/1bppq2p/1p1n1bp1/3P1p2/p4N1P/P1PB1P2/1P1Q1BP1/1K1R3R w - -; bm g4;",
	"r3r1k1/pp1q1p2/2p2npb/PPPp4/3Pp3/2NNPpP1/1R1B1P1K/3Q3R b - -; bm Kg7;",
	"r3r2k/1pq2pp1/4b2p/3pP3/p1nB3P/P2B1RQ1/1PP3P1/3R3K w - -; bm Rf6;",
	"r3rbk1/ppq2ppp/2b1pB2/8/6Q1/1P1B3P/P1P2PP1/R2R2K1 w - -; bm Bxh7 + ; c1 diff=7",
	"r4r1k/1p1n1ppp/1qb1p3/2b1P3/2B3Q1/p1NR4/PPP3PP/1KB1R3 w - -; bm Rh3; c1 diff=7",
	"r4r1k/1p4qp/b1pNRbp1/2P5/3p1P1P/PQ4P1/3R2BK/8 w - -; bm Rd1;",
	"r4r1k/6qp/3R1bb1/pNp2p2/PnB1pP2/4B2P/1P3Q1K/6R1 w - -; bm Rxf6;",
	"r4rk1/1bq1bppp/p1npp3/2nB1NP1/1p2P3/2N1B3/PPP1QP1P/2KR2R1 w - -; bm Qh5;",
	"r4rk1/4bppp/bq2p3/npnpP3/p2B1P2/P1PB1N2/1P2QNPP/R4RK1 w - - ; bm Bxh7 + ;",
	"r4rk1/p4ppp/qp2p3/b5B1/n1R5/5N2/PP2QPPP/1R4K1 w - -; bm Bf6; c1 diff=7",
	"r5k1/1b2bppp/1n2p3/1P6/2qPP3/r1p2N2/1R2QPPP/1BR1B1K1 b - -; bm cxb2; c1 diff=6",
	"r5k1/6b1/2Nq4/2pP1p2/p1P1pPr1/Pp6/3R2PQ/1K3R2 b - -; bm e3; c1 diff=9",
	"r5k1/p1pb2bp/3p3r/P1pPp1p1/2B1Pq2/1R2QPp1/1P4PP/5RBK b - -; bm Rxh2 + ;",
	"r5n1/p1p1q2k/4b2p/3pB3/3PP1pP/8/PPPQ2P1/5RK1 w - -; bm Qf4; c1 diff=7",
	"r5rk/1bq1bpp1/1p1ppn1p/pP5P/2P1P3/P1N2P2/1B1Q1P2/1K1R1BR1 w - - ; bm Nd5;",
	"r6k/N1Rb2bp/p2p1nr1/3Pp2q/1P2Pp1P/5N2/P3QBP1/4R1K1 b - -; bm Bh3;",
	"r6r/4k3/p2bP2p/2n2PP1/2BQn2P/1pP1p3/1q1BK3/3R3R b - -; bm Qc2; c1 diff=8",
	"rn1q1rk1/p2p1pb1/bp3np1/2pP2Bp/7P/P1N2N2/1PQ1PPP1/R3KB1R w KQ - ; bm g4;",
	"rn1qr1k1/1p2bppp/p3p3/3pP3/P2P1B2/2RB1Q1P/1P3PP1/R5K1 w - -; bm Bxh7 + ; c1 diff=7",
	"rn2kbnr/pp2ppp1/2p5/3pP1Bp/3P3P/3Q4/PqP2PP1/RN2K1NR w KQkq -  ; bm e6;",
	"rnbRrk2/2p5/1p2PB1p/pP4p1/8/P3R2P/2P2P2/6K1 w - -; bm h4; c1 diff=7",
	"X"
};

char bratkokopec[][MAXEPD_LENGTH] =
{
	"1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - - ; bm Qd1 + ; id BK.01;",
	"3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - - ; bm d5; id BK.02;",
	"2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b - - ; bm f5; id BK.03;",
	"rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq -  ; bm e6; id BK.04;",
	"r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w - - ; bm Nd5 a4; id BK.05;",
	"2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w - - ; bm g6; id BK.06;",
	"1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w - - ; bm Nf6; id BK.07;",
	"4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w - - ; bm f5; id BK.08;",
	"2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w - - ; bm f5; id BK.09;",
	"3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b - - ; bm Ne5; id BK.10;",
	"2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w - - ; bm f4; id BK.11;",
	"r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b - - ; bm Bf5; id BK.12;",
	"r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w - - ; bm b4; id BK.13;",
	"rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1 w - - ; bm Qd2 Qe1; id BK.14;",
	"2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w - - ; bm Qxg7 + ; id BK.15;",
	"r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq -  ; bm Ne4; id BK.16;",
	"r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b - - ; bm h5; id BK.17;",
	"r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b - - ; bm Nb3; id BK.18;",
	"3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b - - ; bm Rxe4; id BK.19;",
	"r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w - - ; bm g4; id BK.20;",
	"3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w - - ; bm Nh6; id BK.21;",
	"2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - - ; bm Bxe4; id BK.22;",
	"r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq -  ; bm f6; id BK.23;",
	"r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w - - ; bm f4; id BK.24;",
	"X"
};

// Dann Corbitt - Silent but Deadly positions
char silentbutdeadly[][MAXEPD_LENGTH] =
{
	"1qr3k1/p2nbppp/bp2p3/3p4/3P4/1P2PNP1/P2Q1PBP/1N2R1K1 b - - ; bm Qc7; id  sbd.001",
	"1r2r1k1/3bnppp/p2q4/2RPp3/4P3/6P1/2Q1NPBP/2R3K1 w - - ; bm Rc7; id  sbd.002",
	"2b1k2r/2p2ppp/1qp4n/7B/1p2P3/5Q2/PPPr2PP/R2N1R1K b k -  ; bm O-O; id  sbd.003",
	"2b5/1p4k1/p2R2P1/4Np2/1P3Pp1/1r6/5K2/8 w - - ; bm Rd8; id  sbd.004",
	"2brr1k1/ppq2ppp/2pb1n2/8/3NP3/2P2P2/P1Q2BPP/1R1R1BK1 w - - ; bm g3; id  sbd.005",
	"2kr2nr/1pp3pp/p1pb4/4p2b/4P1P1/5N1P/PPPN1P2/R1B1R1K1 b - - ; bm Bf7; id  sbd.006",
	"2r1k2r/1p1qbppp/p3pn2/3pBb2/3P4/1QN1P3/PP2BPPP/2R2RK1 b k -  ; bm O-O; id  sbd.007",
	"2r1r1k1/pbpp1npp/1p1b3q/3P4/4RN1P/1P4P1/PB1Q1PB1/2R3K1 w - - ; bm Rce1; id  sbd.008",
	"2r2k2/r4p2/2b1p1p1/1p1p2Pp/3R1P1P/P1P5/1PB5/2K1R3 w - - ; bm Kd2; id  sbd.009",
	"2r3k1/5pp1/1p2p1np/p1q5/P1P4P/1P1Q1NP1/5PK1/R7 w - - ; bm Rd1; id  sbd.010",
	"2r3qk/p5p1/1n3p1p/4PQ2/8/3B4/5P1P/3R2K1 w - - ; bm e6; id  sbd.011",
	"3b4/3k1pp1/p1pP2q1/1p2B2p/1P2P1P1/P2Q3P/4K3/8 w - - ; bm Qf3; id  sbd.012",
	"3n1r1k/2p1p1bp/Rn4p1/6N1/3P3P/2N1B3/2r2PP1/5RK1 w - - ; bm Na4 Nce4; id  sbd.013",
	"3q1rk1/3rbppp/ppbppn2/1N6/2P1P3/BP6/P1B1QPPP/R3R1K1 w - - ; bm Nd4; id  sbd.014",
	"3r1rk1/p1q4p/1pP1ppp1/2n1b1B1/2P5/6P1/P1Q2PBP/1R3RK1 w - - ; bm Bh6; id  sbd.015",
	"3r2k1/2q2p1p/5bp1/p1pP4/PpQ5/1P3NP1/5PKP/3R4 b - - ; bm Qd6; id  sbd.016",
	"3r2k1/p1q1npp1/3r1n1p/2p1p3/4P2B/P1P2Q1P/B4PP1/1R2R1K1 w - - ; bm Bc4; id  sbd.017",
	"3r4/2k5/p3N3/4p3/1p1p4/4r3/PPP3P1/1K1R4 b - - ; bm Kd7; id  sbd.018",
	"3r4/2R1np1p/1p1rpk2/p2b1p2/8/PP2P3/4BPPP/2R1NK2 w - - ; bm b4; id  sbd.019",
	"3rk2r/1b2bppp/p1qppn2/1p6/4P3/PBN2PQ1/1PP3PP/R1B1R1K1 b k -  ; bm O-O; id  sbd.020",
	"3rk2r/1bq2pp1/2pb1n1p/p3pP2/P1B1P3/8/1P2QBPP/2RN1R1K b k -  ; bm Be7 O-O; id  sbd.021",
	"3rkb1r/pppb1pp1/4n2p/2p5/3NN3/1P5P/PBP2PP1/3RR1K1 w - - ; bm Nf5; id  sbd.022",
	"3rr1k1/1pq2ppp/p1n5/3p4/6b1/2P2N2/PP1QBPPP/3R1RK1 w - - ; bm Rfe1; id  sbd.023",
	"4r1k1/1q1n1ppp/3pp3/rp6/p2PP3/N5P1/PPQ2P1P/3RR1K1 w - - ; bm Rc1; id  sbd.024",
	"4rb1k/1bqn1pp1/p3rn1p/1p2pN2/1PP1p1N1/P1P2Q1P/1BB2PP1/3RR1K1 w - - ; bm Qe2; id  sbd.025",
	"4rr2/2p5/1p1p1kp1/p6p/P1P4P/6P1/1P3PK1/3R1R2 w - - ; bm Rfe1; id  sbd.026",
	"5r2/pp1b1kpp/8/2pPp3/2P1p2P/4P1r1/PPRKB1P1/6R1 b - - ; bm Ke7; id  sbd.027",
	"6k1/1R5p/r2p2p1/2pN2B1/2bb4/P7/1P1K2PP/8 w - - ; bm Nf6 + ; id  sbd.028",
	"6k1/pp1q1pp1/2nBp1bp/P2pP3/3P4/8/1P2BPPP/2Q3K1 w - - ; bm Qc5; id  sbd.029",
	"6k1/pp2rp1p/2p2bp1/1n1n4/1PN3P1/P2rP2P/R3NPK1/2B2R2 w - - ; bm Rd2; id  sbd.030",
	"8/2p2kpp/p6r/4Pp2/1P2pPb1/2P3P1/P2B1K1P/4R3 w - - ; bm h4; id  sbd.031",
	"Q5k1/5pp1/5n1p/2b2P2/8/5N1P/5PP1/2q1B1K1 b - - ; bm Kh7; id  sbd.032",
	"r1b1k1nr/1p3ppp/p1np4/4p1q1/2P1P3/N1NB4/PP3PPP/2RQK2R w Kkq -  ; bm O-O; id  sbd.033",
	"r1b1k2r/p1pp1ppp/1np1q3/4P3/1bP5/1P6/PB1NQPPP/R3KB1R b KQkq -  ; bm O-O; id  sbd.034",
	"r1b1k2r/ppppqppp/8/2bP4/3p4/6P1/PPQPPPBP/R1B2RK1 b kq -  ; bm O-O; id  sbd.035",
	"r1b1k2r/ppq1bppp/2n5/2N1p3/8/2P1B1P1/P3PPBP/R2Q1RK1 b kq -  ; bm O-O; id  sbd.036",
	"r1b1kb1r/pp2qppp/2pp4/8/4nP2/2N2N2/PPPP2PP/R1BQK2R w KQkq -  ; bm O-O; id  sbd.037",
	"r1b1qrk1/pp4b1/2pRn1pp/5p2/2n2B2/2N2NPP/PPQ1PPB1/5RK1 w - - ; bm Rd3; id  sbd.038",
	"r1b2rk1/1pqn1pp1/p2bpn1p/8/3P4/2NB1N2/PPQB1PPP/3R1RK1 w - - ; bm Rc1; id  sbd.039",
	"r1b2rk1/2qnbp1p/p1npp1p1/1p4PQ/4PP2/1NNBB3/PPP4P/R4RK1 w - - ; bm Qh6; id  sbd.040",
	"r1b2rk1/pp2ppbp/2n2np1/2q5/5B2/1BN1PN2/PP3PPP/2RQK2R w K -  ; bm O-O; id  sbd.041",
	"r1b2rk1/pp4pp/1q1Nppn1/2n4B/1P3P2/2B2RP1/P6P/R2Q3K b - - ; bm Na6; id  sbd.042",
	"r1b2rk1/ppp1qppp/1b1n4/8/B2n4/3NN3/PPPP1PPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.043",
	"r1b2rk1/ppq1bppp/2p1pn2/8/2NP4/2N1P3/PP2BPPP/2RQK2R w K -  ; bm O-O; id  sbd.044",
	"r1bq1rk1/1p1n1pp1/p4n1p/2bp4/8/2NBPN2/PPQB1PPP/R3K2R w KQ -  ; bm O-O; id  sbd.045",
	"r1bq1rk1/1p2ppbp/p2p1np1/6B1/2P1P3/2N5/PP1QBPPP/R3K2R w KQ -  ; bm O-O; id  sbd.046",
	"r1bq1rk1/1p3ppp/p1np4/3Np1b1/2B1P3/P7/1PP2PPP/RN1QK2R w KQ -  ; bm O-O; id  sbd.047",
	"r1bq1rk1/4bppp/ppnppn2/8/2P1P3/2N5/PPN1BPPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.048",
	"r1bq1rk1/pp1n1pbp/2n1p1p1/2ppP3/8/2PP1NP1/PP1N1PBP/R1BQ1RK1 w - - ; bm d4; id  sbd.049",
	"r1bq1rk1/pp1pppbp/2n2np1/8/4P3/1NN5/PPP1BPPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.050",
	"r1bq1rk1/pp2ppbp/2n2np1/2p3B1/4P3/2P2N2/PP1NBPPP/R2QK2R w KQ -  ; bm O-O; id  sbd.051",
	"r1bq1rk1/pp2ppbp/2n3p1/2p5/2BPP3/2P1B3/P3NPPP/R2QK2R w KQ -  ; bm O-O; id  sbd.052",
	"r1bq1rk1/pp3ppp/2n1pn2/2p5/1bBP4/2N1PN2/PP3PPP/R1BQ1RK1 w - - ; bm a3; id  sbd.053",
	"r1bq1rk1/pp3ppp/2n2n2/3p4/8/P1NB4/1PP2PPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.054",
	"r1bq1rk1/ppp1npb1/3p2pp/3Pp2n/1PP1P3/2N5/P2NBPPP/R1BQR1K1 b - - ; bm Nf4; id  sbd.055",
	"r1bq1rk1/ppp2ppp/2n1pn2/3p4/1bPP4/2NBPN2/PP3PPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.056",
	"r1bq1rk1/pppp1pbp/2n2np1/4p3/2P5/P1N2NP1/1P1PPPBP/R1BQK2R w KQ -  ; bm O-O; id  sbd.057",
	"r1bqk2r/2ppbppp/p1n2n2/1p2p3/4P3/1B3N2/PPPPQPPP/RNB2RK1 b kq -  ; bm O-O; id  sbd.058",
	"r1bqk2r/5ppp/p1np4/1p1Np1b1/4P3/2P5/PPN2PPP/R2QKB1R b KQkq -  ; bm O-O; id  sbd.059",
	"r1bqk2r/bp3ppp/p1n1pn2/3p4/1PP5/P1N1PN2/1B3PPP/R2QKB1R b KQkq -  ; bm O-O; id  sbd.060",
	"r1bqk2r/p2pppbp/2p3pn/2p5/4P3/2P2N2/PP1P1PPP/RNBQR1K1 b kq -  ; bm O-O; id  sbd.061",
	"r1bqk2r/pp2bppp/2n1p3/1B1n4/3P4/2N2N2/PP3PPP/R1BQ1RK1 b kq -  ; bm O-O; id  sbd.062",
	"r1bqk2r/pp2bppp/2n1p3/3n4/3P4/2NB1N2/PP3PPP/R1BQ1RK1 b kq -  ; bm O-O; id  sbd.063",
	"r1bqk2r/pp2ppbp/2np1np1/2p5/4P3/1B1P1N1P/PPP2PP1/RNBQK2R w KQkq -  ; bm O-O; id  sbd.064",
	"r1bqk2r/ppn1bppp/2n5/2p1p3/8/2NP1NP1/PP1BPPBP/R2Q1RK1 b kq -  ; bm O-O; id  sbd.065",
	"r1bqk2r/ppp1bppp/2n5/3p4/3P4/2PB1N2/P1P2PPP/R1BQ1RK1 b kq -  ; bm O-O; id  sbd.066",
	"r1bqk2r/ppp2ppp/2nb4/3np3/8/PP2P3/1BQP1PPP/RN2KBNR b KQkq -  ; bm O-O; id  sbd.067",
	"r1bqk2r/ppp2ppp/3b4/4p3/8/1PPP1N2/2PB1PPP/R2Q1RK1 b kq -  ; bm O-O; id  sbd.068",
	"r1bqk2r/pppp1ppp/5n2/4p3/Bb2P3/5Q2/PPPPNPPP/R1B1K2R b KQkq -  ; bm O-O; id  sbd.069",
	"r1bqkb1r/pp3ppp/2n5/2pp4/3Pn3/2N2N2/PPP1BPPP/R1BQK2R w KQkq -  ; bm O-O; id  sbd.070",
	"r1bqkb1r/pp3ppp/2npp3/3nP3/2BP4/5N2/PP3PPP/RNBQK2R w KQkq -  ; bm O-O; id  sbd.071",
	"r1bqkbnr/3p1ppp/p1p1p3/8/4P3/3B4/PPP2PPP/RNBQK2R w KQkq -  ; bm O-O; id  sbd.072",
	"r1bqkbnr/ppp2ppp/2n5/8/2BpP3/5N2/PP3PPP/RNBQK2R w KQkq -  ; bm O-O; id  sbd.073",
	"r1bqrbk1/1pp3pp/2n2p2/p2np3/8/PP1PPN2/1BQNBPPP/R3K2R w KQ -  ; bm O-O; id  sbd.074",
	"r1br2k1/1p2qppp/pN2pn2/P7/2pn4/4N1P1/1P2PPBP/R3QRK1 b - - ; bm Rb8; id  sbd.075",
	"r1q1k2r/1b1nbppp/pp1ppn2/8/2PQP3/1PN2NP1/PB3PBP/R2R2K1 b kq -  ; bm O-O; id  sbd.076",
	"r1q1k2r/pb1nbppp/1p2pn2/8/P1PNP3/2B3P1/2QN1PBP/R4RK1 b kq -  ; bm O-O; id  sbd.077",
	"r1r3k1/1bq2pbp/pp1pp1p1/2n5/P3PP2/R2B4/1PPBQ1PP/3N1R1K w - - ; bm Bc3; id  sbd.078",
	"r1rn2k1/pp1qppbp/6p1/3pP3/3P4/1P3N1P/PB1Q1PP1/R3R1K1 w - - ; bm Rac1; id  sbd.079",
	"r2q1rk1/1b1nbpp1/pp2pn1p/8/2BN3B/2N1P3/PP2QPPP/2R2RK1 w - - ; bm Rfd1; id  sbd.080",
	"r2q1rk1/1b3ppp/4pn2/1pP5/1b6/2NBPN2/1PQ2PPP/R3K2R w KQ -  ; bm O-O; id  sbd.081",
	"r2q1rk1/pb1nppbp/6p1/1p6/3PP3/3QBN1P/P3BPP1/R3K2R w KQ -  ; bm O-O; id  sbd.082",
	"r2q1rk1/pb2bppp/npp1pn2/3pN3/2PP4/1PB3P1/P2NPPBP/R2Q1RK1 w - - ; bm e4; id  sbd.083",
	"r2q1rk1/pppb1pbp/2np1np1/4p3/2P5/P1NPPNP1/1P3PBP/R1BQK2R w KQ -  ; bm O-O; id  sbd.084",
	"r2qk2r/1b1n1ppp/4pn2/p7/1pPP4/3BPN2/1B3PPP/R2QK2R w KQkq -  ; bm O-O; id  sbd.085",
	"r2qk2r/1b2bppp/p1n1pn2/1p6/1P6/P2BPN2/1B2QPPP/RN3RK1 b kq -  ; bm O-O; id  sbd.086",
	"r2qk2r/2p2ppp/p1n1b3/1pbpP3/4n3/1BP2N2/PP1N1PPP/R1BQ1RK1 b kq -  ; bm O-O; id  sbd.087",
	"r2qk2r/3n1ppp/p3p3/3nP3/3R4/5N2/1P1N1PPP/3QR1K1 b kq -  ; bm O-O; id  sbd.088",
	"r2qk2r/p1pn1ppp/b3pn2/3p4/Nb1P4/1P3NP1/P3PPBP/1RBQ1RK1 b kq -  ; bm O-O Qe7; id  sbd.089",
	"r2qk2r/ppp1bppp/2n2n2/8/2BP2b1/2N2N2/PP3PPP/R1BQR1K1 b kq -  ; bm O-O; id  sbd.090",
	"r2qkb1r/pb1n1p2/2p1p2p/4P1pn/PppP4/2N2NB1/1P2BPPP/R2Q1RK1 w kq -  ; bm Ne4; id  sbd.091",
	"r2qkb1r/pp2nppp/1np1p3/4Pb2/3P4/PB3N2/1P3PPP/RNBQ1RK1 b kq -  ; bm Ned5; id  sbd.092",
	"r2qkb1r/pp3ppp/2bppn2/8/2PQP3/2N2N2/PP3PPP/R1B1K2R w KQkq -  ; bm O-O; id  sbd.093",
	"r2qr1k1/p3bppp/1p2n3/3Q1N2/5P2/4B1P1/PP3R1P/R5K1 w - - ; bm Rd1; id  sbd.094",
	"r2r2k1/p1pnqpp1/1p2p2p/3b4/3P4/3BPN2/PP3PPP/2RQR1K1 b - - ; bm c5; id  sbd.095",
	"r2r2k1/pp1b1ppp/8/3p2P1/3N4/P3P3/1P3P1P/3RK2R b K -  ; bm Rac8; id  sbd.096",
	"r3k2r/1b1nb1p1/p1q1pn1p/1pp3N1/4PP2/2N5/PPB3PP/R1BQ1RK1 w kq -  ; bm Nf3; id  sbd.097",
	"r3k2r/1pqnnppp/p5b1/1PPp1p2/3P4/2N5/P2NB1PP/2RQ1RK1 b kq -  ; bm O-O; id  sbd.098",
	"r3k2r/p1q1nppp/1pn5/2P1p3/4P1Q1/P1P2P2/4N1PP/R1B2K1R b kq -  ; bm O-O; id  sbd.099",
	"r3k2r/pp2pp1p/6p1/2nP4/1R2PB2/4PK2/P5PP/5bNR w kq -  ; bm Ne2; id  sbd.100",
	"r3k2r/ppp1bppp/2n5/3n4/3PB3/8/PP3PPP/RNB1R1K1 b kq -  ; bm O-O-O; id  sbd.101",
	"r3kb1r/pp3ppp/4bn2/3p4/P7/4N1P1/1P2PPBP/R1B1K2R w KQkq -  ; bm O-O; id  sbd.102",
	"r3kbnr/1pp3pp/p1p2p2/8/3qP3/5Q1P/PP3PP1/RNB2RK1 w kq -  ; bm Rd1; id  sbd.103",
	"r3kr2/pppb1p2/2n3p1/3Bp2p/4P2N/2P5/PP3PPP/2KR3R b q -  ; bm O-O-O; id  sbd.104",
	"r3nrk1/pp2qpb1/3p1npp/2pPp3/2P1P2N/2N3Pb/PP1BBP1P/R2Q1RK1 w - - ; bm Re1; id  sbd.105",
	"r3r1k1/1pqn1pbp/p2p2p1/2nP2B1/P1P1P3/2NB3P/5PP1/R2QR1K1 w - - ; bm Rc1; id  sbd.106",
	"r3r1k1/pp1q1ppp/2p5/P2n1p2/1b1P4/1B2PP2/1PQ3PP/R1B2RK1 w - - ; bm e4; id  sbd.107",
	"r3r1k1/pp3ppp/2ppqn2/5R2/2P5/2PQP1P1/P2P2BP/5RK1 w - - ; bm Qd4; id  sbd.108",
	"r3rbk1/p2b1p2/5p1p/1q1p4/N7/6P1/PP1BPPBP/3Q1RK1 w - - ; bm Nc3; id  sbd.109",
	"r4r1k/pp1bq1b1/n2p2p1/2pPp1Np/2P4P/P1N1BP2/1P1Q2P1/2KR3R w - - ; bm Ne6; id  sbd.110",
	"r4rk1/1bqp1ppp/pp2pn2/4b3/P1P1P3/2N2BP1/1PQB1P1P/2R2RK1 w - - ; bm b3; id  sbd.111",
	"r4rk1/1q2bppp/p1bppn2/8/3BPP2/3B2Q1/1PP1N1PP/4RR1K w - - ; bm e5; id  sbd.112",
	"r4rk1/pp2qpp1/2pRb2p/4P3/2p5/2Q1PN2/PP3PPP/4K2R w K -  ; bm O-O; id  sbd.113",
	"r7/3rq1kp/2p1bpp1/p1Pnp3/2B4P/PP4P1/1B1RQP2/2R3K1 b - - ; bm Rad8; id  sbd.114",
	"r7/pp1bpp2/1n1p2pk/1B3P2/4P1P1/2N5/PPP5/1K5R b - - ; bm Kg5; id  sbd.115",
	"rn1q1rk1/p4pbp/bp1p1np1/2pP4/8/P1N2NP1/1PQ1PPBP/R1B1K2R w KQ -  ; bm O-O; id  sbd.116",
	"rn1q1rk1/pb3p2/1p5p/3n2P1/3p4/P4P2/1P1Q1BP1/R3KBNR b KQ -  ; bm Re8 + ; id  sbd.117",
	"rn1q1rk1/pp2bppp/1n2p1b1/8/2pPP3/1BN1BP2/PP2N1PP/R2Q1RK1 w - - ; bm Bc2; id  sbd.118",
	"rn1q1rk1/pp3ppp/4bn2/2bp4/5B2/2NBP1N1/PP3PPP/R2QK2R w KQ -  ; bm O-O; id  sbd.119",
	"rn1qkbnr/pp1b1ppp/8/1Bpp4/3P4/8/PPPNQPPP/R1B1K1NR b KQkq -  ; bm Qe7; id  sbd.120",
	"rn1qr1k1/pb3p2/1p5p/3n2P1/3p4/P4P2/1P1QNBP1/R3KB1R b KQ -  ; bm d3; id  sbd.121",
	"rn2kb1r/pp2nppp/1q2p3/3pP3/3P4/5N2/PP2NPPP/R1BQK2R w KQkq -  ; bm O-O; id  sbd.122",
	"rn3rk1/1bqp1ppp/p3pn2/8/Nb1NP3/4B3/PP2BPPP/R2Q1RK1 w - - ; bm Rc1; id  sbd.123",
	"rn3rk1/pbp1qppp/1p1ppn2/8/2PP4/P1Q2NP1/1P2PPBP/R1B1K2R w KQ -  ; bm O-O; id  sbd.124",
	"rnb1k2r/1pq2ppp/p2ppn2/2b5/3NPP2/2P2B2/PP4PP/RNBQ1R1K b kq -  ; bm O-O; id  sbd.125",
	"rnb2rk1/ppq1ppbp/6p1/2p5/3PP3/2P2N2/P3BPPP/1RBQK2R w K -  ; bm O-O; id  sbd.126",
	"rnbq1rk1/5ppp/p3pn2/1p6/2BP4/P1P2N2/5PPP/R1BQ1RK1 w - - ; bm Bd3; id  sbd.127",
	"rnbq1rk1/pp2ppbp/2pp1np1/8/P2PP3/2N2N2/1PP1BPPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.128",
	"rnbq1rk1/ppp1ppbp/6p1/8/8/2P2NP1/P2PPPBP/R1BQK2R w KQ -  ; bm O-O; id  sbd.129",
	"rnbqk1nr/pp3pbp/2ppp1p1/8/2BPP3/2N2Q2/PPP2PPP/R1B1K1NR w KQkq -  ; bm Nge2; id  sbd.130",
	"rnbqk2r/ppp2ppp/1b1p1n2/4p3/2B1P3/2PP1N2/PP1N1PPP/R1BQK2R b KQkq -  ; bm O-O; id  sbd.131",
	"rnbqk2r/pppp2pp/4pn2/5p2/1b1P4/2P2NP1/PP2PPBP/RNBQK2R b KQkq -  ; bm Be7; id  sbd.132",
	"rnbqr1k1/pp1p1ppp/5n2/3Pb3/1P6/P1N3P1/4NPBP/R1BQK2R w KQ -  ; bm O-O; id  sbd.133",
	"rnq1nrk1/pp3pbp/6p1/3p4/3P4/5N2/PP2BPPP/R1BQK2R w KQ -  ; bm O-O; id  sbd.134",
	"X"
};


char perft_epd[][MAXEPD_LENGTH] =
{
	"3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1; D6 1134888",
	"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1; D4 1274206",
	"8/8/8/8/k1p4R/8/3P4/3K4 w - - 0 1; D6 1134888",
	"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1; D6 1440467",
	"8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1; D6 1440467",
	"8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1; D6 1015133",
	"8/b2p2k1/8/2P5/8/4K3/8/8 b - - 0 1; D6 1015133",
	"5k2/8/8/8/8/8/8/4K2R w K - 0 1; D6 661072",
	"4k2r/8/8/8/8/8/8/5K2 b k - 0 1; D6 661072",
	"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1; D6 803711",
	"r3k3/8/8/8/8/8/8/3K4 b q - 0 1; D6 803711",
	// "# en passant capture checks opponent :",
	"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1; D6 1440467",
	"8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1; D6 1440467",
	// "# avoid illegal ep(thanks to Steve Maughan) ",
	"3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1; D6 1134888",
	"8/8/8/8/k1p4R/8/3P4/3K4 w - - 0 1; D6 1134888",
	// "# avoid illegal ep #2",
	"8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1; D6 1015133",
	"8/b2p2k1/8/2P5/8/4K3/8/8 b - - 0 1; D6 1015133",
	// "# short castling gives check :",
	"5k2/8/8/8/8/8/8/4K2R w K - 0 1; D6 661072",
	"4k2r/8/8/8/8/8/8/5K2 b k - 0 1; D6 661072",
	// "# long castling gives check :",
	"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1; D6 803711",
	"r3k3/8/8/8/8/8/8/3K4 b q - 0 1; D6 803711",
	// "# castling(including losing cr due to rook capture) :",
	"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1; D4 1274206",
	"r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1; D4 1274206",
	// "# castling prevented :",
	"r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1; D4 1720476",
	"r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1; D4 1720476",
	//  promote out of check :",
	"2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1; D6 3821001",
	"3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1; D6 3821001",
	// "# discovered check :",
	"8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1; D5 1004658",
	"5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1; D5 1004658",
	// "# promote to give check :",
	"4k3/1P6/8/8/8/8/K7/8 w - - 0 1; D6 217342",
	"8/k7/8/8/8/8/1p6/4K3 b - - 0 1; D6 217342",
	// "# underpromote to check :",
	"8/P1k5/K7/8/8/8/8/8 w - - 0 1; D6 92683",
	"8/8/8/8/8/k7/p1K5/8 b - - 0 1; D6 92683",
	// "# self stalemate :",
	"K1k5/8/P7/8/8/8/8/8 w - - 0 1; D6 2217",
	"8/8/8/8/8/p7/8/k1K5 b - - 0 1; D6 2217",
	//"# stalemate/checkmate:",
	"8/k1P5/8/1K6/8/8/8/8 w - - 0 1; D7 567584",
	"8/8/8/8/1k6/8/K1p5/8 b - - 0 1; D7 567584",
	//"# double check :",
	"8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1; D4 23527",
	//"# short castling impossible although the rook never moved away from its corner",
	"1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1; D5 1063513",
	"4k2r/8/8/7r/8/8/1B6/1K6 w k - 0 1; D5 1063513",
	//"# long castling impossible although the rook never moved away from its corner",
	"1k6/8/8/8/R7/1n6/8/R3K3 b Q - 0 1; D5 346695",
	"r3k3/8/1N6/r7/8/8/8/1K6 w q - 0 1; D5 346695",
	"8/8/8/8/8/K7/P7/k7 w - - 0 1; D1 3; D2 7; D3 43; D4 199; D5 1347; D6 6249",
	"8/8/8/8/8/7K/7P/7k w - - 0 1; D1 3; D2 7; D3 43; D4 199; D5 1347; D6 6249",
	"K7/p7/k7/8/8/8/8/8 w - - 0 1; D1 1; D2 3; D3 12; D4 80; D5 342; D6 2343",
	"7K/7p/7k/8/8/8/8/8 w - - 0 1; D1 1; D2 3; D3 12; D4 80; D5 342; D6 2343",
	"8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1; D1 7; D2 35; D3 210; D4 1091; D5 7028; D6 34834",
	"8/8/8/8/8/K7/P7/k7 b - - 0 1; D1 1; D2 3; D3 12; D4 80; D5 342; D6 2343",
	"8/8/8/8/8/7K/7P/7k b - - 0 1; D1 1; D2 3; D3 12; D4 80; D5 342; D6 2343",
	"K7/p7/k7/8/8/8/8/8 b - - 0 1; D1 3; D2 7; D3 43; D4 199; D5 1347; D6 6249",
	"7K/7p/7k/8/8/8/8/8 b - - 0 1; D1 3; D2 7; D3 43; D4 199; D5 1347; D6 6249",
	"8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1; D1 5; D2 35; D3 182; D4 1091; D5 5408; D6 34822",
	"8/8/8/8/8/4k3/4P3/4K3 w - - 0 1; D1 2; D2 8; D3 44; D4 282; D5 1814; D6 11848",
	"4k3/4p3/4K3/8/8/8/8/8 b - - 0 1; D1 2; D2 8; D3 44; D4 282; D5 1814; D6 11848",
	"8/8/7k/7p/7P/7K/8/8 w - - 0 1; D1 3; D2 9; D3 57; D4 360; D5 1969; D6 10724",
	"8/8/k7/p7/P7/K7/8/8 w - - 0 1; D1 3; D2 9; D3 57; D4 360; D5 1969; D6 10724",
#if 1
	"8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1; D1 5; D2 25; D3 180; D4 1294; D5 8296; D6 53138",
	"8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1; D1 8; D2 61; D3 483; D4 3213; D5 23599; D6 157093",
	"8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1; D1 8; D2 61; D3 411; D4 3213; D5 21637; D6 158065",
	"k7/8/3p4/8/3P4/8/8/7K w - - 0 1; D1 4; D2 15; D3 90; D4 534; D5 3450; D6 20960",
	"8/8/7k/7p/7P/7K/8/8 b - - 0 1; D1 3; D2 9; D3 57; D4 360; D5 1969; D6 10724",
	"8/8/k7/p7/P7/K7/8/8 b - - 0 1; D1 3; D2 9; D3 57; D4 360; D5 1969; D6 10724",
	"8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1; D1 5; D2 25; D3 180; D4 1294; D5 8296; D6 53138",
	"8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1; D1 8; D2 61; D3 411; D4 3213; D5 21637; D6 158065",
	"8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1; D1 8; D2 61; D3 483; D4 3213; D5 23599; D6 157093",
	"k7/8/3p4/8/3P4/8/8/7K b - - 0 1; D1 4; D2 15; D3 89; D4 537; D5 3309; D6 21104",
	"7k/3p4/8/8/3P4/8/8/K7 w - - 0 1; D1 4; D2 19; D3 117; D4 720; D5 4661; D6 32191",
	"7k/8/8/3p4/8/8/3P4/K7 w - - 0 1; D1 5; D2 19; D3 116; D4 716; D5 4786; D6 30980",
	"k7/8/8/7p/6P1/8/8/K7 w - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"k7/8/7p/8/8/6P1/8/K7 w - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"k7/8/8/6p1/7P/8/8/K7 w - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"k7/8/6p1/8/8/7P/8/K7 w - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"k7/8/8/3p4/4p3/8/8/7K w - - 0 1; D1 3; D2 15; D3 84; D4 573; D5 3013; D6 22886",
	"k7/8/3p4/8/8/4P3/8/7K w - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4271; D6 28662",
	"7k/3p4/8/8/3P4/8/8/K7 b - - 0 1; D1 5; D2 19; D3 117; D4 720; D5 5014; D6 32167",
	"7k/8/8/3p4/8/8/3P4/K7 b - - 0 1; D1 4; D2 19; D3 117; D4 712; D5 4658; D6 30749",
	"k7/8/8/7p/6P1/8/8/K7 b - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"k7/8/7p/8/8/6P1/8/K7 b - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"k7/8/8/6p1/7P/8/8/K7 b - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"k7/8/6p1/8/8/7P/8/K7 b - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"k7/8/8/3p4/4p3/8/8/7K b - - 0 1; D1 5; D2 15; D3 102; D4 569; D5 4337; D6 22579",
	"k7/8/3p4/8/8/4P3/8/7K b - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4271; D6 28662",
	"7k/8/8/p7/1P6/8/8/7K w - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"7k/8/p7/8/8/1P6/8/7K w - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"7k/8/8/1p6/P7/8/8/7K w - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"7k/8/1p6/8/8/P7/8/7K w - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"k7/7p/8/8/8/8/6P1/K7 w - - 0 1; D1 5; D2 25; D3 161; D4 1035; D5 7574; D6 55338",
	"k7/6p1/8/8/8/8/7P/K7 w - - 0 1; D1 5; D2 25; D3 161; D4 1035; D5 7574; D6 55338",
	"3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1; D1 7; D2 49; D3 378; D4 2902; D5 24122; D6 199002",
	"7k/8/8/p7/1P6/8/8/7K b - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"7k/8/p7/8/8/1P6/8/7K b - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"7k/8/8/1p6/P7/8/8/7K b - - 0 1; D1 5; D2 22; D3 139; D4 877; D5 6112; D6 41874",
	"7k/8/1p6/8/8/P7/8/7K b - - 0 1; D1 4; D2 16; D3 101; D4 637; D5 4354; D6 29679",
	"k7/7p/8/8/8/8/6P1/K7 b - - 0 1; D1 5; D2 25; D3 161; D4 1035; D5 7574; D6 55338",
	"k7/6p1/8/8/8/8/7P/K7 b - - 0 1; D1 5; D2 25; D3 161; D4 1035; D5 7574; D6 55338",
	"3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1; D1 7; D2 49; D3 378; D4 2902; D5 24122; D6 199002",
	"4k3/8/8/8/8/8/8/4K2R w K - 0 1; D1 15; D2 66; D3 1197; D4 7059; D5 133987; D6 764643",
	"4k3/8/8/8/8/8/8/R3K3 w Q - 0 1; D1 16; D2 71; D3 1287; D4 7626; D5 145232; D6 846648",
	"4k2r/8/8/8/8/8/8/4K3 w k - 0 1; D1 5; D2 75; D3 459; D4 8290; D5 47635; D6 899442",
	"r3k3/8/8/8/8/8/8/4K3 w q - 0 1; D1 5; D2 80; D3 493; D4 8897; D5 52710; D6 1001523",
	"4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1; D1 26; D2 112; D3 3189; D4 17945; D5 532933; D6 2788982",
	"r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1; D1 5; D2 130; D3 782; D4 22180; D5 118882; D6 3517770",
	"8/8/8/8/8/8/6k1/4K2R w K - 0 1; D1 12; D2 38; D3 564; D4 2219; D5 37735; D6 185867",
	"8/8/8/8/8/8/1k6/R3K3 w Q - 0 1; D1 15; D2 65; D3 1018; D4 4573; D5 80619; D6 413018",
	"4k2r/6K1/8/8/8/8/8/8 w k - 0 1; D1 3; D2 32; D3 134; D4 2073; D5 10485; D6 179869",
	"r3k3/1K6/8/8/8/8/8/8 w q - 0 1; D1 4; D2 49; D3 243; D4 3991; D5 20780; D6 367724",
	"4k3/8/8/8/8/8/8/4K2R b K - 0 1; D1 5; D2 75; D3 459; D4 8290; D5 47635; D6 899442",
	"4k3/8/8/8/8/8/8/R3K3 b Q - 0 1; D1 5; D2 80; D3 493; D4 8897; D5 52710; D6 1001523",
	"4k2r/8/8/8/8/8/8/4K3 b k - 0 1; D1 15; D2 66; D3 1197; D4 7059; D5 133987; D6 764643",
	"r3k3/8/8/8/8/8/8/4K3 b q - 0 1; D1 16; D2 71; D3 1287; D4 7626; D5 145232; D6 846648",
	"4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1; D1 5; D2 130; D3 782; D4 22180; D5 118882; D6 3517770",
	"r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1; D1 26; D2 112; D3 3189; D4 17945; D5 532933; D6 2788982",
	"8/8/8/8/8/8/6k1/4K2R b K - 0 1; D1 3; D2 32; D3 134; D4 2073; D5 10485; D6 179869",
	"8/8/8/8/8/8/1k6/R3K3 b Q - 0 1; D1 4; D2 49; D3 243; D4 3991; D5 20780; D6 367724",
	"4k2r/6K1/8/8/8/8/8/8 b k - 0 1; D1 12; D2 38; D3 564; D4 2219; D5 37735; D6 185867",
	"r3k3/1K6/8/8/8/8/8/8 b q - 0 1; D1 15; D2 65; D3 1018; D4 4573; D5 80619; D6 413018",
	"8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1; D1 14; D2 195; D3 2760; D4 38675; D5 570726; D6 8107539",
	"8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1; D1 11; D2 156; D3 1636; D4 20534; D5 223507; D6 2594412",
	"8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1; D1 19; D2 289; D3 4442; D4 73584; D5 1198299; D6 19870403",
	"K7/8/2n5/1n6/8/8/8/k6N w - - 0 1; D1 3; D2 51; D3 345; D4 5301; D5 38348; D6 588695",
	"k7/8/2N5/1N6/8/8/8/K6n w - - 0 1; D1 17; D2 54; D3 835; D4 5910; D5 92250; D6 688780",
	"8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1; D1 15; D2 193; D3 2816; D4 40039; D5 582642; D6 8503277",
	"8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1; D1 16; D2 180; D3 2290; D4 24640; D5 288141; D6 3147566",
	"8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1; D1 4; D2 68; D3 1118; D4 16199; D5 281190; D6 4405103",
	"K7/8/2n5/1n6/8/8/8/k6N b - - 0 1; D1 17; D2 54; D3 835; D4 5910; D5 92250; D6 688780",
	"k7/8/2N5/1N6/8/8/8/K6n b - - 0 1; D1 3; D2 51; D3 345; D4 5301; D5 38348; D6 588695",
	"B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1; D1 17; D2 278; D3 4607; D4 76778; D5 1320507; D6 22823890",
	"8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1; D1 21; D2 316; D3 5744; D4 93338; D5 1713368; D6 28861171",
	"k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1; D1 21; D2 144; D3 3242; D4 32955; D5 787524; D6 7881673",
	"K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1; D1 7; D2 143; D3 1416; D4 31787; D5 310862; D6 7382896",
	"B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1; D1 6; D2 106; D3 1829; D4 31151; D5 530585; D6 9250746",
	"8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1; D1 17; D2 309; D3 5133; D4 93603; D5 1591064; D6 29027891",
	"k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1; D1 7; D2 143; D3 1416; D4 31787; D5 310862; D6 7382896",
	"K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1; D1 21; D2 144; D3 3242; D4 32955; D5 787524; D6 7881673",
	"7k/RR6/8/8/8/8/rr6/7K w - - 0 1; D1 19; D2 275; D3 5300; D4 104342; D5 2161211; D6 44956585",
	"6kq/8/8/8/8/8/8/7K w - - 0 1; D1 2; D2 36; D3 143; D4 3637; D5 14893; D6 391507",
	"6KQ/8/8/8/8/8/8/7k b - - 0 1; D1 2; D2 36; D3 143; D4 3637; D5 14893; D6 391507",
	"K7/8/8/3Q4/4q3/8/8/7k w - - 0 1; D1 6; D2 35; D3 495; D4 8349; D5 166741; D6 3370175",
	"6qk/8/8/8/8/8/8/7K b - - 0 1; D1 22; D2 43; D3 1015; D4 4167; D5 105749; D6 419369",
	"6KQ/8/8/8/8/8/8/7k b - - 0 1; D1 2; D2 36; D3 143; D4 3637; D5 14893; D6 391507",
	"K7/8/8/3Q4/4q3/8/8/7k b - - 0 1; D1 6; D2 35; D3 495; D4 8349; D5 166741; D6 3370175",
	"8/Pk6/8/8/8/8/6Kp/8 w - - 0 1; D1 11; D2 97; D3 887; D4 8048; D5 90606; D6 1030499",
	"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1; D1 24; D2 421; D3 7421; D4 124608; D5 2193768; D6 37665329",
	"8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1; D1 18; D2 270; D3 4699; D4 79355; D5 1533145; D6 28859283",
	"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1; D1 24; D2 496; D3 9483; D4 182838; D5 3605103; D6 71179139",
	"8/Pk6/8/8/8/8/6Kp/8 b - - 0 1; D1 11; D2 97; D3 887; D4 8048; D5 90606; D6 1030499",
	"r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1; D1 26; D2 583; D3 14252; D4 334705; D5 8198901; D6 198328929",
	"r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1; D1 25; D2 560; D3 13592; D4 317324; D5 7710115; D6 185959088",
	"r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1; D1 25; D2 560; D3 13607; D4 320792; D5 7848606; D6 190755813",
	"1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1; D1 25; D2 567; D3 14095; D4 328965; D5 8153719; D6 195629489",
	"2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1; D1 25; D2 548; D3 13502; D4 312835; D5 7736373; D6 184411439",
	"r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1; D1 25; D2 547; D3 13579; D4 316214; D5 7878456; D6 189224276",
	"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1; D1 26; D2 568; D3 13744; D4 314346; D5 7594526; D6 179862938",
	"r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1; D1 25; D2 567; D3 14095; D4 328965; D5 8153719; D6 195629489",
	"r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1; D1 25; D2 548; D3 13502; D4 312835; D5 7736373; D6 184411439",
	"r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1; D1 25; D2 547; D3 13579; D4 316214; D5 7878456; D6 189224276",
	"1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1; D1 26; D2 583; D3 14252; D4 334705; D5 8198901; D6 198328929",
	"2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1; D1 25; D2 560; D3 13592; D4 317324; D5 7710115; D6 185959088",
	"r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1; D1 25; D2 560; D3 13607; D4 320792; D5 7848606; D6 190755813",
	"8/p7/8/1P6/K1k3p1/6P1/7P/8 w - - 0 1; D8 8103790",
	"r3k2r/p6p/8/B7/1pp1p3/3b4/P6P/R3K2R w KQkq - 0 1; D6 77054993",
	"8/5p2/8/2k3P1/p3K3/8/1P6/8 b - - 0 1; D8 64451405",
	"r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq - 0 1; D6 867640754",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; D1 20; D2 400; D3 8902; D4 197281; D5 4865609; D6 119060324",
	"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1; D1 24; D2 421; D3 7421; D4 124608; D5 2193768; D6 37665329",
	"8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1; D1 18; D2 270; D3 4699; D4 79355; D5 1533145; D6 28859283",
	"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1; D1 24; D2 496; D3 9483; D4 182838; D5 3605103; D6 71179139",
	"R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1; D1 36; D2 1027; D3 29215; D4 771461; D5 20506480; D6 525169084",
	"7k/RR6/8/8/8/8/rr6/7K b - - 0 1; D1 19; D2 275; D3 5300; D4 104342; D5 2161211; D6 44956585",
	"R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1; D1 36; D2 1027; D3 29227; D4 771368; D5 20521342; D6 524966748",
	"rnbqkbnr/pppp1pp1/8/3Pp2p/3Q4/8/PPP1PPPP/RNB1KBNR w KQkq e6 0 1; D1 44; D2 1307; D3 51845; D4 1549650; D5 59257777; D6 1801085024",
	"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1; D1 48; D2 2039; D3 97862; D4 4085603; D5 193690690; D6 8031647685",
#endif
	"X"
};

char mixed_epd[][MAXEPD_LENGTH] =
{
	"startpos",	// as per UCI standard, startpos string stands for the initboard FEN
	"8/p2p4/r7/1k6/8/pK5Q/P7/b7 w - - ; bm Qd3; id ERET 015 - Endspiel",
	"3kB3/5K2/7p/3p4/3pn3/4NN2/8/1b4B1 w - - ; bm Nf5; id ERET 019 - Endspiel",
	"8/3Pk1p1/1p2P1K1/1P1Bb3/7p/7P/6P1/8 w - - ; bm g4; id ERET 027 - Zugzwang",
	"8/7p/5P1k/1p5P/5p2/2p1p3/P1P1P1P1/1K3Nb1 w - - ; bm Ng3; id ERET 032 - Zugzwang",
	"r1k1n2n/8/pP6/5R2/8/1b1B4/4N3/1K5N w - - ; bm b7; id ERET 036 - Endspiel",
	"1k6/bPN2pp1/Pp2p3/p1p5/2pn4/3P4/PPR5/1K6 w - - ; bm Na8; id ERET 037 - Zugzwang",
	"8/6N1/3kNKp1/3p4/4P3/p7/P6b/8 w - - ; bm exd5; id ERET 038 - Endspiel",
	"r1b1k2r/pp3ppp/1qn1p3/2bn4/8/6P1/PPN1PPBP/RNBQ1RK1 w kq - ; bm a3; id ERET 039 - Entwicklung",
	"8/4nk2/1p3p2/1r1p2pp/1P1R1N1P/6P1/3KPP2/8 w - - ; bm Nd3; id ERET 046 - Endspiel",
	"2b1r3/5pkp/6p1/4P3/QppqPP2/5RPP/6BK/8 b - - ; bm c3; id ERET 054 - Endspiel",
	"r2qr1k1/pp1bbp2/n5p1/2pPp2p/8/P2PP1PP/1P2N1BK/R1BQ1R2 w - - ; bm d6; id ERET 057 - Abtausch",
	"rq6/5k2/p3pP1p/3p2p1/6PP/1PB1Q3/2P5/1K6 w - - ; bm Qd3; id ERET 059 - Endspiel",
	"1r3rk1/2qbppbp/3p1np1/nP1P2B1/2p2P2/2N1P2P/1P1NB1P1/R2Q1RK1 b - - ; bm Qb6; id ERET 065 - Zwischenzug",
	"8/2pN1k2/p4p1p/Pn1R4/3b4/6Pp/1P3K1P/8 w - - ; bm Ke1; id ERET 066 - Endspiel",
	"r4rk1/ppp2ppp/1nnb4/8/1P1P3q/PBN1B2P/4bPP1/R2QR1K1 w - - ; bm Qxe2; id ERET 073 - Mobilitaet",
	"6r1/6r1/2p1k1pp/p1pbP2q/Pp1p1PpP/1P1P2NR/1KPQ3R/8 b - - ; bm Qf5; id ERET 075 - Festung",
	"r3r1k1/1bq2ppp/p1p2n2/3ppPP1/4P3/1PbB4/PBP1Q2P/R4R1K w - - ; bm gxf6; id ERET 077 - Rochadeangriff",
	"r1bqr1k1/3n1ppp/p2p1b2/3N1PP1/1p1B1P2/1P6/1PP1Q2P/2KR2R1 w - - ; bm Qxe8; id ERET 079 - Damenopfer",
	"4r1k1/1r1np3/1pqp1ppB/p7/2b1P1PQ/2P2P2/P3B2R/3R2K1 w - - ; bm Bg7; id ERET 084 - Koenigsangriff",
	"8/2P1P3/b1B2p2/1pPRp3/2k3P1/P4pK1/nP3p1p/N7 w - - ; bm e8N; id ERET 089 - Unterverwandlung",
	"4K1k1/8/1p5p/1Pp3b1/8/1P3P2/P1B2P2/8 w - - ; bm f4; id ERET 090 - Endspiel",
	"2b1r3/r2ppN2/8/1p1p1k2/pP1P4/2P3R1/PP3PP1/2K5 w - - ; bm Nd6; id ERET 093 - Endspiel",
	"2k2Br1/p6b/Pq1r4/1p2p1b1/1Ppp2p1/Q1P3N1/5RPP/R3N1K1 b - - ; bm Rf6; id ERET 094 - Damenopfer",
	"8/8/4p1Pk/1rp1K1p1/4P1P1/1nP2Q2/p2b1P2/8 w - - ; bm Kf6; id ERET 096 - Endspiel",
	"2k5/p7/Pp1p1b2/1P1P1p2/2P2P1p/3K3P/5B2/8 w - - ; bm c5; id ERET 097 - Endspiel",
	"4k1br/1K1p1n1r/2p2pN1/P2p1N2/2P3pP/5B2/P2P4/8 w - - ; bm Kc8; id ERET 103 - Zugzwang",
	"1r1r2k1/2pq3p/4p3/2Q1Pp2/1PNn1R2/P5P1/5P1P/4R2K b - - ; bm Rb5; id ERET 107 - Verteidigung",



	"1q1r3k/3P1pp1/ppBR1n1p/4Q2P/P4P2/8/5PK1/8 w - - ; bm Rxf6; ",
	"1r1rb1k1/5ppp/4p3/1p1p3P/1q2P2Q/pN3P2/PPP4P/1K1R2R1 w - - ; bm Rxg7; ",
	"1r3rk1/3bbppp/1qn2P2/p2pP1P1/3P4/2PB1N2/6K1/qNBQ1R2 w - - ; bm Bxh7 + ; ",
	"1r3rk1/5p1p/p2pb1p1/3Np1P1/2pRPR2/qP2Q1N1/P1PK3P/8 w - - ; bm Nf5; ",
	"1r6/4k3/r2p2p1/2pR1p1p/2P1pP1P/pPK1P1P1/P7/1B6 b - - ; bm Rxb3; c1 9",
	"1rb1nrk1/1p1qp2p/p1np1p1Q/3NPp2/2P5/3B1N1P/P2B2P1/R6K w - - ; bm g4; ",
	"1rb1r1k1/2qn1pbp/pp1p2p1/2nP4/P1p1PP2/2N1BQNP/1PB3P1/R4R1K w - - ; bm e5; ",
	"1rb2r1k/3p1pp1/p3p2p/1p2P3/4q3/1N2B1R1/PPP1N1P1/2KR1B2 w - - ; bm Nf4; ",
	"1rb4k/p5np/3p1rp1/1ppB4/2N2P2/1P2R1P1/P1P4P/4R1K1 w - - ; bm Re8; ",
	"1rr1b1k1/1pq1bp2/p2p1np1/4p3/P2BP3/2NB2Q1/1PP3PP/4RR1K w - - ; bm Rxf6; ",
	"2R5/k1p1p1K1/P1p1Pp2/5P2/7P/pB6/2p5/2r5 w - - ; bm Rxc7; ",
	"2b1r3/r2ppN2/8/1p1p1k2/pP1P4/2P3R1/PP3PP1/2K5 w - - ; bm Nd6; ",
	"2b1rk2/5p2/p1P5/2p2P2/2p5/7B/P7/2KR4 w - - ; bm f6; id study by Smyslov",
	"2bq1b2/1p3k2/1r1p1p1p/pNpP1PpP/P1P3P1/6B1/1P5Q/2K1R3 w - - ; bm Re6; ",
	"2k2Br1/p6b/Pq1r4/1p2p1b1/1Ppp2p1/Q1P3N1/5RPP/R3N1K1 b - - ; bm Rf6; ",
	"2k5/1pP4p/3p3P/3p1N2/1P3P2/7K/5b2/8 w - - ; bm Kg2; c1 4",
	"2k5/2p5/1q1p4/pPpPp1pp/N1P1Pp2/P4PbP/KQ4P1/8 w - - ; am Nxb6 + ? ; ",
	"2k5/p7/Pp1p1b2/1P1P1p2/2P2P1p/3K3P/5B2/8 w - - ; bm c5; ",
	"2q2r1k/p1p1b1pp/br1p1n2/2p1N1BP/Q1P1P3/8/PP1N1PP1/2KR3R w - - ; bm Ng6 + ; ",
	"2r1r1k1/2P2bp1/p1Q2pq1/2R3b1/1P6/4p1Bp/4B2P/3R3K w - - ; bm Qf3; ",
	"2r2rk1/1b3ppp/pn2p3/2qpP3/1p1Q1PP1/3B2R1/PPP1N2P/1K1R4 w - - ; bm Bxh7 + ;",
	"2r2rk1/1bpR1p2/1pq1pQp1/p3P2p/P1PR3P/5N2/2P2PPK/8 w - - ; bm Kg3; ",
	"2r2rk1/2q2p1p/p1p1nPpQ/3p4/4p1B1/8/PPP3PP/R4R1K w - - ; bm Rf5; ",
	"2r4r/3n1pk1/pq1p1bp1/3B4/1p2P1N1/7P/PP1Q1PK1/3RR3 w - - ; bm Bxf7; ",
	"2r5/prkpR1p1/2p1ppK1/P1p1N1B1/P1P1P3/8/8/8 w - - ; bm Bf4; c1 9",
	"3B4/1r2p3/r2p1p2/bkp1P1p1/1p1P1PPp/p1P1K2P/PPB5/8 w - - ; bm Ba4 + ; ",
	"3Q4/3p4/P2p4/N2b4/8/4P3/5p1p/5Kbk w - - ; bm Qa8; ",
	"3k4/2pq3p/pp5R/4P3/P6P/2PN4/1PN3K1/8 w - - ; bm Nd4; ",
	"3k4/4pp2/n2n4/5BN1/3N4/b7/4K3/8 w - - ; bm Nxf7 + ; ",
	"3q4/4k3/1p1b1p1r/p2Q4/3B1p1p/7P/1P4P1/3R3K w - - ; bm b4; id arasan19.120",
	"3r2rk/5pp1/p2p1q1p/n1pPpP2/PpP1Q2P/1P3RR1/2B2PP1/6K1 w - - ; bm Rg5;",
	"4K1k1/8/1p5p/1Pp3b1/8/1P3P2/P1B2P2/8 w - - ; bm f4; ",
	"4R3/P7/2bB2p1/r3pk2/2p2p1P/2P2P1K/6P1/8 w - - ; bm Rxe5 + ; ",
	"4bbk1/3nq2r/pr2p3/Np1p1p1p/1PpP1Pp1/2P1P1P1/PQ1NB1PR/2K4R b - - ; bm e5; ",
	"4k1br/1K1p1n1r/2p2pN1/P2p1N2/2P3pP/5B2/P2P4/8 w - - ; bm Kc8; ",
	"4q1kr/p6p/1prQPppB/4n3/4P3/2P5/PP2B2P/R5K1 w - - ; bm Qxe5; ",
	"5kN1/3p1q2/5P2/5PP1/2pK4/1p1R4/1P6/8 w - - ; bm Rxb3; ",
	"5nr1/2Pp2pk/3Pp1p1/4P1P1/6P1/5K2/8/7n w - - ; bm c8N; ",
	"5rk1/2p2Rpq/p2p2n1/2pPpQPp/4P2P/3P2N1/PP6/6K1 w - - ; bm Qe6; ",
	"5rk1/pp3p1p/7b/2pR4/8/2P4P/P1PNr1P1/2K4R b - - ; bm Rxd2; ",
	"5rr1/1p1bk3/p3pp2/8/3PP1pp/1PN1P3/1P3RPP/5RK1 w - - ; bm e5; MD vs JL, Chess Thinkers Forum 2015",
	"6R1/8/2pB3k/2P4p/5p1q/5P2/4P1K1/8 w - - ; bm Bf8 + ; ",
	"6b1/5p2/1p3Bp1/1P3pP1/5P2/pp1B2K1/pr4P1/k7 w - - ; bm Kh2; ",
	"6br/1KNp1n1r/2p2p2/P1ppRP2/1kP3pP/3PBB2/PN1P4/8 w - - ; bm Bxc5 + ; ",
	"6k1/p7/8/4B3/Pp3p1P/qP5P/P1Rb2K1/8 w - - ; bm Kf3; ",
	"6k1/ppn2p2/8/2P2P1p/1P2K3/2N5/P7/8 w - - ; bm Nd5; ",
	"7k/3q1pp1/1p3r2/p1bP4/P1P2p2/1P2rNpP/2Q3P1/4RR1K b - - ; bm Rxf3; id arasan19.122",
	"7k/pp2Np1p/2p2P1N/5pP1/7K/P4n2/1q3Q2/5n2 w - - ; bm Kh5; ",
	"7q/b1p5/1p1Npkb1/pPP2ppP/P1P5/3B2P1/5P1R/K3R3 w - - ; bm hxg6; ",
	"8/1p1r2R1/p5p1/5bp1/2PB1k2/1PK5/P7/8 b - - ; bm Rxd4; ",
	"8/1p2b3/6k1/5p2/p2NbP1p/P1B1P1p1/1P5P/6K1 w - - ; bm h3;  ",
	"8/2N4r/1p3pkp/8/5K1p/2P4N/P3Bn2/8 w - - ; bm Bh5; ",
	"8/2p1q3/p3P3/2P4p/1PBP2kP/2N3P1/7K/8 w - - ; bm Be2; ",
	"8/3P3k/n2K3p/2p3n1/1b4N1/2p1p1P1/8/3B4 w - - ; bm Nf6 + ; study by Van Breukelen",
	"8/4nk2/1p3p2/1r1p2pp/1P1R1N1P/6P1/3KPP2/8 w - - ; bm Nd3; id arasan19.116",
	"8/4nk2/1p3p2/2rp2pp/1P1R1N1P/6P1/3KPP2/8 b - - ; am Rb5 ? ; ",
	"8/5p2/N1p3p1/3pP1P1/7P/2kP4/Pb6/7K w - - ; bm h5; ",
	"8/6k1/2p2qp1/1p1n1p2/r2P4/p1P3P1/3Q1PK1/R6R w - - ; bm c4; ",
	"8/6pp/1K6/N5P1/3N4/8/npn1P3/k7 w - - ; bm Nab3 + ; ",
	"8/8/4kpp1/3p1b2/p6P/2B5/6P1/6K1 b - - ; bm Bh3; ",
	"8/8/8/2pK3p/8/5N2/1P4pq/5Rbk w - - ; bm Rb1; ",
	"8/P5k1/P5p1/5pKp/4bP1P/6P1/5B2/8 w - - ; bm g4; ",
	"8/bp4Pk/4KB2/R6p/2pNp1P1/p7/3p1Pnq/2r5 w - - ; bm Nc6; ",
	"8/p4pk1/r1p1p1p1/P1PpRnPp/1P3P2/2P5/4RK2/8 w - - ; bm Rxf5; ",
	"N7/8/2KQ2rp/6k1/3p3p/2p4P/4PP2/5N2 w - - ; bm f4 + ; ",
	"b3r3/q2B2k1/3Q2p1/1p5p/3pP3/5P2/1p4PP/5RK1 b - - ; bm d3 + ; ",
	"k7/p1pR4/1p3p2/2P2rpB/PbP4p/2N1p2p/KP6/8 w - - ; bm Bf3; ",
	"kr1b4/p1p2B2/2p5/2p1p3/KpP1P3/1P4N1/8/7R w - - ; bm Rh6; ",
	"n1QBq1k1/5p1p/5KP1/p7/8/8/8/8 w - - ; bm Bc7; c1 9",
	"q1r5/k3ppp1/2R5/PP1KP2r/2p4Q/2p1p3/5pP1/3R4 w - - ; bm Qd4 + ; ",
	"q3nrk1/4bppp/3p4/r3nPP1/4P2P/NpQ1B3/1P4B1/1K1R3R b - - ; bm Nc7; ",
	"q7/b1p5/kp1p4/p2PN3/PPP5/1K6/8/5B2 w - - ; bm c5 + ; ",
	"r1b1k1r1/1p2np1p/p1n1pQp1/3p4/3NPP2/P2RB3/2PK2PP/q4B1R w q d6 ; bm Be2; ",
	"r1b1r1k1/p2q1ppp/np2n3/3p1NBQ/8/6PB/P4P1P/1R2R1K1 w - - ; bm Bf6; ",
	"r1b1r2k/pp1n1pp1/1qpb3p/8/3PB2B/5N2/PPQ2PPP/R3R1K1 w - - ; bm Bg6; c1 9",
	"r1b2rk1/1pq2ppp/p1p1p3/2n1P3/2N2P2/3B4/PPP3PP/R2Q1RK1 w - - ; bm Bxh7; ",
	"r1b2rk1/2q1bppp/p2pp3/2n3PQ/1p1BP3/1BN5/PPP2P1P/2KR2R1 w - - ; bm Bf6; ",
	"r1b3k1/ppp1q1pp/2n1p3/3pP2P/8/2NB2Q1/PPP3P1/2K2R2 w - - ; bm Bg6; Hector-So, 19th Sigeman & Co., Malmo 2011",
	"r1b3r1/5p1k/p1n2P1p/P1qpp1P1/1p1p4/3P2Q1/BPPB2P1/R4RK1 w - - ; bm Kf2; ",
	"r1bq1rk1/pp2b2p/2p1pppQ/8/1n1PN2P/3B1N2/PPP2PP1/2KR3R w - - ; bm Neg5; ",
	"r1bq1rk1/pp2bppp/1n2p3/3pP3/8/2RBBN2/PP2QPPP/2R3K1 w - - ; bm Bxh7 + ; ",
	"r1bqn1nk/ppp4p/3p4/3Pp1r1/2P1Pp2/2NB1P2/PP2N3/2K1Q1RR w - - ; bm Rxh7; ",
	"r1bqr1k1/3n1ppp/p2p1b2/3N1PP1/1p1B1P2/1P6/1PP1Q2P/2KR2R1 w - - ; bm Qxe8 + ; ",
	"r1q1rbk1/2pp1ppp/p1b5/1p1NP3/3Q4/3R2B1/PPP2PPP/5RK1 w - - ; bm Nf6; ",
	"r1q2rk1/ppp1bp2/6p1/3pNPn1/7p/2PP3P/P6B/1R1Q1RK1 w - - ; bm f6; ",
	"r2q1r2/1b2bpkp/n3p1p1/2ppP1P1/p6R/1PN1BQR1/NPP2P1P/4K3 w - - ; bm Qf6 + ; ",
	"r2q1r2/1b2bpkp/p3p1p1/2ppP1P1/7R/1PN1BQR1/1PP2P1P/4K3 w - - ; bm Qf6 + ;  ",
	"r2q1rk1/ppp2p2/3p1np1/4pNQ1/4P1pP/1PPP4/1P3P2/R3K1R1 w Q - ; bm Qh6; ",
	"r2qkb1r/3bpppp/p4nn1/3P4/1p1N4/1QN3B1/PP3PPP/R3KB1R w KQkq - ; bm Nc6; ",
	"r2qrbk1/1b3p2/3p2pB/1p1P1N2/1np1P3/5NRP/1P3PP1/1B1Qn1K1 w - - ; bm N3h4; ",
	"r3r1k1/pp1q1p2/2p2npb/PPPp4/3Pp3/2NNPpP1/1R1B1P1K/3Q3R b - - ; bm Kg7; ",
	"r3r2k/1pq2pp1/4b2p/3pP3/p1nB3P/P2B1RQ1/1PP3P1/3R3K w - - ; bm Rf6; ",
	"r3rbk1/ppq2ppp/2b1pB2/8/6Q1/1P1B3P/P1P2PP1/R2R2K1 w - - ; bm Bxh7 + ; ",
	"r4r1k/1p1n1ppp/1qb1p3/2b1P3/2B3Q1/p1NR4/PPP3PP/1KB1R3 w - - ; bm Rh3; ",
	"r4r1k/6qp/3R1bb1/pNp2p2/PnB1pP2/4B2P/1P3Q1K/6R1 w - - ; bm Rxf6; ",
	"r4rk1/1bq1bppp/p1npp3/2nB1NP1/1p2P3/2N1B3/PPP1QP1P/2KR2R1 w - - ; bm Qh5; ",
	"r4rk1/4bppp/bq2p3/npnpP3/p2B1P2/P1PB1N2/1P2QNPP/R4RK1 w - - ; bm Bxh7 + ; ",
	"r5k1/1b2bppp/1n2p3/1P6/2qPP3/r1p2N2/1R2QPPP/1BR1B1K1 b - - ; bm cxb2; ",
	"r5k1/6b1/2Nq4/2pP1p2/p1P1pPr1/Pp6/3R2PQ/1K3R2 b - - ; bm e3; c1 9",
	"r5k1/p1pb2bp/3p3r/P1pPp1p1/2B1Pq2/1R2QPp1/1P4PP/5RBK b - - ; bm Rxh2 + ; ",
	"r5rk/1bq1bpp1/1p1ppn1p/pP5P/2P1P3/P1N2P2/1B1Q1P2/1K1R1BR1 w - - ; bm Nd5; ",
	"r6k/N1Rb2bp/p2p1nr1/3Pp2q/1P2Pp1P/5N2/P3QBP1/4R1K1 b - - ; bm Bh3; id arasan19.158",
	"r6r/4k3/p2bP2p/2n2PP1/2BQn2P/1pP1p3/1q1BK3/3R3R b - - ; bm Qc2; ",
	"rn1qr1k1/1p2bppp/p3p3/3pP3/P2P1B2/2RB1Q1P/1P3PP1/R5K1 w - - ; bm Bxh7 + ; c1 7",
	"rn2kbnr/pp2ppp1/2p5/3pP1Bp/3P3P/3Q4/PqP2PP1/RN2K1NR w KQkq -  ; bm e6; ",
	"r3r1k1/1bppq2p/1p1n1bp1/3P1p2/p4N1P/P1PB1P2/1P1Q1BP1/1K1R3R w - - ; bm g4; ",
	"4rrk1/p1q1bpp1/b1n1p2p/1pppP1N1/n4B1P/3P2P1/PPP1QPBN/R3R1K1 w - - 0 1 ; bm Qh5; ",



	"1B1K1k2/4N2p/3pP1pP/6p1/p7/3N2b1/2r2p2/8 w - - ; bm Nc6; ",
	"1B1k4/3p4/1Kp5/PpP2p2/1PqP1P2/8/1P6/8 w - - ; bm a6; ",
	"1B1r4/rp2npkp/2b1pbp1/1qp5/nPN1R3/1P1P1QP1/2P2PBP/5R1K w - - ; bm Qxf6; ",
	"1k6/3p4/1B6/4Pp1p/1p5R/1p4p1/pP3n2/K6n w - - ; bm Rxh1;  eval = draw",
	"1nb3rr/2pp3p/p1k2P2/n1q1PN2/P1pR3N/b4PBp/1R2P3/1KQ5 w - - ; bm Rd6 + ; ",
	"1r1r1qk1/pn1p2p1/1pp1npBp/8/2PB2QP/4R1P1/P4PK1/3R4 w - - ; bm Bc3; id AlphaZero - Stockfish8",
	"1r2kb1r/p5p1/1np1p1N1/3pPnB1/1p1P2B1/qP6/2PQ1P1P/1K1R3R b k - ; bm Nc4; ",
	"1r3q1k/2N2P1p/6pQ/4p3/7p/n6P/3B4/7K w - - ; bm Qf4; ",
	"1r3r2/4bpkp/1qb1p1p1/3pP1P1/p1pP1Q2/PpP2N1R/1Pn1B2P/3RB2K w - - ; bm Qf6 + ; ",
	"1r3rk1/3bb1pp/1qn1p3/3pP3/3P1N2/2Q2N2/2P3PP/R1BR3K w - - ; bm h4; id AlphaZero - Stockfish8",
	"1r4k1/pb3p1p/r1p1p1p1/2p1P3/P1P2P2/1PB5/2K3PP/5R2 w - - ; bm a5; ",
	"1r4Q1/7p/2N1B2k/7P/3Pp1P1/K1N1P2R/PP2PR2/8 b - -; +- no repet",
	"1rb1qrk1/6pp/2n1pn2/3pN1P1/2pP1P2/b1p1B3/P3QN1P/1BR2RK1 w - - ; bm Rxc3; ",
	"2bk2r1/5q2/1p2pP1Q/pP1pP1P1/P2P4/8/2RK4/8 w - - ; am Rc6; ",
	"2bqrr1k/p5b1/1p1p2pp/nPpBp3/P1P1N2P/3PN1P1/R4P1K/3Q1R2 w - - ; bm Nc3; ",
	"2kr3r/1p3pb1/pBnqp2p/P2p2p1/8/2PB4/1P3PPP/R2Q1RK1 w - - ; bm b4; ",
	"2n3k1/p5p1/3p1pPP/1PpP1p2/2p2P2/1bP5/2R1P2r/R4K2 w - - ; bm h7; c0 eval +-",
	"2r1r1k1/1p4pp/pP1b4/2pP1p1b/2P1pP2/2B3PP/1P3KB1/R3R3 w - - ; bm g4; ",
	"2r2bk1/pNq3p1/2Pp3p/1p1Prp2/1P2p1P1/6Q1/P4P1P/2RR2K1 w - - ; bm Qxe5; ",
	"2r3k1/1r3p2/1qb1pPpR/3pP1N1/p1pP4/PpP5/1Pn1BB1P/3R3K w - - ; bm Rg1; ",
	"2r5/6pk/7p/8/8/7P/1rbR1RPK/8 b - -;  eval = draw no progress because of pin",
	"3r1r1k/pp5p/4b1pb/6q1/3P4/4p1BP/PP2Q1PK/3RRB2 b - - ; bm Qxg3 + ; ",
	"3r1rk1/pbq1bp1p/1n1Rp1p1/2p1P1N1/4N2P/1P3Q2/PB3PP1/K6R w - - ; bm h5; ",
	"3r2k1/p2prp1p/1p1Rn1pP/4P3/4q3/5RP1/P2Q1PK1/3B4 w - - ; bm Bb3; id AlphaZero - Stockfish8",
	"3r2kq/p2prp1p/1p4pP/2nR4/1Q6/1B3RP1/P4PK1/8 w - - ; bm Rxc5 ; Qh4; ",
	"3rqbk1/2p3p1/b6p/Q1p5/2N1PP2/1P1P2P1/2P4P/5R1K b - - ; bm Bc8; id Stockfish8 - AlphaZero",
	"3rr1k1/1b2nnpp/1p1q1p2/pP1p1P2/P1pP2P1/2N1P1QP/3N1RB1/2R3K1 w - - ; bm Nf1; id AlphaZero - Stockfish8",
	"4b1k1/P4pPp/1R3P1P/2r5/8/1P6/1K6/8 w - - ; bm a8 = B; ",
	"4B3/1Pb4p/8/4p1k1/3p1p2/5K2/2P5/8 b - - ; bm e4; ",
	"4b3/7p/4k3/B4pPP/5K2/P4P2/8/8 w - - ; bm g6; ",
	"4k2b/7P/4KP1P/8/8/pB6/P7/8 w - - ; bm f7; eval=mate",
	"4K2k/1pQp3p/1R4p1/1P4P1/8/3P3P/1q3r2/8 w - - ; bm Qd8; ",
	"4q2r/1b1kbp2/1p2p1p1/pP1pP1N1/P2P1PQP/3BK3/2R5/8 w - - ; bm Bxg6; id AlphaZero - Stockfish8",
	"4r1k1/2pbnrq1/1p1b1p2/2pPp1pp/p1P1P2P/P2QNNB1/1P3PP1/3RR2K b - - ; bm Nc8; g4; id Stockfish8 - AlphaZero",
	"4r1rk/p3qpp1/1pnp1n1p/5P2/P1PPP3/4Q2P/2BB2R1/6RK w - - ; bm Rxg7; ",
	"4r2r/pppkq1pp/2n1pn2/4p1B1/4N2Q/8/PPP3PP/4RRK1 w - - ; bm Nxf6; ",
	"4rb2/1p1q1ppk/p1n1p2p/2p1Pn2/3PNR2/2P2K2/P2QBP2/6R1 w - - ; bm Rxf5; ",
	"4rrk1/p1q1bpp1/b1n1p2p/1pppP1N1/n4B1P/3P2P1/PPP1QPBN/R3R1K1 w - - 0 1 ; bm Qh5; ",
	"5bk1/1b1rq2p/2p1p1p1/1pP1P3/1Pp1B2Q/2B3P1/5PK1/7R w - - ; bm Qg4; id AlphaZero ",
	"5k2/8/pppppppp/8/PPPPPPPP/8/8/5K2 w - - ; bm d5; ",
	"5N2/6N1/q2k3p/8/6p1/4K1PP/B5P1/8 w - - ; bm Nf5 + ; eval = draw by fortress ",
	"5nr1/2Pp2pk/3Pp1p1/4P1P1/6P1/5K2/8/7n w - - ; bm c8 = N; ",
	"5rk1/1ppb1pb1/rq3np1/1NpPp2p/p1P1P1PP/P2BNPQ1/1P6/1K1R2R1 w - - ; bm  Nf5; ",
	"5rk1/2p2Rpq/p2p2n1/2pPpQPp/4P2P/3P2N1/PP6/6K1 w - - ; bm Qe6; ",
	"5rk1/pp3p1p/7b/2pR4/8/2P4P/P1PNr1P1/2K4R b - - ; bm Rxd2; ",
	"6b1/5p2/1p3Bp1/1P3pP1/5P2/pp1B2K1/pr4P1/k7 w - - ; bm Kh2; ",
	"6k1/1bq1bpp1/p6p/2p1pP2/1rP1P1P1/2NQ4/2P4P/K2RR3 b - - ; bm Bd5; ",
	"6k1/8/1pK4p/bPp5/8/1P6/P1B2P2/8 w - - ; bm b4; ",
	"6k1/8/8/8/q5p1/3p2P1/3N1R2/6K1 b - -1 45; eval = draw by fortress",
	"6k1/p7/8/4B3/Pp3p1P/qP5P/P1Rb2K1/8 w - - ; bm Kf3; ",
	"6Q1/8/3p4/2pPk1p1/1p5P/1P4PK/2q5/8 b - - ; bm g4 + ; ",
	"6q1/PpB5/k7/P6K/6n1/1P4p1/5p2/1B3N1N w - - ; bm a8 = Q + a8 = R + ; ",
	"6r1/6r1/2p1k1pp/p1pbP2q/Pp1p1PpP/1P1P2NR/1KPQ3R/8 b - - ; bm Qf5; id deep.09 fortress",
	"7q/b1p5/1p1Npkb1/pPP2ppP/P1P5/3B2P1/5P1R/K3R3 w - - ; bm hxg6; c0 ; ",
	"8/1k4p1/3p1pPp/1P2pP1P/2n1P3/3K4/2B5/8 b - - ; bm d5; ",
	"8/1P6/8/1b1K4/6p1/5pP1/5P1B/7k w - - ; bm b8 = R; ",
	"8/2B2k2/6p1/5P1p/6p1/p1p3PP/6P1/1n2K2R w K - ; bm fxg6 + ; eval = draw",
	"8/2p2kb1/np1p2p1/pN1Pp1Pp/2P1P2P/1P6/P4BK1/8 w - - ; bm Nxc7; ",
	"8/3ppp1p/3P4/4N3/8/3n1pKp/8/7k w - - ; bm Nxd3; ",
	"8/4K2p/6pr/5pk1/1pB2R2/p4PP1/8/8 w - - ; bm Rg4 + ; ",
	"8/4r1pk/2B5/1P3P2/2PrPb1p/6qP/1R1p2P1/3R2QK b - - ; bm Ra7; ",
	"8/8/1P1k3p/3P2pP/4KpP1/8/8/8 w - -; eval = draw",
	"8/8/4b1p1/2Bp3p/5P1P/1pK1Pk2/8/8 b - - ; bm g5 d4; ",
	"8/8/5k2/8/2B4p/5RbP/5pP1/7K b - -; eval = draw",
	"8/8/8/1B6/6p1/8/4KPpp/3N2kr w - - ; bm Kd3; ",
	"8/bB1p1p1k/Pr2nPpn/4P3/1PR3P1/2B4p/4K2P/8 w - - ; bm g5; ",
	"8/bp4Pk/4KB2/R6p/2pNp1P1/p7/3p1Pnq/2r5 w - - ; bm Nc6; ",
	"8/p1K5/6Bk/PP5P/8/8/5b2/8 w - - 0 1; eval = draw",
	"8/p3N3/1n3N2/2n3pP/pkP5/8/3P4/K7 b - - ; bm Nxc4;  eval = draw",
	"8/p5p1/1pP3p1/p5p1/k3p3/4p3/K7/8 w - - ; bm c7; eval = mate in 35",
	"8/p5pq/8/p2N3p/k2P3P/8/KP3PB1/8 w - - ; bm Be4; ",
	"8/p7/5k2/pPp1p3/2P3KP/8/8/3Bb3 b - - ; bm Bxh4; ",
	"8/pp3p1k/6pp/7P/P5P1/3K4/1PP5/8 b - a3; +-(SF pawn EG bug); ",
	"k7/4r1p1/2p5/5P2/2P2N1P/8/K6P/8 b - - ; bm Re4; ",
	"k7/p1pR4/1p3p2/2P2rpB/PbP4p/2N1p2p/KP6/8 w - - ; bm Bf3 + ; ",
	"kB5K/2P4B/P4rP1/4r3/2N1n3/1RP4N/3b4/6RQ b - -; eval =+- no perpet",
	"nk1b4/2rPp3/4Q1p1/1K2Br2/5P2/8/1P6/8 w - - ; bm Ka6;  ",
	"q7/7R/k7/1p6/1p6/1P2B3/7K/8 w - - ; bm Bd4; eval + -",
	"q7/8/2p5/B2p2pp/5pp1/2N3k1/6P1/7K w - - ; bm Ne4 + ; eval = draw",
	"qrb5/b1Pp3p/1pkB4/1p2P3/1P1KPp2/R4P1p/P3pP1P/4N1n1 w - - ; bm Rc3 + ; eval = mate",
	"r1b1k2r/1p1pppb1/p5pp/3P4/q2p1B2/3P1Q2/PPP2PPP/R3R1K1 w kq - ; bm Rxe7 + ; ",
	"r1b1k3/1p1p1pp1/1p1P3p/pP6/P7/8/5PPP/4R1K1 b q - ; eval +/-",
	"r1b1qr1k/2p3pp/4p3/1pb1PpN1/pn3N1P/6B1/PPP1QPP1/2KR3R w - - ; bm Rd8; ",
	"r1b2rk1/1pq1nppp/pbn1p3/8/3N4/3BBN2/PPP1QPPP/3R1RK1 w - - ; bm Bxh7 + ; ",
	"r1b2rk1/2q1bppp/pp2p3/2npP3/1n3P1P/2NBBN2/PPPQ2P1/1K1R3R w - - ; bm Bxh7; ",
	"r1b2rk1/pp4pp/2n1p1p1/3pP1B1/q1pP1PQ1/P1P5/2P1N1PP/R4RK1 w - - ; bm f5; ",
	"r1b3k1/pppp2pp/4n1q1/5n2/8/2N1B3/PPP3QP/3R1RK1 w - - ; bm Rxf5; ",
	"r1b3k1/ppppn1pp/4n1q1/8/5P2/2N1B3/PPP3QP/3R1RK1 w - - ; bm f5; ",
	"r1b3r1/5p1k/p1n2P1p/P1qpp1P1/1p1p4/3P2Q1/BPPB2P1/R4RK1 w - - ; bm Kf2; ",
	"r1b4r/p4pk1/P3p1p1/1pBpPnPp/2pP1K2/2P3P1/4BP2/R6R w - - ; bm g4; ",
	"r1bq1rk1/1p1nbppp/p3p3/2ppP3/5B1P/2PBP3/PP1N1PP1/R2QK2R w - - ; bm Bxh7 + ; ",
	"r1bq1rk1/1p2b2p/2p1pppQ/p7/1n1PN2P/3B1N2/PPPR1PP1/2K4R w - - ; bm Neg5; ",
	"r1br2k1/1p4p1/p1nBp1n1/q5PB/4Np2/2P1Q3/3R4/4K1R1 w - - ; bm Nf6 + ; ",
	"r1n2N1k/2n2K1p/3pp3/5Pp1/b5R1/8/1PPP4/8 w - - ; bm Ng6; id ERET ",
	"r1q1rbk1/2pp1ppp/p1b5/3NP3/3Q4/1P1R2B1/P4PPP/R5K1 w - - ; bm Nf6 + ; ",
	"r1r3k1/p3bppp/2bp3Q/q2pP1P1/1p1BP3/8/PPP1B2P/2KR2R1 w - - ; bm e6; ",
	"r2br1k1/1b3q1p/pp1p1ppB/3Nn3/n1PNP2R/4Q3/1P2B1PP/5R1K w - - ; bm Nf5; ",
	"r2q3r/1p1bbQ2/4p1Bk/3pP3/1n1P1P1p/pP6/Pn4PP/R1B1R1K1 w - - ; bm g4; ",
	"r2qrbk1/1bpn1p1p/p4np1/1p1pp3/3PP3/1PP2NNP/P1B2PP1/R1BQR1K1 w - - ; bm Bg5; ",
	"r2r2k1/pqp1bppp/1pn1pn2/8/2NP1B2/2N1P1P1/PPQ2P1P/R4RK1 b - - ; bm Rxd4; ",
	"r3r1k1/1bppq2p/1p1n1bp1/3P1p2/p4N1P/P1PB1P2/1P1Q1BP1/1K1R3R w - - ; bm g4; ",
	"r3r1k1/pp1q1p2/2p2npb/PPPp4/3Pp3/2NNPpP1/1R1B1P1K/3Q3R b - - ; bm Kg7; ",
	"r4r1k/ppq4p/2n1bppP/2P1p3/2B2n2/1N3Q2/PBP2PP1/3RR1K1 w - - ; bm Qxf4; ",
	"r4rk1/pb3pp1/1p1qpR2/2ppN3/3P4/2P4P/PP1pQ1P1/3R2K1 w - - ; bm Qxd2 Ng4; ",
	"r4rk1/ppp3pp/3b4/7q/1PPp2b1/P2P1Nn1/3NPRB1/R1B1Q1K1 b - b3 ; bm Rae8; ",
	"r5k1/p1pb2bp/3p3r/P1pPp1p1/2B1Pq2/1R2QPp1/1P4PP/5RBK b - - ; bm Rxh2 + ; ",
	"r7/1QP1N1Kp/pn2P1b1/Nnk2p2/7p/p4BP1/1rPPp2q/8 w - - ; bm Nd5; eval = mate",
	"r7/1QpnN1Kp/p3P1b1/Nn1P1p2/3k3p/p4BP1/1rPPp2q/8 w - - ; bm d6; ",
	"rn1q1rk1/2pbb3/pn2p3/1p1pPpp1/3P4/1PNBBN2/P1P1Q1PP/R4R1K w - - ; bm Nxg5; ",
	"rn2r1q1/p5k1/2p2bp1/1p4p1/2P5/4B1PQ/5PK1/3R3R w - - ; bm Bd4; ",
	"rn3r1k/pn1p1ppq/bpp4p/7P/4N1Q1/6P1/PP3PB1/R1B1R1K1 w - - ; bm Bg5; ",
	"rn3rk1/p2q1pp1/4p2p/2ppP1NQ/P7/2P1P3/2P3PP/R4RK1 w - - ; bm Rf6; ",
	"rn3rk1/pn1p1ppq/bpp4p/7P/4N3/6P1/PP3PB1/R1BQR1K1 w - - ; bm Qg4; id AlphaZero - Stockfish8",
	"rn3rq1/p5k1/2p2bp1/1p4p1/8/2P1B1PQ/5PK1/3R3R w - - ; bm c4; id AlphaZero - Stockfish8",
	"rnb2rk1/1pq1bppp/p3p3/2npP1P1/3N1P2/2NBB3/PPP4P/R2QK2R w KQ - ; bm Bxh7 + ; ",
	"rnbqk2r/p4p2/2p1p3/1p2N1pp/PbpPn2P/2N1P1B1/1P3PP1/R2QKB1R w KQkq - - ; bm Qf3; ",


	"2rr2k1/1bq1bp2/pn2p1p1/1pnpP2p/3N1P1P/PPP3P1/3NQBB1/R1R3K1 w - - ; bm g4;  ",
	"3r1rk1/pbq1bp1p/1n1Rp1p1/2p1P1N1/4N2P/1P3Q2/PB3PP1/K6R w - - ; bm h5;  ",
	"7k/4r1p1/1p2P2p/P1p4q/1r5P/4Q1PB/4R2K/8 w - - ; bm a6; Komodo 1589.00 64-bit 8CPU-Stockfish 020316 64-bit 8CPU, G. Banks 2016",
	"1r2kb1r/p5p1/1np1p1N1/3pPnB1/1p1P2B1/qP6/2PQ1P1P/1K1R3R b k - ; bm Nc4; Van Foreest-L'Ami, Dutch Chmp., Amsterdam 2017",
	"8/4r1pk/2B5/1P3P2/2PrPb1p/6qP/1R1p2P1/3R2QK b - - ; bm Ra7; Rybka forum, 2017",
	"8/6bk/8/p1pp1R1P/r7/2PN4/1P4KP/8 w - - ; bm Nxc5; Morozov-Kozlov, RCCA 20, fin (RUS) ICCF 2016",
	"r5rk/1n1b2qp/3p1b2/p1nP1p2/1pPNpP2/1P2B1P1/1N2B1RP/1R1Q3K b - - ; bm Nxb3; Stockfish dev forum, 2017",
	"r2q1rk1/ppp2p2/3p1np1/4pNQ1/4P1pP/1PPP4/1P3P2/R3K1R1 w Q - ; bm Qh6; McBrain test, posted by Michael B",
	"8/5pk1/p4npp/1pPN4/1P2p3/1P4PP/5P2/5K2 w - - ; bm Nxf6; McBrain test position, posted by Michael B",
	"5nk1/r4p1p/2r1p1p1/3nB1PP/p2PBP2/R7/1P6/K6R w - - ; bm h6; Stockfish 260218 64 BMI2-Komodo  ",
	"2bk2r1/5q2/1p2pP1Q/pP1pP1P1/P2P4/8/2RK4/8 w - - ; bm Kc1; ",
	"X"
};



char* fetch_suite_ptr(int n_suite)
{
	char* suiteptr = NULL;

	switch (n_suite)
	{
	case 1:
		suiteptr = &mixed_epd[0][0];
		printf("mixed epd\n");
		break;
	case 2:
		suiteptr = &nolot[0][0];
		printf("nolot\n");
		break;
	case 3:
		suiteptr = &kaufman[0][0];
		printf("kaufman\n");
		break;
	case 4:
		suiteptr = &rybkaforum[0][0];
		printf("rybka forum\n");
		break;
	case 5:
		suiteptr = &silentbutdeadly[0][0];
		printf("silent but deadly\n");
		break;
	case 6:
		suiteptr = &bratkokopec[0][0];
		printf("bratko kopec\n");
		break;
	case 7:
		suiteptr = &lct2[0][0];
		printf("lct2\n");
		break;
	case 8:
		// crash causers etc
		suiteptr = &grief_causers[0][0];
		printf("grief causers\n");
		break;
	case 9:
		suiteptr = &fortresses[0][0];
		printf("fortresses\n");
		break;
	case 10:
		suiteptr = &destruction_test[0][0];
		printf("destructions tests\n");
		break;
	}
	return suiteptr;
}
//

//
//
char* GetEPDString(char* ptr, int stringno)
{
	// bodgy, but this should work since all strings are stored as MAXEPD_LENGTH length
	ptr += stringno * MAXEPD_LENGTH;
	if (*ptr == '\0') return 0;
	return ptr;
}



//
int ep_string_to_square(const char *str)
{
	if ((strlen(str) == 0) || (strlen(str) > 2)) return 0;
	if (str[0] == '-') return 0;
	if ((str[0] < 'a') || (str[0] > 'h')) return 0;
	if ((str[1] < '1') || (str[1] > '8')) return 0;
	return (rank_file_to_square(str[1] - '1', str[0] - 'a'));
}
//
void set_bb_struct_element(GAMEDATA* gd, char ch, int rank, int file)
{
	int index;
	index = (rank << 3) + file;
	switch (ch)
	{
	case 'p':
		gd->pawns |= (ONE << (index));
		gd->pieces[1] |= (ONE << (index));
		break;
	case 'P':
		gd->pawns |= (ONE << (index));
		gd->pieces[0] |= (ONE << (index));
		break;
	case 'n':
		gd->nites |= (ONE << (index));
		gd->pieces[1] |= (ONE << (index));
		break;
	case 'N':
		gd->nites |= (ONE << (index));
		gd->pieces[0] |= (ONE << (index));
		break;
	case 'b':
		gd->diagonals |= (ONE << (index));
		gd->pieces[1] |= (ONE << (index));
		break;
	case 'B':
		gd->diagonals |= (ONE << (index));
		gd->pieces[0] |= (ONE << (index));
		break;
	case 'r':
		gd->manhattans |= (ONE << (index));
		gd->pieces[1] |= (ONE << (index));
		break;
	case 'R':
		gd->manhattans |= (ONE << (index));
		gd->pieces[0] |= (ONE << (index));
		break;
	case 'q':
		gd->diagonals |= (ONE << (index));
		gd->manhattans |= (ONE << (index));
		gd->pieces[1] |= (ONE << (index));
		break;
	case 'Q':
		gd->diagonals |= (ONE << (index));
		gd->manhattans |= (ONE << (index));
		gd->pieces[0] |= (ONE << (index));
		break;
	case 'k':
		gd->king_sq[1] = index;
		gd->pieces[1] |= (ONE << (index));
		break;
	case 'K':
		gd->king_sq[0] = index;
		gd->pieces[0] |= (ONE << (index));
		break;
	}
	return;
}


//
// decodes FEN as 6-part string
// highly tolerant of errors, enforces rules of normal chess
// returns gamedata strucure, ptr to end of FEN, 
//  else return 0 (NB and a smashed up board data structure)
// 
char* LoadFEN(GAMEDATA* gd, char* fen)
{
	// first initialise our board structure to zero ...
	gd->pieces[0] = 0;
	gd->pieces[1] = 0;
	gd->pawns = 0;	
	gd->nites = 0;	
	gd->diagonals= 0;	
	gd->manhattans = 0;
	gd->king_sq[0] = -1;
	gd->king_sq[1] = -1;

	// initialise to default values
	gd->castlestatus = 0;
	gd->epsquare = 0;
	gd->moveno = 1;
	gd->rule50 = 0;
	gd->color = 0;

	// transfer EPD to temp buffer
	//char temp_epd[MAXEPD_LENGTH];
	//for (int i = 0; i < MAXEPD_LENGTH; i++)
	//{
	//	temp_epd[i] = *fen++;
	//}
	bool startpos_flag = FALSE;
	// decode FEN string
	char* rest_of_epd_str = fen;
	char* token_x = strtok_s(fen, " ", &rest_of_epd_str);
	char* token;
	// special handler of string "startpos" as FEN, as per UCI position command
	if (strcmp(token_x, "startpos") == 0)
	{	// caller sent us "startpos" instead of a FEN string
		// reconstruct the FEN string and tell token
		char startpos[128] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
		//strcpy(rest_of_epd_str, &startpos[strlen(startpos)]);
		//rest_of_epd_str = startpos;
		//token = strtok_s(startpos, " ", &rest_of_epd_str);
		token = startpos;
		startpos_flag = TRUE;
	}
	else
	{
		token = token_x;
	}
	// piece-squares
	char ch;
	int file = 0, rank = 7;
	while ((ch = *token++))
	{
		if ('1' <= ch && ch <= '8')
		{
			file += ch - '0';
			if (file > 8)
			{
				return 0;
			}
		}
		else
		{
			if (ch == '/')
			{
				rank--;
				if (rank < 0)
				{
					return 0;
				}
				if (file != 8)
				{
					return 0;
				}
				file = 0;
			}
			else
			{
				if (!strchr("nbrqkpNBRQKP", ch))
				{
					return 0;
				}
				if ((rank < 0) || (file > 7))
				{
					return 0;
				}
				set_bb_struct_element(gd, ch, rank, file);
				file++;
			}
		}
	}

	// pawns on back ranks are absolutely illegal ...
	if (gd->pawns & (H1 | H8))
	{
		return 0;
	}

	if (startpos_flag)
	{
		// do "w KQkq - 0 1"
		gd->castlestatus = W_SHORTCASTLE | W_LONGCASTLE | B_LONGCASTLE | B_SHORTCASTLE;
		gd->epsquare = 0;
		gd->moveno = 1;
		gd->rule50 = 0;
		gd->color = 0;
	}
	else
	{
		// side to move, we don't tolerate anything other than single char 'w' or 'b'
		token = strtok_s(NULL, " ", &rest_of_epd_str);
		if (token == NULL)
		{	// we must have a colour field
			return 0;
		}
		if (strlen(token) != 1)
		{
			return 0;
		}
		if (token[0] == 'w')
		{
			gd->color = 0;
		}
		else
		{
			if (token[0] == 'b')
			{
				gd->color = 1;
			}
			else
			{
				return 0;
			}
		}

		// castling rights
		// tolerates errors by overrule and setting flags sensibly
		token = strtok_s(NULL, " ", &rest_of_epd_str);

		if (token != NULL)
		{	// presumably no castling rights, we are being very tolerant here
			int i = strlen(token);
			if (i > 4)
			{
				return 0;
			}
			while ((ch = *token++))
			{
				if (!strchr("KQkq", ch))
				{	// exit on '-' or any other junk (very tolerant of KQkq status field errors)
					break;
				}
				if (ch == 'K') gd->castlestatus |= W_SHORTCASTLE;
				if (ch == 'Q') gd->castlestatus |= W_LONGCASTLE;
				if (ch == 'k') gd->castlestatus |= B_SHORTCASTLE;
				if (ch == 'q') gd->castlestatus |= B_LONGCASTLE;
				i--;
				if (i == 0) break;
			}

			// en passent and rule50 (we are tolerant of these fields not even existing, let alone being trash)
			token = strtok_s(NULL, " ", &rest_of_epd_str);
			if (token != NULL)
			{
				if (*token != NULL)
				{	// if token available, get epsquare
					gd->epsquare = ep_string_to_square(token);
					token = strtok_s(NULL, " ", &rest_of_epd_str);
					if (token != NULL)
					{
						if (*token != NULL)
						{
							// atoi should return an integer, or 0 if the string is not a number
							gd->rule50 = atoi(token);
							// get the final movenum out of the way, if it exists at all
							token = strtok_s(NULL, " ", &rest_of_epd_str);
							if (token != NULL)
							{
								if (*token != NULL)
								{
									// atoi should return an integer, or 0 if the string is not a number
									gd->moveno = atoi(token);
								}
							}
						}
					}
				}
			}
		}
		// we're done with the EPD string now
		// ptr to the end of it gets returned for whatever else you might want to do with it

		// test piece counts
		// we are pretty much exhaustive here, extreme pedants may want to add more tests though
		u64 all_w = gd->pieces[0];
		u64 all_b = gd->pieces[1];
		if (popcount64(all_w) > 16) return 0;
		if (popcount64(all_b) > 16) return 0;

		if (gd->king_sq[0] < 0) return 0;
		if (gd->king_sq[1] < 0) return 0;

		u64 kings = (ONE << gd->king_sq[0]) | (ONE << gd->king_sq[1]);

		if (popcount64(kings) != 2) return 0;

		if ((gd->pieces[0] | gd->pieces[1]) != (kings | gd->pawns | gd->nites | gd->diagonals | gd->manhattans)) return 0;
		
		int n_wp = popcount64(gd->pawns & gd->pieces[0]);
		int n_bp = popcount64(gd->pawns & gd->pieces[1]);
		if ((n_wp > 8) || (n_bp > 8)) return 0;

		u64 queens = gd->diagonals & gd->manhattans;
		u64 bishops = gd->diagonals & ~queens;
		u64 rooks = gd->manhattans & ~queens;
		int n_wn = popcount64(gd->nites & gd->pieces[0]);
		int n_bn = popcount64(gd->nites & gd->pieces[1]);
		int n_wb = popcount64(bishops & gd->pieces[0]);
		int n_bb = popcount64(bishops & gd->pieces[1]);
		int n_wr = popcount64(rooks & gd->pieces[0]);
		int n_br = popcount64(rooks & gd->pieces[1]);
		int n_wq = popcount64(queens & gd->pieces[0]);
		int n_bq = popcount64(queens & gd->pieces[1]);

		// white piece counts
		if (n_wn > (2 + (8 - n_wp))) return 0;
		if (n_wb > (2 + (8 - n_wp))) return 0;
		if (n_wr > (2 + (8 - n_wp))) return 0;
		if (n_wq > (1 + (8 - n_wp))) return 0;
		// again for black
		if (n_bn > (2 + (8 - n_bp))) return 0;
		if (n_bb > (2 + (8 - n_bp))) return 0;
		if (n_br > (2 + (8 - n_bp))) return 0;
		if (n_bq > (1 + (8 - n_bp))) return 0;

		// total promotions
		int w_promo = 0;
		w_promo += (n_wn > 2) ? n_wn - 2 : 0;
		w_promo += (n_wb > 2) ? n_wb - 2 : 0;
		w_promo += (n_wr > 2) ? n_wr - 2 : 0;
		w_promo += (n_wq > 1) ? n_wq - 1 : 0;

		int b_promo = 0;
		b_promo += (n_bn > 2) ? n_bn - 2 : 0;
		b_promo += (n_bb > 2) ? n_bb - 2 : 0;
		b_promo += (n_br > 2) ? n_br - 2 : 0;
		b_promo += (n_bq > 1) ? n_bq - 1 : 0;

		if ((w_promo + n_wp) > 8) return 0;
		if ((b_promo + n_bp) > 8) return 0;

		// test validity rule50 (clear on error)
		if ((gd->rule50 >= NORMALRULE50) || (gd->rule50 < 0)) gd->rule50 = 0;
		// moveno must be >= rule50 and minimum 1
		gd->moveno = maxint(gd->rule50, gd->moveno);
		gd->moveno = maxint(1, gd->moveno);

		// there's an argument for an engine to set these fields to 0 1 and override anything in the EPD
		// obvs each programmer can decide for himself, for various reasons the author prefers 0 1, so ...
#if 1
		gd->rule50 = 0;
		gd->moveno = 1;
#endif

		// ep square needs to be correct rank for colour, empty, in front pawn of same colour, behind empty (clear on error)
		int ep_sq = gd->epsquare;
		if ((ep_sq < 0) || (ep_sq > 63))
		{
			gd->epsquare = 0;
		}
		else
		{
			if (!((ONE << ep_sq) & ((gd->color == 1) ? H3 : H6)))
			{
				gd->epsquare = 0;
			}
			else
			{	// sq in front must have own pawn
				if (!(PAWNSPUSH((ONE << ep_sq), gd->color ^ 1) & gd->pawns & gd->pieces[gd->color ^ 1]))
				{
					gd->epsquare = 0;
				}
				else
				{	// sq behind must be empty
					if (((PAWNSPUSH((ONE << ep_sq), (gd->color))) & (all_w | all_b)) != 0)
					{
						gd->epsquare = 0;
					}
				}
			}
		}

		// castle status requries K original square, rook original square (clear on error)
		// error tolerant, clears dumb flag bits if set
		u64 w_queens = gd->manhattans & ~gd->diagonals & gd->pieces[0];
		u64 w_rooks = gd->manhattans & ~w_queens;
		u64 b_queens = gd->manhattans & ~gd->diagonals & gd->pieces[1];
		u64 b_rooks = gd->manhattans & ~b_queens;
		if (gd->castlestatus & W_SHORTCASTLE)
		{
			if (!((gd->king_sq[0] == SQ_E1) && (w_rooks & (ONE << SQ_H1))))
			{
				gd->castlestatus &= (~W_SHORTCASTLE);
			}
		}
		if (gd->castlestatus & B_SHORTCASTLE)
		{
			if (!((gd->king_sq[1] == SQ_E8) && (b_rooks & (ONE << SQ_H8))))
			{
				gd->castlestatus &= (~B_SHORTCASTLE);
			}
		}
		if (gd->castlestatus & W_LONGCASTLE)
		{
			if (!((gd->king_sq[0] == SQ_E1) && (w_rooks & (ONE << SQ_A1))))
			{
				gd->castlestatus &= (~W_LONGCASTLE);
			}
		}
		if (gd->castlestatus & B_LONGCASTLE)
		{
			if (!((gd->king_sq[1] == SQ_E8) && (b_rooks & (ONE << SQ_A8))))
			{
				gd->castlestatus &= (~B_LONGCASTLE);
			}
		}

		// incheck status (error intolerant)
		if (IsGiveCheck(gd, gd->color)) return 0;
	}
	// finally, if any, handle the UCI position format <FEN moves e2e4 g8f6 etc>
	// delete if you want to do this elsewhere, or not at all
	token = strtok_s(NULL, " ", &rest_of_epd_str);
	if (token != NULL)
	{
		if (strcmp(token, "moves") == 0)
		{
			// moves loop
			printf("not done this yet, just print the moves meantime");
			while (TRUE)
			{
				token = strtok_s(NULL, " ", &rest_of_epd_str);
				if (token == NULL) break;
				printf("%s\n", token);
			}
		}
	}
	return rest_of_epd_str;
}



//
//
int InitBoard(GAMEDATA *gd)
{
	char* ptr;
	// EPD loader treats "startpos" as a special FEN (as per UCI position definition)
	ptr = LoadFEN(gd, "startpos");
	if (ptr == 0)
	{
		printf("Init Board FEN fail %s\n", "startpos");
		// insert your own debug handler here if you want
	}
	return 0;
}

//
void TestUCI_PositionFormatEPD(GAMEDATA* gd)
{
	char epd_moves[MAXEPD_LENGTH] = "startpos moves e2e4 g8f6 e4e5";
	char epd_str[MAXEPD_LENGTH];
	strcpy(epd_str, epd_moves);
	char* ptr = LoadFEN(gd, epd_str);
	printf("\n");
	if (ptr == 0)
	{
		printf("startpos FEN moves fail %s\n", epd_moves);
	}
	else
	{
		printf("FEN moves ok %s\n", epd_moves);
	}
	strcpy(epd_moves, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves e2e4 g8f6 e4e5");
	strcpy(epd_str, epd_moves);
	ptr = LoadFEN(gd, epd_str);
	if (ptr == 0)
	{
		printf("FEN moves fail %s\n", epd_moves);
	}
	else
	{
		printf("FEN moves ok %s\n", epd_moves);
	}
}


void TestEpdCode(GAMEDATA* gamedata)
{
	int n_suite;
	int n_epds = 0;
	int n_fails = 0;
	for (n_suite = 1; n_suite < 11; n_suite++)
	{
		char* suiteptr;
		suiteptr = fetch_suite_ptr(n_suite);
		if (suiteptr == NULL) break;
		int EPDpos = 0;
		while (1)
		{
			char* ptr = GetEPDString(suiteptr, EPDpos);
			if (*ptr == 'X') break;
			char* saveptr = ptr;
			char copy_epd[MAXEPD_LENGTH];
			strcpy(copy_epd, ptr);
			// get standard position data and save in board arrays, return ptr->rest of EPD string
			// handles UCI position format <FEN moves e2e4 g8f6 etc>, returns pos for end of line
			ptr = LoadFEN(gamedata, copy_epd);
			n_epds++;
			if (ptr == 0)
			{
				n_fails++;
				printf("EPD parse fail %s\n", saveptr);
			}
			else
			{
				printf("EPD ok %s\n", saveptr);
			}
			EPDpos++;
		}
	}
	printf("\n%i epds with %i fails. should be 807 epds with 84 fails\n", n_epds, n_fails);
	printf("You may disagree with the fail count if you modify the verification code, what\n");
	printf(" some people might call a fail isn't, and vice versa\n");
}
//
