#ifndef _C_BUFFER_DC_H
#define _C_BUFFER_DC_H


class CBufferDC : public CDC
{
	HDC     m_hDestDC;
	CBitmap m_bitmap;     // Bitmap in Memory DC
	CRect   m_rect;
	HGDIOBJ m_hOldBitmap; // Previous Bitmap

public:
	CBufferDC (HDC hDestDC, const CRect& rcPaint = CRect(0,0,0,0));
	~CBufferDC ();
};

#endif