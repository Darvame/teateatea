#ifndef TEA_TRIM_H
#define TEA_TRIM_H

#ifndef TEA_TRIM_USE_CTYPE
	#define TEA_PACK_WHILE_ISSPACE(w, c, modify)\
		while(w) { switch(c) {\
			case 0x9: case 0xA: case 0xB:\
			case 0xC: case 0xD: case 0x20:\
			modify; continue;\
		} break; }

#else
	#include "ctype.h"

	#define TEA_PACK_WHILE_ISSPACE(w, c, modify)\
		while(w && isspace(c)) modify;

#endif

#define TEA_PACK_SPACE_TRIM_WORD(s, b, e)\
	TEA_PACK_WHILE_ISSPACE(e > b, s[b], ++b);\
	TEA_PACK_WHILE_ISSPACE(e > b, s[e-1], --e);

#endif
