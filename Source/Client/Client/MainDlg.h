#pragma once
#include "afxcmn.h"

#define RECV_BUFFER_SIZE	10000
// MainDlg dialog

class MainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MainDlg)

public:
	MainDlg(SOCKET s,CString ,CWnd* pParent = NULL);   // standard constructor
	virtual ~MainDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	LRESULT SockMsg(WPARAM wParam, LPARAM lParam);
	char* ConvertToChar(const CString &s);
	void Split(CString src, CString des[2]);
	void mSend(CString Command);
	int mRecv(CString &Command);

	SOCKET sClient;
	sockaddr_in servAdd;
	CString Command;
	int	buffLength;
	CString strResult[2];
	CString userOnline;
	afx_msg void OnBnClickedButtonDownload();
	CListCtrl listFile;
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonLogout();
	CString m_msgString;
	char* downloadFileName;
	bool receiveFile(char*, int);
	
};
