
申请ID是本地计算
封装一个函数，计算好的放在reg里

安装时，nsis把0，4写在reg里
程序启动后，检查出0和4，根据注册表里标记，知道3，timer地发送1，
卸载时用exe发送一个卸载消息

以上功能都封装在一个dll里，在程序中调用dll的一个函数即可，dll的函数里起线程去做这个事

推送系统pushbox做了

离线卸载怎么弄？

用户日志上传也在dll里，只记录上次上传的时间就行，如果这个时间是未来的时间，就认为无效，2小时之内不要再次上传日志

IsUpdate什么意思？Run不是立即运行，那是什么时候运行？
为什么要给安装包大小？

1、记录用户安装卸载等记录
http://g.najiuwan.com/i/i.php?a=1&id=6c626dff145e283564&qd=ldt011&v=1.49
get_info.php?a=1&id=6c626dff145e283564&qd=ldt011&v=1.49
 
用户情况:   $a （0：安装 1：在线 2：卸载 3：新用户第一次启动 4：覆盖安装 5：离线卸载[卸载时没有网络，
有网络后提交] ）
用户ID:   $id 为从svr申请到的ID
下载源:     $qd 渠道版本好
程序版本：  $v

2、推送系统
卸载时本地记录一个卸载时间untime

第一次向服务器请求任务
addpush.php?id=0026c75acde4&type=unldt&qd=ldt007&v=1.40&untime=2012-06-22

$id = G("id");         // svr申请来的ID
$about = G("type");      // type: inldt为 未卸载的正常用户，unldt为 已卸载掉的用户
$iqudao = G("qd");    // 渠道信息
$dd_ver = G("v");      // 客户端版本
$un_time = G("untime");  // 用户卸载日期（可以为 时间戳）

服务器返回：
一些信息客户端验证后重新拼接url向服务器提交

第二次向服务器获得任务
action.php?itask=3&task_name=up007&task_ver=1.30&id=0026c75acde4&type=unldt

然后服务器推给客户端真正的下载地址

3、用户行为分析
getinfo.php?id=000000000001&url=http://iuser.duoduobox.com
此模块为记录用户浏览器的访问日志。

4、升级模块
升级请求增加ID，这样可以实现按ID来局部升级
up.php?type=ldt&id=<id>&qd=ldt111&v=1.10&instime=2012-06-12

返回的格式:
[Update]
url=[http://ssss.com/xxx.exe][]
IsUpdate=[True:1][False:0]
Ver=[2.00] 服务器上的版本信息.
allsize=xxxx // 升级包的大小.(字节)
Run=[0][1]  // 是否立即运行.



function check_num($mac){
    $i_number = array('a'=>10, 'b'=>11, 'c'=>12, 'd'=>13, 'e'=>14, 'f'=>15);
    $mac_key = substr($mac, 12, 17);

    for($i=0;$i<6;$i++){
        $c = $i * 2 + 1;
        $k = $mac[$c];
        $v = $mac_key[$i];
        if(!preg_match('/\d+/',$k)){
            $k = $i_number[$k];
        }
    
        if(!preg_match('/\d+/',$v)){
            $v = $i_number[$v];
        }
    
        if($k>10){
            $num = $k - $v;
        }else{
            $num = $k + $v;
        }
    
        if("$num" !== "10"){
            //echo "验证失败!</br>";
            exit;
        }else{
            //echo "有效mac地址!</br>";
        }
    }
    
    $mac = substr($mac, 0, 12);
    return $mac;
}


1小夏的安装包
3和server端的对接
4主程序的icogaochaohua

test