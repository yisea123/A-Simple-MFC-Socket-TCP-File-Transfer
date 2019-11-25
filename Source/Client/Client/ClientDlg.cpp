// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "MainDlg.h"
#include "Xuly.h"
#include "RegisterDlg.h"
#include <vector>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg dialog
std::vector <CString> List;
CString Item_Picked;
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientDlg::IDD, pParent)
, m_msgString(_T(""))
, IP(_T(""))
, m_userName(_T(""))
, m_password(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER, m_userName);
	DDX_Text(pDX, IDC_Pass, m_password);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE (WM_SOCKET, SockMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGIN, &CClientDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_LOGOUT, &CClientDlg::OnBnClickedLogout)


	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CClientDlg::OnBnClickedButtonRegister)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	IP= ("127.0.0.1");
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::Split(CString src,CString des[2])
{
	int p1,p2;

	p1=src.Find(_T("\r\n"),0);
	des[0]=src.Mid(0,p1);

	p2=src.Find(_T("\r\n"),p1+1);
	des[1]=src.Mid(p1+2,p2-(p1+2));

}

char* CClientDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize+1];
	memset(pAnsiString,0,nSize+1);
	wcstombs(pAnsiString, s, nSize+1);
	return pAnsiString;
}

void CClientDlg::mSend(CString Command)
{
	int Len=Command.GetLength();
	Len+=Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff,0,1000);
	memcpy(sendBuff,(PBYTE)(LPCTSTR)Command, Len);
	send(sClient,(char*)&Len,sizeof(Len),0);
	send(sClient,(char*)sendBuff,Len,0);
	delete[] sendBuff;
}

int CClientDlg::mRecv(CString &Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer,0, 1000);
	recv(sClient ,(char*)&buffLength,sizeof(int),0);
	recv(sClient,(char*)buffer,buffLength,0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;
	delete[] buffer;
	if(Command.GetLength()==0)
		return -1 ;
	return 0;
}

LRESULT CClientDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_READ:
		{
			CString temp;
			if (mRecv(temp) < 0)
				break;

			Split(temp, strResult);
			int flag1 = _ttoi(strResult[0]);
			
			switch (flag1)
			{
				case 1: //login
				{
					if (strResult[1] == "1")
					{
						/*m_msgString += _T("Dang nhap thanh cong\r\n");
						GetDlgItem(IDC_LOGOUT)->EnableWindow(TRUE);
						GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);*/
						MainDlg main(this->sClient,m_msgString);
						EndDialog(1);
						main.DoModal();
					}
					else MessageBox(_T("Login Failed!"), _T("Error"), MB_OK | MB_ICONERROR);
						//m_msgString += _T("Dang nhap that bai\r\n");

					UpdateData(FALSE);
					break;
				}
			}
			break;
		}
		case FD_CLOSE:
		{
			closesocket(sClient);
			MessageBox(_T("Server has been shutdown!"), _T("Error"), MB_OK | MB_ICONERROR);
			break;
		}
	}
	return 0;
}
void CClientDlg::OnBnClickedLogin()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if(m_userName=="" || m_password == "")
	{
		MessageBox(_T("Vui long nhap ten user"));
		return;
	}

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	hostent* host=NULL;
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(_T("socket() failed"),_T("ERROR"), 0);
		return;
	}

	servAdd.sin_family = AF_INET;
	servAdd.sin_port = htons(PORT);

	char* cIP=ConvertToChar(IP);

	servAdd.sin_addr.s_addr = inet_addr(cIP);

	CStringA cpy_IP(IP); 

	if (servAdd.sin_addr.s_addr==INADDR_NONE)
	{
		host = (gethostbyname(cpy_IP));
		if (host == NULL)
		{
			MessageBox(_T("Khong the ket noi den server."),_T("ERROR"), 0);
		}
		CopyMemory(&servAdd.sin_addr, host->h_addr_list[0],
			host->h_length);
		return;
	}

	int err=connect(sClient,(struct sockaddr*)&servAdd,sizeof(servAdd));
	if (err==SOCKET_ERROR){
		MessageBox(_T("Ket noi that bai"),_T("ERROR"), 0);
		return;
	}

	Command=_T("1\r\n");
	Command+= m_userName+ _T(" ") + m_password +_T("\r\n");

	mSend(Command); //Gui thong tin username + password ve cho server
	m_msgString = m_userName;
	WSAAsyncSelect(sClient,m_hWnd,WM_SOCKET,FD_READ|FD_CLOSE);
	UpdateData(FALSE);
	
}

void CClientDlg::OnBnClickedLogout()
{
	

}

void CClientDlg::OnBnClickedAdd()
{
	
}

void CClientDlg::OnBnClickedSubtract()
{
}



void CClientDlg::OnBnClickedButtonRegister()
{
	// TODO: Add your control notification handler code here
	RegisterDlg reg;
	reg.DoModal();
}



