// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include "ClientDlg.h"

// MainDlg dialog

IMPLEMENT_DYNAMIC(MainDlg, CDialogEx)

MainDlg::MainDlg(SOCKET s,CString name,CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAINDLG, pParent)
{
	sClient = s;
	
	m_msgString = _T("You are logging in as username: ") + name;

}

MainDlg::~MainDlg()
{
}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_msgString);
	DDX_Control(pDX, IDC_LIST1, listFile);
}


BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
	
	ON_MESSAGE(WM_SOCKET, SockMsg)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &MainDlg::OnBnClickedButtonDownload)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &MainDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &MainDlg::OnBnClickedButtonLogout)
END_MESSAGE_MAP()


// MainDlg message handlers
void MainDlg::Split(CString src, CString des[2])
{
	int p1, p2;

	p1 = src.Find(_T("\r\n"), 0);
	des[0] = src.Mid(0, p1);

	p2 = src.Find(_T("\r\n"), p1 + 1);
	des[1] = src.Mid(p1 + 2, p2 - (p1 + 2));

}

char* MainDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize + 1];
	memset(pAnsiString, 0, nSize + 1);
	wcstombs(pAnsiString, s, nSize + 1);
	return pAnsiString;
}

void MainDlg::mSend(CString Command)
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

int MainDlg::mRecv(CString &Command)
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

LRESULT MainDlg::SockMsg(WPARAM wParam, LPARAM lParam)
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
		case 3: //Refresh file list

			listFile.InsertItem(0, strResult[1]);
			break;
		case 4: //receive file from server
			//char* fileName = ConvertToChar(strResult[1]);
			char* c_port = ConvertToChar(strResult[1]);
			int iPort = atoi(c_port);
			CString cs_port(c_port);
			if (receiveFile(downloadFileName,iPort))	
				MessageBox(_T("Download succeed!"));
			else MessageBox(_T("Download failed!"));
			
			break;
		}

	}

	}
	return 0;
}


void MainDlg::OnBnClickedButtonDownload()
{
	// TODO: Add your control notification handler code here
	for (int nItem = 0; nItem < listFile.GetItemCount(); nItem++)
	{
		if (listFile.GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CString fileName = listFile.GetItemText(nItem, 0);
			CString cs = _T("You have downloaded file: ");
			downloadFileName = ConvertToChar(fileName);
			cs += fileName;
			Command = _T("4\r\n") + fileName + _T("\r\n");
			mSend(Command);
			WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
			//MessageBox(cs);
		}
	}
}


void MainDlg::OnBnClickedButtonRefresh()
{
	listFile.DeleteAllItems();
	Command = _T("5\r\n");
	mSend(Command); //Gui thong tin username + password ve cho server
	WSAAsyncSelect(sClient, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	UpdateData(FALSE);

}


void MainDlg::OnBnClickedButtonLogout()
{
	INT_PTR i = MessageBox(_T("Ban muon logout?"), _T("Confirm"), MB_OKCANCEL);
	if (i == IDCANCEL) return;
	closesocket(sClient);
	EndDialog(1);
}

//------------------------File transfer implementation------------------------
bool MainDlg::receiveFile(char* file_name,int port)
{
	/*if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));

	}
	else*/
	{
		// TODO: code your application's behavior here.

		// Khoi tao Thu vien
		if (AfxSocketInit() == FALSE)
		{
			//cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		// Tao socket dau tien
		CSocket ClientSocket;
		ClientSocket.Create();

		// Ket noi den Server
		if (ClientSocket.Connect(_T("127.0.0.1"), port) != 0)
		{
			//cout << "Ket noi toi Server thanh cong !!!" << endl << endl;
			///-------------------------------------------------------------------------------
			/// Tham so truyen vao: char* file_name, CSocket ClientSocket
			///-----------------------------------------------------------------------------------
			// Khai bao
			int file_size = 0, bytes_recevived, bytes_to_receive;
			byte* buffer = NULL;

			// Mo file
			FILE* fo = fopen(file_name, "wb");

			// Nhan kich thuoc file
			int size = 0;
			bytes_to_receive = sizeof(file_size);
			do
			{
				size = (file_size)+sizeof(file_size) - bytes_to_receive;
				bytes_recevived = recv(ClientSocket, (char*)&size, bytes_to_receive, 0);

				if (bytes_recevived == SOCKET_ERROR || bytes_recevived == 0)
					return 0;

				bytes_to_receive -= bytes_recevived;
			} while (bytes_to_receive > 0);

			// Nhan file's data
			buffer = new byte[RECV_BUFFER_SIZE];
			bytes_to_receive = size;

			do {
				int buffer_size;
				buffer_size = (RECV_BUFFER_SIZE > bytes_to_receive) ? bytes_to_receive : RECV_BUFFER_SIZE;

				do {
					bytes_recevived = recv(ClientSocket, (char*)buffer, buffer_size, 0);
				} while (bytes_recevived == -1);

				fwrite((char*)buffer, bytes_recevived, 1, fo);

				memset(buffer, 0, RECV_BUFFER_SIZE);
				bytes_to_receive -= bytes_recevived;
			} while (bytes_to_receive > 0);

			if (buffer) delete[] buffer;
			fclose(fo);
			///-------------------------------------------------------------------------------
		}
		else
		{
			//cout << "Khong the ket noi den Server !!!" << endl;
		}

		// Dong ket noi
		ClientSocket.Close();
	}
	return 1;
}