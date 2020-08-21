// RegEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RegEdit.h"
#include "RegEditDlg.h"
#include "ListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegEditDlg dialog

CRegEditDlg::CRegEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegEditDlg)
	m_iRootSelected = -1;
	m_strSubKey = _T("");
	m_strValueName = _T("");
	m_strValue = _T("");
	m_strSubKeyName = _T("");

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegEditDlg)
	DDX_Radio(pDX, IDC_RADIO_LOCAL_MACHINE, m_iRootSelected);
	DDX_Text(pDX, IDC_EDIT_SUBKEY, m_strSubKey);
	DDX_Text(pDX, IDC_EDIT_VALUENAME, m_strValueName);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strValue);
	DDX_Text(pDX, IDC_SUBKEY_NAME, m_strSubKeyName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegEditDlg, CDialog)
	//{{AFX_MSG_MAP(CRegEditDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CREATE_SUBKEY, OnCreateSubkey)
	ON_BN_CLICKED(IDC_DELETE_SUBKEY, OnDeleteSubkey)
	ON_BN_CLICKED(IDC_DELETE_VALUENAME, OnDeleteValuename)
	ON_BN_CLICKED(IDC_QUERY_INFOKEY, OnQueryInfokey)
	ON_BN_CLICKED(IDC_RADIO_LOCAL_MACHINE, OnRadioLocalMachine)
	ON_BN_CLICKED(IDC_RADIO_CURRENT_USER, OnRadioCurrentUser)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CRegEditDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegEditDlg message handlers

BOOL CRegEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_iRootSelected = 0;

