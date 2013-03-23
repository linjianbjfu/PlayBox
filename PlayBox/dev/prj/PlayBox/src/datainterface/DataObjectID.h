#ifndef _DATA_OBJECT_H
#define _DATA_OBJECT_H

static int g_iDataIDCounter = 0;
#define DATA_ID(sig) const DWORD sig = g_iDataIDCounter++;
#define DATA_ID_CONST(i, sig) const DWORD sig = i;

DATA_ID_CONST(0, ID_DATA_PANELCHANGE)
DATA_ID_CONST(1, ID_DATA_TABBAR)
DATA_ID_CONST(2, ID_DATA_TABPAGE)
DATA_ID_CONST(3, ID_DATA_GAME)
DATA_ID_CONST(4, ID_DATA_AD)

#undef DATA_ID
#undef DATA_ID_CONST
#endif