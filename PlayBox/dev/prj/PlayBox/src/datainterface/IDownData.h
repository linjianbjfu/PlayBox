#pragma once

#include "IData.h"
#include "IDownDataDef.h"
#include "IDownDataObserver.h"

class IDownData : public IData
{
public:
	virtual BOOL IsEmpty() = 0;	//当前是否有下载任务
	virtual BOOL IsDowning () = 0; // 当前是否有正在下载的任务
	virtual void LockDownInfoList() = 0;
	virtual const ExeDownInfoList* GetDownInfoList() const = 0;
	virtual const ExeDownInfo*		GetDownInfo(const string& rid) const = 0;
	virtual void UnlockDownInfoList() = 0;
	virtual BOOL Download(const ExeGameVector& siv) = 0;
	virtual BOOL Start(const RidVector& rv) = 0;
	virtual BOOL Restart(const ExeGameVector& siv, bool bCheckExist = true ) = 0;
	virtual BOOL Pause(const RidVector& rv) = 0;
	virtual BOOL Delete(const RidVector& rv) = 0;
	virtual BOOL IsDowning(const string& rid) = 0;
	virtual BOOL IsDowned(const string& rid) = 0;
};