// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "Xuly.h"

string file_name;
int iPort = 10000;

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif


// CServerDlg dialog




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CServerDlg::IDD, pParent)
, m_msgString(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BOXCHAT, m_msgString);
	DDX_Control(pDX, IDC_LIST1, listFile);
	DDX_Control(pDX, IDC_LIST_USER, listUser);
	DDX_Control(pDX, IDC_STATIC1, static_file);
	DDX_Control(pDX, IDC_STATIC2, static_hist);
	DDX_Control(pDX, IDC_STATIC3, static_user);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET,SockMsg)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_CANCEL, &CServerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &CServerDlg::OnBnClickedButtonAddfile)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEFILE, &CServerDlg::OnBnClickedButtonRemovefile)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(140, _T("Arial"));
	GetDlgItem(IDC_STATIC1)->SetFont(m_pFont, TRUE);
	GetDlgItem(IDC_STATIC2)->SetFont(m_pFont, TRUE);
	GetDlgItem(IDC_STATIC3)->SetFont(m_pFont, TRUE);

	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(PORT);
	serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockServer, (SOCKADDR*)&serverAdd, sizeof(serverAdd));
	listen(sockServer, 32);
	int err = WSAAsyncSelect(sockServer, m_hWnd, WM_SOCKET, FD_READ | FD_ACCEPT | FD_CLOSE);
	if (err)
		MessageBox((LPCTSTR)"Cant call WSAAsyncSelect");

	number_Socket = 0;
	pSock = new SockName[200];


	listFile.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	//Doc file
	ifstream f;
	f.open("filePath.txt");
	if (f)
	{
		while (!f.eof())
		{
			string line = "";
			getline(f, line);
			if (line != "")
			{
				CA2T str(getFileName(line).c_str());
				listFile.InsertItem(0, str);
			}
		}
		f.close();
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::Split(CString src,CString des[2])
{
	int p1,p2;

	p1=src.Find(_T("\r\n"),0);
	des[0]=src.Mid(0,p1);

	p2=src.Find(_T("\r\n"),p1+1);
	des[1]=src.Mid(p1+2,p2-(p1+2));

}

char* CServerDlg::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize+1];
	memset(pAnsiString,0,nSize+1);
	wcstombs(pAnsiString, s, nSize+1);
	return pAnsiString;
}

void CServerDlg::mSend(SOCKET sk, CString Command)
{
	int Len=Command.GetLength();
	Len+=Len;
	PBYTE sendBuff = new BYTE[1000];
	memset(sendBuff,0,1000);
	memcpy(sendBuff,(PBYTE)(LPCTSTR)Command, Len);
	send(sk,(char*)&Len,sizeof(Len),0);
	send(sk,(char*)sendBuff,Len,0);
	delete[] sendBuff;
}

int CServerDlg::mRecv(SOCKET sk, CString &Command)
{
	PBYTE buffer = new BYTE[1000];
	memset(buffer,0, 1000);
	recv(sk ,(char*)&buffLength,sizeof(int),0);
	recv(sk,(char*)buffer,buffLength,0);
	TCHAR* ttc = (TCHAR*)buffer;
	Command = ttc;
	delete[] buffer;
	if(Command.GetLength()==0)
		return -1;
	return 0;

}



void CServerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CServerDlg::GetAcc(){
	// Get Account information from file here
}

bool CServerDlg::checkAcc(CString inputName, CString inputPass) {
	//Check Acc Exist here
	/*for (int i = 0; i < Acc[0].size();i++) {
		if (user == Acc[0][i] && pass == Acc[1][i])
			return 1;
	}*/
	bool res = false;
	ifstream fi;
	fi.open("account.txt");
	if (fi)
	{
		while (!fi.eof())
		{
			string line;
			getline(fi, line);
			int found = line.find(','); //username va pw dc ngan cach boi dau ,
			string id, pass;
			if (found != string::npos)
			{
				id = line.substr(0, found);
				pass = line.substr(found + 1, line.size() - found - 1);
			}
			CA2T cs_id(id.c_str());
			CA2T cs_pass(pass.c_str());
			if (inputName.CompareNoCase(cs_id) == 0 && inputPass.CompareNoCase(cs_pass) == 0)
			{
				res = true;
				break;
			}
		}
		fi.close();
	}
	return res;
}

