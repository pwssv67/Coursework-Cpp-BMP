#include <minwindef.h>
#define _CRT_SECURE_NO_WARNINGS

struct BITMAPFILEHEADER {
	WORD    Type;     // �BM� 0x4D42   
	DWORD   Size;     // ������ ����� � ������, BitCount*Height*Width+ OffsetBits   
	WORD    Reserved1;    // ��������������; ������ ���� ����  
	WORD    Reserved2;    // ��������������; ������ ���� ����  
	DWORD   OffsetBits;   // �������� ������ �� ������ ����� � ������ // = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) 
};

struct BITMAPINFOHEADER {
	DWORD   Size;           // ����� ������ ����������� ��� ��������� = 40  
	DWORD   Width;          // ������ ��������� ������� � ��������   
	DWORD   Height;         // ������ ��������� ������� � ��������   
	WORD    Planes;         // ����� ���������� �������� ���������� = 1 
	WORD    BitCount;       // ������� �����, ����� ��� �� ����� = 0,1,4,8,16,24,32   
	DWORD   Compression;    // ��� ������ = 0 ��� ��������� �����������   
	DWORD   SizeImage;      // ������ ����������� � ������ BitCount*Height*Width  
	DWORD   XPelsPerMeter;  // ����������� ����������� �� �����������  
	DWORD   YPelsPerMeter;  // ����������� ����������� �� ���������  
	DWORD   ColorUsed;      // ����� �������� ������������ ������. ���� ��� ����� = 0  
	DWORD   ColorImportant; // ����� ����������� ������ = 0
};



struct RGBQUAD {
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Reserved;
};

class Image {
	RGBQUAD** Rgbquad;
	BITMAPINFOHEADER BMInfoHeader;

};