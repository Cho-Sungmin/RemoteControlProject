#pragma once
#define GLOBAL_MEMORY_SIZE 1024 * 512
class CaptureScreen
{
private:
	HGLOBAL h_buffer;
	IStream *p_istream;
	HDC h_screen_dc;
	CImage img;
	int width, height, color;
	
public:
	CaptureScreen();
	~CaptureScreen();
	void unLockBuffer();
	const char* capture(int& size);
};

