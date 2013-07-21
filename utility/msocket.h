/**
 * code from http://blog.csdn.net/langresser/article/details/8646088
 * 2013-07-21
 */

/**
 * cross socket for iOS, android,...
 * read the end
 */

#pragma once

#ifdef WIN32
#include <windows.h>
#include <WinSock.h>
#else
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

#endif

#ifndef CHECKF
#define CHECKF(x) \
	do \
{ \
	if (!(x)) { \
	log_msg("CHECKF", #x, __FILE__, __LINE__); \
	return 0; \
	} \
} while (0)
#endif

#define _MAX_MSGSIZE 16 * 1024		// �ݶ�һ����Ϣ���Ϊ16k
#define BLOCKSECONDS	30			// INIT��������ʱ��
#define INBUFSIZE	(64*1024)		//?	����ߴ�������汨�����  �������ݵĻ���
#define OUTBUFSIZE	(8*1024)		//? ����ߴ�������汨������� �������ݵĻ��棬��������8Kʱ��FLUSHֻ��ҪSENDһ��

class CGameSocket {
public:
	CGameSocket(void);
	bool	Create(const char* pszServerIP, int nServerPort, int nBlockSec = BLOCKSECONDS, bool bKeepAlive = false);
	bool	SendMsg(void* pBuf, int nSize);
	bool	ReceiveMsg(void* pBuf, int& nSize);
	bool	Flush(void);
	bool	Check(void);
	void	Destroy(void);
	SOCKET	GetSocket(void) const { return m_sockClient; }
private:
	bool	recvFromSock(void);		// �������ж�ȡ�����ܶ������
	bool    hasError();			// �Ƿ�������ע�⣬�첽ģʽδ��ɷǴ���
	void    closeSocket();

	SOCKET	m_sockClient;

	// �������ݻ���
	char	m_bufOutput[OUTBUFSIZE];	//? ���Ż�Ϊָ������
	int		m_nOutbufLen;

	// ���λ�����
	char	m_bufInput[INBUFSIZE];
	int		m_nInbufLen;
	int		m_nInbufStart;				// INBUFʹ��ѭ��ʽ���У��ñ���Ϊ������㣬0 - (SIZE-1)
};

/**
 * DEMO

	// ������Ϣ
	bSucSend = m_pSocket->SendMsg(buf, nLen);

	// ������Ϣ�����ŵ���Ϸ��ѭ���У�ÿ֡����
	if (!m_pSocket) {
		return;
	}

	if (!m_pSocket->Check()) {
		m_pSocket = NULL;
		// ������
		onConnectionAbort();
		return;
	}

	// �������ݣ��������������Ϣ��
	m_pSocket->Flush();

	// �������ݣ�ȡ�û������е�������Ϣ��ֱ��������Ϊ�գ�
	while (true)
	{
		char buffer[_MAX_MSGSIZE] = { 0 };
		int nSize = sizeof(buffer);
		char* pbufMsg = buffer;
		if(m_pSocket == NULL)
		{
			break;
		}
		if (!m_pSocket->ReceiveMsg(pbufMsg, nSize)) {
			break;
		}
		
		while (true)
		{
			MsgHead* pReceiveMsg = (MsgHead*)(pbufMsg);
			uint16	dwCurMsgSize = pReceiveMsg->usSize;
//			CCLOG("msgsize: %d", dwCurMsgSize);

			if((int)dwCurMsgSize > nSize || dwCurMsgSize <= 0) {	// broken msg
				break;
			}

			CMessageSubject::instance().OnMessage((const char*)pReceiveMsg, pReceiveMsg->usSize);

			pbufMsg	+= dwCurMsgSize;
			nSize	-= dwCurMsgSize;
			if(nSize <= 0) {
				break;
			}
		}
	}
 */

 /** 
  * some chinese tips:
  * 	������һ��Socket��װ��������windows mac ios android��ƽ̨�� 
  * 	Socket�������첽�������ģ����Կ��Է��ĵķŵ����̴߳�����Ϣ�� 
  *	���֧��64k�Ľ�����Ϣ���壨һ��һ����Ϣ�����ܴ���3k����
  *
  * 1. 	������ģ�ͣ������Ҳŷ��İ���Ϣ����ŵ����̣߳����̴߳�����Ϣ	
  * 	��ʵ���˷ѡ���֪���ö���͵�mmo�Ż��õ���
  *
  * 2.	��Ϣ���ջ��洦���������Ϣ���ص���
  *
  * 3. 	û�ˣ�ʣ�µ�һЩ���⴦��Ӧ�����ϲ��߼������ǵġ�������������ȡ�
  *
  * 4.	��Ϸ������Ϣʱ���첽�ģ�����������ʵ�����ʼ���ӷ�������һ��
  * 	��ѯ����(select)���Ҹ�����Ϊ������Ҫ���⴦����Ϊ������ӷ�����
  *	����Ҫ�����ӵĻ�����ô���������Ϸûû�����ˡ����������˵���
  *	����ֻ���м��ٺ��롣
  */
