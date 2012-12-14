#include "YL_Encrypter.h"
#include <string.h>
#include <math.h>

/* Encrypt ���ܺ���
	@param	memptr			[in/out]Ҫ���ܵ��ڴ�ռ�����
			memlen			[in]���ݳ���
			type			[in]�����㷨
*/
const int XOR_KEY1_LEN = 13;		
#define KT_ECRP_KEY1 "KoOtOiTvINGwd"

const int XOR_KEY2_LEN = 23;		
#define KT_ECRP_KEY2 "_Y8g2E6n0E1i7L5t2IoOoNk"

const int MOVE_POS1 = 133;
const int MOVE_POS2 = 71;

void XOR( char *data, char *key, int datalen, int keylen)
{
	for (int i=0; i<datalen; i++)
		*(data+i) ^= *(key+i%keylen);
}
//��λ(pos>0 ����posλ, pos<0 ����|pos|λ)
void MOVE( char *data, int datalen, int pos )
{
	if (pos == 0 || pos%datalen == 0)
		return;

	int realpos = abs(pos);
	while (realpos > datalen)
		realpos -= datalen;
	
	char* tmp = new char[realpos];
	char* ptr;
	if (pos > 0)
	{
		memcpy( tmp, data+datalen-realpos, realpos );
		ptr = data+datalen-1;
		for (; ptr>data+realpos-1; ptr--)
			*ptr = *(ptr-realpos);
		memcpy( data, tmp, realpos );
	}
	else
	{
		memcpy( tmp, data, realpos );
		ptr = data;
		for (; ptr<data+datalen-realpos; ptr++)
			*ptr = *(ptr+realpos);
		memcpy( data + datalen - realpos, tmp, realpos );
	}
	delete []tmp;
}

void YL_Encrypter::Encrypt(char* memptr, int memlen, int algorithmType)
{
	switch (algorithmType)
	{
	case 0:						//ȡǰ16���ֽ������Ĳ��ֱַ����
		{
			char key1[XOR_KEY1_LEN];
			memcpy(key1,KT_ECRP_KEY1,XOR_KEY1_LEN);
			MOVE( memptr, memlen,MOVE_POS1);		
			XOR( memptr, key1, memlen, XOR_KEY1_LEN );
			char key2[XOR_KEY2_LEN];
			memcpy(key2,KT_ECRP_KEY2,XOR_KEY2_LEN);
			MOVE( memptr, memlen,-MOVE_POS2);
			XOR( memptr, key2, memlen, XOR_KEY2_LEN );
			break;
		}
	}
}

void YL_Encrypter::Decrypt(char* memptr, int memlen, int algorithmType)
{
	switch (algorithmType)
	{
	case 0:						//ȡǰ16���ֽ������Ĳ��ֱַ����
		{
			char key2[XOR_KEY2_LEN];
			memcpy(key2,KT_ECRP_KEY2,XOR_KEY2_LEN);	
			XOR( memptr, key2, memlen, XOR_KEY2_LEN );
			MOVE( memptr, memlen,MOVE_POS2);	

			char key1[XOR_KEY1_LEN];
			memcpy(key1,KT_ECRP_KEY1,XOR_KEY1_LEN);
			XOR( memptr, key1, memlen, XOR_KEY1_LEN );
			MOVE( memptr, memlen,-MOVE_POS1);	

			break;
		}
	}
}
