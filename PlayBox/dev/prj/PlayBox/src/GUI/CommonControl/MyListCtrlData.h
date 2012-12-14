#ifndef _MY_LISTCTRL_DATA_H
#define _MY_LISTCTRL_DATA_H

#include "../Bitmap/DibBitmap.h"

struct MYLIST_COLUMN_IMAGE
{
	MYLIST_COLUMN_IMAGE():pBitmapNormal(NULL),
						  pBitmapDonw(NULL),
						  pBitmapOver(NULL),	
						  pBitmapSelected(NULL)					  
	{}

	void Clear()
	{
		pBitmapNormal	= NULL;
		pBitmapDonw		= NULL;
		pBitmapSelected	= NULL;
		pBitmapOver		= NULL;		
	}

	CDibBitmap*	pBitmapNormal;		//一般状态
	CDibBitmap*	pBitmapSelected;	//行被选中
	CDibBitmap*	pBitmapOver;		//鼠标Over
	CDibBitmap* pBitmapDonw;		//鼠标按下	
};

struct MYLIST_COLUMN_INFO
{
	MYLIST_COLUMN_INFO():strColumnName(""),
						 enumColumnType(COLUMN_TYPE_TEXT),
						 iColumnIndex(-1),
						 iColumnWidth(0),
						 bOperatorBtn(false)
	{}
	enum  COLUMN_TYPE
	{
		COLUMN_TYPE_TEXT =0,
		COLUMN_TYPE_IMAGE
	};
	string				strColumnName;			
	COLUMN_TYPE			enumColumnType;
	int					iColumnIndex;
	int					iColumnWidth;
	bool				bOperatorBtn;
	MYLIST_COLUMN_IMAGE	ColumnImage;			//如果为Text，则为空
	MYLIST_COLUMN_IMAGE ColumnOperatorImage;

	MYLIST_COLUMN_INFO&	operator=(const MYLIST_COLUMN_INFO& columnInfo)
	{
		strColumnName	= columnInfo.strColumnName;
		enumColumnType	= columnInfo.enumColumnType;
		iColumnIndex	= columnInfo.iColumnIndex;
		iColumnWidth	= columnInfo.iColumnWidth;
		bOperatorBtn	= columnInfo.bOperatorBtn;
		ColumnImage		= columnInfo.ColumnImage;	
		ColumnOperatorImage = columnInfo.ColumnOperatorImage;
		return *this;
	}
};


#endif