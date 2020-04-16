# Chess-EPD-verifier
C source code for chess engine programmers, integrity checks EPD/FEN according to all rules of chess.

Copyright bla bla: Kindly credit (c) Chris Whittington 2020 in any source releases, and preferably (not compulsory)
likewise in any binary. Credit as in source code file. Apart from that, do whatever you like with the code. Obvs it is
without any warranty and provided free. Code is pedantic, not optimised (not time critical), primitive C, first-shot
brute force code. Important feature is that it works, basically.

The idea is for a self-contained module which also has all the chess necessary code for legality checking.

Objective is to get chess engine programmers and all engines actually fully verifying the EPDs they use. There's 
a lot of EPDs out there, many of which contain bad data and we can't rely on our data inputs being perfect,
when they often aren't.

The code to port into an engine would be:

    // decodes FEN as 6-part string
    // highly tolerant of errors, enforces rules of normal chess (including in check legality)
    // returns gamedata strucure, ptr to end of FEN, 
    //  else return 0 (NB and a smashed up board data structure)
    // 
    char* LoadFEN(GAMEDATA* gd, char* fen);
    
it returns a ptr to the end of the EPD data (for further processing, maybe, depends on what you're doing)
and a structure (gd) consisting of piece bitboards and position status flags
see definition of GAMEDATA structure
On EPD fail returns a zero pointer.

Actual FEN string is remorselessly checked
w/b string likewise
the following fields can be missing from the EPD, then just get set with obvious defaults
castling status is set permissively (do what EPD says, but override on chess integrity fail)
ep square, likewise
move number, set to allowable limits as EPD says
rule50 likewise
personally, for purposes of engine, some may like to set those fields to 0 1 and override the EPD, there's
a compile switch to handle that.

finally, position checked for side to move not in check. done by sets of routines that don't
requite lookups or magics. code is pedantic and slow, but speed is not critical anyway, so.
An alternative would be to perform the legality check using own engine code (hey, you have
that anyway, no?). 

There's a bunch of EPD test positions (credited where author is known) to provide code
integrity testing. Obvs you can junk these when using the code in an engine main() runs all that stuff.

