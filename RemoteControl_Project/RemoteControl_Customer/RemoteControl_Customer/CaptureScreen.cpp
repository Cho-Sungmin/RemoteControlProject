#include "stdafx.h"
#include "CaptureScreen.h"


CaptureScreen::CaptureScreen()
{
	h_screen_dc = ::GetDC(NULL);

	//ÇØ»óµµ
	width = ::GetSystemMetrics(SM_CXSCREEN);//::GetDeviceCaps(h_screen_dc, HORZRES);
	height = ::GetSystemMetrics(SM_CYSCREEN);//::GetDeviceCaps(h_screen_dc, VERTRES); 
	color = ::GetDeviceCaps(h_screen_dc, BITSPIXEL);

	img.Create(width, height, color, 0);

	/////DIB file format/////

	/*dib_define.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib_define.bmiHeader.biWidth = width;
	dib_define.bmiHeader.biHeight = height;
	dib_define.bmiHeader.biPlanes = 1;
	dib_define.bmiHeader.biBitCount = 24;
	dib_define.bmiHeader.biCompression = BI_RGB;
	dib_define.bmiHeader.biSizeImage = (((width * 24 + 31) & ~31) >> 3) * height;
	dib_define.bmiHeader.biXPelsPerMeter = 0;
	dib_define.bmiHeader.biYPelsPerMeter = 0;
	dib_define.bmiHeader.biClrImportant = 0;
	dib_define.bmiHeader.biClrUsed = 0;

	///// DIB file header /////
	ZeroMemory(&dib_format_layout, sizeof(BITMAPFILEHEADER));
	dib_format_layout.bfType = *(WORD*)"BM";
	dib_format_layout.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dib_define.bmiHeader.biSizeImage;
	dib_format_layout.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	*/
}


CaptureScreen::~CaptureScreen()
{
	img.Destroy();
	if(h_screen_dc != NULL)
		::ReleaseDC(NULL, h_screen_dc);
}

bool CaptureScreen::capture()
{
	::BitBlt(img.GetDC(), 0, 0, width, height, h_screen_dc, 0, 0, SRCCOPY);
	img.Save(L"image.jpg", Gdiplus::ImageFormatJPEG);
	if(img != NULL)
		img.ReleaseDC();
	return true;
}