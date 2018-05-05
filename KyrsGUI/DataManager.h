#pragma once
#include "headers.h"


	lvl1 *load_lvl(char* lvlFile, int maxStringLen, lvl1 *root, int &countlvl1, int &countlvl2, int &countlvl3);
	void loadlvl1(char* str, lvl1 &lvl);
	void loadlvl2(char* str, lvl2 &lvl);
	void loadlvl3(char* str, lvl3 &lvl, bool needMeta);
	void loadlvlMeta(char* str, meta &met, char **text, int &integer);
	boolean haveEnd(char *string);
	void print(lvl1 *root, int countlvl1);


