
#include "stdafx.h"
#include "ChatRoomDlg.h"
#include "Inc.h"

DWORD WINAPI ConnectThreadFunc(LPVOID pParam)
{
	CChatRoomDlg *pChatRoom = (CChatRoomDlg *)pParam;
	ASSERT(pChatRoom != NULL);
	pChatRoom->m_ConnectSock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	if ( pChatRoom->m_ConnectSock == INVALID_SOCKET ) {
		AfxMessageBox(_T("新建Socket失败！"));
		return FALSE;
	}
	CString strServIp;
	pChatRoom->GetDlgItemText(IDC_IP_ADDR, strServIp);
	int iPort = pChatRoom->GetDlgItemInt(IDC_CONNECT_PORT);
	if ( iPort <= 0 || iPort > 65535 ) {
		AfxMessageBox(_T("请输入合适的端口：1 - 65535"));
		goto __Error_End;
	}
	char szIpAddr[16] = {0};
	USES_CONVERSION;
	strcpy_s(szIpAddr, 16, T2A(strServIp));

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szIpAddr);
	if ( connect(pChatRoom->m_ConnectSock, (struct sockaddr *)&server,  sizeof(struct sockaddr)) == SOCKET_ERROR ) {
			AfxMessageBox(_T("连接失败，请重试！"));
			goto __Error_End;
	}
	pChatRoom->m_bIsServer = FALSE;
	pChatRoom->ShowMsg(_T("系统信息：连接服务器成功！"));
	while( TRUE && !(pChatRoom->bShutDown)) {
		if ( SOCKET_Select(pChatRoom->m_ConnectSock) ) {
			TCHAR szBuf[MAX_BUF_SIZE] = {0};
			int iRet = recv(pChatRoom->m_ConnectSock, (char *)szBuf, MAX_BUF_SIZE, 0);
			if ( iRet > 0 ) {
				//right;
				pChatRoom->ShowMsg(szBuf);
			}else{
				//close socket;
				pChatRoom->ShowMsg(_T("聊天室服务器已停止，请重新进行连接！"));
				break;
			}
		}
		Sleep(500);
	}

__Error_End:
	closesocket(pChatRoom->m_ConnectSock);
	return TRUE;
}