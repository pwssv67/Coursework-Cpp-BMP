#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <minwindef.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>

class Image {
   std::vector <RGBTRIPLE> Rgbtriple;
   std::vector <RGBTRIPLE> Palette;
   BITMAPINFOHEADER BMInfoHeader;

public:

   // Конструктор изображения цвета Mode, битности BCount и размеров Height и Width
   Image(char Mode, WORD BCount, long int Width, long int Height) {
      //header
      BMInfoHeader.biBitCount = BCount;
      BMInfoHeader.biSize = 40;
      BMInfoHeader.biHeight = Height;
      BMInfoHeader.biWidth = Width;
      BMInfoHeader.biSizeImage = Width * Height * int(BCount) / 8;
      BMInfoHeader.biClrImportant = 0;
      BMInfoHeader.biClrUsed = 0;
      BMInfoHeader.biCompression = 0;
      BMInfoHeader.biPlanes = 1;
      BMInfoHeader.biXPelsPerMeter = 2000;
      BMInfoHeader.biYPelsPerMeter = 2000;
      //palette
      if (BCount <= 8) {
         BMInfoHeader.biClrUsed = (int)pow(2, BMInfoHeader.biBitCount);
         Palette = std::vector <RGBTRIPLE>((int)pow(2, BMInfoHeader.biBitCount));
         switch (BCount) {
         case 4:
            for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
               Palette[i].rgbtRed = Palette[i].rgbtGreen = Palette[i].rgbtBlue = 17 * (i);
            }
            BMInfoHeader.biSizeImage = BMInfoHeader.biWidth * BMInfoHeader.biHeight / 2.0;
            break;
         case 8:
            for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
               Palette[i].rgbtRed = Palette[i].rgbtGreen = Palette[i].rgbtBlue = i;
            }
            BMInfoHeader.biSizeImage = BMInfoHeader.biWidth * BMInfoHeader.biHeight;
            break;
         }
      }

      //rgbquad
      if (BCount == 24) {
         Rgbtriple = std::vector <RGBTRIPLE>((BMInfoHeader.biHeight + 1)*BMInfoHeader.biWidth);

         for (int i = 0; i < (BMInfoHeader.biHeight + 1) * BMInfoHeader.biWidth; i++) {
               Rgbtriple[i].rgbtBlue = Mode;
               Rgbtriple[i].rgbtGreen = Mode;
               Rgbtriple[i].rgbtRed = Mode;
         }
      }

      else {
         Rgbtriple = std::vector <RGBTRIPLE>(BMInfoHeader.biHeight * BMInfoHeader.biWidth);

         for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
            for (int j = 0; j < BMInfoHeader.biWidth; j++) {
               Rgbtriple[i].rgbtBlue = Mode;
               Rgbtriple[i].rgbtGreen = Mode;
               Rgbtriple[i].rgbtRed = Mode;
            }
         }
      }

   }
   // Конструктор, считывающий изображение из файла 
   Image(const char* fileName) {
      loadimage(fileName);
   }
   // Пустой конструктор, создаёт пустое изображение с неинициализированными полями
   Image() {}

   // Конструктор, создающий копию изображения i
   Image(const Image& i) {
      BMInfoHeader = i.BMInfoHeader;
      Rgbtriple = i.Rgbtriple;
      Palette = i.Palette;
   }

   // загрузка изображения из файла
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
         Rgbtriple = std::vector <RGBTRIPLE>((BMInfoHeader.biHeight + 1) * BMInfoHeader.biWidth);
      }
      else {
         Rgbtriple = std::vector <RGBTRIPLE>(BMInfoHeader.biHeight * BMInfoHeader.biWidth);

      }

      BYTE buffer = 0;

      switch (BMInfoHeader.biBitCount) {
      case 32:
         for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
            //std::cout << i << "\n" ;
            fread(&Rgbtriple[i], sizeof(RGBQUAD), 1, f);
         }
         break;

      case 24:
         for (int i = 0; i < (BMInfoHeader.biHeight + 1)*BMInfoHeader.biWidth; i++) {
               fread(&Rgbtriple[i], sizeof(RGBTRIPLE), 1, f);
         }
         break;

      case 8:
         Palette = std::vector <RGBTRIPLE>((int)pow(2, BMInfoHeader.biBitCount));
         for (int i = 0; i < (int)pow(2, BMInfoHeader.biBitCount); i++) {
            Palette[i].rgbtRed = Palette[i].rgbtGreen = Palette[i].rgbtBlue = i;
         }

         buffer = 0;
         for (int i = 0; i < BMInfoHeader.biHeight*BMInfoHeader.biWidth; i++) {
               fread(&buffer, 1, 1, f);
               Rgbtriple[i] = Palette[buffer];
         }
         break;

      case 4:
         Palette = std::vector <RGBTRIPLE>((int)pow(2, BMInfoHeader.biBitCount));
         for (int i = 0; i < (int)pow(2, BMInfoHeader.biBitCount); i++) {
            Palette[i].rgbtRed = Palette[i].rgbtGreen = Palette[i].rgbtBlue = 17 * (i);
         }

         for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
            if (i % 2 == 0) {
               fread(&buffer, 1, 1, f);
               Rgbtriple[i] = Palette[(buffer >> 4) & 0b1111];
            }
            else {
               Rgbtriple[i] = Palette[(buffer) & 0b1111];
            }
         }
         break;
      
      }
      return 0;
   }

   // запись изображения в файл
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
         for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
            fwrite(&Rgbtriple[i], sizeof(RGBQUAD), 1, f2);
         }
         for (int i = 0; i < 4 - BMInfoHeader.biSizeImage % 4; i++) {
            fwrite(&empty, 1, 1, f2);
         }

         break;
      case 24:
         for (int i = 0; i < (int)((BMInfoHeader.biHeight + 1) * BMInfoHeader.biWidth); i++) {
            //std::cout << i << "\n" ;
            fwrite(&Rgbtriple[i], sizeof(RGBTRIPLE), 1, f2);
         }
         for (int i = 0; i < 4 - ((BMInfoHeader.biHeight + 1) * BMInfoHeader.biWidth) % 4; i++) {
            fwrite(&empty, 1, 1, f2);
         }
         break;

      case 8:
         buffer = 0;
         for (int i = 0; i < BMInfoHeader.biHeight*BMInfoHeader.biWidth; i++) {
               buffer = Rgbtriple[i].rgbtBlue;
               fwrite(&buffer, 1, 1, f2);
         }
         break;

      case 4:
         buffer = 0;
         for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
            temp = i;
            buffer = buffer << 4;
            buffer += (int)(Rgbtriple[i].rgbtBlue / 16);
            if (i % 2 == 1) {
               fwrite(&buffer, 1, 1, f2);
               buffer = 0;
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
         for (int i = 0; i < BMInfoHeader.biHeight * BMInfoHeader.biWidth; i++) {
            buffer = buffer << 1;
            buffer += Rgbtriple[i].rgbtBlue / 128;
            if (i % 8 == 7) {
               fwrite(&buffer, 1, 1, f2);
               buffer = 0;
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

   // перегруз оператора присваивания. Возвращает НЕЗАВИСИМУЮ копию изображения
   Image operator = (Image Inp) {
      BMInfoHeader = Inp.BMInfoHeader;
      Rgbtriple = Inp.Rgbtriple;
      Palette = Inp.Palette;
      return *this;
   }

   // приведение изображения справа к размеру изображения слева, записывает в изображение слева
   Image operator /= (Image InpImage) {
      Image img = Image();
      img.BMInfoHeader = BMInfoHeader;
      if (BMInfoHeader.biBitCount == 24) {
         img.Rgbtriple = std::vector <RGBTRIPLE>((BMInfoHeader.biHeight + 1)*BMInfoHeader.biWidth);
      }
      else {
         img.Rgbtriple = std::vector <RGBTRIPLE>(BMInfoHeader.biHeight * BMInfoHeader.biWidth);
      }
      int height = img.BMInfoHeader.biHeight;
      int width = img.BMInfoHeader.biWidth;
      bool height_bigger;
      bool width_bigger;
      img.BMInfoHeader.biHeight > InpImage.BMInfoHeader.biHeight ? height_bigger = true : height_bigger = false;
      img.BMInfoHeader.biWidth > InpImage.BMInfoHeader.biWidth ? width_bigger = true : width_bigger = false;

      float counter_width = abs((InpImage.BMInfoHeader.biWidth - width) / (float)BMInfoHeader.biWidth);
      float counter_height = abs((InpImage.BMInfoHeader.biHeight - height) / (float)BMInfoHeader.biHeight);

      float counter_height_temp;
      height_bigger ? counter_height_temp = counter_height : counter_height_temp = 0;
      float counter_width_temp = 0;
      int source_i = 0;
      int source_j = 0;

      for (int i = 0; i < height; i++) {
         source_j = 0;
         width_bigger ? counter_width_temp = counter_width : counter_width_temp = 0;
         for (int j = 0; j < width; j++) {
            img.Rgbtriple[i*width + j] = InpImage.Rgbtriple[source_i*InpImage.BMInfoHeader.biWidth + source_j];
            counter_width_temp += counter_width;
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

   //изменение битности изображения, новая битность = Depth
   Image operator / (short Depth) {
      BMInfoHeader.biBitCount = Depth;

      if (Depth <= 8) {
         BMInfoHeader.biClrUsed = (int)pow(2, BMInfoHeader.biBitCount);
         Palette = std::vector <RGBTRIPLE>((int)pow(2, BMInfoHeader.biBitCount));

         switch (Depth) {
         case 4:
            for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
               Palette[i].rgbtRed = Palette[i].rgbtGreen = Palette[i].rgbtBlue = 17 * (i);
            }
            BMInfoHeader.biSizeImage = ceil((int)BMInfoHeader.biWidth * BMInfoHeader.biHeight / 2.0);
         case 8:
            for (int i = 0; i < BMInfoHeader.biClrUsed; i++) {
               Palette[i].rgbtRed = Palette[i].rgbtGreen = Palette[i].rgbtBlue = i;
            }
            BMInfoHeader.biSizeImage = (int)BMInfoHeader.biWidth * BMInfoHeader.biHeight;
         }

         int color = 0;
         for (int i = 0; i < this->Rgbtriple.size(); i++) {
               color = 0.299 * Rgbtriple[i].rgbtRed + 0.597 * Rgbtriple[i].rgbtGreen + 0.114 * Rgbtriple[i].rgbtBlue;
               color >= 256 ? color = 255 : color;

               switch (Depth) {
               case 4:
                  Rgbtriple[i] = Palette[color / 16];
                  break;

               case 8:
                  Rgbtriple[i] = Palette[color];
                  break;
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

int main() {
   Image img("test32.bmp");
   img.writeimage("test32_write.bmp");
   Image img4 = img;
   Image img2 = Image('b', 32, 64, 64);
   img2 /= img;
   img2.writeimage("testrescale32.bmp");
   Image img5 = img2;
   img2 = img2 / 1;
   img2.writeimage("test_write1bit.bmp");
   img = img / 8;
   img.writeimage("test_write8bit.bmp");
   Image img3 = Image("test_write8bit.bmp");
   img3 = img3 / 32;
   img3.writeimage("test8to32bit.bmp");
   img2 = Image('b', 32, 300, 200);
   img2 /= img4;
   img2.writeimage("testrescale32_2.bmp");
   img5 / 4;
   img5.writeimage("test4bit.bmp");
}