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


    char *filename = "out";

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

    int maxVal = width * height;

    for (int i = 0; i < maxVal; i+= 1){
        fprintf(f, "255 ");
        fprintf(f, "255 ");
        fprintf(f, "255 ");

    }


    fclose(f);
    free(imgPointer);

    return 0;
}
