// HeroEmblemsAI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HeroEmblemsAI.h"
#include "Winuser.h"
#include <atlimage.h> 
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

const double WIDTH = 48.375;
const double HEIGHT = 48.42857;

void WriteBmpToFile(HBITMAP hBitmap, LPCTSTR path) {
	HDC hDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * ::GetDeviceCaps(hDC, PLANES);//��ǰ�ֱ�����ÿ��������ռ�ֽ���    
	::DeleteDC(hDC);

	WORD   wBitCount;   //λͼ��ÿ��������ռ�ֽ���      
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = iBits;

	DWORD   dwPaletteSize = 0;    //��ɫ���С�� λͼ�������ֽڴ�С   
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *    sizeof(RGBQUAD);


	BITMAP  bm;        //λͼ���Խṹ  
	::GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);


	BITMAPINFOHEADER   bi;       //λͼ��Ϣͷ�ṹ       
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB; //BI_RGB��ʾλͼû��ѹ��  
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmBitsSize = ((bm.bmWidth * wBitCount + 31) / 32) * 4 * bm.bmHeight;
	HANDLE hDib = ::GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));  //Ϊλͼ���ݷ����ڴ�  
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	HANDLE hPal = ::GetStockObject(DEFAULT_PALETTE);  // �����ɫ��   
	HANDLE  hOldPal = NULL;
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	::GetDIBits(hDC, hBitmap, 0, (UINT)bm.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);// ��ȡ�õ�ɫ�����µ�����ֵ  
	if (hOldPal)//�ָ���ɫ��  
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	BITMAPFILEHEADER   bmfHdr; //λͼ�ļ�ͷ�ṹ       
	bmfHdr.bfType = 0x4D42;  // "BM"    // ����λͼ�ļ�ͷ  
	DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);//����λͼ�ļ�     
	DWORD dwWritten;
	WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);   // д��λͼ�ļ�ͷ  
	WriteFile(hFile, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);// д��λͼ�ļ���������  

	GlobalUnlock(hDib);   //���     
	GlobalFree(hDib);
	CloseHandle(hFile);
}

void cropImage(HBITMAP input) {
	CImage tem;
	tem.Attach(input);
	double width = tem.GetWidth();
	double height = tem.GetHeight();

	for (int row = 0; row < 7; row++) {
		for (int col = 0; col < 8; col++) {
			CImage piece;



			piece.Create(width / 8, height / 7, tem.GetBPP());
			tem.BitBlt(piece.GetDC(), 0, 0, width / 8, height / 7, width / 8 * col, height / 7 * row, SRCCOPY);


			string s = "out/" + to_string(row) + to_string(col) + ".bmp";
			std::wstring stemp = std::wstring(s.begin(), s.end());
			LPCWSTR sw = stemp.c_str();


			WriteBmpToFile(piece, sw);
			piece.ReleaseDC();
			piece.Destroy();
		}
	}
}

HBITMAP getScreenshotByHWND(HWND hWnd) {
	
	double dy = 0.5376;
	double dx = 0.9724;

	HDC hScreenDC = ::GetDC(hWnd);
	HDC MemDC = ::CreateCompatibleDC(hScreenDC);
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	SIZE screensize;
	screensize.cx = rect.right - rect.left;
	screensize.cy = rect.bottom - rect.top;
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hScreenDC, screensize.cx*dx*0.9773, screensize.cy*dy*0.83498);
	HGDIOBJ hOldBMP = ::SelectObject(MemDC, hBitmap);

	::BitBlt(MemDC, 0, 0, screensize.cx*dx, screensize.cy*(1-dy), hScreenDC, screensize.cx*(1-dx), screensize.cy*dy, SRCCOPY);
	cout << rect.left << " " << rect.top << endl;
	::SelectObject(MemDC, hOldBMP);
	::DeleteObject(MemDC);
	::ReleaseDC(hWnd, hScreenDC);
	return hBitmap;
}


int main() {
	HWND hWnd;

	hWnd = ::FindWindow(_T("CHWindow"), NULL);

	if (hWnd) cout << "a" << endl;

	HBITMAP screenshot = getScreenshotByHWND(hWnd);
	WriteBmpToFile(screenshot,_T("a.bmp"));
	cropImage(screenshot);

	

	return 0;
}