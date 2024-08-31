#include <iostream>
#include "Ringbuffer.h"

using namespace std;

CRingBuffer::CRingBuffer(int iBufferSize)
{
    BufferSize = iBufferSize;
    ReadPos = 0;
    WritePos = 0;
    CurrentSize = 0;
    pRingBuffer = new char[iBufferSize];
}

CRingBuffer::~CRingBuffer()
{
    delete[] pRingBuffer;
}

void CRingBuffer::Resize(int size)
{
}

int CRingBuffer::GetBufferSize(void)
{
    return BufferSize;
}

int CRingBuffer::GetUseSize(void)
{
    return CurrentSize;
}

int CRingBuffer::GetFreeSize(void)
{
    return BufferSize - CurrentSize;
}

int CRingBuffer::Enqueue(const char* chpData, int iSize)
{
    if (BufferSize != CurrentSize) // 링버퍼에 여유 공간이 있는 경우
    {
        if (BufferSize - CurrentSize >= iSize) // 버퍼에 남은 공간이 넣을 데이터보다 크거나 같은 경우
        {
            if (WritePos + iSize - 1 >= BufferSize) // 데이터 삽입 시 버퍼의 경계를 넘는 경우
            {
                memcpy(&pRingBuffer[WritePos], chpData, BufferSize - WritePos); // 데이터 버퍼 경계 전까지 삽입
                memcpy(&pRingBuffer[0], chpData + (BufferSize - WritePos), iSize - (BufferSize - WritePos)); // 남은 데이터 삽입
                WritePos = iSize - (BufferSize - WritePos);
            }
            else // 데이터 삽입 시 버퍼의 경계를 넘지 않는 경우
            {
                memcpy(&pRingBuffer[WritePos], chpData, iSize);

                if (WritePos + iSize == BufferSize) // 삽입된 데이터의 마지막 부분이 경계인 경우
                {
                    WritePos = 0;
                }
                else
                {
                    WritePos += iSize;
                }
            }

            CurrentSize += iSize;
            //cout << "입력된 바이트 수: " << iSize << endl;
            return iSize;
        }
        else  // 버퍼에 남은 공간이 넣을 데이터보다 작은 경우
        {
            int temp = BufferSize - CurrentSize;

            if ((ReadPos != 0 && ReadPos < WritePos) || ReadPos == WritePos) // 데이터 삽입 시 버퍼의 경계를 넘은 경우
            {
                memcpy(&pRingBuffer[WritePos], chpData, BufferSize - WritePos); // 데이터 버퍼 경계 전까지 삽입
                CurrentSize += BufferSize - WritePos;

                memcpy(&pRingBuffer[0], chpData + (BufferSize - WritePos), BufferSize - CurrentSize); // 남은 데이터 삽입
                WritePos = ReadPos;
            }
            else // 데이터 삽입 시 버퍼의 경계를 넘지 않은 경우
            {
                memcpy(&pRingBuffer[WritePos], chpData, BufferSize - CurrentSize);
                WritePos = 0; // 한 바퀴 돌아서 rear가 0으로 옴
            }

            CurrentSize = BufferSize; // 가득 참
            //cout << "(여유 공간 부족, 데이터 잘림) 입력된 바이트 수: " << temp << endl;
            return temp;
        }
    }
    else // 링버퍼가 가득 차 있는 경우
        //cout << "링버퍼가 가득 차 있습니다." << endl;
        return 0;
}

