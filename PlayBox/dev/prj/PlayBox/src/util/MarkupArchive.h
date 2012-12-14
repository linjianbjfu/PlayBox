// PGLMarkup.h: interface for the CMarkupArchive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PGLMARKUP_H__3BB8B027_BBD6_4DCF_ACDE_7F1922437BEB__INCLUDED_)
#define AFX_PGLMARKUP_H__3BB8B027_BBD6_4DCF_ACDE_7F1922437BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdisp.h>


#include "Markup.h"


/*! \brief Class wrapper for CMarkup adding useful features.

This class mimics the well-known CArchive class and implements a few useful helpers:
<ul>
<li>File handling with Open and Close functions (zipping using zlib)
<li>Classic IsStoring() function.
<li>Added Namespace Support
<li>Helpers to write and read bool, int, float, double, UINT, DWORD, STL vectors
<li>FindChildElemAttrib( LPCTSTR szName , LPCTSTR szAttrib, LPCTSTR szAttribValue )
	Finds a child elem with tag szName and an attribute szAttrib of value szAttribValue
<li>Helper functions FindGetElem, FindGetChildElem : Search for tag, if found loads corresponding value.
</ul>

*/
class CMarkupArchive : public CMarkup  
{
public:
	//! Archive flags	
	static enum EArchiveMode
	{	
		//!	Create mode : when loading object, previous data is erased. (used in PGL)
		ArchiveModeStoring =		0x0001,
		//!	Create mode : when loading object, previous data is erased. (used in PGL)
		ArchiveModeCreate =			0x0002,
		//! Must find name : load data, only if element with same name ( name is an attribute)
		ArchiveModeMustFindName =	0x0004,
		//! Loads and stores non-zipped information
		ArchiveModeNotZipped =		0x0008,
		//! Working with string only, no files
		ArchiveModeString =			0x0010
	};

	//! \name Constructors
	//@{
	/*! Default constructor

  \param dwMode the archive mode. Can be a combination (with |) of the EArchiveMode Enum. Default is AchiveModeCreate | ArchiveModeStoring
	*/
	CMarkupArchive(DWORD dwMode = ArchiveModeCreate | ArchiveModeStoring): CMarkup() {	m_dwMode = dwMode; m_bOpened=false; m_strEncodingString = _T("ISO-8859-1");};
	virtual ~CMarkupArchive();
	//@}


	bool OpenNewFile( LPCSTR szFileName );
	bool SaveNewFile();
	//! \name File Handling
	//@{
	/*! Open a file

	\param szFileName the xml file name
	\param bStoring true if storing xml to a file, false if loading
	*/
	bool Open(LPCTSTR szFileName);
	/*! Open a xml string

	\param szXMLString the xml string
	\return true if success, false otherwize
	*/
	bool OpenString(LPCTSTR szXMLString);
	//! Close the file
	bool Close();	
	/*! Sets the xml encoding string.

	When the xml is stored, a first line is appended to the file:
	<pre>
	<?xml version="1.0" encoding="'Here the encoding string'" ?>
	</pre>

	The default value is ISO-8859-1
	*/
	void SetEncodingString(LPCTSTR lpszEncodingString) { if (lpszEncodingString != NULL) {m_strEncodingString = lpszEncodingString;}};
	//@}

	//! \name Flags
	//@{
	//! Returns true if the file is currently opened
	bool IsOpen() const					{	return m_bOpened;};
	//! Returns true if storing xml
	bool IsStoring() const				{	return m_dwMode & ArchiveModeStoring;};
	//! Returns true if loading xml
	bool IsLoading() const				{	return !(m_dwMode & ArchiveModeStoring);};
	//! Returns true if zipping
	bool IsZipping() const				{	return !(m_dwMode & ArchiveModeNotZipped);};
	bool IsCreateMode() const			{	return ((m_dwMode & ArchiveModeCreate) != 0);};
	bool IsMustFindNameMode() const		{	return ((m_dwMode & ArchiveModeMustFindName) != 0);};

	bool Save();
	/*! Sets the archive mode.

	\param _dwMode a combination of the members of EArchiveMode
	*/
	void SetModeFlag( DWORD _dwMode = ArchiveModeCreate | ArchiveModeStoring) {	m_dwMode = _dwMode;};
	//@}

	//! \name Namespace
	//@{
	/*! Sets the xml namespace

	\param szNameSpace the namespace

	The namespace, if non null, is appended to every xml entry: <a>...</a> becomes <namespace:a>...</namespace:a>
	*/
	bool SetNameSpace(LPCTSTR szNameSpace)	
	{	
		m_sNameSpace = szNameSpace; 
		if (!m_sNameSpace.IsEmpty()) 
			m_sNameSpace+=_T(":"); 
		return true;
	};
	//! Return the namespace
	CString GetNameSpace() const			
	{	
		if (m_sNameSpace.IsEmpty())
			return m_sNameSpace;
		else
			return m_sNameSpace.Left(m_sNameSpace.GetLength()-1);
	};
	//@}




	
	//! \name Base class override to handle namespace
	//@{
	virtual bool AddElem( LPCTSTR szName, LPCTSTR szData=NULL )				
	{	
		if (m_sNameSpace.IsEmpty()) 
			return CMarkup::AddElem( szName, szData); 
		else 
			return CMarkup::AddElem( m_sNameSpace+szName, szData);
	};

	virtual bool AddChildElem( LPCTSTR szName, LPCTSTR szData=NULL )		
	{	
		if (m_sNameSpace.IsEmpty()) 
			return CMarkup::AddChildElem( szName, szData); 
		else 
			return CMarkup::AddChildElem( m_sNameSpace+szName, szData);
	};

	virtual bool FindElem( LPCTSTR szName=NULL )							
	{	
		if (szName==NULL) 
			return CMarkup::FindElem( NULL); 
		else 
			if (m_sNameSpace.IsEmpty())
				return CMarkup::FindElem(szName);
			else
				return CMarkup::FindElem( m_sNameSpace+szName);
	};

	virtual bool FindChildElem( LPCTSTR szName=NULL )
	{	
		if (szName==NULL) 
			return CMarkup::FindElem( NULL); 
		else 
			if (m_sNameSpace.IsEmpty())
				return CMarkup::FindChildElem(szName);
			else
				return CMarkup::FindChildElem( m_sNameSpace+szName);
	};

	bool FindElemAttrib( LPCTSTR szName, LPCTSTR szAttrib, LPCTSTR szAttribValue );
	bool FindChildElemAttrib( LPCTSTR szName , LPCTSTR szAttrib, LPCTSTR szAttribValue );
	//@}

	CString GetFilePath(){return m_sFileName;}
protected:
	//! \name Attributes
	//@{
	//! Falgs
	DWORD m_dwMode;
	//! encoding string
	CString m_strEncodingString;
	//! true if already opened
	bool m_bOpened;
	//! Output-input filename
	CString m_sFileName;
	//! Current namespace, can be empty
	CString m_sNameSpace;
	//@}
private:
	CString m_sTemp;
};


#endif // !defined(AFX_PGLMARKUP_H__3BB8B027_BBD6_4DCF_ACDE_7F1922437BEB__INCLUDED_)
