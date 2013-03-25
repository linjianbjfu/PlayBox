// PGLMarkup.cpp: implementation of the CMarkupArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkupArchive.h"

#include <io.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMarkupArchive::~CMarkupArchive()
{
	Close();
}

bool CMarkupArchive::OpenNewFile( LPCSTR szFileName )
{
	m_sFileName = szFileName;
	SetDoc(NULL);
	m_bOpened=true;

	return true;
}
bool CMarkupArchive::SaveNewFile()
{
	CFile			f;
	if(!f.Open(m_sFileName, CFile::modeCreate| CFile::modeWrite|CFile::modeRead))
		return false;
	CString doc = GetDoc();
	f.Write(doc, doc.GetLength());
	f.Close();
	return true;
}

bool CMarkupArchive::Open(LPCTSTR szFileName)
{
	CFile			f;
	CString			strXMLContent;
	int				nFileLen;
	unsigned char * pBuffer;

	m_sFileName= szFileName;

	ASSERT(m_bOpened==false);

	if(!f.Open(szFileName, CFile::modeRead))
		return false;

	nFileLen = (int)(f.GetLength());
	pBuffer = new unsigned char[nFileLen + 2];
	f.Read(pBuffer, nFileLen);
	f.Close();

	pBuffer[nFileLen] = '\0';
	pBuffer[nFileLen+1] = '\0'; // in case 2-byte encoded

	strXMLContent = pBuffer;
	delete [] pBuffer;
	m_bOpened = SetDoc(strXMLContent);

	return m_bOpened;
}

bool CMarkupArchive::Save()
{
	CFile			f;
	CString			strXMLContent;
	CString			m_strRealFileName  = m_sFileName;
	CString			szFileName = m_sFileName + ".temp";

	if(!f.Open(szFileName, CFile::modeCreate| CFile::modeWrite|CFile::modeRead))
		return false;
	CString doc = GetDoc();
	f.Write(doc, doc.GetLength());
	f.Close();

	//如果 panes.xml.temp存在, 删除panes.xml, 将panes.xml.temp 改名为panes.xml
	if(f.Open(szFileName, CFile::modeRead))
	{
		int len = f.GetLength();
		f.Close();

		if(len == 0)
			return false;

		Close();
		if(!Open(szFileName))
			return false;
		else
			Close();

		CopyFile(szFileName, m_strRealFileName, FALSE);
	}
	return true;
}

bool CMarkupArchive::OpenString(LPCTSTR szXMLString)
{
	if (szXMLString==NULL)
		return false;

	m_dwMode = ~(~m_dwMode | ArchiveModeStoring);

	m_sFileName=_T("");
	m_bOpened=true;

	return SetDoc(szXMLString);
}


bool CMarkupArchive::Close()
{
	m_bOpened = false;
	//if (!m_bOpened)
	//	return false;

	//CString str;
	//LPTSTR pBuffer;
	//int length;

	//// --- start --- TMB! 29-08-2002 ** INSAD **
	//// Initialy fill str with the XML header ;)
	//str.Format(_T("<?xml version=\"1.0\" encoding=\"%s\" ?>\r\n"), m_strEncodingString);
	//// --- end --- TMB! 29-08-2002 ** INSAD **

	//if (IsStoring())
	//{
	//	if (!( m_dwMode & ArchiveModeNotZipped))
	//	{
	//#ifdef _UNICODE
	//		char mbFileName[512];
	//		VERIFY( WideCharToMultiByte(CP_THREAD_ACP, // code page
	//			WC_SEPCHARS,            // performance and mapping flags
	//			m_sFileName,    // wide-character string
	//			-1,          // number of chars in string
	//			mbFileName,     // buffer for new string
	//			512*sizeof(TCHAR),          // size of buffer
	//			NULL,     // default for unmappable chars
	//			NULL  // set when default char used
	//			) );
	//		gzFile gzf = gzopen(mbFileName,"wb9");
	//#else
	//		gzFile gzf = gzopen( m_sFileName, "wb9");
	//#endif

	//		// --- Old code --- TMB! 29-08-2002 ** INSAD ** Now already filled!
	//		//str=_T("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n");
	//		// --- end --- TMB! 29-08-2002 ** INSAD **
	//		length=str.GetLength()*sizeof(TCHAR);
	//		pBuffer=str.LockBuffer();
	//		gzwrite(gzf,pBuffer,length);
	//		str.UnlockBuffer();

	//		// writing document
	//		str=GetDoc();
	//		length=str.GetLength();
	//		pBuffer=str.LockBuffer();
	//		gzwrite(gzf,pBuffer, length);
	//		str.UnlockBuffer();
	//		gzclose(gzf);
	//	}
	//	else
	//	{
	//		TRY
	//		{
	//			CStdioFile file(m_sFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeText);
	//			// --- start --- TMB! 29-08-2002 ** INSAD ** // Already filled!
	//			file.WriteString(str);
	//			// --- Old code --- TMB! 29-08-2002 ** INSAD **
	//			//file.WriteString(_T("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n"));
	//			// --- end --- TMB! 29-08-2002 ** INSAD **
	//			file.WriteString(GetDoc());
	//			file.Close();
	//		}
	//		CATCH( CFileException, e )
	//		{
	//			AfxMessageBox(_T("File could not be opened ")+e->m_cause);
	//		}
	//		END_CATCH
	//	}
	//}

	//m_bOpened=false;

	return true;
}

