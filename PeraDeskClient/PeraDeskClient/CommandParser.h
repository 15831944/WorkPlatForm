#pragma once

//网页中启动个人工作台时，取得对应的JSON串的方法

//PS:如果命令行参数为-i，则将自定义协议写入注册表，如果命令行参数为-u，则将自定义协议从注册表删除

/*
网页中启动个人工作台接口文档

网页中调用示例：

<HTML><HEAD><META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=utf-8">		</HEAD>
<BODY>
<a href='PeraDC:{"userName":"admin","realName":"超级管理员","ticket-proxy":"aaabbbcccddd","mainUrl":"http://www.baidu.com/baidu?word=%25E6%25B1%2589%25E5%25AD%2597"}'>个人工作平台</>
</BODY></HTML>

格式说明：
采用自定义协议的超链接方式启动个人工作平台
超链接以PeraDC:开头
超链接字符上限500
超链接除去头部外是个json格式的字符串
{
"userName": "admin",
"realName": "超级管理员",
"ticket-proxy": "aaabbbcccddd",
"mainUrl": "http://www.baidu.com/baidu?word=%E6%B1%89%E5%AD%97"
}
对象至少有userName、realName、ticket-proxy、mainUrl四个属性
json格式的字符串中如出现%，需要使用%25进行转义
PeraDC:{"userName":"admin","realName":"超级管理员","ticket-proxy":"aaabbbcccddd","mainUrl":"http://www.baidu.com/baidu?word=%25E6%25B1%2589%25E5%25AD%2597"}
*/

//完整命令行
extern CString g_sCommandLine;

//取出的json串
extern CString g_sParamLine;

//解析json串，只有满足自定义协议或者不传命令行参数才返回TRUE，其它返回FALSE
//如果命令行参数为-i，则将自定义协议写入注册表，如果命令行参数为-u，则将自定义协议从注册表删除，同时返回FALSE
BOOL ParseJsonCommandLine();

class CCxPeraDCData
{
public:
	CCxPeraDCData();
	virtual ~CCxPeraDCData();
	BOOL Parse( LPCTSTR lpszData );
public:
	void Clear(void);
	CString m_sUrl;
	CString m_sTicket;
	CString m_sUser;
	CString m_sRealName;
};