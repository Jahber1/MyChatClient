#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <synchapi.h>
#include <list>
#include "Console.h"
#include "Message.h"
#include "PacketMessage.h"
#include "PacketProcedure.h"
#include "MyChatClient.h"
#include "RingBuffer.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 1589
#define BUFSIZE 1000

using namespace std;

HANDLE hConsoleMutex;
CRingBuffer RecvQ;
CRingBuffer SendQ;
SOCKET sock;
HANDLE hThread;

list<pair <string, string>> Chatting;
char NickName[50];


DWORD WINAPI recvThread(LPVOID lpParam) {
	char buffer[1024];
	int recv_size;

	while (1) {
		recv_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
		if (recv_size > 0) {
			switch (buffer[2])
			{
			case dfPACKET_MESSAGE:
			{
				buffer[recv_size] = '\0';
				stMessage* pMsg = (stMessage*)(buffer + 3);

				WaitForSingleObject(hConsoleMutex, INFINITE);

				if (Chatting.size() > 27)
				{
					Chatting.pop_front();
				}
				Chatting.push_back(make_pair(string(pMsg->NickName), string(pMsg->Message)));

				system("cls");

				for (auto& entry : Chatting)
				{
					printf("%s: %s", entry.first.c_str(), entry.second.c_str());
				}

				// 입력 프롬프트의 위치를 고정합니다.
				CScreenBuffer::GetInstance()->cs_MoveCursor(0, 28);
				printf("----------------------------------------------------");
				CScreenBuffer::GetInstance()->cs_MoveCursor(0, 29);
				printf("%s: ", NickName);
				fflush(stdout);

				ReleaseMutex(hConsoleMutex); // 뮤텍스 해제
				break;
			}
			case dfPACKET_CREATE_OTHER:
			{
				stCreateOther* pMsg = (stCreateOther*)(buffer + 3);
				WaitForSingleObject(hConsoleMutex, INFINITE);

				if (Chatting.size() > 27)
				{
					Chatting.pop_front();
				}
				Chatting.push_back(make_pair(string(pMsg->NickName), "님이 접속하셨습니다.\n"));

				system("cls");

				for (auto& entry : Chatting)
				{
					printf("%s: %s", entry.first.c_str(), entry.second.c_str());
				}

				// 입력 프롬프트의 위치를 고정합니다.
				CScreenBuffer::GetInstance()->cs_MoveCursor(0, 28);
				printf("----------------------------------------------------");
				CScreenBuffer::GetInstance()->cs_MoveCursor(0, 29);
				printf("%s: ", NickName);
				fflush(stdout);

				ReleaseMutex(hConsoleMutex); // 뮤텍스 해제
				break;
			}
			}
		}
		else if (recv_size == 0) {
			printf("Connection closed by server.\n");
			break;
		}
		else {
			if (WSAGetLastError() == 10035)
			{
				continue;
			}
			printf("recv failed: %d\n", WSAGetLastError());
			break;
		}
	}

	return 0;
}

int main()
{
	int select_retval;
	int recv_retval;
	int send_retval;
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return 1;

	char ServerIpBuf[16];
	cout << "접속할 IP 주소를 입력하세요 : ";
	fgets(ServerIpBuf, 16, stdin);
	ServerIpBuf[strlen(ServerIpBuf) - 1] = '\0';

	CScreenBuffer::GetInstance()->cs_Initial();

	// 닉네임 입력
	cout << endl << "사용할 닉네임을 입력하세요 : ";
	fgets(NickName, 50, stdin);
	NickName[strlen(NickName) - 1] = '\0';

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, ServerIpBuf, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	cout << endl << "채팅방에 접속하였습니다." << endl;

	hThread = CreateThread(NULL, 0, recvThread, NULL, 0, NULL);
	if (hThread == NULL) {
		printf("Could not create thread: %d\n", GetLastError());
		return 1;
	}

	// 닉네임 서버에 전달
	stPacketHeader Header;
	stCreate SendMsg;
	mpCreate(&Header, &SendMsg, NickName);
	SendToServer(&Header, (char*)&SendMsg);

	// 넌블로킹 소켓으로 전환
	u_long on = 1;
	retval = ioctlsocket(sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR)
		return 1;

	// 데이터 통신에 사용할 변수
	FD_SET rset;
	FD_SET wset;
	char ChatMessage[100];
	stMessage Message;


	CScreenBuffer::GetInstance()->cs_MoveCursor(0, 0);
	CScreenBuffer::GetInstance()->cs_ClearScreen();
	// 서버와 데이터 통신
	while (1)
	{
		WaitForSingleObject(hConsoleMutex, INFINITE); // 뮤텍스 획득

		CScreenBuffer::GetInstance()->cs_MoveCursor(0, 28);
		printf("----------------------------------------------------");
		CScreenBuffer::GetInstance()->cs_MoveCursor(0, 29);
		printf("%s: ", NickName);
		fflush(stdout);

		ReleaseMutex(hConsoleMutex); // 뮤텍스 해제

		// 입력된 메시지를 서버로 전송
		if (fgets(ChatMessage, sizeof(ChatMessage), stdin) != nullptr) {
			mpMessage(&Header, &Message, NickName, ChatMessage);
			SendToServer(&Header, (char*)&Message);
		}
	}

	// 종료 시 뮤텍스 해제
	CloseHandle(hConsoleMutex);

	WSACleanup();
	return 0;
}

void SendToServer(stPacketHeader* Header, char* pPacket)
{
	SendQ.Enqueue((char*)Header, sizeof(stPacketHeader));
	SendQ.Enqueue((char*)pPacket, (int)Header->bySize);

	int send_retval;
	send_retval = send(sock, SendQ.GetFrontBufferPtr(), SendQ.DirectDequeueSize(), 0);
	if (send_retval == SOCKET_ERROR || send_retval == 0)
	{
		if (WSAGetLastError() == 10053 || WSAGetLastError() == 10054)
		{
			if (sock != INVALID_SOCKET)
			{
				closesocket(sock);
			}
		}
		else
		{
			cout << "send 에러1" << endl;
		}
	}
	else
	{
		SendQ.MoveFront(send_retval);
		if (SendQ.ReadPos == 0) // 보낸 데이터가 링버퍼 경계에 걸렸을 때 한 번 더 보내보기
		{
			send_retval = send(sock, SendQ.GetFrontBufferPtr(), SendQ.DirectDequeueSize(), 0);
			if (send_retval == SOCKET_ERROR)
			{
				cout << "send 에러" << endl;
				if (WSAGetLastError() == 10053 || WSAGetLastError() == 10054)
				{
					if (sock != INVALID_SOCKET)
					{
						closesocket(sock);
						return;
					}
				}
			}
			else
			{
				SendQ.MoveFront(send_retval);
			}
		}
	}
}