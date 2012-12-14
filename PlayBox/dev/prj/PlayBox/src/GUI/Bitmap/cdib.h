// cdib.h declaration for Inside Visual C++ CDib class

#ifndef _INSIDE_VISUAL_CPP_CDIB
#define _INSIDE_VISUAL_CPP_CDIB



class CDib : public CObject
{
	enum Alloc {noAlloc, crtAlloc, heapAlloc};

	DECLARE_SERIAL(CDib)
	typedef struct CutRect
	{
		CRect* rect;
		UINT count;
		UINT index;
		CutRect()
		{
			rect = NULL;
			count = 0;
			index = 0;
		}
	}STRUCT_CUTRECT,*P_STRUCT_CUTRECT;

public:
	BOOL AttachMemory(LPVOID lpvMem, BOOL bMustDelete = FALSE, HGLOBAL hGlobal = NULL);
	enum DrawMode {NORMAL,STRETCH, TILE};
	CSize GetDimensions();
	void Empty();
	LPBITMAPINFOHEADER m_lpBMIH; //  buffer containing the BITMAPINFOHEADER
	LPVOID m_lpvColorTable;
protected:

	HBITMAP m_hBitmap;
	LPBYTE m_lpImage;  // starting address of DIB bits


	HGLOBAL m_hGlobal; // For external windows we need to free;
private:
	                   //  could be allocated by this class or allocated externally
	Alloc m_nBmihAlloc;
	Alloc m_nImageAlloc;
	DWORD m_dwSizeImage; // of bits -- not BITMAPINFOHEADER or BITMAPFILEHEADER
protected:
	int m_nColorTableEntries;

private:
	
	HANDLE m_hFile;
	HANDLE m_hMap;
	LPVOID m_lpvFile;
protected:
	HPALETTE m_hPalette;
//public:
	CDib();
	CDib(CSize size, int nBitCount);	// builds BITMAPINFOHEADER
	~CDib();
	int GetSizeImage() {return m_dwSizeImage;}
	int GetSizeHeader()
		{return sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_nColorTableEntries;}

	BOOL AttachMapFile(const char* strPathname, BOOL bShare = FALSE);
	BOOL AttachMapFile2(void* pViod,UINT size,P_STRUCT_CUTRECT pCutRect, BOOL bShare = FALSE);
	BOOL AttachMapFile2(const char* strPathname,P_STRUCT_CUTRECT pCutRect, BOOL bShare = FALSE);
	BOOL CopyToMapFile(const char* strPathname);

	BOOL Draw(CDC* pDC, CPoint origin, CSize size,BOOL isBKTransparent=FALSE, COLORREF mask=RGB(0,0,0),DrawMode bStrecth = STRETCH);  // until we implemnt CreateDibSection
	HBITMAP CreateSection(CDC* pDC = NULL);
	UINT UsePalette(CDC* pDC, BOOL bBackground = FALSE);
	BOOL MakePalette();
	BOOL SetSystemPalette(CDC* pDC);
	BOOL Compress(CDC* pDC, BOOL bCompress = TRUE); // FALSE means decompress
	HBITMAP CreateBitmap(CDC* pDC);
	BOOL Read(CFile* pFile);
	BOOL ReadSection(CFile* pFile, CDC* pDC = NULL);
	BOOL Write(CFile* pFile);
	void Serialize(CArchive& ar);

	UINT DrawTransParent(CDC *pDC, CRect rcSrc, CRect rcDst, COLORREF color);
	//BOOL CDib::DrawDib(CDC* pDC, CPoint origin, CSize size);
	BOOL CcutBitmap(LPSTR lpImage,P_STRUCT_CUTRECT pCutRect,CMemFile ** ppFile);
private:
	void DetachMapFile();
	void ComputePaletteSize(int nBitCount);
	void ComputeMetrics();
	HRGN CreateRgnFromBitmap(COLORREF color);

};
#endif // _INSIDE_VISUAL_CPP_CDIB
