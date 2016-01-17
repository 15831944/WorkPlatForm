
// testiceclientDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CtesticeclientDlg �Ի���




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


// CtesticeclientDlg ��Ϣ�������

BOOL CtesticeclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetDlgItemText(IDC_EDIT1, "RobotRemoteService:tcp -h localhost -p 20133");

	SetDlgItemText(IDC_EDIT3, "WsICEService:tcp -h localhost -p 20131");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtesticeclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtesticeclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtesticeclientDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT1, strEndPoint);
	CString strInfoTest;
	GetDlgItemText(IDC_EDIT2, strInfoTest);

	Ice::CommunicatorPtr communicator;

	/*
	Json���������������ʾ
	{"id":"001",
	"taskName":"����1",
	"robotSvn":"\robot\002\���a.bot",
	"inputSvn":"\files\input.xml",
	" taskUser ":"admin",
	"assignNode":"Node01",
	"taskInfo":"������",
	"processName":"��������",
	"isForm":"1"}

	��������˵�����£�
	Id:Ψһ��ʶ
	taskName:��������
	robotSvn��bot�ļ���svn���·��
	inputSvn��input�ļ���svn���·��
	taskUser������ִ����
	assignNode��Զ�̼���ڵ������
	taskInfo������Ϣ
	processName����������
	isForm���Ƿ��
	*/

	/*
	{\"id\":\"390219#1384164301692#4\",\"inputFile\":\"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\\r\\n<Parameters>\\r\\n<Package InstanceID=\\\"92bfe440-87ee-4761-9658-9ac4ee6f8822\\\">\\r\\n<Parameter id=\\\"60081a9c-846b-4c52-b6fc-6caa007f5f16\\\" name=\\\"y\\\" type=\\\"Double\\\" value=\\\"0\\\" />\\r\\n</Package>\\r\\n</Parameters>\\r\\n\",\"manifestFile\":\"5280948e860b878ff96b2047\",\"robotFile\":{\"fileID\":\"5280948f860b878ff96b2057\",\"fileName\":\"��ʽ���_3.bot\"},\"taskName\":\"��ʽ���_3\"}
	*/
	Json::Value jInfo;  // ��ʾ���� json ����
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
	jInfo["taskInfo"] = Json::Value("��");
	jInfo["processName"] = Json::Value("��������");
	jInfo["isForm"] = Json::Value("0");

	Json::FastWriter writer;
	std::string strInfo = writer.write(jInfo);
	for (int i = 0; i < 500; i++)
	{

		strInfo = ZTools::FormatString("{\"id\":\"%d#%s\",\"inputFile\":\"<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\"?>\\r\\n<Parameters>\\r\\n<Package InstanceID=\\\"92bfe440-87ee-4761-9658-9ac4ee6f8822\\\">\\r\\n<Parameter id=\\\"60081a9c-846b-4c52-b6fc-6caa007f5f16\\\" name=\\\"y\\\" type=\\\"Double\\\" value=\\\"0\\\" />\\r\\n</Package>\\r\\n</Parameters>\\r\\n\",\"manifestFile\":\"5280948e860b878ff96b2047\",\"robotFile\":{\"fileID\":\"5280948f860b878ff96b2057\",\"fileName\":\"��ʽ���_3.bot\"},\"taskName\":\"��ʽ���_3\"}", i, ZTools::GetCurrentTimeString16().c_str());


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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT1, strEndPoint);

	Ice::CommunicatorPtr communicator;

	/*
	Json���������������ʾ
	{"id":"001",
	"taskName":"����1",
	"robotSvn":"\robot\002\���a.bot",
	"inputSvn":"\files\input.xml",
	" taskUser ":"admin",
	"assignNode":"Node01",
	"taskInfo":"������",
	"processName":"��������",
	"isForm":"1"}

	��������˵�����£�
	Id:Ψһ��ʶ
	taskName:��������
	robotSvn��bot�ļ���svn���·��
	inputSvn��input�ļ���svn���·��
	taskUser������ִ����
	assignNode��Զ�̼���ڵ������
	taskInfo������Ϣ
	processName����������
	isForm���Ƿ��
	*/
	Json::Value jInfo;  // ��ʾ���� json ����
	jInfo["id"] = Json::Value("id002");
	jInfo["taskName"] = Json::Value("����");
	jInfo["robotSvn"] = Json::Value("/svn/template/component/18858/matlab_word.bot");
	jInfo["inputSvn"] = Json::Value("/Robot/19160/input.xml");
	jInfo["taskUser"] = Json::Value("f");
	jInfo["assignNode"] = Json::Value("pc1");
	jInfo["taskInfo"] = Json::Value("��");
	jInfo["processName"] = Json::Value("��������");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		std::string strTaskId("taskid����");
		ZTools::MBToUTF8(strTaskId);
		std::string strOutxml("output.xml����");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		wsICEServicePrx->runRobotLog("taskid����");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strEndPoint;
	GetDlgItemText(IDC_EDIT3, strEndPoint);

	Ice::CommunicatorPtr communicator;

	try
	{
		communicator = Ice::initialize();
		WsICEServicePrx wsICEServicePrx = WsICEServicePrx::checkedCast(communicator->stringToProxy(strEndPoint.GetBuffer()));
		wsICEServicePrx->runRobotRate("taskid����");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		std::string strJson("json����");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
		MessageBox("json ��ʽ����");
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