void Parse(CString source, CString &User, CString &Pass) {
	CString demp = (CString)"";
	int pos = -1;
	for (int i = 0; i < source.GetLength(); i++) {
		if (source[i] == ' ') {
			User = demp;
			pos = i + 1;
			demp = (CString)"";
			break;
		}
		else {
			demp += source[i];
		}
	}
	for (int i = pos; i < source.GetLength(); i++) {
		demp += source[i];
	}
	Pass = demp;
}
int CServerDlg::checkUserNameExist(CString inputName,CString inputPass)
{
	ifstream fi;
	fi.open("account.txt");
	if (fi) //Kiem  tra xem co bi trung username da dk hay khong
	{
		while (!fi.eof())
		{
			string line;
			getline(fi, line);
			int found = line.find(','); //username va pw dc ngan cach boi dau ,
			string id, pass;
			if (found != string::npos)
			{
				id = line.substr(0, found);
				pass = line.substr(found + 1, line.size() - found - 1);
			}
			CA2T cs_id(id.c_str());
			CA2T cs_pass(pass.c_str());
			if (inputName.CompareNoCase(cs_id) == 0)
			{
				fi.close();
				return 0;
			}
		}

		fi.close();
	}
	return 1;
}

void test()
{
	cout << "hello";
}
void CServerDlg::updateAccount(CString inputName,CString inputPass)
{
	fstream fo;
	fo.open("account.txt", fstream::app);
	fo << CStringA(inputName) << ',' << CStringA(inputPass) << endl;
	fo.close();
}
LRESULT CServerDlg::SockMsg(WPARAM wParam, LPARAM lParam)
{
	CString User, Pass;
	if (WSAGETSELECTERROR(lParam))
	{
		// Display the error and close the socket
		closesocket(wParam);
	}
	switch(WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
		{
			pSock[number_Socket].sockClient = accept(wParam,NULL,NULL);
			break;
		}
	case FD_READ:
		{

			int post = -1, dpos = -1;

			for(int i=0;i<number_Socket;i++)
			{
				if(pSock[i].sockClient==wParam)
				{
					if (i<number_Socket)
						post=i;
				}
			}

			CString temp;
			if(mRecv(wParam, temp) < 0)
				break;
			Split(temp,strResult);
			int flag =_ttoi(strResult[0]);
			char* tem = ConvertToChar(strResult[1]);
			switch(flag)
			{
			case 1://Login
				{
					t = 0;
					// Parse strResult[1] to username and pass;
					Parse(strResult[1], User, Pass);
					char* temp = ConvertToChar(User);
					if(number_Socket>0)
					{
						for(int i=0;i<number_Socket;i++)
						{
							if(strcmp(temp,pSock[i].Name)==0)//Trung ten user
							{
								t=1;
								break;
							}
						}
					}

					
					
					
					if(checkAcc(User,Pass) && t==0 )
					{	
						strcpy(pSock[number_Socket].Name,temp);
						Command=_T("1\r\n1\r\n");
						// this line will show user name and pass
						//m_msgString+=strResult[1] + _T(" login\r\n");
						// the cmt below will show only user is user and pass were parsed
						m_msgString += User + _T(" Login\r\n");
						
						listUser.InsertItem(0, User);
						UpdateData(FALSE);
						number_Socket++;

					}
					else
						Command=_T("1\r\n0\r\n");
					mSend(wParam,Command);
					UpdateData(FALSE);
					break;
				}

			case 2: //Register
			{

				// Parse strResult[1] to username and pass;
				Parse(strResult[1], User, Pass);

				t = checkUserNameExist(User, Pass);
				if (t == 0) {
					Command = _T("2\r\n0\r\n");
					m_msgString += _T("Dang Ky tu Client That Bai \r\n");
				}
				else {
					Command = _T("2\r\n1\r\n");
					m_msgString += _T("User: ") + User + _T(" da duoc them vao Database \r\n");
					// We need a function that update the file which include Acc data //
					updateAccount(User, Pass);
				}

				mSend(wParam, Command);
				UpdateData(FALSE);

				break;
			}
		

			case 3: //Log out - ko can xai
				{	
					int post = -1;
					for(int i=0;i<number_Socket;i++)
					{
						if(pSock[i].sockClient==wParam)
						{
							if (i<number_Socket)
								post=i;
						}
					}
	
					// Parse strResult[1] to username and pass;
					//Parse((CString)pSock[post].Name, User, Pass);
					//m_msgString+=pSock[post].Name;
					m_msgString += User + _T(" Logout\r\n");
					closesocket(wParam);
					for(int j = post; j < number_Socket;j++)
					{
						pSock[post].sockClient = pSock[post+1].sockClient;
						strcpy(pSock[post].Name,pSock[post+1].Name);				
					}
					number_Socket--;
					UpdateData(FALSE);
					break;
				}
			case 4: { //Download
				// find socket which sent mess
				int post = -1;
				for (int i = 0;i < number_Socket;i++)
				{
					if (pSock[i].sockClient == wParam)
					{
						if (i < number_Socket)
							post = i;
					}
				}
				// Parse strResult[1] to username and pass;
				
				iPort++;
				string s_port = to_string(iPort);
				CString cs_port(s_port.c_str());

				m_msgString += ((CString)pSock[post].Name + _T(" Request download: ") + strResult[1] );
				m_msgString += "\r\n";
			
				UpdateData(FALSE);
				Command = _T("4\r\n") + cs_port + _T("\r\n");
				mSend(wParam,Command);
				char* fileName = ConvertToChar(strResult[1]);
				string sFileName(fileName);
				file_name = getFilePath(sFileName);
				AfxBeginThread(sendFile, 0);
				
				
				break;
			}
			case 5: //Refresh file from client
			{
	
				for (int i = 0; i < listFile.GetItemCount(); i++) {
					CString demp = listFile.GetItemText(i,0);
					Command = _T("3\r\n") + demp + _T("\r\n");
					mSend(wParam, Command);	
				}
				//CA2T str((to_string(number_Socket)+"- Update\r\n").c_str()) ;
				//m_msgString += str ;
				UpdateData(FALSE);
				break;
			}

		}
			break;
	}

	case FD_CLOSE: //client log out
		{
			UpdateData();
			int post = -1;
			for(int i=0;i<number_Socket;i++)
			{
				if(pSock[i].sockClient==wParam)
				{
					if (i<number_Socket)
						post=i;
				}
			}
			if (post != -1)
			{

				m_msgString += pSock[post].Name;
				m_msgString += " Logout\r\n";

				closesocket(wParam);
				for (int i = 0; i < listUser.GetItemCount(); i++)
				{
					CString cs_name(pSock[post].Name);
					if (cs_name == listUser.GetItemText(i, 0))
					{
						listUser.DeleteItem(i);
						break;
					}
				}
				for (int j = post; j < number_Socket; j++)
				{
					pSock[post].sockClient = pSock[post + 1].sockClient;
					strcpy(pSock[post].Name, pSock[post + 1].Name);
				}
				number_Socket--;
				
			}
			UpdateData(FALSE);
			break;
		}

	}
	return 0;
}