//	m_strSubKeyName.LoadString(IDS_SUBKEY_NAME);
//	lstrcpy(m_cstSubKeyName, m_strSubKeyName);
	hMyKey=HKEY_LOCAL_MACHINE;
	m_cstSubKeyName[0]=0;
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRegEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRegEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRegEditDlg::OnCreateSubkey() 
{
	// TODO: Add your control notification handler code here
	UINT  const MAX_KEY_VALUE = 100;
	LONG  lnRes;
//	HKEY  hMyKey;
	HKEY  hNewKey;
	DWORD dwDisp;    //记录了创建的结果

	UpdateData();

	lnRes = RegOpenKeyEx(hMyKey, (LPCTSTR)m_cstSubKeyName, 0L, KEY_WRITE, &hMyKey );

	if  (lnRes != ERROR_SUCCESS)
		AfxMessageBox(" CAN NOT Open the Root Key !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
	else  {
		lnRes = RegCreateKeyEx(hMyKey, m_strSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
							   KEY_ALL_ACCESS, NULL, &hNewKey, &dwDisp);
		if  (lnRes == ERROR_ACCESS_DENIED)
			AfxMessageBox(" You must be a SUPERVISOR !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
		lnRes = RegCloseKey(hMyKey);
	}  //  else

	if  (!m_strValue.IsEmpty())  {
		unsigned  char  szKeyValue[ MAX_KEY_VALUE ];
		lstrcpy((char *)szKeyValue, m_strValue);
		lnRes = RegSetValueEx(hNewKey, m_strValueName, 0,
							  REG_SZ, szKeyValue, MAX_KEY_VALUE);

		if  (lnRes != ERROR_SUCCESS)
			AfxMessageBox(" CAN NOT Set Value !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
	}  //  if

	RegCloseKey(hNewKey);
}

void CRegEditDlg::OnDeleteSubkey() 
{
	// TODO: Add your control notification handler code here
	LONG  lnRes;
//	HKEY  hMyKey;


	UpdateData();

	if  (m_strSubKey.IsEmpty())  {
		AfxMessageBox(" CAN NOT Delete a Empty Key !", MB_OK | MB_ICONSTOP, (UINT)-1);
		return;
	}  else
		if  (IDYES != AfxMessageBox(" Do You REALLY want delete this SubKey ? ",
									MB_YESNO | MB_ICONSTOP, (UINT)-1))   return;

	
	lnRes = RegOpenKeyEx(hMyKey, (LPCTSTR)m_cstSubKeyName, 0L, KEY_WRITE, &hMyKey );

	if  (lnRes != ERROR_SUCCESS)
		AfxMessageBox(" CAN NOT Open the Root Key !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
	else  {
		lnRes = RegDeleteKey(hMyKey, m_strSubKey);
		switch  (lnRes)  {
		case  ERROR_ACCESS_DENIED : 
			AfxMessageBox(" You must be a SUPERVISOR !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
			break;
		case  ERROR_FILE_NOT_FOUND :
			AfxMessageBox(" The SubKey IS NOT exit !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
			break;
		default :;
		}  //  switch
		lnRes = RegCloseKey(hMyKey);
	}  //  else
}

void CRegEditDlg::OnDeleteValuename() 
{
	// TODO: Add your control notification handler code here
	LONG  lnRes;
	HKEY  hMyKey;
	HKEY  hMySubKey;

	UpdateData();
	
	lnRes = RegOpenKeyEx((m_iRootSelected == 0 ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER),
		                 (LPCTSTR)m_cstSubKeyName, 0L, KEY_WRITE, &hMyKey );

	if  (lnRes != ERROR_SUCCESS)
		AfxMessageBox(" CAN NOT Open the Root Key !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
	else  {
		lnRes = RegOpenKeyEx(hMyKey, m_strSubKey, 0L, KEY_WRITE, &hMySubKey);
		if  (lnRes != ERROR_SUCCESS)
			AfxMessageBox(" CAN NOT Open the SubKey !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
		else  {
			lnRes = RegDeleteValue(hMySubKey, m_strValueName);
			switch  (lnRes)  {
			case  ERROR_ACCESS_DENIED : 
				AfxMessageBox(" You must be a SUPERVISOR !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
				break;
			case  ERROR_FILE_NOT_FOUND :
				AfxMessageBox(" The SubKey IS NOT exit !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
				break;
			default :;
			}  //  switch
			lnRes = RegCloseKey(hMySubKey);
		}  //  else
		lnRes = RegCloseKey(hMyKey);
	}  //  else
}	//  OnDeleteValuename

void CRegEditDlg::OnQueryInfokey() 
{
	// TODO: Add your control notification handler code here
	LONG  lnRes;
	HKEY  hMySubKey;
	DWORD SubKeys, Values, MaxSubKeyLen, MaxValueLen;

	UpdateData();
	
	lnRes = RegOpenKeyEx(hMyKey, (LPCTSTR)m_cstSubKeyName, 0L, KEY_READ, &hMySubKey );

	if  (lnRes != ERROR_SUCCESS)  {
		if  (lnRes == ERROR_ACCESS_DENIED)
			AfxMessageBox(" You must be a SUPERVISOR !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
		else
			AfxMessageBox(" CAN NOT Open the Key !", MB_OK | MB_ICONEXCLAMATION, (UINT)-1);
	}  else  {
		lnRes=RegQueryInfoKey(hMySubKey, 
							  NULL,	// address of buffer for class string 
							  NULL, // address of size of class string buffer 
							  NULL, // reserved 
							  &SubKeys,	// address of buffer for number of subkeys 
							  &MaxSubKeyLen,	// address of buffer for longest subkey name length  
							  NULL,	// address of buffer for longest class string length 
							  &Values,	// address of buffer for number of value entries 
							  &MaxValueLen,	// address of buffer for longest value name length 
							  NULL,	// address of buffer for longest value data length 
							  NULL,	// address of buffer for security descriptor length 
							  NULL 	// address of buffer for last write time
							  );
		char  szMessage[80];
		if  (lnRes == ERROR_SUCCESS)  {
			sprintf(szMessage, " There are %d SubKeys and %d Values ! ", SubKeys, Values);
//				AfxMessageBox(szMessage, MB_OK | MB_ICONINFORMATION, (UINT)-1);
		}  //  if

		char * SubKeyNames;
		SubKeyNames = new char [MaxSubKeyLen+9];
		CListDlg ldlg;
		ldlg.Create(IDD_LIST_DIALOG);

		ldlg.m_ListT.SetWindowText(szMessage);
		for  (int i=0;  i<SubKeys;  i++)  {
		  for  (int j=8;  j<(MaxSubKeyLen+9);  j++)  SubKeyNames[j]=0;
		  sprintf(SubKeyNames,"%4d -- ",i+1);
		  lnRes=RegEnumKey(hMySubKey, i, &SubKeyNames[8], 1+MaxSubKeyLen);
		  ldlg.m_knmlist.AddString(SubKeyNames);
		}

		ldlg.ShowWindow(SW_SHOWNORMAL);
		ldlg.RunModalLoop();

		if  (SubKeys<=0)  return;

		lnRes=RegEnumKey(hMySubKey, ldlg.m_knmlist.GetCaretIndex(), &SubKeyNames[0], 1+MaxSubKeyLen);
		m_strSubKeyName=SubKeyNames;
		UpdateData(FALSE);
		//if  (lstrlen(m_cstSubKeyName)>0)  lstrcat(m_cstSubKeyName,"\\");
		//lstrcat(m_cstSubKeyName,SubKeyNames);

		delete [] SubKeyNames;
		lnRes = RegCloseKey(hMySubKey);
	}  //  else
	lnRes = RegCloseKey(hMyKey);
}

void CRegEditDlg::OnRadioLocalMachine() 
{
	// TODO: Add your control notification handler code here
	m_iRootSelected = 0;
	hMyKey=HKEY_LOCAL_MACHINE;
	m_cstSubKeyName[0]=0;
	m_strSubKeyName=m_cstSubKeyName;
	UpdateData(FALSE);

}

void CRegEditDlg::OnRadioCurrentUser() 
{
	// TODO: Add your control notification handler code here
	m_iRootSelected = 1;
	hMyKey=HKEY_CURRENT_USER;
	m_cstSubKeyName[0]=0;
	m_strSubKeyName=m_cstSubKeyName;
	UpdateData(FALSE);
}	



void CRegEditDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}
