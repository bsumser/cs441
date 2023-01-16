#include <stdio.h>
#include <stdlib.h>

struct Image {
    int data;
};

int main(int argc, char* argv[])
{
    printf("Hello World");

    int width = 6;
    int height = 6;
    int chan = 3;
    int totalSize = width * height * chan;

    char *filename = "out.ppm";

    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        printf("file open failure");
        return 0;
    }
    else {
        printf("file open success");
    }

    fprintf(f, "P3\n");
    fprintf(f, "6 6\n");
    fprintf(f, "255\n");

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i <= 1 && j <= 1) {fprintf(f, "0 0 0  ");} //black
            else if (i <= 1 && j >= 2 && j <= 3) {fprintf(f, "128 128 128  ");} //grey
            else if (i <= 1 && j >= 4) {fprintf(f, "255 255 255  ");} //white
            else if (i <= 3 && j <= 1) {fprintf(f, "255 0 0  ");} //red
            //else if (i >= 4 && j >= 2 && j <= 3) {fprintf(f, "0 255 0");} //green
            else if (i <= 3 && j >= 4) {fprintf(f, "0 0 255  ");} //blue
            else if (i >= 4 && j <= 1) {fprintf(f, "255 0 255  ");} //pink
            else if (i >= 4 && j >= 2 && j <= 3) {fprintf(f, "0 255 255  ");} //cyan
            else if (i >= 4 && j >= 4) {fprintf(f, "255 255 0  ");} //yellow
            else {fprintf(f, "0 0 0  ");}
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 0;
}
