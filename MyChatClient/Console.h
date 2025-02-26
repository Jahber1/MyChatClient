//////////////////////////////////////////////////////////////
// Windows 의 콘솔 화면에서 커서 제어.
//
//////////////////////////////////////////////////////////////
#ifndef __CONSOLE__
#define __CONSOLE__

#define dfSCREEN_WIDTH		82		// 콘솔 가로 81칸 + NULL
#define dfSCREEN_HEIGHT		24		// 콘솔 세로 24칸

class CScreenBuffer
{
private:
	CScreenBuffer() {}
	~CScreenBuffer() {}

public:
	char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

	static CScreenBuffer* GetInstance(void);
	//-------------------------------------------------------------
	// 콘솔 제어를 위한 준비 작업.
	//
	//-------------------------------------------------------------
	void cs_Initial(void);


	//-------------------------------------------------------------
	// 콘솔 화면의 커서를 X, Y 좌표로 이동시킨다.
	//
	//-------------------------------------------------------------
	void cs_MoveCursor(int iPosX, int iPosY);

	//-------------------------------------------------------------
	// 콘솔 화면을 조기화 한다.
	//
	//-------------------------------------------------------------
	void cs_ClearScreen(void);
};

#endif

