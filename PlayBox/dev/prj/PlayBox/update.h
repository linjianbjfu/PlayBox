/* 
该函数负责检查是否有明示升级包，之后还完成更新服务器列表，下载升级包等工作
*/

#include "windows.h"
#include <string>
using namespace std;

void checkUpdate(const char* player);//更新服务器列表，下载升级包等
void checkPopUpUpdate(HWND *phWnd = NULL);	//检测是否有明示升级包
//上边的三个函数，头两个放在组件启动处，最后一个放在组件退出处
//下边的函数是被上边三个函数调用的
void realCheckLastWrongExit(LPVOID pparam); //检查播放器上次是否正常退出
void runPopUpUpdate(LPVOID pparam);//检查是否有明示升级包，有则运行安装包，并退出播放器
void realUpdate(LPVOID pparam);	//具体操作更新服务器列表，下载升级包等工作
void stopCheckUpdate();