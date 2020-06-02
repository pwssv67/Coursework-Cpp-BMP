#include <minwindef.h>
#define _CRT_SECURE_NO_WARNINGS

struct BITMAPFILEHEADER {
	WORD    Type;     // ‘BM’ 0x4D42   
	DWORD   Size;     // Размер файла в байтах, BitCount*Height*Width+ OffsetBits   
	WORD    Reserved1;    // Зарезервирован; должен быть нуль  
	WORD    Reserved2;    // Зарезервирован; должен быть нуль  
	DWORD   OffsetBits;   // Смещение данных от начала файла в байтах // = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) 
}; 

struct BITMAPINFOHEADER {
	DWORD   Size;           // Число байтов необходимое для структуры = 40  
	DWORD   Width;          // Ширина точечного рисунка в пикселях   
	DWORD   Height;         // Высота точечного рисунка в пикселях   
	WORD    Planes;         // Число плоскостей целевого устройства = 1 
	WORD    BitCount;       // Глубина цвета, число бит на точку = 0,1,4,8,16,24,32   
	DWORD   Compression;    // Тип сжатия = 0 для несжатого изображения   
	DWORD   SizeImage;      // Размер изображения в байтах BitCount*Height*Width  
	DWORD   XPelsPerMeter;  // Разрешающая способность по горизонтали  
	DWORD   YPelsPerMeter;  // Разрешающая способность по вертикали  
	DWORD   ColorUsed;      // Число индексов используемых цветов. Если все цвета = 0  
	DWORD   ColorImportant; // Число необходимых цветов = 0
};


struct RGBQUAD {
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Reserved;
};

class Image {
	RGBQUAD **Rgbquad;
	BITMAPINFOHEADER BMInfoHeader;

};