#pragma once
#include "Message.h"

void mpCreate(stPacketHeader* pHeader, stCreate* pPacket, char* NickName);
void mpCreateOther(stPacketHeader* pHeader, stCreateOther* pPacket, char* NickName);
void mpMessage(stPacketHeader* pHeader, stMessage* pPacket, char* NickName, char* chat);
void mpDelete(stPacketHeader* pHeader, stDelete* pPacket, char* NickName);