int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
    if (CurrentSize != 0) // 링버퍼에 데이터가 있는 경우
    {
        if (CurrentSize >= iSize) // 버퍼에 있는 데이터가 뺄 데이터보다 크거나 같은 경우
        {
            if (ReadPos + iSize - 1 >= BufferSize) // 데이터를 가져올 때 버퍼의 경계를 넘는 경우
            {
                memcpy(chpDest, &pRingBuffer[ReadPos], BufferSize - ReadPos); // 데이터 버퍼 경계 전까지 가져오기
                memcpy(chpDest + (BufferSize - ReadPos), &pRingBuffer[0], iSize - (BufferSize - ReadPos)); // 남은 데이터 가져오기
                ReadPos = iSize - (BufferSize - ReadPos);
            }
            else // 데이터를 가져올 때 버퍼의 경계를 넘지 않는 경우
            {
                memcpy(chpDest, &pRingBuffer[ReadPos], iSize);

                if (ReadPos + iSize == BufferSize) // 가져올 데이터의 마지막 부분이 경계인 경우
                {
                    ReadPos = 0;
                }
                else
                {
                    ReadPos += iSize;
                }
            }

            CurrentSize -= iSize;
            //cout << "가져온 바이트 수: " << iSize << endl;
            return iSize;
        }
        else  // 버퍼에 남은 데이터가 가져올 데이터보다 작은 경우
        {
            int temp = CurrentSize;

            if (ReadPos > WritePos) // 데이터를 가져올 때 버퍼의 경계를 넘는 경우
            {
                memcpy(chpDest, &pRingBuffer[ReadPos], BufferSize - ReadPos); // 데이터 버퍼 경계 전까지 가져오기
                memcpy(chpDest + BufferSize - ReadPos, &pRingBuffer[0], CurrentSize - (BufferSize - ReadPos)); // 남은 데이터 가져오기
            }
            else // 데이터를 가져올 때 버퍼의 경계를 넘지 않는 경우
            {
                memcpy(chpDest, &pRingBuffer[ReadPos], CurrentSize);
            }

            ReadPos = WritePos;
            //cout << "(가져올 데이터 부족)가져온 바이트 수: " << CurrentSize << endl;
            CurrentSize = 0;
            return temp;
        }
    }
    else // 링버퍼가 비어 있는 경우
        //cout << "링버퍼가 비어 있습니다." << endl;
        return 0;
}

int CRingBuffer::Peek(char* chpDest, int iSize)
{
    if (CurrentSize >= iSize) // 버퍼에 있는 데이터가 확인할 데이터보다 크거나 같은 경우
    {
        memcpy(chpDest, &pRingBuffer[ReadPos], iSize);

        // Test 코드
        //cout << endl << "데이터 " << iSize << "바이트 출력: " << chpDest << endl;

        return iSize;
    }
    else  // 버퍼에 있는 데이터가 확인할 데이터보다 작은 경우
    {
        memcpy(chpDest, &pRingBuffer[ReadPos], CurrentSize);

        // Test 코드
        //cout << endl << "데이터 " << iSize << "바이트 출력: " << chpDest << endl;

        return CurrentSize;
    }
}

void CRingBuffer::ClearBuffer(void)
{
    ReadPos = 0;
    WritePos = 0;
    CurrentSize = 0;
}

int CRingBuffer::DirectEnqueueSize(void)
{
    if (ReadPos < WritePos)
    {
        return BufferSize - WritePos;
    }
    else if (ReadPos > WritePos)
    {
        return  ReadPos - WritePos;
    }
    else
    {
        if (CurrentSize != BufferSize)
        {
            return BufferSize - WritePos;
        }
        else
        {
            return 0;
        }
    }
}

int CRingBuffer::DirectDequeueSize(void)
{
    if (ReadPos < WritePos)
    {
        return WritePos - ReadPos;
    }
    else if (ReadPos > WritePos)
    {
        return BufferSize - ReadPos;
    }
    else
    {
        if (CurrentSize == BufferSize)
        {
            return BufferSize - ReadPos;
        }
        else
        {
            return 0;
        }
    }
}

int CRingBuffer::MoveRear(int iSize)
{
    if (CurrentSize + iSize <= BufferSize)
    {
        if (WritePos + iSize >= BufferSize)
        {
            CurrentSize += BufferSize - WritePos;
            WritePos = 0;
            return BufferSize - WritePos;
        }
        else
        {
            WritePos += iSize;
            CurrentSize += iSize;
            return iSize;
        }
    }
    else
    {
        int temp = BufferSize - CurrentSize;
        WritePos = ReadPos;
        CurrentSize = 0;
        return temp;
    }
}

int CRingBuffer::MoveFront(int iSize)
{
    if (iSize <= CurrentSize)
    {
        if (ReadPos + iSize >= BufferSize)
        {
            CurrentSize -= BufferSize - ReadPos;
            ReadPos = 0;
            return BufferSize - ReadPos;
        }
        else
        {
            ReadPos += iSize;
            CurrentSize -= iSize;
            return iSize;
        }
    }
    else
    {
        int temp = CurrentSize;
        ReadPos = WritePos;
        CurrentSize = 0;
        return temp;
    }
}

char* CRingBuffer::GetFrontBufferPtr(void)
{
    return pRingBuffer + ReadPos;
}

char* CRingBuffer::GetRearBufferPtr(void)
{
    return pRingBuffer + WritePos;
}