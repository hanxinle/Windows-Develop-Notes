// ChatRoomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChatRoom.h"
#include "ChatRoomDlg.h"
#include "Inc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
   public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

   protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

    // Implementation
   protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CChatRoomDlg dialog

CChatRoomDlg::CChatRoomDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CChatRoomDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    bShowAll = FALSE;
    m_ListenSock = INVALID_SOCKET;
    m_ConnectSock = INVALID_SOCKET;
    m_hListenThread = NULL;
    m_hConnectThred = NULL;
    m_bIsServer = -1;
    bShutDown = FALSE;
}

void CChatRoomDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STOP_CLIENT, m_StopClientBtn);
    DDX_Control(pDX, IDC_SHOW_MSG, m_MsgEdit);
}

BEGIN_MESSAGE_MAP(CChatRoomDlg, CDialog)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_NETSET, &CChatRoomDlg::OnBnClickedNetset)
ON_BN_CLICKED(IDC_START_SERVER, &CChatRoomDlg::OnBnClickedStartServer)
ON_BN_CLICKED(IDC_SENDMSG, &CChatRoomDlg::OnBnClickedSendmsg)
ON_BN_CLICKED(IDC_START_CLIENT, &CChatRoomDlg::OnBnClickedStartClient)
ON_EN_CHANGE(IDC_INPUT_MSG, &CChatRoomDlg::OnEnChangeInputMsg)
ON_BN_CLICKED(IDC_STOP_CLIENT, &CChatRoomDlg::OnBnClickedStopClient)
ON_BN_CLICKED(IDC_STOP_SERVER, &CChatRoomDlg::OnBnClickedStopServer)
ON_BN_CLICKED(IDC_RADIO_CLIENT, &CChatRoomDlg::OnBnClickedRadioClient)
ON_BN_CLICKED(IDC_RADIO_SERVER, &CChatRoomDlg::OnBnClickedRadioServer)
ON_BN_CLICKED(IDCANCEL, &CChatRoomDlg::OnBnClickedCancel)
ON_BN_CLICKED(IDC_OTHER, &CChatRoomDlg::OnBnClickedOther)
ON_COMMAND(ID_MENU_TRAYINCO, &CChatRoomDlg::OnMenuTrayinco)
ON_MESSAGE(WM_TRAYICON_MSG, OnTrayCallBackMsg)
ON_COMMAND(ID_MENU_SHOW, &CChatRoomDlg::OnMenuShow)
ON_COMMAND(ID_MENU_EXIT, &CChatRoomDlg::OnMenuExit)
END_MESSAGE_MAP()

// CChatRoomDlg message handlers

