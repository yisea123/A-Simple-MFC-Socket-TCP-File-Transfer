#pragma once
#include "afxwin.h"


// RegisterDlg dialog

class RegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~RegisterDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = REGISTERDLG };
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

	CEdit edit_user;
	CEdit edit_pass;
	CEdit edit_repass;

	afx_msg void OnBnClickedButtonReg();
};
