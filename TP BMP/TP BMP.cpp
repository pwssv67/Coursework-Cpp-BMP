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
	std::vector <RGBQUAD> palette;
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
		BMInfoHeader.biXPelsPerMeter = 200;
		BMInfoHeader.biYPelsPerMeter = 200;
		if (BCount <= 8) {
			BMInfoHeader.biClrUsed = (int)pow(2, BMInfoHeader.biBitCount);
			palette = std::vector <RGBQUAD>((int)pow(2, BMInfoHeader.biBitCount));
			switch (BCount) {
			case 1:
				palette[0].rgbRed = palette[0].rgbGreen = palette[0].rgbBlue = 0x0F;
				palette[0].rgbReserved = palette[0].rgbReserved = 0;
				palette[1].rgbRed = palette[1].rgbGreen = palette[1].rgbBlue = 0xFF;
				BMInfoHeader.biSizeImage = ceil((int)Width * (int)Height / 8.0);
				break;
			case 4:
				for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
					palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = 17 * (i);
					palette[i].rgbReserved = 0;
				}
				BMInfoHeader.biSizeImage = ceil((int)Width * Height / 2.0);
			}
		}

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
		case 8:
			break;
		case 4:
			Rgbquad = std::vector <RGBQUAD>(BMInfoHeader.biHeight * BMInfoHeader.biWidth);

			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				Rgbquad[i].rgbBlue = 15;
				Rgbquad[i].rgbGreen = Mode / 16;
				Rgbquad[i].rgbRed = Mode / 16;
				Rgbquad[i].rgbReserved = 0;
			}
			break;
		case 1:
			Rgbquad = std::vector <RGBQUAD>(BMInfoHeader.biHeight * BMInfoHeader.biWidth);

			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				Rgbquad[i].rgbBlue = 1;
				Rgbquad[i].rgbGreen = Mode / 127;
				Rgbquad[i].rgbRed = Mode / 127;
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
		BYTE buffer = 0;
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

		case 4:
			palette = std::vector <RGBQUAD>((int)pow(2, BMInfoHeader.biBitCount));
			for (int i = 0; i < (int)pow(2, BMInfoHeader.biBitCount); i++) {
				palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = 17 * (i);
				palette[i].rgbReserved = 0;
			}

			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				if (i % 2 == 0) {
					fread(&buffer, 1, 1, f);
					Rgbquad[i] = palette[(buffer>>4) & 0b1111];
				}
				else {
					Rgbquad[i] = palette[(buffer) & 0b1111];
				}
			}
			break;
		case 1:
			palette = std::vector <RGBQUAD>(2);
			palette[0].rgbRed = palette[0].rgbGreen = palette[0].rgbBlue = 0x0F;
			palette[0].rgbReserved = palette[0].rgbReserved = 0;
			palette[1].rgbRed = palette[1].rgbGreen = palette[1].rgbBlue = 0xFF;
			buffer = 0;
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				if (i % 8 == 0) {
					fread(&buffer, 1, 1, f);
				}
				Rgbquad[i] = palette[(buffer >> (i % 8)) & 0b1];
			}
		}
		std::cout << (int)Rgbquad[BMInfoHeader.biHeight * BMInfoHeader.biWidth - 1].rgbBlue << " " << (int)Rgbquad[BMInfoHeader.biHeight* BMInfoHeader.biWidth - 1].rgbGreen;
		return 0;
	}

	void writeimage(const char* filename) {
		FILE* f2;
		BITMAPFILEHEADER BMFileHeader;
		BMFileHeader.bfOffBits = 14 + BMInfoHeader.biSize;
		if (BMInfoHeader.biBitCount <= 8) {
			BMFileHeader.bfOffBits += pow(2, BMInfoHeader.biBitCount) * 4;
		}
		BMFileHeader.bfReserved1 = 0;
		BMFileHeader.bfReserved2 = 0;
		BMFileHeader.bfType = 0x4D42;
		BMFileHeader.bfSize = BMInfoHeader.biSizeImage + BMFileHeader.bfOffBits;
		BMFileHeader.bfSize += 4 - BMFileHeader.bfSize % 4;
		//BMInfoHeader.biSizeImage += 2;
		f2 = fopen(filename, "wb");
		fwrite(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f2);
		fwrite(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f2);
		if (BMInfoHeader.biBitCount <= 8) {
			for (int i = 0; i < palette.size(); i++) {
				fwrite(&palette[i], sizeof(RGBQUAD), 1, f2);
			}
		}
		fseek(f2, BMFileHeader.bfOffBits, SEEK_SET);
		int empty = 0;
		int buffer = 0;
		switch (BMInfoHeader.biBitCount) {
		case 32:
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				//std::cout << i << "\n" ;
				fwrite(&Rgbquad[i], sizeof(RGBQUAD), 1, f2);
			}
			for (int i = 0; i < 4 - BMInfoHeader.biSizeImage % 4; i++) {
				fwrite(&empty, 1, 1, f2);
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
		case 8:
			break;
		case 4:
			buffer = 0;
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				buffer = buffer << 4;
				buffer += Rgbquad[i].rgbBlue/16;
				if (i % 2 == 1) {
					fwrite(&buffer, 1, 1, f2);
					buffer = 0;
				}
			}
			if (buffer != 0) {
				buffer = buffer << 4;
				fwrite(&buffer, 1, 1, f2);
			}
			break;
		case 1:
			buffer = 0;
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				buffer = buffer << 1;
				buffer += Rgbquad[i].rgbBlue/128;
				if (i % 8 == 7) {
					fwrite(&buffer, 1, 1, f2);
					buffer = 0;
				}
			}
			if (buffer != 0) {
				while ((buffer & 128) != 128) {
					buffer = buffer << 1;
				}
				fwrite(&buffer, 1, 1, f2);
			}
			for (int i = 0; i < 2; i++) {
				fwrite(&empty, 1, 1, f2);
			}
			break;
		}
	}

	Image operator = (Image Inp) {
		BMInfoHeader = Inp.BMInfoHeader;
		Rgbquad = Inp.Rgbquad;
		return *this;
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
		}
		*this = img;
		return *this;
	}

	Image operator / (short Depth) {
		int oldBitCount = this->BMInfoHeader.biBitCount;
		this->BMInfoHeader.biBitCount = Depth;
		if (Depth <= 8) {
			this->BMInfoHeader.biClrUsed = (int)pow(2, BMInfoHeader.biBitCount);
			this->palette = std::vector <RGBQUAD>((int)pow(2, BMInfoHeader.biBitCount));
			switch (Depth) {
			case 1:
				this->palette[0].rgbRed = this->palette[0].rgbGreen = this->palette[0].rgbBlue = 0x0F;
				this->palette[0].rgbReserved = this->palette[0].rgbReserved = 0;
				this->palette[1].rgbRed = this->palette[1].rgbGreen = this->palette[1].rgbBlue = 0xFF;
				this->BMInfoHeader.biSizeImage = ceil((int)this->BMInfoHeader.biWidth * (int)this->BMInfoHeader.biHeight / 8.0);
				break;
			case 4:
				for (int i = 0; i < this->BMInfoHeader.biClrUsed; i++) {
					this->palette[i].rgbRed = this->palette[i].rgbGreen = this->palette[i].rgbBlue = 17 * (i);
					this->palette[i].rgbReserved = 0;
				}
				this->BMInfoHeader.biSizeImage = ceil((int)this->BMInfoHeader.biWidth * this->BMInfoHeader.biHeight / 2.0);
			}
			int color = 0;
			for (int i = 0; i < this->Rgbquad.size(); i++) {
				color = 0.299 * Rgbquad[i].rgbRed + 0.597 * Rgbquad[i].rgbGreen + 0.114 * Rgbquad[i].rgbBlue;
				color >= 256 ? color = 255:color;
				if (Depth == 1) {
					Rgbquad[i] = palette[color / 128];
				}
				else {
					Rgbquad[i] = palette[color / 16];
				}
			}
		}
		else {
			this->BMInfoHeader.biClrUsed = 0;

			palette.clear();
		}
		return *this;
	}

};

int main(){
	//Image->loadimage("test.bmp");

	Image img("test32.bmp");
	Image img2 = Image('b', 32, 300, 400);
	img2 /= img/1; 
	img2.writeimage("test_write.bmp");
	//Image img(char(255), 4, 64, 64);
	//Image img("input1bit.bmp");
	//img.writeimage("test1bit.bmp");
}