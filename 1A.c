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


    char *filename = "out.pnm";

    FILE *f = fopen(filename, "wb");

    if (f == NULL) {
        printf("Hello World");
        return 0;
    }
    else {
        printf("file open success");
    }

    fprintf(f, "P6\n");
    fprintf(f, "300 300\n");
    fprintf(f, "255\n");

    int maxVal = (width * height) - 2;

    for (int i = 0; i < maxVal; i++){
        fprintf(f, "255 ");
        fprintf(f, "0 ");
        fprintf(f, "0 ");

        if (i == 299) {
            fprintf(f, "\n");
        }
    }

    fclose(f);
    free(imgPointer);

    return 0;
}
