#include "stdafx.h"
#include "Sound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CSound* CSound::m_self = NULL;

CSound* CSound::GetInstance()
{
	if( m_self == NULL )
	{
		m_self = new CSound();
	}
	return m_self;
}

void CSound::DelInstance()
{
	if( m_self != NULL )
	{
		delete m_self;
	}
	m_self = NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSound::CSound()
{
	m_device=1;
}

CSound::~CSound()
{

}


bool CSound::GetDevice(int dev,HMIXER &hmixer)
{
	switch(dev)   
	{   
	case 1:  //WAVE
		m_device=MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
		break;
	case 2:  //MIDI
		m_device=MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
		break;
	case 3:  //LINE IN / cd音量
	case 4:
		m_device=MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
		break;
	case 5:  //麦克风音量
		m_device=MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
		break;
	case 6:  //PC   扬声器音量
		m_device=MIXERLINE_COMPONENTTYPE_SRC_LINE;
		break;  
	default:   //主音量
		m_device=MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	}
	if(mixerOpen(&hmixer,0,0,0,0))
		return FALSE;

	return TRUE;
}


//设定音量
bool CSound::GetVolumeControl(HMIXER hmixer,long componentType,long ctrlType,MIXERCONTROL* mxc)   
{   
	MIXERLINECONTROLS mxlc;   
	MIXERLINE mxl;   
	mxl.cbStruct = sizeof(mxl);   
	mxl.dwComponentType = componentType;   
	if(!mixerGetLineInfo((HMIXEROBJ)hmixer,&mxl,MIXER_GETLINEINFOF_COMPONENTTYPE))   
	{   
		mxlc.cbStruct = sizeof(mxlc);   
		mxlc.dwLineID = mxl.dwLineID;   
		mxlc.dwControlType = ctrlType;   
		mxlc.cControls = 1;   
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);   
		mxlc.pamxctrl = mxc;   
		if(mixerGetLineControls((HMIXEROBJ)hmixer,&mxlc,MIXER_GETLINECONTROLSF_ONEBYTYPE))   
			return FALSE;   
		else   
			return TRUE;   
	}   
	return FALSE;   
}  

long CSound::GetMuteValue(HMIXER hmixer,MIXERCONTROL* mxc)   
{   
	MIXERCONTROLDETAILS   mxcd;   
	MIXERCONTROLDETAILS_BOOLEAN   mxcdMute;   
	mxcd.hwndOwner = 0;   
	mxcd.cbStruct = sizeof(mxcd);   
	mxcd.dwControlID = mxc->dwControlID;   
	mxcd.cbDetails = sizeof(mxcdMute);   
	mxcd.paDetails = &mxcdMute;   
	mxcd.cChannels = 1;   
	mxcd.cMultipleItems = 0;   
	if(mixerGetControlDetails((HMIXEROBJ)hmixer,&mxcd,
		MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))
		return -1;   
	return mxcdMute.fValue;   
}  

bool CSound::SetMuteValue(HMIXER hmixer,MIXERCONTROL* mxc,bool mute)   
{   
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcdMute.fValue=mute;   
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;   
	mxcd.cbStruct = sizeof(mxcd);   
	mxcd.cbDetails = sizeof(mxcdMute);   
	mxcd.paDetails = &mxcdMute;   
	mxcd.cChannels = 1;   
	mxcd.cMultipleItems = 0;   
	if(mixerSetControlDetails((HMIXEROBJ)hmixer,&mxcd,
		MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
		return   FALSE; 

	return   TRUE;   
}

unsigned CSound::GetVolumeValue(HMIXER hmixer,MIXERCONTROL *mxc)   
{   
	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_UNSIGNED   vol;   vol.dwValue=0;   
	mxcd.hwndOwner = 0;   
	mxcd.cbStruct = sizeof(mxcd);   
	mxcd.dwControlID = mxc->dwControlID;   
	mxcd.cbDetails = sizeof(vol);   
	mxcd.paDetails = &vol;   
	mxcd.cChannels = 1;   
	if(mixerGetControlDetails((HMIXEROBJ)hmixer,&mxcd,MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE))  
		return   -1;   
	return   vol.dwValue;   
}

bool CSound::SetVolumeValue(HMIXER hmixer,MIXERCONTROL *mxc,long volume)
{   
	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	vol.dwValue = volume;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	if(mixerSetControlDetails((HMIXEROBJ)hmixer,&mxcd,
		MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))   
		return FALSE; 

	return TRUE;   
}

//得到设备的音量  
unsigned CSound::GetVolume(UINT dev)
{
	unsigned rt=0;
	HMIXER   hmixer;   
	if(!GetDevice(dev,hmixer))
		return 0;
	if(!GetVolumeControl(hmixer,m_device,MIXERCONTROL_CONTROLTYPE_VOLUME,&m_volCtrl))
		return 0;
	rt=GetVolumeValue(hmixer,&m_volCtrl)*100/m_volCtrl.Bounds.lMaximum;
	mixerClose(hmixer);
	return   rt;    
}

//设置设备的音量
bool CSound::SetVolume(long dev,long vol)   
{   
	bool rc=FALSE;
	HMIXER hmixer;   
	if(!GetDevice(dev,hmixer))
		return FALSE;    
	if(GetVolumeControl(hmixer,m_device,MIXERCONTROL_CONTROLTYPE_VOLUME,&m_volCtrl))   
	{   
		vol=vol*m_volCtrl.Bounds.lMaximum/100;   
		if(SetVolumeValue(hmixer,&m_volCtrl,vol))   
			rc=TRUE;   
	}   
	mixerClose(hmixer);
	return rc;   
}

//设置设备静音
bool CSound::SetMute(long dev,bool sound)   
{
	bool rc=FALSE; 
	HMIXER hmixer;   
	if(!GetDevice(dev,hmixer))
		return FALSE;
	if(GetVolumeControl(hmixer,m_device,MIXERCONTROL_CONTROLTYPE_MUTE,&m_volCtrl))
		if(SetMuteValue(hmixer,&m_volCtrl,sound))   
			rc=TRUE;   
	mixerClose(hmixer);   
	return rc;   
}

//检查设备是否静音(静音返回TRUE)
bool CSound::GetMute(long dev)
{ 
	long rc=0;  
	HMIXER hmixer;   
	if(!GetDevice(dev,hmixer))
		return false; 
	if(GetVolumeControl(hmixer,m_device,MIXERCONTROL_CONTROLTYPE_MUTE,&m_volCtrl))   
		rc=GetMuteValue(hmixer,&m_volCtrl);
	mixerClose(hmixer);
	if(rc)
		return true;

	return false;
}