void CServerDlg::OnBnClickedButtonAddfile()
{
	// TODO: Add your control notification handler code here
	CFileDialog t(true);
	if (t.DoModal() == IDOK)
	{
		fstream f;
		f.open("filePath.txt", fstream::app);
		listFile.InsertItem(0, t.GetFileName());
		string line = CStringA(t.GetPathName());
		f << line << endl;
		f.close();

	}
}


void CServerDlg::OnBnClickedButtonRemovefile()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nItem = 0; //Represents the row number inside CListCtrl
	for (nItem = 0; nItem < listFile.GetItemCount();)
	{
		BOOL bChecked = listFile.GetCheck(nItem);
		if (bChecked == 1)
		{

			//Xoa 1 dong trong file luu duong dan
			{
				//CString strText = listFile .GetItemText(nItem, 0);
				//Xoa tren giao dien
				string line;

				string delLine = CStringA(listFile.GetItemText(nItem, 0));
				listFile.DeleteItem(nItem);
				ifstream fi;

				fi.open("filePath.txt");
				if (fi)
				{
					ofstream fo;
					fo.open("temp.txt");
					while (!fi.eof())
					{
						getline(fi, line);
						size_t found = line.find(delLine);
						if (found != string::npos || line == "")
							continue;
						fo << line << endl;
					}

					fo.close();
					fi.close();
					remove("filePath.txt");
					rename("temp.txt", "filePath.txt");
				}

			}
		}
		else nItem++;
	}

	UpdateData(FALSE);
}


