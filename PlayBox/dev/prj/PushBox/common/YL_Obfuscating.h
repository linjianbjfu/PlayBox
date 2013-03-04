#pragma once
//tag:lrc

#ifndef _DEBUG
	#define _KUWO_USE_OBFUSCATING
#endif

#ifndef _KUWO_USE_OBFUSCATING
	#define __OBFUSCATING_XX0  _asm nop
	#define __OBFUSCATING_XX1  _asm nop
	#define __OBFUSCATING_XX2  _asm nop
	#define __OBFUSCATING_XX3  _asm nop
	#define __OBFUSCATING_XX4  _asm nop
	#define __OBFUSCATING_XX5  _asm nop
#else

	#define __OBFUSCATING_XX0 _asm \
		{\
		_asm jz  $+14\
		_asm nop\
		_asm jnz $+7\
		_asm _emit 0e8h\
		}

	#define __OBFUSCATING_XX1 _asm \
		{\
		_asm jz  $+15\
		_asm jnz $+9\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		}

	#define __OBFUSCATING_XX2 _asm \
		{\
		_asm jz  $+16\
		_asm jnz $+10\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		}

	#define __OBFUSCATING_XX3 _asm \
		{\
		_asm jz  $+17\
		_asm jnz $+11\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		}

	#define __OBFUSCATING_XX4 _asm \
		{\
		_asm jz  $+15\
		_asm jnz $+9\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm jz  $+15\
		_asm jnz $+9\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		}


	#define __OBFUSCATING_XX5 _asm \
		{\
		_asm jz  $+13\
		_asm jnz $+7\
		_asm _emit 0e8h\
		_asm jz  $+15\
		_asm jnz $+9\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm jz  $+17\
		_asm jnz $+11\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		_asm _emit 0e8h\
		}
#endif