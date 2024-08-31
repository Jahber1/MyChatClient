#pragma once
#include <windows.h>

#define MESSAGE_SIZE 100
#define dfNETWORK_PACKET_CODE 0x89

struct stPacketHeader
{
	BYTE	byCode;			// 패킷코드 0x89 고정
	BYTE	bySize;			// 패킷 사이즈
	BYTE	byType;			// 패킷 타입
};

#define dfPACKET_CREATE 0
struct stCreate
{
	char NickName[50];
};

#define dfPACKET_CREATE_OTHER 1
struct stCreateOther
{
	char NickName[50];
};

#define dfPACKET_MESSAGE 2
struct stMessage
{
	char NickName[50];
	char Message[MESSAGE_SIZE + 1];
};

#define dfPACKET_DELETE 3
struct stDelete
{
	char NickName[50];
};