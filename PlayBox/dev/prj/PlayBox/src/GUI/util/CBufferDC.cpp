#include "stdafx.h"
#include "CBufferDC.h"

CBufferDC::CBufferDC (HDC hDestDC, const CRect& rcPaint) : m_hDestDC (hDestDC)
{
	if ( rcPaint.IsRectEmpty() )
	{
		::GetClipBox (m_hDestDC, m_rect);
	}
	else
	{
		m_rect = rcPaint;
	}
	VERIFY(Attach (::CreateCompatibleDC (m_hDestDC)));
	m_bitmap.Attach (::CreateCompatibleBitmap (m_hDestDC, m_rect.right, m_rect.bottom));
	m_hOldBitmap = ::SelectObject (m_hDC, m_bitmap);

	if ( m_rect.top > 0 )
	{
		ExcludeClipRect (0, 0, m_rect.right, m_rect.top);
	}
	if ( m_rect.left > 0 )
	{
		ExcludeClipRect (0, m_rect.top, m_rect.left, m_rect.bottom);
	}
}

///////////////////////////////////////////////////////////////////////////////
CBufferDC::~CBufferDC ()
{
	VERIFY(::BitBlt (m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC, m_rect.left, m_rect.top, SRCCOPY));
	::SelectObject (m_hDC, m_hOldBitmap);
	HGDIOBJ pObj = m_bitmap.Detach();
	::DeleteObject(pObj);
	HDC pDc=Detach();
	::DeleteDC(pDc);
}


