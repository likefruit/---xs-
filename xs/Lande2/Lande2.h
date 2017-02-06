// Lande2.h : main header file for the LANDE2 application
//

#if !defined(AFX_LANDE2_H__20EBE86C_BCA8_44A3_8C17_582A4300BD36__INCLUDED_)
#define AFX_LANDE2_H__20EBE86C_BCA8_44A3_8C17_582A4300BD36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLande2App:
// See Lande2.cpp for the implementation of this class
//

class CLande2App : public CWinApp
{
public:
	CLande2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLande2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLande2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANDE2_H__20EBE86C_BCA8_44A3_8C17_582A4300BD36__INCLUDED_)
