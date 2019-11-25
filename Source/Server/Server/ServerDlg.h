// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "stdlib.h"
#include "time.h"
#include <vector>
#include "afxcmn.h"
#include "resource.h"

#define PORT 25000
#define SEND_BUFFER_SIZE 10000
#define WM_SOCKET WM_USER+1
using namespace std;
// CServerDlg dialog
class CServerDlg : public CDialog
{
	// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor
	~CServerDlg() { if (pSock) delete[] pSock; }
	// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString &s);
	void Split(CString src,CString des[2]);
	void mSend(SOCKET sk, CString Command);
	int mRecv(SOCKET sk, CString &Command);

	struct SockName
	{

		SOCKET sockClient;
		char Name[200];
	};

	SOCKET sockServer,sockClient,flag,sclient;
	struct sockaddr_in serverAdd;
	int msgType;
	int buffLength,t,lenguser,flagsend,kq, count_sock;
	int number_Socket;
	int first_send;
	SockName *pSock;
	CString strResult[2];
	CString Command;
	int R;
	void GetAcc();
	bool checkAcc(CString user, CString pass);
	std::vector<CString> Acc[2];
	int checkUserNameExist(CString,CString);
	void updateAccount(CString inputName, CString inputPass);

	CString m_msgString;
	
	afx_msg void OnBnClickedCancel();
	CListCtrl listFile;
	afx_msg void OnBnClickedButtonAddfile();
	afx_msg void OnBnClickedButtonRemovefile();

	static UINT sendFile(LPVOID pParam); //send file to connector-client

	CListCtrl listUser;

	CStatic static_file;
	CStatic static_hist;
	CStatic static_user;

	
};
extern int iPort;
extern string file_name;