bool CMarkupArchive::FindElemAttrib( LPCTSTR szName, LPCTSTR szAttrib, LPCTSTR szAttribValue )
{
	if ( szName == NULL || !szName[0] || szAttrib == NULL || !szAttrib[0])
		return false;

	// Otherwise go to next sibling element with matching tag name and attrib value
	// If the current position is valid, start looking from next
	// Change current position only if found
	int iPos = m_iPos;
	if ( ! iPos )
	{
		if ( m_aPos.GetSize() )
			iPos = m_aPos[0].iElemChild;
	}
	else
	{
		iPos = m_aPos[iPos].iElemNext;
	}

	BOOL bGotIt;											// --- Added --- TMB! 22-08-2002 ** INSAD **
	while ( iPos )
	{
		// Compare tag name unless szName is not specified
		// --- start --- TMB! 22-08-2002 ** INSAD **
		bGotIt = (x_GetTagName(iPos) == m_sNameSpace+szName);
		if (bGotIt && szAttribValue == NULL) 
		{
			bGotIt = x_GetAttrib(iPos, szAttrib) != "";		// Assume empty value is same as attribute not there...
		}
		else 
		{
			CString temp = x_GetAttrib(iPos, szAttrib);
			bGotIt = (x_GetAttrib(iPos, szAttrib) == szAttribValue);
		}
		if (bGotIt)
		// --- oldcode --- TMB! 22-08-2002 ** INSAD **
		//if ( x_GetTagName(iPos) == m_sNameSpace+szName && x_GetAttrib(iPos, szAttrib) )
		// --- end --- TMB! 22-08-2002 ** INSAD **
		{
			// Assign new position
			m_iPos = iPos;
			m_iPosChild = 0;
			return true;
		}
		iPos = m_aPos[iPos].iElemNext;
	}
	return false;
}



bool CMarkupArchive::FindChildElemAttrib( LPCTSTR szName , LPCTSTR szAttrib, LPCTSTR szAttribValue )
{
	if ( szName == NULL || !szName[0] || szAttrib == NULL || !szAttrib[0])
		return false;

	// If szName is NULL or empty, go to next sibling child element
	// Otherwise go to next sibling child element with matching tag name
	// If the current child position is valid, start looking from next
	// Change current child position only if found
	//
	// Shorthand: call this with no current position means under root element
	if ( ! m_iPos )
		FindElem();

	// Is main position valid and not empty?
	if ( ! m_iPos || m_aPos[m_iPos].IsEmptyElement() )
		return false;

	// Is current child position valid?
	int iPosChild = m_iPosChild;
	if ( iPosChild )
		iPosChild = m_aPos[iPosChild].iElemNext;
	else
		iPosChild = m_aPos[m_iPos].iElemChild;

	// Search
	BOOL bGotIt;											// --- Added --- TMB! 22-08-2002 ** INSAD **
	while ( iPosChild )
	{
		// Compare tag name unless szName is not specified
		CString temp = x_GetTagName(iPosChild);
		// --- start --- TMB! 22-08-2002 ** INSAD **
		bGotIt = (x_GetTagName(iPosChild) == m_sNameSpace+szName);
		if (bGotIt && szAttribValue == NULL) 
		{
			bGotIt = x_GetAttrib(iPosChild, szAttrib) != _T("");		// Assume empty value is same as attribute not there...
		}
		else 
		{
			bGotIt = (x_GetAttrib(iPosChild, szAttrib) == szAttribValue);
		}
		if (bGotIt)
		// --- oldcode --- TMB! 22-08-2002 ** INSAD **
		//if ((x_GetTagName(iPosChild) == m_sNameSpace+szName) && (x_GetAttrib(iPosChild, szAttrib) == szAttribValue))
		// --- end --- TMB! 22-08-2002 ** INSAD **
		{
			// Assign new position
			m_iPosChild = iPosChild;
			return true;
		}
		iPosChild = m_aPos[iPosChild].iElemNext;
	}
	return false;
}
