#pragma once
#include <iostream>

#define RINGBUF_SIZE 1000

class CRingBuffer
{
public:

	CRingBuffer(int iBufferSize = RINGBUF_SIZE);
	~CRingBuffer();

	void Resize(int size);
	int	GetBufferSize(void);
	int	GetUseSize(void);
	int	GetFreeSize(void);
	int	Enqueue(const char* chpData, int iSize);
	int	Dequeue(char* chpDest, int iSize);
	int	Peek(char* chpDest, int iSize);
	void ClearBuffer(void);

	int	DirectEnqueueSize(void);
	int	DirectDequeueSize(void);
	int	MoveRear(int iSize);
	int	MoveFront(int iSize);
	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);

	char* pRingBuffer;
	int BufferSize;
	int ReadPos;
	int WritePos;
	int CurrentSize;
};