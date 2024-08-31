#include "PacketMessage.h"

void mpCreate(stPacketHeader* pHeader, stCreate* pPacket, char* NickName)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = sizeof(stCreate);
	pHeader->byType = dfPACKET_CREATE;

	strcpy_s(pPacket->NickName, NickName);
}

void mpCreateOther(stPacketHeader* pHeader, stCreateOther* pPacket, char* NickName)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = sizeof(stCreateOther);
	pHeader->byType = dfPACKET_CREATE_OTHER;

	strcpy_s(pPacket->NickName, NickName);
}

void mpMessage(stPacketHeader* pHeader, stMessage* pPacket, char* NickName, char* chat)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = sizeof(stMessage);
	pHeader->byType = dfPACKET_MESSAGE;

	strcpy_s(pPacket->NickName, NickName);
	strcpy_s(pPacket->Message, chat);
}

void mpDelete(stPacketHeader* pHeader, stDelete* pPacket, char* NickName)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = sizeof(stDelete);
	pHeader->byType = dfPACKET_DELETE;

	strcpy_s(pPacket->NickName, NickName);
}
