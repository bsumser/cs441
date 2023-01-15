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
    int chan = 3;
    int totalSize = width * height * chan;

    unsigned char* imgPointer = (unsigned char*) malloc(totalSize * sizeof(unsigned char));
    char pixel[] = "255 0 0 ";
    char lineBreak[] = "\n";
    char heading[] = "P6\n300 300\n255\n";

    char *filename = "out.pnm";

    FILE *f = fopen(filename, "wb");

    if (f == NULL) {
        printf("file open failure");
        return 0;
    }
    else {
        printf("file open success");
    }

    fwrite(heading, 1, sizeof(heading), f);

    int maxVal = (299 * 299);

    for (int i = 0; i < maxVal; i++){
        fwrite(pixel, 1, sizeof(pixel), f);

        if (i != 0 && i % 299 == 0) {
            fwrite(lineBreak, 1, sizeof(lineBreak), f);
        }
    }

    fclose(f);
    free(imgPointer);

    return 0;
}