BOOL CChatRoomDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);   // Set big icon
    SetIcon(m_hIcon, FALSE);  // Set small icon

    DlgAllInit();
    ExtendDiaog(FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatRoomDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatRoomDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this);  // device context for painting

        SendMessage(WM_ICONERASEBKGND,
                    reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user
// drags
//  the minimized window.
HCURSOR CChatRoomDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

void CChatRoomDlg::DlgAllInit() {
    CheckRadioButton(IDC_RADIO_CLIENT, IDC_RADIO_SERVER, IDC_RADIO_CLIENT);
    SetDlgItemText(IDC_IP_ADDR, _T("127.0.0.1"));
    SetDlgItemText(IDC_CONNECT_PORT, _T("5566"));
    SetDlgItemText(IDC_LISTEN_PORT, _T("5566"));

    EnableWindow(IDC_STOP_CLIENT, FALSE);
    EnableWindow(IDC_LISTEN_PORT, FALSE);
    EnableWindow(IDC_STOP_SERVER, FALSE);
    EnableWindow(IDC_START_SERVER, FALSE);
    EnableWindow(IDC_STATIC_LISTEN_PORT, FALSE);

    EnableWindow(IDC_SENDMSG, FALSE);
}

BOOL CChatRoomDlg::EnableWindow(UINT uID, BOOL bEnable) {
    return GetDlgItem(uID)->EnableWindow(bEnable);
}

void CChatRoomDlg::ExtendDiaog(BOOL bShow) {
    static CRect m_DlgRectLarge(0, 0, 0, 0);
    static CRect m_DlgRectSmall(0, 0, 0, 0);
    static CRect m_GroupRectLarge(0, 0, 0, 0);
    static CRect m_GroupRectSmall(0, 0, 0, 0);

    if (m_DlgRectLarge.IsRectNull()) {
        GetWindowRect(&m_DlgRectLarge);
        m_DlgRectSmall = m_DlgRectLarge;
        m_DlgRectSmall.right -= 220;

        ::GetWindowRect(GetDlgItem(IDC_FRAME)->GetSafeHwnd(),
                        &m_GroupRectLarge);
        m_GroupRectSmall = m_GroupRectLarge;
        m_GroupRectSmall.right -= 220;
    }
    if (bShow) {
        bShowAll = TRUE;
        SetWindowPos(NULL, 0, 0, m_DlgRectLarge.Width(),
                     m_DlgRectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
        ::SetWindowPos(GetDlgItem(IDC_FRAME)->GetSafeHwnd(), NULL, 0, 0,
                       m_GroupRectLarge.Width(), m_GroupRectLarge.Height(),
                       SWP_NOZORDER | SWP_NOMOVE);
    } else {
        bShowAll = FALSE;
        SetWindowPos(NULL, 0, 0, m_DlgRectSmall.Width(),
                     m_DlgRectSmall.Height(), SWP_NOZORDER | SWP_NOMOVE);
        ::SetWindowPos(GetDlgItem(IDC_FRAME)->GetSafeHwnd(), NULL, 0, 0,
                       m_GroupRectSmall.Width(), m_GroupRectSmall.Height(),
                       SWP_NOZORDER | SWP_NOMOVE);
    }
}
void CChatRoomDlg::OnBnClickedNetset() {
    if (bShowAll) {
        ExtendDiaog(FALSE);
    } else {
        ExtendDiaog(TRUE);
    }
}

void CChatRoomDlg::OnBnClickedStartServer() {
    m_hListenThread = CreateThread(NULL, 0, ListenThreadFunc, this, 0, NULL);
}

void CChatRoomDlg::ShowMsg(CString strMsg) {
    m_MsgEdit.SetSel(-1, -1);
    m_MsgEdit.ReplaceSel(strMsg + _T("\r\n"));
}

void CChatRoomDlg::RemoveClientFromArray(CClientItem in_Item) {
    for (int idx = 0; idx < m_ClientArray.GetCount(); idx++) {
        CClientItem tItem = m_ClientArray.GetAt(idx);
        if (tItem.m_Socket == in_Item.m_Socket &&
            tItem.hThread == in_Item.hThread &&
            tItem.m_strIp == in_Item.m_strIp) {
            m_ClientArray.RemoveAt(idx);
        }
    }
}

void CChatRoomDlg::OnBnClickedSendmsg() {
    CString strMsg;
    GetDlgItemText(IDC_INPUT_MSG, strMsg);
    if (m_bIsServer == TRUE) {
        strMsg = _T("服务器：>") + strMsg;
        ShowMsg(strMsg);
        SendClientsMsg(strMsg);
    } else if (m_bIsServer == FALSE) {
        CString strTmp = _T("本地客户端：>") + strMsg;
        ShowMsg(strTmp);
        int iSend = send(m_ConnectSock, (char*)strMsg.GetBuffer(),
                         strMsg.GetLength() * sizeof(TCHAR), 0);
        strMsg.ReleaseBuffer();
    } else {
        AfxMessageBox(_T("请您先进入聊天室！"));
    }
    SetDlgItemText(IDC_INPUT_MSG, _T(""));
}

void CChatRoomDlg::OnBnClickedStartClient() {
    m_hConnectThred = CreateThread(NULL, 0, ConnectThreadFunc, this, 0, NULL);
}

void CChatRoomDlg::SendClientsMsg(CString strMsg, CClientItem* pNotSend) {
    TCHAR szBuf[MAX_BUF_SIZE] = {0};
    _tcscpy_s(szBuf, MAX_BUF_SIZE, strMsg);
    for (INT_PTR idx = 0; idx < m_ClientArray.GetCount(); idx++) {
        if (!pNotSend ||
            pNotSend->m_Socket != m_ClientArray.GetAt(idx).m_Socket ||
            pNotSend->hThread != m_ClientArray.GetAt(idx).hThread ||
            pNotSend->m_strIp != m_ClientArray.GetAt(idx).m_strIp) {
            send(m_ClientArray.GetAt(idx).m_Socket, (char*)szBuf,
                 _tcslen(szBuf) * sizeof(TCHAR), 0);
        }
    }
}
void CChatRoomDlg::OnEnChangeInputMsg() {
    CString strMsg;
    GetDlgItemText(IDC_INPUT_MSG, strMsg);
    if (strMsg.IsEmpty()) {
        EnableWindow(IDC_SENDMSG, FALSE);
    } else {
        EnableWindow(IDC_SENDMSG);
    }
}

void CChatRoomDlg::StopClient() {
    bShutDown = TRUE;
    DWORD dwRet = WaitForSingleObject(m_hConnectThred, 1000);
    if (dwRet != WAIT_OBJECT_0) {
        TerminateThread(m_hConnectThred, -1);
        closesocket(m_ConnectSock);
    }
    m_hConnectThred = NULL;
    m_ConnectSock = INVALID_SOCKET;
    m_bIsServer = -1;
    bShutDown = FALSE;
}

void CChatRoomDlg::StopServer() {
    UINT nCount = m_ClientArray.GetCount();
    HANDLE* m_pHandles = new HANDLE[nCount + 1];
    m_pHandles[0] = m_hListenThread;
    for (int idx = 0; idx < nCount; idx++) {
        m_pHandles[idx + 1] = m_ClientArray.GetAt(idx).hThread;
    }
    bShutDown = TRUE;
    DWORD dwRet = WaitForMultipleObjects(nCount + 1, m_pHandles, TRUE, 1000);
    if (dwRet != WAIT_OBJECT_0) {
        for (INT_PTR i = 0; i < m_ClientArray.GetCount(); i++) {
            TerminateThread(m_ClientArray.GetAt(i).hThread, -1);
            closesocket(m_ClientArray.GetAt(i).m_Socket);
        }
        TerminateThread(m_hListenThread, -1);
        closesocket(m_ListenSock);
    }
    delete[] m_pHandles;
    m_hListenThread = NULL;
    m_ListenSock = INVALID_SOCKET;
    m_bIsServer = -1;
    bShutDown = FALSE;
}

void CChatRoomDlg::OnBnClickedStopClient() {
    StopClient();
    ShowMsg(_T("停止客户端成功！"));
    EnableWindow(IDC_START_CLIENT);
    EnableWindow(IDC_STOP_CLIENT, FALSE);
}

void CChatRoomDlg::OnBnClickedStopServer() {
    StopServer();
    ShowMsg(_T("停止服务器成功！"));
    EnableWindow(IDC_START_SERVER);
    EnableWindow(IDC_STOP_SERVER, FALSE);
}

void CChatRoomDlg::OnBnClickedRadioClient() {
    int iRet = -1;
    if (m_bIsServer == TRUE) {
        int iRet = MessageBox(
            _T("您是聊天室的服务器端，如果您退出，所有的客户端都将掉线！\r\n您")
            _T("确定退出吗？"),
            _T("提示"), MB_OKCANCEL | MB_ICONWARNING);
        if (iRet == IDOK) {
            StopServer();
        } else {
            CheckRadioButton(IDC_RADIO_CLIENT, IDC_RADIO_SERVER,
                             IDC_RADIO_SERVER);
        }
    }
    if (iRet == IDOK || m_bIsServer == -1) {
        EnableWindow(IDC_IP_ADDR);
        EnableWindow(IDC_CONNECT_PORT);
        EnableWindow(IDC_STATIC_SERVER_IP);
        EnableWindow(IDC_STATIC_SERVER_PORT);
        EnableWindow(IDC_START_CLIENT);
        EnableWindow(IDC_STOP_CLIENT, FALSE);

        EnableWindow(IDC_LISTEN_PORT, FALSE);
        EnableWindow(IDC_STOP_SERVER, FALSE);
        EnableWindow(IDC_START_SERVER, FALSE);
        EnableWindow(IDC_STATIC_LISTEN_PORT, FALSE);
    }
}

void CChatRoomDlg::OnBnClickedRadioServer() {
    int iRet = -1;
    if (m_bIsServer == FALSE) {
        int iRet = MessageBox(_T("您正在聊天室中，确定退出吗？"), _T("提示"),
                              MB_OKCANCEL | MB_ICONWARNING);
        if (iRet == IDOK) {
            StopClient();
        } else {
            CheckRadioButton(IDC_RADIO_CLIENT, IDC_RADIO_SERVER,
                             IDC_RADIO_CLIENT);
        }
    }
    if (iRet == IDOK || m_bIsServer == -1) {
        EnableWindow(IDC_LISTEN_PORT);
        EnableWindow(IDC_STOP_SERVER, FALSE);
        EnableWindow(IDC_START_SERVER);
        EnableWindow(IDC_STATIC_LISTEN_PORT);

        EnableWindow(IDC_IP_ADDR, FALSE);
        EnableWindow(IDC_CONNECT_PORT, FALSE);
        EnableWindow(IDC_STATIC_SERVER_IP, FALSE);
        EnableWindow(IDC_STATIC_SERVER_PORT, FALSE);
        EnableWindow(IDC_START_CLIENT, FALSE);
        EnableWindow(IDC_STOP_CLIENT, FALSE);
    }
}

void CChatRoomDlg::OnBnClickedCancel() {
    if (m_bIsServer == TRUE) {
        StopServer();
    } else if (m_bIsServer == FALSE) {
        StopClient();
    }
    OnCancel();
}

void CChatRoomDlg::OnBnClickedOther() {
    CPoint pt;
    CRect mRect;
    CMenu mMenu, *pMenu = NULL;
    GetDlgItem(IDC_OTHER)->GetWindowRect(&mRect);
    pt = mRect.BottomRight();
    pt.y = mRect.top + 10;
    mMenu.LoadMenu(IDR_MENU1);
    pMenu = mMenu.GetSubMenu(0);
    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
}

BOOL CChatRoomDlg::TrayMyIcon(BOOL bAdd) {
    BOOL bRet = FALSE;
    NOTIFYICONDATA tnd;
    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = GetSafeHwnd();
    tnd.uID = IDR_MAINFRAME;
    if (bAdd == TRUE) {
        tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        tnd.uCallbackMessage = WM_TRAYICON_MSG;
        tnd.hIcon =
            LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
        _tcscpy_s(tnd.szTip, sizeof(tnd.szTip), _T("聊天室v1.0"));
        ShowWindow(SW_MINIMIZE);
        ShowWindow(SW_HIDE);
        bRet = Shell_NotifyIcon(NIM_ADD, &tnd);
    } else {
        ShowWindow(SW_SHOWNA);
        SetForegroundWindow();
        bRet = Shell_NotifyIcon(NIM_DELETE, &tnd);
    }
    return bRet;
}

void CChatRoomDlg::OnMenuTrayinco() { TrayMyIcon(); }

LRESULT CChatRoomDlg::OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam) {
    switch (lparam) {
        case WM_RBUTTONUP: {
            CMenu mMenu, *pMenu = NULL;
            CPoint pt;
            mMenu.LoadMenu(IDR_MENU2);
            pMenu = mMenu.GetSubMenu(0);
            GetCursorPos(&pt);
            SetForegroundWindow();
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y,
                                  this);
            break;
        }
        case WM_LBUTTONDBLCLK:
            ShowWindow(SW_RESTORE);
            SetForegroundWindow();
            TrayMyIcon(FALSE);
            break;
        default:
            break;
    }
    return NULL;
}

void CChatRoomDlg::OnMenuShow() {
    ShowWindow(SW_RESTORE);
    TrayMyIcon(FALSE);
}

void CChatRoomDlg::OnMenuExit() {
    TrayMyIcon(FALSE);
    OnBnClickedCancel();
}