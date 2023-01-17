#include <stdio.h>
#include <stdlib.h>

struct Image {
    int data;
};

int main(int argc, char* argv[])
{
    printf("Hello World");

    int width = 300;
    int height = 300;

    int first = width / 3;
    int second = width * 0.6666;
    int third = width;

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
    fprintf(f, "300 300\n");
    fprintf(f, "255\n");

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i <= first - 1 && j <= first - 1) {fprintf(f, "0 0 0  ");} //black
            else if (i <= first - 1 && j >= first && j <= second - 1) {fprintf(f, "128 128 128  ");} //grey
            else if (i <= first - 1 && j >= second && j <= third - 1) {fprintf(f, "255 255 255  ");} //white
            else if (i >= first && i <= second - 1 && j >= 0 && j <= first - 1) {fprintf(f, "255 0 0  ");} //red
            else if (i >= first && i <= second - 1 && j >= second && j <= third - 1) {fprintf(f, "0 0 255  ");} //blue
            else if (i >= second && i <= third - 1 && j >= 0 && j <= first - 1) {fprintf(f, "255 0 255  ");} //pink
            else if (i >= second && i <= third - 1 && j >= first && j <= second - 1) {fprintf(f, "0 255 255  ");} //cyan
            else if (i >= second && i <= third - 1 && j >= second && j <= third - 1) {fprintf(f, "255 255 0  ");} //yellow
            else {fprintf(f, "0 255 0  ");} //green
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 0;
}
