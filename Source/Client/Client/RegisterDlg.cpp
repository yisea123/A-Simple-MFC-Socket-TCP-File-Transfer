// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "ClientDlg.h"

// RegisterDlg dialog

IMPLEMENT_DYNAMIC(RegisterDlg, CDialogEx)

RegisterDlg::RegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(REGISTERDLG, pParent)
{

}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER, edit_user);
	DDX_Control(pDX, IDC_EDIT_PASS, edit_pass);
	DDX_Control(pDX, IDC_EDIT_REPASS, edit_repass);
}


BEGIN_MESSAGE_MAP(RegisterDlg, CDialogEx)
	ON_MESSAGE(WM_SOCKET, SockMsg)

	ON_BN_CLICKED(IDC_BUTTON_REG, &RegisterDlg::OnBnClickedButtonReg)
END_MESSAGE_MAP()


// RegisterDlg message handlers
void RegisterDlg::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

}

char* RegisterDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void RegisterDlg::mSend(CString Command)
{
	int Len = Command.GetLength();
	Len += Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff, 0, 1000);
	memcpy(sendBuff, (PBYTE)(LPCTSTR)Command, Len);
	send(sClient, (char*)&Len, sizeof(Len), 0);
	send(sClient, (char*)sendBuff, Len, 0);
	delete[] sendBuff;
}

int RegisterDlg::mRecv(CString &Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer, 0, 1000);
	recv(sClient, (char*)&buffLength, sizeof(int), 0);
	recv(sClient, (char*)buffer, buffLength, 0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;
	delete[] buffer;
	if (Command.GetLength() == 0)
		return -1;
	return 0;
}

LRESULT RegisterDlg::SockMsg(WPARAM wParam, LPARAM lParam)
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
		case 2:
			if (strResult[1] == "1")
			{
				/*m_msgString += _T("Dang nhap thanh cong\r\n");
				GetDlgItem(IDC_LOGOUT)->EnableWindow(TRUE);
				GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);*/
				MessageBox(_T("Register Success!"));
				EndDialog(1);
				
			} 
			else MessageBox(_T("Username has already taken!"), _T("Error"), MB_OK | MB_ICONERROR);
			//m_msgString += _T("Dang nhap that bai\r\n");

	
			break;
		}
		
	}
	case FD_CLOSE:
	
		//closesocket(sClient);
		//m_msgString += _T("Server da dong ket noi\r\n");
		//GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
		//GetDlgItem(IDC_LOGIN)->EnableWindow(TRUE);
		UpdateData(FALSE);
		break;

	}
	return 0;
}

void RegisterDlg::OnBnClickedButtonReg()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString inputName;
	CString inputPass;
	CString inputRepass;
	edit_user.GetWindowText(inputName);
	edit_pass.GetWindowText(inputPass);
	edit_repass.GetWindowText(inputRepass);

	CString IP = _T("127.0.0.1");
	if (inputName == "" || inputPass == "" || inputRepass == "")
	{
		MessageBox(_T("Vui long nhap ten user!"));
		return;
	}

	if (inputPass != inputRepass)
	{
		MessageBox(_T("Mat khau nhap lai khong khop!"));
		return;
	}

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	hostent* host = NULL;
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(_T("socket() failed"), _T("ERROR"), 0);
		return;
	}

	servAdd.sin_family = AF_INET;
	servAdd.sin_port = htons(PORT);

	char* cIP = ConvertToChar(IP);

	servAdd.sin_addr.s_addr = inet_addr(cIP);

	CStringA cpy_IP(IP);

	if (servAdd.sin_addr.s_addr == INADDR_NONE)
	{
		host = (gethostbyname(cpy_IP));
		if (host == NULL)
		{
			MessageBox(_T("Khong the ket noi den server."), _T("ERROR"), 0);
		}
		CopyMemory(&servAdd.sin_addr, host->h_addr_list[0],
			host->h_length);
		return;
	}

	int err = connect(sClient, (struct sockaddr*)&servAdd, sizeof(servAdd));
	if (err == SOCKET_ERROR) {
		MessageBox(_T("Ket noi that bai"), _T("ERROR"), 0);
		return;
	}

	Command = _T("2\r\n");
	Command += inputName + _T(" ") + inputPass + _T("\r\n");

	mSend(Command); //Gui thong tin username + password ve cho server

	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);

}
