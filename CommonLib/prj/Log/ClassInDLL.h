#ifndef ClassInDLL_H
#define ClassInDLL_H

#ifdef _CLASSINDLL
	#define CLASSINDLL_CLASS_DECL __declspec(dllexport)
#else
	#define CLASSINDLL_CLASS_DECL __declspec(dllimport)
#endif

#endif // ClassInDLL_H
