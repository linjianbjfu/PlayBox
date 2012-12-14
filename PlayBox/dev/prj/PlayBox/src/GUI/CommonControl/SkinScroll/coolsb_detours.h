#ifndef COOLSB_DETOURS_INCLUDED
#define COOLSB_DETOURS_INCLUDED

#include "coolscroll.h"

#ifdef __cplusplus
extern "C" {
#endif

BOOL WINAPI CoolSB_InitializeApp(void);
BOOL WINAPI CoolSB_UninitializeApp(void);
void InitTest(void);

#ifdef __cplusplus
}
#endif

#endif