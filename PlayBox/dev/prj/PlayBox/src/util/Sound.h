#include "mmsystem.h"
#pragma comment (lib,"winmm.lib")
class CSound  
{
	long m_device;
	MIXERCONTROL m_volCtrl;
private:
	bool CSound::GetDevice(int dev,HMIXER &hmixer);
public:
	CSound();
	virtual ~CSound();

	bool GetVolumeControl(HMIXER hmixer,long componentType,long ctrlType,MIXERCONTROL* mxc);
	unsigned GetVolumeValue(HMIXER hmixer,MIXERCONTROL *mxc);
	bool SetVolumeValue(HMIXER hmixer,MIXERCONTROL *mxc,long volume);
	long GetMuteValue(HMIXER hmixer,MIXERCONTROL* mxc);
	bool SetMuteValue(HMIXER hmixer,MIXERCONTROL *mxc,bool mute);
	unsigned GetVolume(UINT dev);
	bool SetVolume(long dev,long vol);
	bool GetMute(long dev);
	bool SetMute(long dev,bool sound=TRUE);
public:
	static CSound* GetInstance();
	static void DelInstance();
	static CSound* m_self;
};