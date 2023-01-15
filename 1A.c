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

    char *filename = "out.pbm";

    FILE *f = fopen(filename, "wb");

    if (f == NULL) {
        printf("file open failure");
        return 0;
    }
    else {
        printf("file open success");
    }

    fprintf(f, "P1 ");
    fprintf(f, "300 300 ");
    //fprintf(f, "255 ");

    int maxVal = (299 * 299);

    for (int i = 0; i < maxVal; i++) {
        fprintf(f, "1 ");

        if (i % 2 == 0) {
            fprintf(f, "1 ");
        }
        else {
            fprintf(f, "0 ");
        }
        if (i != 0 && i % 299 == 0) {
            fprintf(f, "\n");
        }
    }

    fclose(f);
    return 0;
}
