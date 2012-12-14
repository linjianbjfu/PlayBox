#pragma once

   int FindString(const char* str1, const char* str2, bool& end);
   int FindPinyin(const char* str1, const char* str2, bool& end);
   int ChToPinyin(__int16& nChChar,  char* cOutPinyin  );
   const char*  ChStringToPinyin(char   *pinyin,   const char *ch, int nMaxLenght=64);
   const char* GetFirstLetter(char   *pinyin,   const char *ch, int *nPos,int nMaxLenght=8);
   bool IsICchar(const char* str,int nPos);


