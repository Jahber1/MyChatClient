#include <iostream>
#include "Message.h"
#include "PacketProcedure.h"
#include "PacketMessage.h"
#include "MyChatClient.h"

using namespace std;

bool PacketProc(BYTE byPacketType, char* pPacket)
{
    switch (byPacketType)
    {
    case dfPACKET_CREATE:
        return netPacketProc_Create(pPacket);
        break;
    case dfPACKET_CREATE_OTHER:
        return netPacketProc_Create_Other(pPacket);
        break;
    case dfPACKET_MESSAGE:
        return netPacketProc_Message(pPacket);
        break;
    case dfPACKET_DELETE:
        return netPacketProc_Delete(pPacket);
        break;
    }
    return TRUE;
}

bool netPacketProc_Create(char* pPacket)
{
    stCreate* pMessage = (stCreate*)pPacket;

    stPacketHeader Header;
    stCreate SendMsg;
    mpCreate(&Header, &SendMsg, pMessage->NickName);
    SendToServer(&Header, (char*)&SendMsg);

    return true;
}

bool netPacketProc_Create_Other(char* pPacket)
{
    stCreateOther* pMessage = (stCreateOther*)pPacket;

    cout << pMessage->NickName << "´ÔÀÌ Á¢¼ÓÇÏ¼Ì½À´Ï´Ù." << endl;

    return true;
}

bool netPacketProc_Message(char* pPacket)
{
    stMessage* pMessage = (stMessage*)pPacket;

    cout << pMessage->NickName << ": " << pMessage->Message << endl;

    return true;
}

bool netPacketProc_Delete(char* pPacket)
{
    stDelete* pMessage = (stDelete*)pPacket;

    stPacketHeader Header;
    stDelete SendMsg;
    mpDelete(&Header, &SendMsg, pMessage->NickName);
    SendToServer(&Header, (char*)&SendMsg);

    return true;
}