//-----------------------File transfer implementation---------------------
UINT CServerDlg::sendFile(LPVOID pParam)
{
	/*if (!AfxWiznInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
	}
	else*/
	{
		// TODO: code your application's behavior here.

		// Khoi tao thu vien Socket
		if (AfxSocketInit() == FALSE)
		{
			//cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}

		CSocket ServerSocket; //cha
							  // Tao socket cho server, dang ky port la 1234, giao thuc TCP
		if (ServerSocket.Create(iPort, SOCK_STREAM, NULL) == 0) //SOCK_STREAM or SOCK_DGRAM.
		{
			//cout << "Khoi tao that bai !!!" << endl;
			//cout << ServerSocket.GetLastError();
			return FALSE;
		}
		else
		{
			//cout << "Server khoi tao thanh cong !!!" << endl;

			if (ServerSocket.Listen(32) == FALSE)
			{
				//cout << "Khong the lang nghe tren port nay !!!" << endl;
				ServerSocket.Close();
				return FALSE;
			}

		}


		CSocket Connector;
		// Khoi tao mot socket de duy tri viec ket noi va trao doi du lieu
		if (ServerSocket.Accept(Connector))
		{
			//cout << "Da co Client ket Noi !!!" << endl << endl;
			///-----------------------------------------------------------------------------------
			/// Tham so truyen vao: char* file_name, CSocket Connector
			///-----------------------------------------------------------------------------------
			int file_size, bytes_to_send, bytes_sent;
			byte* buffer = NULL;

			// Mo file
			
			FILE* fi = fopen(file_name.c_str(), "rb");
			if (!fi)
				return 0;

			// Lay kich thuoc file
			fseek(fi, 0, SEEK_END);
			file_size = ftell(fi);
			fseek(fi, 0, SEEK_SET);

			// Gui kich thuoc file

			bytes_to_send = sizeof(file_size);
			do
			{
				int size = file_size + sizeof(file_size) - bytes_to_send;
				bytes_sent = send(Connector, (char*)&size, bytes_to_send, 0);

				if (bytes_sent == SOCKET_ERROR)
					return 0;

				bytes_to_send -= bytes_sent;
			} while (bytes_to_send>0);


			// Gui file's data 
			buffer = new byte[SEND_BUFFER_SIZE];
			bytes_to_send = file_size;// So bytes data can gui

			do {
				int buffer_size;
				buffer_size = (SEND_BUFFER_SIZE > bytes_to_send) ? bytes_to_send : SEND_BUFFER_SIZE;
				fread(buffer, buffer_size, 1, fi);

				// Neu gui bi loi thi gui lai goi tin do
				do {
					bytes_sent = send(Connector, (char*)buffer, buffer_size, 0);
				} while (bytes_sent == -1);

				// Cap nhat lai so bytes can gui
				bytes_to_send -= bytes_sent;
			} while (bytes_to_send > 0);

			if (buffer) delete[] buffer;
			fclose(fi);
			///------------------------------------------------------------------------------------
		}
		Connector.Close();
		ServerSocket.Close();
	}
	return 1;
}
