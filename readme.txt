
����ID�Ǳ��ؼ���
��װһ������������õķ���reg��

��װʱ��nsis��0��4д��reg��
���������󣬼���0��4������ע������ǣ�֪��3��timer�ط���1��
ж��ʱ��exe����һ��ж����Ϣ

���Ϲ��ܶ���װ��һ��dll��ڳ����е���dll��һ���������ɣ�dll�ĺ��������߳�ȥ�������

����ϵͳpushbox����

����ж����ôŪ��

�û���־�ϴ�Ҳ��dll�ֻ��¼�ϴ��ϴ���ʱ����У�������ʱ����δ����ʱ�䣬����Ϊ��Ч��2Сʱ֮�ڲ�Ҫ�ٴ��ϴ���־

IsUpdateʲô��˼��Run�����������У�����ʲôʱ�����У�
ΪʲôҪ����װ����С��

1����¼�û���װж�صȼ�¼
http://g.najiuwan.com/i/i.php?a=1&id=6c626dff145e283564&qd=ldt011&v=1.49
get_info.php?a=1&id=6c626dff145e283564&qd=ldt011&v=1.49
 
�û����:   $a ��0����װ 1������ 2��ж�� 3�����û���һ������ 4�����ǰ�װ 5������ж��[ж��ʱû�����磬
��������ύ] ��
�û�ID:   $id Ϊ��svr���뵽��ID
����Դ:     $qd �����汾��
����汾��  $v

2������ϵͳ
ж��ʱ���ؼ�¼һ��ж��ʱ��untime

��һ�����������������
addpush.php?id=0026c75acde4&type=unldt&qd=ldt007&v=1.40&untime=2012-06-22

$id = G("id");         // svr��������ID
$about = G("type");      // type: inldtΪ δж�ص������û���unldtΪ ��ж�ص����û�
$iqudao = G("qd");    // ������Ϣ
$dd_ver = G("v");      // �ͻ��˰汾
$un_time = G("untime");  // �û�ж�����ڣ�����Ϊ ʱ�����

���������أ�
һЩ��Ϣ�ͻ�����֤������ƴ��url��������ύ

�ڶ�����������������
action.php?itask=3&task_name=up007&task_ver=1.30&id=0026c75acde4&type=unldt

Ȼ��������Ƹ��ͻ������������ص�ַ

3���û���Ϊ����
getinfo.php?id=000000000001&url=http://iuser.duoduobox.com
��ģ��Ϊ��¼�û�������ķ�����־��

4������ģ��
������������ID����������ʵ�ְ�ID���ֲ�����
up.php?type=ldt&id=<id>&qd=ldt111&v=1.10&instime=2012-06-12

���صĸ�ʽ:
[Update]
url=[http://ssss.com/xxx.exe][]
IsUpdate=[True:1][False:0]
Ver=[2.00] �������ϵİ汾��Ϣ.
allsize=xxxx // �������Ĵ�С.(�ֽ�)
Run=[0][1]  // �Ƿ���������.



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
            //echo "��֤ʧ��!</br>";
            exit;
        }else{
            //echo "��Чmac��ַ!</br>";
        }
    }
    
    $mac = substr($mac, 0, 12);
    return $mac;
}


1С�ĵİ�װ��
3��server�˵ĶԽ�
4�������icogaochaohua

test