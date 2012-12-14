#pragma once
#include "../Global/GlobalVariable.h"
#include <string>
#include <vector>
using namespace std;

//歌曲集合
typedef vector<string>		RidVector;

//下载通知码与状态码
enum DownloadNotifyCode
{						//状态码说明						用途
	DNC_ERROR = 0,		//P2P错误						通知码/状态码
	DNC_DOWNLOAD,		//已提交下载						通知码
	DNC_PAUSE,			//暂停下载						通知码/状态码
	DNC_WAITING,		//已满足条件，但未提交P2P		通知码/状态码
	DNC_RESTART,		//重新开始下载						通知码				
	DNC_START,			//已提交P2P						通知码/状态码
	DNC_DOWNING,		//下载中							状态码
	DNC_DELETE,			//删除下载							通知码
	DNC_DECOMPRESS,		// 解压
	DNC_FINISHED,		//完成下载							通知码
	DNC_ADD,
	DNC_EXISTED,
	DNC_DATACHANGE
};

//每个下载文件的下载状态
struct DownState
{
	DownloadNotifyCode	dnc;//当前状态
};
