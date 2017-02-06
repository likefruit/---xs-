// Lande2Dlg.h : header file
//

#if !defined(AFX_LANDE2DLG_H__B462E6D3_E12A_48EB_A2C3_8AEFE8CE88CE__INCLUDED_)
#define AFX_LANDE2DLG_H__B462E6D3_E12A_48EB_A2C3_8AEFE8CE88CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLande2Dlg dialog

class CLande2Dlg : public CDialog
{
// Construction
public:
	CLande2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLande2Dlg)
	enum { IDD = IDD_LANDE2_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLande2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLande2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANDE2DLG_H__B462E6D3_E12A_48EB_A2C3_8AEFE8CE88CE__INCLUDED_)
