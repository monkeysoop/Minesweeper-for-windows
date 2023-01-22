#include <stdio.h>
#include <windows.h>
#include "readBMP.h"




int* LoadBitmapFile(char *filename, int* Width, int* Height) {
    FILE *filePtr;   // file pointer
    BITMAPFILEHEADER bitmapFileHeader;   // bitmap file header
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char* bitmapImage;   // image data



    // open file in binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL) {
        return NULL;
    }



    // read the bitmap header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);
    if (bitmapFileHeader.bfType != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }



    // reading header and moving pointer to the start of the image data
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); 
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);



    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader.biSizeImage);
    if (bitmapImage == NULL) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }



    fread(bitmapImage,bitmapInfoHeader.biSizeImage,1,filePtr);
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }


    int W = bitmapInfoHeader.biWidth;
    int H = bitmapInfoHeader.biHeight;
    *Width = W;
    *Height = H;
    int bytesPerPixel = bitmapInfoHeader.biBitCount / 8.0;



    int* convertedData = (int*)malloc(W * H * sizeof(int));
    if (convertedData == NULL) {
        printf("Failed allocating memory for convertedData!\n");
        convertedData = NULL;
    }



    // calculates the amount of padding at the end of each line
    int padding = (4 - (W * bytesPerPixel) % 4) % 4;
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            unsigned char r = *(bitmapImage + 3 * x + 3 * (H - y - 1) * W + (H - y - 1) * padding + 2);
            unsigned char g = *(bitmapImage + 3 * x + 3 * (H - y - 1) * W + (H - y - 1) * padding + 1);
            unsigned char b = *(bitmapImage + 3 * x + 3 * (H - y - 1) * W + (H - y - 1) * padding);
            int color = 256 * 256 * r + 256 * g + b;
            *(convertedData + x + y * W) = color;
        }
    }

  

    fclose(filePtr);
    free(bitmapImage);
    return convertedData;
}
