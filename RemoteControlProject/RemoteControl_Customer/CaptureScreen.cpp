#include "stdafx.h"
#include "CaptureScreen.h"


CaptureScreen::CaptureScreen()
{
	h_buffer = ::GlobalAlloc(GMEM_MOVEABLE, GLOBAL_MEMORY_SIZE);
	if (::CreateStreamOnHGlobal(h_buffer, FALSE, &p_istream) == S_OK)
	{
		h_screen_dc = ::GetDC(NULL);

		/* create CImage */
		width = ::GetSystemMetrics(SM_CXSCREEN);//::GetDeviceCaps(h_screen_dc, HORZRES);
		height = ::GetSystemMetrics(SM_CYSCREEN);//::GetDeviceCaps(h_screen_dc, VERTRES); 
		color = ::GetDeviceCaps(h_screen_dc, BITSPIXEL);
		img.Create(width, height, color, 0);
	}

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
	p_istream->Release();  // delete IStream object
	if(h_buffer != NULL)
		::GlobalFree(h_buffer);

}
const char* CaptureScreen::capture(int& size)
{
	/* get contents of screen image */

	if (h_buffer != NULL)
	{
		if (p_istream != NULL)
		{
			LARGE_INTEGER startPos = { 0, 0 };
			ULARGE_INTEGER tempSize;

			p_istream->Seek(startPos, STREAM_SEEK_SET, NULL);
			
			::BitBlt(img.GetDC(), 0, 0, width, height, h_screen_dc, 0, 0, SRCCOPY);
			img.Save(p_istream, Gdiplus::ImageFormatJPEG);
			p_istream->Seek(startPos, STREAM_SEEK_CUR, &tempSize);	//to get img size
		
			size = tempSize.LowPart;
		}
	}
	if(img != NULL)
		img.ReleaseDC();

	return (const char*)::GlobalLock(h_buffer);
}

void CaptureScreen::unLockBuffer() 
{
	::GlobalUnlock(h_buffer);
}