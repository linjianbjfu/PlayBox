#ifdef FL_EXPORTS
#define FL_API __declspec(dllexport)
#else
#define FL_API __declspec(dllimport)
#endif

#include <vector>
#include <string>
#include <algorithm>
using namespace std;

FL_API int GetFL (vector<string> vStr);
FL_API int GetFL2 (vector<string> vStr);
