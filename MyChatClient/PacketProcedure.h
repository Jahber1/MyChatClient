#pragma once

bool PacketProc(BYTE byPacketType, char* pPacket);
bool netPacketProc_Create(char* pPacket);
bool netPacketProc_Create_Other(char* pPacket);
bool netPacketProc_Message(char* pPacket);
bool netPacketProc_Delete(char* pPacket);