
// testiceclientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testiceclient.h"
#include "testiceclientDlg.h"
#include "afxdialogex.h"


#include <Ice/Ice.h>
#include "WsICEService.h"
#include "RobotRemoteService.h"
#include "Tools.h"
#include "WsICEServiceI.h"
#include "RunTimeICEService.h"
#include <iostream>
#include <sstream>
using namespace com::pera::base::runtime::remote;
using namespace FlexwareRuntimeICEService;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtesticeclientDlg 对话框




CtesticeclientDlg::CtesticeclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtesticeclientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtesticeclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtesticeclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtesticeclientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtesticeclientDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CtesticeclientDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CtesticeclientDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CtesticeclientDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CtesticeclientDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CtesticeclientDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CtesticeclientDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CtesticeclientDlg 消息处理程序

BOOL CtesticeclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetDlgItemText(IDC_EDIT1, "RobotRemoteService:tcp -h localhost -p 20133");

	SetDlgItemText(IDC_EDIT3, "WsICEService:tcp -h localhost -p 20131");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CtesticeclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CtesticeclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CtesticeclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtesticeclientDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT1, strEndPoint);
	CString strInfoTest;
	GetDlgItemText(IDC_EDIT2, strInfoTest);

	Ice::CommunicatorPtr communicator;

	/*
	Json对象的内容如下所示
	{"id":"001",
	"taskName":"任务1",
	"robotSvn":"\robot\002\组件a.bot",
	"inputSvn":"\files\input.xml",
	" taskUser ":"admin",
	"assignNode":"Node01",
	"taskInfo":"表单名称",
	"processName":"流程名称",
	"isForm":"1"}

	具体属性说明如下：
	Id:唯一标识
	taskName:任务名称
	robotSvn：bot文件的svn相对路径
	inputSvn：input文件的svn相对路径
	taskUser：任务执行人
	assignNode：远程计算节点的名称
	taskInfo：表单信息
	processName：流程名称
	isForm：是否表单
	*/

	/*
	{\"id\":\"390219#1384164301692#4\",\"inputFile\":\"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\\r\\n<Parameters>\\r\\n<Package InstanceID=\\\"92bfe440-87ee-4761-9658-9ac4ee6f8822\\\">\\r\\n<Parameter id=\\\"60081a9c-846b-4c52-b6fc-6caa007f5f16\\\" name=\\\"y\\\" type=\\\"Double\\\" value=\\\"0\\\" />\\r\\n</Package>\\r\\n</Parameters>\\r\\n\",\"manifestFile\":\"5280948e860b878ff96b2047\",\"robotFile\":{\"fileID\":\"5280948f860b878ff96b2057\",\"fileName\":\"公式组件_3.bot\"},\"taskName\":\"公式组件_3\"}
	*/
	Json::Value jInfo;  // 表示整个 json 对象
	jInfo["id"] = Json::Value(ZTools::GetCurrentTimeString16());
	jInfo["taskName"] = Json::Value("dp");
	Json::Value jRobot;
	jRobot["fileID"] = Json::Value("51ef8871a69c710437184891");
	jRobot["fileName"] = Json::Value("xxx.bot");
	jInfo["robotSvn"] = jRobot;
	Json::Value jInput;
	jInput["fileID"] = Json::Value("51ef899fa69c710437184895");
	jInput["fileName"] = Json::Value("in.xml");
	jInfo["inputSvn"] = jInput;
	jInfo["taskUser"] = Json::Value("f");
	jInfo["assignNode"] = Json::Value("pc1");
	jInfo["taskInfo"] = Json::Value("测");
	jInfo["processName"] = Json::Value("测试流程");
	jInfo["isForm"] = Json::Value("0");

	Json::FastWriter writer;
	std::string strInfo = writer.write(jInfo);
	for (int i = 0; i < 500; i++)
	{

		strInfo = ZTools::FormatString("{\"id\":\"%d#%s\",\"inputFile\":\"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\\r\\n<Parameters>\\r\\n<Package InstanceID=\\\"92bfe440-87ee-4761-9658-9ac4ee6f8822\\\">\\r\\n<Parameter id=\\\"60081a9c-846b-4c52-b6fc-6caa007f5f16\\\" name=\\\"y\\\" type=\\\"Double\\\" value=\\\"0\\\" />\\r\\n</Package>\\r\\n</Parameters>\\r\\n\",\"manifestFile\":\"5280948e860b878ff96b2047\",\"robotFile\":{\"fileID\":\"5280948f860b878ff96b2057\",\"fileName\":\"公式组件_3.bot\"},\"taskName\":\"公式组件_3\"}", i, ZTools::GetCurrentTimeString16().c_str());


		ZTools::MBToUTF8(strInfo);

		try
		{
			communicator = Ice::initialize();
			RobotRemoteServicePrx robotRemoteService = RobotRemoteServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
			//std::string strReturn = robotRemoteService->runRobotStart(strInfo);
			//std::string strstrInfoTest = (LPCTSTR)strInfoTest;
			//ZTools::MBToUTF8(strstrInfoTest);
			//std::string strReturn = robotRemoteService->runRobotStart(strstrInfoTest);
			std::string strReturn = robotRemoteService->runRobotStart(strInfo);
			//OutputDebugString(strReturn.c_str());
			communicator->destroy();
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
			try
			{
				if(communicator)
				{
					communicator->destroy();
				}
			}
			catch(const Ice::Exception& ex)
			{
				//cerr << ex << endl;
			}
		}
	}
}


void CtesticeclientDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT1, strEndPoint);

	Ice::CommunicatorPtr communicator;

	/*
	Json对象的内容如下所示
	{"id":"001",
	"taskName":"任务1",
	"robotSvn":"\robot\002\组件a.bot",
	"inputSvn":"\files\input.xml",
	" taskUser ":"admin",
	"assignNode":"Node01",
	"taskInfo":"表单名称",
	"processName":"流程名称",
	"isForm":"1"}

	具体属性说明如下：
	Id:唯一标识
	taskName:任务名称
	robotSvn：bot文件的svn相对路径
	inputSvn：input文件的svn相对路径
	taskUser：任务执行人
	assignNode：远程计算节点的名称
	taskInfo：表单信息
	processName：流程名称
	isForm：是否表单
	*/
	Json::Value jInfo;  // 表示整个 json 对象
	jInfo["id"] = Json::Value("id002");
	jInfo["taskName"] = Json::Value("报告");
	jInfo["robotSvn"] = Json::Value("/svn/template/component/18858/matlab_word.bot");
	jInfo["inputSvn"] = Json::Value("/Robot/19160/input.xml");
	jInfo["taskUser"] = Json::Value("f");
	jInfo["assignNode"] = Json::Value("pc1");
	jInfo["taskInfo"] = Json::Value("测");
	jInfo["processName"] = Json::Value("测试流程");
	jInfo["isForm"] = Json::Value("1");

	Json::FastWriter writer;
	std::string strInfo = writer.write(jInfo);
	ZTools::MBToUTF8(strInfo);

	try
	{
		communicator = Ice::initialize();
		RobotRemoteServicePrx robotRemoteService = RobotRemoteServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		std::string strReturn = robotRemoteService->runRobotStart(strInfo);
		OutputDebugString(strReturn.c_str());
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		try
		{
			if(communicator)
			{
				communicator->destroy();
			}
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
		}
	}
}


void CtesticeclientDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		std::string strTaskId("taskid汉字");
		ZTools::MBToUTF8(strTaskId);
		std::string strOutxml("output.xml汉字");
		ZTools::MBToUTF8(strOutxml);
		std::string strReturn = wsICEServicePrx->runRobotFinished(strTaskId, strOutxml, "");
		OutputDebugString(strReturn.c_str());
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		try
		{
			if(communicator)
			{
				communicator->destroy();
			}
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
		}
	}
}


void CtesticeclientDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		wsICEServicePrx->runRobotLog("taskid汉字");
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		try
		{
			if(communicator)
			{
				communicator->destroy();
			}
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
		}
	}
}


void CtesticeclientDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		wsICEServicePrx->runRobotRate("taskid汉字");
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		try
		{
			if(communicator)
			{
				communicator->destroy();
			}
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
		}
	}
}


void CtesticeclientDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE);

	m_communicator = Ice::initialize();
	Ice::ObjectAdapterPtr adapter = m_communicator->createObjectAdapterWithEndpoints("WsICEService", "tcp -h 0.0.0.0 -p 20131");
	Ice::ObjectPtr servant = new WsICEServiceI();
	adapter->add(servant, m_communicator->stringToIdentity("WsICEService"));
	adapter->activate();
}


void CtesticeclientDlg::OnBnClickedButton7()
{
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		std::string strJson("json汉字");
		ZTools::MBToUTF8(strJson);
		ByteSeq bs;
		for (int i = 1; i < 255; i++)
		{
			bs.push_back(i);
		}
		
		std::string strReturn = wsICEServicePrx->savePlanByByteXpdl(strJson, bs);
		OutputDebugString(strReturn.c_str());
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		try
		{
			if(communicator)
			{
				communicator->destroy();
			}
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
		}
	}
}


void CtesticeclientDlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strEndPoint("FlexwareRuntimeICEService:tcp -h 127.0.0.1 -p 20139");
	CString sInfo;
	GetDlgItemText(IDC_EDIT4, sInfo);
	std::string strInfo = (LPCTSTR)sInfo;

	CString sInxml;
	GetDlgItemText(IDC_EDIT5, sInxml);
	std::string strInxml = (LPCTSTR)sInxml;

	Json::Value jValue;
	Json::Reader jReader;
	if (jReader.parse(strInfo, jValue))
	{
		if (!strInxml.empty())
		{
			jValue["inXml"] = Json::Value(strInxml);
		}
		strInfo = jValue.toStyledString();
	}
	else
	{
		MessageBox("json 格式有误");
		return;
	}

	ZTools::MBToUTF8(strInfo);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		RunTimeICEServicePrx runTimeICEServicePrx = RunTimeICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		runTimeICEServicePrx->RunRobot(strInfo, 0);
		communicator->destroy();
	}
	catch(const Ice::Exception& ex)
	{
		//cerr << ex << endl;
		try
		{
			if(communicator)
			{
				communicator->destroy();
			}
		}
		catch(const Ice::Exception& ex)
		{
			//cerr << ex << endl;
		}
	}
}
