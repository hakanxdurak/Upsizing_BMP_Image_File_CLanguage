#include <stdio.h>
#include <stdlib.h>

#include "header.h"

#define FACTOR 2

 struct rgb{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

typedef struct rgb rgb;

int main() {
    FILE *inputPtr;
    if ((inputPtr = fopen("starry_night_downscaled_downscaled.bmp", "rb")) == NULL) {
        printf("The file \"starry_night.bmp\" could not be opened\n");
    }else{
        BITMAPFILEHEADER fileheader_input;
        //reading first 14 bytes from the file
        fread(&fileheader_input, sizeof(BITMAPFILEHEADER), 1, inputPtr);
        BITMAPINFOHEADER infoheader_input;
        //reading next 40 bytes
        fread(&infoheader_input, sizeof(BITMAPINFOHEADER), 1, inputPtr);

        if (fileheader_input.bfType != 0x4d42 || fileheader_input.bfOffBits != 54 ||
            infoheader_input.biSize != 40 || infoheader_input.biBitCount != 24 || infoheader_input.biCompression != 0) {
            fclose(inputPtr);
            printf("Unsupported file format.\n");
        }else{
            FILE *outputPtr;
            if ((outputPtr = fopen("starry_night_downscaled_downscaled_upscaled.bmp", "wb")) == NULL) {
                printf("The file \"starry_night_upscaled.bmp\" could not be created\n");
            }else{
                //equating the header datas for the input and the output images
                BITMAPFILEHEADER fileheader_output = fileheader_input;
                BITMAPINFOHEADER infoheader_output = infoheader_input;

                //updating output images header data
                infoheader_output.biWidth = infoheader_input.biWidth * FACTOR;
                infoheader_output.biHeight = infoheader_input.biHeight * FACTOR;

                //image width must be multiple of 4 bytes so we need padding
                unsigned int long long paddingOut = (4- (infoheader_output.biWidth * sizeof(rgb)) % 4) % 4;
                fileheader_output.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 3*abs(infoheader_output.biWidth)* abs(infoheader_output.biHeight)
                        + (paddingOut * infoheader_output.biHeight);

                //writing 14 bytes to the output image
                fwrite(&fileheader_output, sizeof(BITMAPFILEHEADER), 1 , outputPtr);
                //writing the next 40 bytes
                fwrite(&infoheader_output, sizeof(BITMAPINFOHEADER), 1 , outputPtr);

                //creating a buffer
                struct rgb *buffer;
                buffer = (struct rgb *) calloc(infoheader_input.biWidth, sizeof(rgb));
                //rgb *buffer = malloc(infoheader_input.biWidth * sizeof(rgb));

                // looping over for each input images height
                for (int i = 0, biHeight = abs(infoheader_input.biHeight); i < biHeight ; i++) {
                    //we read 54 bytes before and now it comes the pixel data
                    fread(buffer, sizeof(rgb), infoheader_input.biWidth, inputPtr);

                    for (int k = 0; k < FACTOR; ++k) {
                        for (int l = 0; l < infoheader_input.biWidth; ++l) {
                            for (int m = 0; m < FACTOR; ++m)
                                //writing the buffer data to the
                                // output image one by one element where each stores 3 bytes
                                fwrite(&buffer[l], sizeof(rgb), 1, outputPtr);
                        }
                        // padding if it is needed
                        for (int n = 0; n < paddingOut; ++n)
                            fputc(0x00, outputPtr);
                    }
                }
                free(buffer);
                fclose(inputPtr);
                fclose(outputPtr);
            }
        }
    }
}