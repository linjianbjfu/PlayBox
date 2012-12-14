#pragma once
//#ifndef classIevent
//#define classIevent 
class  /*__declspec( dllexport )*/IEventListener
{
	public:
	virtual void OnCliekButton(CButton* pBotton){};
	virtual void OnCliekSlide(CSliderCtrl* pBotton){};
	virtual void OnSendChangeColor( COLORREF acolCol, UINT anLightValue,bool bChangeTop = true ){};
	virtual void OnSendChangeSkin ( CString strPath){};	
	virtual void OnSendChangeSubject(CString,CString subjectName){};
	virtual void OnChangeSkinAndSub(const CString& strPath,const CString& strSub){};
	virtual void OnStartDownSubject(const CString& strSubName){};
	virtual void OnFinishDownSubject(BOOL bSuc){};
};
//#endif 
