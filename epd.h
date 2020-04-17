//
#pragma once
//
// massive overkill
#define MAXEPD_LENGTH 512

//
char* fetch_suite_ptr(int n_suite);
char* GetEPDString(char* ptr, int stringno);
int InitBoard(GAMEDATA *gd);
char* LoadFEN(GAMEDATA* gd, char* fen);
void TestEpdCode(GAMEDATA* gd);
void TestUCI_PositionFormatEPD(GAMEDATA* gamedata);
//

