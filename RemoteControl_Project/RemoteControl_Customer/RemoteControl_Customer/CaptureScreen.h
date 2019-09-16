#pragma once

class CaptureScreen
{
private:
	HDC h_screen_dc;//, h_memory_dc;
	CImage img;
	int width, height, color;
	
public:
	CaptureScreen();
	~CaptureScreen();
	bool capture();
};

