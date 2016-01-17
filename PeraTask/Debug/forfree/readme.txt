com主dll:
PeraPcIceClient.dll


注册命令为：管理员下运行
regsvr32 PeraPcIceClient.dll
反注册
regsvr32 PeraPcIceClient.dll /u

public virtual dynamic RunRobotFinished(string bstrJsonFlag, string bstrTaskId, string bstrOutxmlPath)
public virtual dynamic RunRobotLog(string bstrJsonFlag, string bstrLogInfo)
public virtual dynamic RunRobotRate(string bstrJsonFlag, string bstrRateInfo)
第一个参数均为ICE服务收到网络传来的任务，启动grcrunner时生成的标识，原封不动传回即可
其它参数详见Word文档中的说明