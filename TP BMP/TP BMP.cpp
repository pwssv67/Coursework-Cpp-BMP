#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <minwindef.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
/*
#pragma pack (push,1)
struct BITMAPFILEHEADER {
	WORD    Type;     // ‘BM’ 0x4D42   
	DWORD   Size;     // Размер файла в байтах, BitCount*Height*Width+ OffsetBits   
	WORD    Reserved1;    // Зарезервирован; должен быть нуль  
	WORD    Reserved2;    // Зарезервирован; должен быть нуль  
	DWORD   OffsetBits;   // Смещение данных от начала файла в байтах // = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) 
};
#pragma pack(pop)

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

*/
class Image {
	std::vector <RGBQUAD> Rgbquad;
	BITMAPINFOHEADER BMInfoHeader;
	
public:
	Image(char Mode, WORD BCount, int Width, int Height) {
		BMInfoHeader.biBitCount = BCount;
		BMInfoHeader.biSize = 40;
		BMInfoHeader.biHeight = Height;
		BMInfoHeader.biWidth = Width;
		BMInfoHeader.biSizeImage = Width * Height * int(BCount)/8;
		BMInfoHeader.biClrImportant = 0;
		BMInfoHeader.biClrUsed = 0;
		BMInfoHeader.biCompression = 0;
		BMInfoHeader.biPlanes = 1;
		BMInfoHeader.biXPelsPerMeter = 2000;
		BMInfoHeader.biYPelsPerMeter = 2000;

		switch (BCount) {
		case 32:
			Rgbquad = std::vector <RGBQUAD> (BMInfoHeader.biHeight * BMInfoHeader.biWidth);

			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				Rgbquad[i].rgbBlue = Mode;
				Rgbquad[i].rgbGreen = Mode;
				Rgbquad[i].rgbRed = Mode;
				Rgbquad[i].rgbReserved = 0;
			}
			break;
		case 24:
			Rgbquad = std::vector <RGBQUAD>((BMInfoHeader.biHeight+1) * BMInfoHeader.biWidth);

			for (int i = 0; i < (BMInfoHeader.biHeight+1) * BMInfoHeader.biWidth; i++) {
				Rgbquad[i].rgbBlue = Mode;
				Rgbquad[i].rgbGreen = Mode;
				Rgbquad[i].rgbRed = Mode;
				Rgbquad[i].rgbReserved = 0;
			}
			break;
		}
	}

	Image(const char* fileName) {
		loadimage(fileName);
	}

	Image() {}

	Image(const Image &i) {
		BMInfoHeader = i.BMInfoHeader;
		Rgbquad = i.Rgbquad;
	}

	int loadimage(const char* fileName) {
		FILE* f;
		BITMAPFILEHEADER BMFileHeader;
		f = fopen(fileName, "rb");
		fread(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
		//printf(std::string(BMFileHeader.bfSize, sizeof(BMFileHeader.bfSize)));
		std::cout << BMFileHeader.bfSize << "\n";
		fread(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
		std::cout << BMInfoHeader.biHeight << "\n";
		std::cout << BMFileHeader.bfOffBits << "\n";
		fseek(f, BMFileHeader.bfOffBits, SEEK_SET);
		std::cout << BMFileHeader.bfSize << "\n";
		if (BMInfoHeader.biBitCount == 24) {
			Rgbquad = std::vector <RGBQUAD>((BMInfoHeader.biHeight+1) * BMInfoHeader.biWidth);
		}
		else {
			Rgbquad = std::vector <RGBQUAD>(BMInfoHeader.biHeight * BMInfoHeader.biWidth);
		}

		switch (BMInfoHeader.biBitCount){
		case 32:
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				//std::cout << i << "\n" ;
				fread(&Rgbquad[i], sizeof(RGBQUAD), 1, f);
			}
			break;
		case 24:
			for (int i = 0; i < (int)((BMInfoHeader.biHeight+1) * BMInfoHeader.biWidth ); i++) {
				//std::cout << i << "\n" ;
				fread(&Rgbquad[i], sizeof(RGBQUAD)-1, 1, f);
			}
			break;
		}
		std::cout << (int)Rgbquad[BMInfoHeader.biHeight * BMInfoHeader.biWidth - 1].rgbBlue << " " << (int)Rgbquad[BMInfoHeader.biHeight* BMInfoHeader.biWidth - 1].rgbGreen;
		return 0;
	}

	void writeimage(const char* filename) {
		FILE* f2;
		BITMAPFILEHEADER BMFileHeader;
		BMFileHeader.bfOffBits = 14 + BMInfoHeader.biSize;
		BMFileHeader.bfReserved1 = 0;
		BMFileHeader.bfReserved2 = 0;
		BMFileHeader.bfType = 0x4D42;
		BMFileHeader.bfSize = BMInfoHeader.biSizeImage + BMFileHeader.bfOffBits;
		f2 = fopen(filename, "wb");
		fwrite(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f2);
		fwrite(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f2);
		fseek(f2, BMFileHeader.bfOffBits, SEEK_SET);
		BYTE empty = (BYTE)0;
		switch (BMInfoHeader.biBitCount) {
		case 32:
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				//std::cout << i << "\n" ;
				fwrite(&Rgbquad[i], sizeof(RGBQUAD), 1, f2);
			}
			for (int i = 0; i < 4 - BMInfoHeader.biSizeImage % 4; i++) {
				fwrite(&empty, sizeof(BYTE), 1, f2);
			}
			break;
		case 24:
			for (int i = 0; i < (int)((BMInfoHeader.biHeight+1) * BMInfoHeader.biWidth ); i++) {
				//std::cout << i << "\n" ;
				fwrite(&Rgbquad[i], sizeof(RGBQUAD)-1, 1, f2);
			}
			for (int i = 0; i < 4 - ((BMInfoHeader.biHeight + 1) * BMInfoHeader.biWidth) % 4; i++) {
				//fwrite(&empty, sizeof(BYTE), 1, f2);
			}
			break;
		}
	}

	Image operator = (Image Inp) {
		return Image(Inp);
	}
	
	Image operator /= (Image InpImage) {
		Image img = Image();
		img.BMInfoHeader = BMInfoHeader;
		std::cout << InpImage.Rgbquad[0].rgbGreen;
		if (img.BMInfoHeader.biBitCount == 24) {
			img.Rgbquad = std::vector <RGBQUAD>((img.BMInfoHeader.biHeight + 1) * img.BMInfoHeader.biWidth);
		}
		else {
			img.Rgbquad = std::vector <RGBQUAD>(img.BMInfoHeader.biHeight* img.BMInfoHeader.biWidth);
		}
		//int height = Rgbquad.size() / img.BMInfoHeader.biWidth;
		int height = img.BMInfoHeader.biHeight;
		int width = img.BMInfoHeader.biWidth;
		bool height_bigger;
		bool width_bigger;
		img.BMInfoHeader.biHeight > InpImage.BMInfoHeader.biHeight ? height_bigger = true : height_bigger = false;
		img.BMInfoHeader.biWidth > InpImage.BMInfoHeader.biWidth ? width_bigger = true : width_bigger = false;
		float counter_width = abs((InpImage.BMInfoHeader.biWidth - width)/ (float)BMInfoHeader.biWidth);
		float counter_height = abs((InpImage.BMInfoHeader.biHeight - height) / (float)BMInfoHeader.biHeight);
		float counter_height_temp;
		height_bigger? counter_height_temp = counter_height :counter_height_temp = 0;
		float counter_width_temp = 0;
		int source_i = 0;
		int source_j = 0;   //TODO ибо всрато!
		for (int i = 0; i < height; i++) {
			source_j = 0;
			width_bigger ? counter_width_temp = counter_width : counter_width_temp = 0;
			for (int j = 0; j < width; j++) {
				img.Rgbquad[i * width + j] = InpImage.Rgbquad[source_i *InpImage.BMInfoHeader.biWidth + source_j];
				counter_width_temp += counter_width;
				//if (i<1)
				//	std::cout<<j <<"-" <<counter_width_temp<< "-"<< source_j << "|";
				if (width_bigger) {
					if (counter_width_temp > 1) {
						counter_width_temp--;
						if (counter_width > 1) {
							counter_width_temp -= int(counter_width);
						}        
					}
					else {
						if (source_j + 1 <= InpImage.BMInfoHeader.biWidth - 1) {
							source_j++;
						}
					}
				} 
				else {
					if (source_j + 1 + int(counter_width_temp) <= InpImage.BMInfoHeader.biWidth - 1) {
						source_j += 1 + int(counter_width_temp);
					}
					else {
						source_j = InpImage.BMInfoHeader.biWidth - 1;
					}
					if (counter_width_temp > 1) {
						if (counter_width > 1) {
							counter_width_temp -= int(counter_width_temp);
						}
						else {
							counter_width_temp -= 1.001;
						}
					}
				}
			}
			counter_height_temp += counter_height;

			if (height_bigger) {
				if (counter_height_temp > 1) {
					counter_height_temp--;
					if (counter_height >= 1) {
						counter_height_temp -= int(counter_height);
					}
				}
				else {
					if (source_i + 1 <= InpImage.BMInfoHeader.biHeight - 1) {
						source_i++;
					}
				}
			}
			else {
				if (source_i + 1 + int(counter_height_temp) <= InpImage.BMInfoHeader.biHeight - 1) {
					source_i += 1 + int(counter_height_temp);
				}
				else {
					source_i = InpImage.BMInfoHeader.biHeight - 1;
				}
				if (counter_height_temp > 1) {
					if (counter_height > 1) {
						counter_height_temp -= int(counter_height_temp);
					}
					else {
						counter_height_temp -= 1.01;
					}
				}
			}


			//std::cout << source_i << "|";
			//source_i++;
			
		}
		std::cout << "\n" << img.BMInfoHeader.biWidth << '\n' << img.Rgbquad[0].rgbRed;
		img.writeimage("tesst.bmp");
		return img;
	}

};

int main(){
	//Image->loadimage("test.bmp");
	Image img("test32.bmp");
	Image img2 = Image('b', 32, 50, 100);
	img2 /= img; 
	img2.writeimage("test_write.bmp");
}