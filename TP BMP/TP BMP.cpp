#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <minwindef.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>

class Image {
	std::vector <std::vector <RGBQUAD>> Rgbquad;
	std::vector <RGBQUAD> Palette;
	BITMAPINFOHEADER BMInfoHeader;
	
public:
	Image(char Mode, WORD BCount, long int Width, long int Height) {
		//header
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
		//palette
		if (BCount <= 8) {
			BMInfoHeader.biClrUsed = (int)pow(2, BMInfoHeader.biBitCount);
			Palette = std::vector <RGBQUAD>((int)pow(2, BMInfoHeader.biBitCount));
			switch (BCount) {
			case 1:
				Palette[0].rgbRed = Palette[0].rgbGreen = Palette[0].rgbBlue = 0x0F;
				Palette[0].rgbReserved = Palette[0].rgbReserved = 0;
				Palette[1].rgbRed = Palette[1].rgbGreen = Palette[1].rgbBlue = 0xFF;
				BMInfoHeader.biSizeImage = ceil((Width * Height) / 8.0);
				break;
			case 4:
				for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
					Palette[i].rgbRed = Palette[i].rgbGreen = Palette[i].rgbBlue = 17 * (i);
					Palette[i].rgbReserved = 0;
				}
				BMInfoHeader.biSizeImage = BMInfoHeader.biWidth * BMInfoHeader.biHeight / 2.0;
				break;
			}
		}

