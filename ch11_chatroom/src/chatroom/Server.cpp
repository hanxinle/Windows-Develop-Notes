
#include "stdafx.h"
#include "ChatRoomDlg.h"

BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut, BOOL bRead)
{
	fd_set fdset;
	timeval tv;
	FD_ZERO(&fdset);
	FD_SET(hSocket, &fdset);
	nTimeOut = nTimeOut > 1000 ? 1000 : nTimeOut;
	tv.tv_sec  = 0;
	tv.tv_usec = nTimeOut;

	int iRet = 0;
	if ( bRead ) {
		iRet = select(0, &fdset, NULL , NULL, &tv);
	}else{
		iRet = select(0, NULL , &fdset, NULL, &tv);
	}

	if(iRet <= 0) {
		return FALSE;
	} else if (FD_ISSET(hSocket, &fdset)){
		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI ListenThreadFunc(LPVOID pParam)
{
	CChatRoomDlg *pChatRoom = (CChatRoomDlg *)pParam;
	ASSERT(pChatRoom != NULL);
	pChatRoom->m_ListenSock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if ( pChatRoom->m_ListenSock == INVALID_SOCKET ) {
		AfxMessageBox(_T("新建Socket失败！"));
		return FALSE;
	}

	int iPort = pChatRoom->GetDlgItemInt(IDC_LISTEN_PORT);
	if ( iPort <= 0 || iPort > 65535 ) {
		AfxMessageBox(_T("请输入合适的端口：1 - 65535"));
		goto __Error_End;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(iPort);

	if ( bind(pChatRoom->m_ListenSock, (sockaddr*)&service, sizeof(sockaddr_in)) == SOCKET_ERROR ) {
		AfxMessageBox(_T("绑定端口失败！"));
		goto __Error_End;
	}

	if( listen(pChatRoom->m_ListenSock, 5) == SOCKET_ERROR ) {
		AfxMessageBox(_T("监听失败！"));
		goto __Error_End;
	}
	pChatRoom->ShowMsg(_T("系统信息：启动服务器成功！"));
	pChatRoom->m_bIsServer = TRUE;
	pChatRoom->EnableWindow(IDC_START_SERVER, FALSE);
	pChatRoom->EnableWindow(IDC_STOP_SERVER);

	while( TRUE && !(pChatRoom->bShutDown)) {
		if ( SOCKET_Select(pChatRoom->m_ListenSock, 100, TRUE) ) {
			sockaddr_in clientAddr;
			int iLen = sizeof(sockaddr_in);
			SOCKET accSock = accept(pChatRoom->m_ListenSock, (struct sockaddr *)&clientAddr , &iLen);
			if (accSock == INVALID_SOCKET) {
				continue;
			}
			CClientItem tItem;
			tItem.m_Socket = accSock;
			tItem.m_pMainWnd = pChatRoom;
			tItem.m_strIp = inet_ntoa(clientAddr.sin_addr);
			INT_PTR idx = pChatRoom->m_ClientArray.Add(tItem);
			tItem.hThread = CreateThread(NULL, 0, ClientThreadProc, &(pChatRoom->m_ClientArray.GetAt(idx)), CREATE_SUSPENDED, NULL);
			pChatRoom->m_ClientArray.GetAt(idx).hThread = tItem.hThread;
			ResumeThread(tItem.hThread);
			CString strMsg;

			strMsg = _T("客户端：") + tItem.m_strIp + _T(" 进入聊天室！");
			pChatRoom->ShowMsg(strMsg);
			pChatRoom->SendClientsMsg(strMsg, &tItem);

			Sleep(100);
		}
	}

__Error_End:
	closesocket(pChatRoom->m_ListenSock);
	return TRUE;
}


DWORD WINAPI ClientThreadProc(LPVOID lpParameter)
{
	CString strMsg;
	CClientItem m_ClientItem = *(CClientItem *)lpParameter;
	while( TRUE && !(m_ClientItem.m_pMainWnd->bShutDown)) {
		if ( SOCKET_Select(m_ClientItem.m_Socket, 100, TRUE) ) {
			TCHAR szBuf[MAX_BUF_SIZE] = {0};
			int iRet = recv(m_ClientItem.m_Socket, (char *)szBuf, MAX_BUF_SIZE, 0);
			if ( iRet > 0 ) {
				//right;
				strMsg = szBuf;
				strMsg = _T("客户端：") + m_ClientItem.m_strIp + _T(">") + strMsg;
				m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
				m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg, &m_ClientItem);
			}else{
				//close socket;
				strMsg = _T("客户端：") + m_ClientItem.m_strIp + _T(" 离开了聊天室！");
				m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
				m_ClientItem.m_pMainWnd->RemoveClientFromArray(m_ClientItem);
				m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg, &m_ClientItem);
				break;
			}
		}
		Sleep(500);
	}
	return TRUE;
}
