#include <stdio.h>
#include <stdlib.h>

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct Image {
    struct Pixel pixel_array[300][300];
};


//declare all pixel colors used
struct Pixel black = {.red = 0, .green = 0, .blue = 0};
struct Pixel grey = {.red = 128, .green = 128, .blue = 128};
struct Pixel white = {.red = 255, .green = 255, .blue = 255};
struct Pixel red = {.red = 255, .green = 0, .blue = 0};
struct Pixel green = {.red = 0, .green = 255, .blue = 0};
struct Pixel blue = {.red = 0, .green = 0, .blue = 255};
struct Pixel pink = {.red = 255, .green = 0, .blue = 255};
struct Pixel cyan = {.red = 0, .green = 255, .blue = 255};
struct Pixel yellow = {.red = 255, .green = 255, .blue = 0};

//forward declare custom pixel setter
void writeImage(struct Image image, FILE *fp);

int main(int argc, char* argv[])
{
    printf("Hello World");
    struct Image squares;

    int width = 300;
    int height = 300;
    int colorRange = 255;

    int first = width / 3;
    int second = (width * 0.6666) + 1;
    int third = width;

    char *filename = "proj1A_out.pnm";
    unsigned char newLine = '\n';

    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        printf("file open failure");
        return 0;
    }
    else {
        printf("file open success");
    }

    printf("\nfirst - %d\n", first);
    printf("second - %d\n", second);
    printf("third - %d\n", third);

    fprintf(f, "P6\n");
    fprintf(f, "%d %d\n", width, height);
    fprintf(f, "%d\n", colorRange);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i <= first - 1 && j <= first - 1) {squares.pixel_array[i][j] = black;} //black
            else if (i <= first - 1 && j >= first && j <= second - 1) {squares.pixel_array[i][j] = grey;} //grey
            else if (i <= first - 1 && j >= second && j <= third - 1) {squares.pixel_array[i][j] = white;} //white
            else if (i >= first && i <= second - 1 && j >= 0 && j <= first - 1) {squares.pixel_array[i][j] = red;} //red
            else if (i >= first && i <= second - 1 && j >= second && j <= third - 1) {squares.pixel_array[i][j] = blue;} //blue
            else if (i >= second && i <= third - 1 && j >= 0 && j <= first - 1) {squares.pixel_array[i][j] = pink;} //pink
            else if (i >= second && i <= third - 1 && j >= first && j <= second - 1) {squares.pixel_array[i][j] = cyan;} //cyan
            else if (i >= second && i <= third - 1 && j >= second && j <= third - 1) {squares.pixel_array[i][j] = yellow;} //yellow
            else {squares.pixel_array[i][j] = green;} //green
        }
    }
    fwrite(&squares, sizeof(squares), 900, f);
    fclose(f);
    return 0;
}

void writeImage(struct Image image, FILE *fp)
{
    fwrite(&image, sizeof(struct Image), 1, fp);
}