		//rgbquad
		if (BCount == 24) {
			Rgbquad = std::vector <std::vector <RGBQUAD>>(BMInfoHeader.biHeight+1);
			for (int i = 0; i < BMInfoHeader.biHeight+1; i++) {
				Rgbquad[i] = std::vector <RGBQUAD>(BMInfoHeader.biWidth);
			}

			for (int i = 0; i < BMInfoHeader.biHeight+1; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					Rgbquad[i][j].rgbBlue = Mode;
					Rgbquad[i][j].rgbGreen = Mode;
					Rgbquad[i][j].rgbRed = Mode;
					Rgbquad[i][j].rgbReserved = 0;
				}
			}
		}

		else {
			Rgbquad = std::vector <std::vector <RGBQUAD>>(BMInfoHeader.biHeight);
			for (int i = 0; i < BMInfoHeader.biHeight; i++) {
				Rgbquad[i] = std::vector <RGBQUAD>(BMInfoHeader.biWidth);
			}

			for (int i = 0; i < BMInfoHeader.biHeight ; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					Rgbquad[i][j].rgbBlue = Mode;
					Rgbquad[i][j].rgbGreen = Mode;
					Rgbquad[i][j].rgbRed = Mode;
					Rgbquad[i][j].rgbReserved = 0;
				}
			}
		}

	}

	Image(const char* fileName) {
		loadimage(fileName);
	}

	Image() {}

	Image(const Image &i) {
		BMInfoHeader = i.BMInfoHeader;
		Rgbquad = i.Rgbquad;
		Palette = i.Palette;
	}

	int loadimage(const char* fileName) {
		FILE* f;
		BITMAPFILEHEADER BMFileHeader;
		f = fopen(fileName, "rb");
		fread(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
		std::cout << BMFileHeader.bfSize << "\n";
		fread(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
		std::cout << BMInfoHeader.biHeight << "\n";
		std::cout << BMFileHeader.bfOffBits << "\n";
		fseek(f, BMFileHeader.bfOffBits, SEEK_SET);
		std::cout << BMFileHeader.bfSize << "\n";

		if (BMInfoHeader.biBitCount == 24) {
			Rgbquad = std::vector <std::vector <RGBQUAD>>(BMInfoHeader.biHeight+1);
			for (int i = 0; i < BMInfoHeader.biHeight+1; i++) {
				Rgbquad[i] = std::vector <RGBQUAD>(BMInfoHeader.biWidth);
			}
		}
		else {
			Rgbquad = std::vector <std::vector <RGBQUAD>>(BMInfoHeader.biHeight);
			for (int i = 0; i < BMInfoHeader.biHeight; i++) {
				Rgbquad[i] = std::vector <RGBQUAD>(BMInfoHeader.biWidth);
			}
		}

		BYTE buffer = 0;

		switch (BMInfoHeader.biBitCount) {
		case 32:
			for (int i = 0; i < BMInfoHeader.biHeight; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					fread(&Rgbquad[i][j], sizeof(RGBQUAD), 1, f);
				}
			}
			break;

		case 24:
			for (int i = 0; i < BMInfoHeader.biHeight+1; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					fread(&Rgbquad[i][j], sizeof(RGBQUAD), 1, f);
				}
			}
			break;

		case 8:
			break;

		case 4:
			Palette = std::vector <RGBQUAD>((int)pow(2, BMInfoHeader.biBitCount));
			for (int i = 0; i < (int)pow(2, BMInfoHeader.biBitCount); i++) {
				Palette[i].rgbRed = Palette[i].rgbGreen = Palette[i].rgbBlue = 17 * (i);
				Palette[i].rgbReserved = 0;
			}

			for (int i = 0; i < BMInfoHeader.biHeight; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					if ((i*BMInfoHeader.biWidth + j) % 2 == 0) {
						fread(&buffer, 1, 1, f);
						Rgbquad[i][j] = Palette[(buffer >> 4) & 0b1111];
					}
					else {
						Rgbquad[i][j] = Palette[(buffer) & 0b1111];
					}
				}
			}
			break;
		case 1:
			Palette = std::vector <RGBQUAD>(2);
			Palette[0].rgbRed = Palette[0].rgbGreen = Palette[0].rgbBlue = 0x0F;
			Palette[0].rgbReserved = Palette[0].rgbReserved = 0;
			Palette[1].rgbRed = Palette[1].rgbGreen = Palette[1].rgbBlue = 0xFF;
			buffer = 0;
			for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					if ((i * BMInfoHeader.biWidth +j) % 8 == 0) {
						fread(&buffer, 1, 1, f);
					}
					Rgbquad[i][j] = Palette[(buffer >> (7 - (i * BMInfoHeader.biWidth + j) % 8)) & 0b1];
				}
			}
		}
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
		f2 = fopen(filename, "wb");
		fwrite(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f2);
		fwrite(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f2);

		if (BMInfoHeader.biBitCount <= 8) {
			for (int i = 0; i < Palette.size(); i++) {
				fwrite(&Palette[i], sizeof(RGBQUAD), 1, f2);
			}
		}

		fseek(f2, BMFileHeader.bfOffBits, SEEK_SET);
		int empty = 0;
		int buffer = 0;
		int temp = 0;
		switch (BMInfoHeader.biBitCount) {
		case 32:
			for (int i = 0; i < BMInfoHeader.biHeight ; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					fwrite(&Rgbquad[i][j], sizeof(RGBQUAD), 1, f2);
				}
			}
			for (int i = 0; i < 4 - BMInfoHeader.biSizeImage % 4; i++) {
				fwrite(&empty, 1, 1, f2);
			}

			break;
		case 24:
			for (int i = 0; i < BMInfoHeader.biHeight+1; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					fwrite(&Rgbquad[i][j], sizeof(RGBQUAD)-1, 1, f2);
				}
			}
			for (int i = 0; i < 4 - ((BMInfoHeader.biHeight + 1) * BMInfoHeader.biWidth) % 4; i++) {
				fwrite(&empty, 1, 1, f2);
			}
			break;

		case 8:
			break;

		case 4:
			buffer = 0;
			for (int i = 0; i < BMInfoHeader.biHeight ; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					temp = i;
					buffer = buffer << 4;
					buffer += (int)(Rgbquad[i][j].rgbBlue / 16);
					if ((i*BMInfoHeader.biWidth + j) % 2 == 1) {
						fwrite(&buffer, 1, 1, f2);
						buffer = 0;
					}
				}
			}

			if (buffer != 0) {
				buffer = buffer << 4;
				fwrite(&buffer, 1, 1, f2);
			}

			for (int i = 0; i < temp; i++) {
				fwrite(&empty, 1, 1, f2);
			}

			break;

		case 1:

			buffer = 0;
			for (int i = 0; i < BMInfoHeader.biHeight; i++) {
				for (int j = 0; j < BMInfoHeader.biWidth; j++) {
					buffer = buffer << 1;
					buffer += Rgbquad[i][j].rgbBlue / 128;
					if ((i * BMInfoHeader.biWidth + j) % 8 == 7) {
						fwrite(&buffer, 1, 1, f2);
						buffer = 0;
					}
				}
			}

			if (buffer != 0) {
				while ((buffer & 0b10000000) != 128) {
					buffer = buffer << 1;
				}
				fwrite(&buffer, 1, 1, f2);
			}

			break;
		}
	}

	Image operator = (Image Inp) {
		BMInfoHeader = Inp.BMInfoHeader;
		Rgbquad = Inp.Rgbquad;
		Palette = Inp.Palette;
		return *this;
	}
	
	Image operator /= (Image InpImage) {
		Image img = Image();
		img.BMInfoHeader = BMInfoHeader;
		if (BMInfoHeader.biBitCount == 24) {
			img.Rgbquad = std::vector <std::vector <RGBQUAD>>(BMInfoHeader.biHeight + 1);
			for (int i = 0; i < BMInfoHeader.biHeight + 1; i++) {
				img.Rgbquad[i] = std::vector <RGBQUAD>(BMInfoHeader.biWidth);
			}
		}
		else {
			img.Rgbquad = std::vector <std::vector <RGBQUAD>>(BMInfoHeader.biHeight);
			for (int i = 0; i < BMInfoHeader.biHeight; i++) {
				img.Rgbquad[i] = std::vector <RGBQUAD>(BMInfoHeader.biWidth);
			}
		}
		int height = img.BMInfoHeader.biHeight;       
		int width = img.BMInfoHeader.biWidth;
		bool height_bigger;
		bool width_bigger;
		img.BMInfoHeader.biHeight > InpImage.BMInfoHeader.biHeight ? height_bigger = true : height_bigger = false;
		img.BMInfoHeader.biWidth > InpImage.BMInfoHeader.biWidth ? width_bigger = true : width_bigger = false;

		float width_quotient = abs((InpImage.BMInfoHeader.biWidth - width)/ (float)BMInfoHeader.biWidth);
		float height_quotient = abs((InpImage.BMInfoHeader.biHeight - height) / (float)BMInfoHeader.biHeight);

		float height_q_temp;
		height_bigger ? height_q_temp = height_quotient : height_q_temp = 0;
		float width_q_temp = 0;
		int source_i = 0;
		int source_j = 0;   

		for (int i = 0; i < height; i++) {
			source_j = 0;
			width_bigger ? width_q_temp = width_quotient : width_q_temp = 0;
			for (int j = 0; j < width; j++) {
				img.Rgbquad[i][j] = InpImage.Rgbquad[source_i][source_j];
				width_q_temp += width_quotient;
				if (width_bigger) {
					if (width_q_temp > 1) {
						width_q_temp--;
						if (width_quotient > 1) {
							width_q_temp -= int(width_quotient);
						}        
					}
					else {
						if (source_j + 1 <= InpImage.BMInfoHeader.biWidth - 1) {
							source_j++;
						}
					}
				} 
				else {
					if (source_j + 1 + int(width_q_temp) <= InpImage.BMInfoHeader.biWidth - 1) {
						source_j += 1 + int(width_q_temp);
					}
					else {
						source_j = InpImage.BMInfoHeader.biWidth - 1;
					}
					if (width_q_temp > 1) {
						if (width_quotient > 1) {
							width_q_temp -= int(width_q_temp);
						}
						else {
							width_q_temp -= 1.001;
						}
					}
				}
			}
			height_q_temp += height_quotient;

			if (height_bigger) {
				if (height_q_temp > 1) {
					height_q_temp--;
					if (height_quotient >= 1) {
						height_q_temp -= int(height_quotient);
					}
				}
				else {
					if (source_i + 1 <= InpImage.BMInfoHeader.biHeight - 1) {
						source_i++;
					}
				}
			}
			else {
				if (source_i + 1 + int(height_q_temp) <= InpImage.BMInfoHeader.biHeight - 1) {
					source_i += 1 + int(height_q_temp);
				}
				else {
					source_i = InpImage.BMInfoHeader.biHeight - 1;
				}
				if (height_q_temp > 1) {
					if (height_quotient > 1) {
						height_q_temp -= int(height_q_temp);
					}
					else {
						height_q_temp -= 1.01;
					}
				}
			}	
		}
		*this = img;
		return *this;
	}

	Image operator / (short Depth) {
		BMInfoHeader.biBitCount = Depth;

		if (Depth <= 8) {
			BMInfoHeader.biClrUsed = (int)pow(2, BMInfoHeader.biBitCount);
			Palette = std::vector <RGBQUAD>((int)pow(2, BMInfoHeader.biBitCount));

			switch (Depth) {
			case 1:
				Palette[0].rgbRed = Palette[0].rgbGreen = Palette[0].rgbBlue = 0x0F;
				Palette[0].rgbReserved = Palette[0].rgbReserved = 0;
				Palette[1].rgbRed = Palette[1].rgbGreen = Palette[1].rgbBlue = 0xFF;
				BMInfoHeader.biSizeImage = ceil((int)BMInfoHeader.biWidth * (int)BMInfoHeader.biHeight / 8.0);
				break;
			case 4:
				for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
					Palette[i].rgbRed = Palette[i].rgbGreen = Palette[i].rgbBlue = 17 * (i);
					Palette[i].rgbReserved = 0;
				}
				BMInfoHeader.biSizeImage = ceil((int)BMInfoHeader.biWidth * BMInfoHeader.biHeight / 2.0);
			}

			int color = 0;
			for (int i = 0; i < this->Rgbquad.size(); i++) {
				for (int j = 0; j < Rgbquad[i].size(); j++) {
					color = 0.299 * Rgbquad[i][j].rgbRed + 0.597 * Rgbquad[i][j].rgbGreen + 0.114 * Rgbquad[i][j].rgbBlue;
					color >= 256 ? color = 255 : color;

					if (Depth == 1) {
						Rgbquad[i][j] = Palette[color / 128];
					}
					else {
						Rgbquad[i][j] = Palette[color / 16];
					}
				}
			}
		}

		else {
			BMInfoHeader.biClrUsed = 0;
			BMInfoHeader.biSizeImage = BMInfoHeader.biHeight * BMInfoHeader.biWidth * Depth / 8;
			Palette.clear();
		}

		return *this;
	}

}; 

int main(){
	Image img("test32.bmp");
	img.writeimage("test32_write.bmp");
	Image img4 = img;
	Image img2 = Image('b', 32, 64,64);
	img2 /= img; 
	img2.writeimage("testrescale32.bmp");
	img2 = img2 / 1;
	img2.writeimage("test_write1bit.bmp");
	img = img / 4;
	img.writeimage("test_write4bit.bmp");
	Image img3 = Image("test_write4bit.bmp");
	img3 = img3 / 32;
	img3.writeimage("test4to32bit.bmp");
	img2 = Image('b', 32, 300, 200);
	img2 /= img4;
	img2.writeimage("testrescale32_2.bmp");
	img4 = Image('b', 32, 600, 900);
	img4 /= img2;
	img4.writeimage("testrescale down-and-up.bmp");
}