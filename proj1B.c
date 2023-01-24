#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*------------------------STARTER CODE-------------------------------------------*/
double C441(double f)
{
    return ceil(f-0.00001);
}

double F441(double f)
{
    return floor(f+0.00001);
}

typedef struct
{
   double         X[3];
   double         Y[3];
   unsigned char color[3];
} Triangle;

typedef struct
{
   int numTriangles;
   Triangle *triangles;
} TriangleList;

TriangleList *
GetTriangles(void)
{
   TriangleList *tl = (TriangleList *) malloc(sizeof(TriangleList));
   tl->numTriangles = 100;
   tl->triangles = (Triangle *) malloc(sizeof(Triangle)*tl->numTriangles);

   unsigned char colors[6][3] = { {255,128,0}, {255, 0, 127}, {0,204,204}, 
                                  {76,153,0}, {255, 204, 204}, {204, 204, 0}};
   for (int i = 0 ; i < 100 ; i++)
   {
       int idxI = i%10;
       int posI = idxI*100;
       int idxJ = i/10;
       int posJ = idxJ*100;
       int firstPt = (i%3);
       tl->triangles[i].X[firstPt] = posI;
       if (i == 50)
           tl->triangles[i].X[firstPt] = -10;
       tl->triangles[i].Y[firstPt] = posJ+10*(idxJ+1);
       tl->triangles[i].X[(firstPt+1)%3] = posI+105;
       tl->triangles[i].Y[(firstPt+1)%3] = posJ;
       tl->triangles[i].X[(firstPt+2)%3] = posI+i;
       tl->triangles[i].Y[(firstPt+2)%3] = posJ;
       if (i == 95)
          tl->triangles[i].Y[firstPt] = 1050;
       tl->triangles[i].color[0] = colors[i%6][0];
       tl->triangles[i].color[1] = colors[i%6][1];
       tl->triangles[i].color[2] = colors[i%6][2];
   }

   return tl;
}
/*------------------------END STARTER CODE-------------------------------------------*/

typedef struct
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
}Pixel;


typedef struct
{
    Pixel pixels[1000][1000];
}Image;

//declare all pixel colors used
Pixel black = {.red = 0, .green = 0, .blue = 0};
Pixel grey = {.red = 128, .green = 128, .blue = 128};
Pixel white = {.red = 255, .green = 255, .blue = 255};
Pixel red = {.red = 255, .green = 0, .blue = 0};
Pixel green = {.red = 0, .green = 255, .blue = 0};
Pixel blue = {.red = 0, .green = 0, .blue = 255};
Pixel pink = {.red = 255, .green = 0, .blue = 255};
Pixel cyan = {.red = 0, .green = 255, .blue = 255};
Pixel yellow = {.red = 255, .green = 255, .blue = 0};


void RasterizeGoingUpTriangle(Triangle *triangle, Image *img)
{
    printf("triangle color rgb values are %d %d %d \n",
           triangle->color[0], triangle->color[1], triangle->color[2]);
    printf("triangle vertexes are (%f,%f) (%f,%f) (%f,%f)\n",
           triangle->X[0], triangle->Y[0], triangle->X[1], triangle->Y[1], triangle->X[2], triangle->Y[2]);

    Pixel pixel = {.red = triangle->color[0], .green = triangle->color[1], .blue = triangle->color[2]};

    int leftIdx = 0;
    int topIdx = 0;
    int rightIdx = 0;
    for (int i = 0; i < 3; i++) {
        if (triangle->X[leftIdx] >= triangle->X[i] && triangle->Y[leftIdx] >= triangle->Y[i]) {
            leftIdx = i;
        }
        if (triangle->Y[topIdx] <= triangle->Y[i]) {
            topIdx = i;
        }
    }
    rightIdx = 6 - (topIdx + leftIdx + 3);

    printf("left vertex index is %d at (%f,%f)\n", leftIdx, triangle->X[leftIdx], triangle->Y[leftIdx]);
    printf("top vertex index is %d at (%f,%f)\n", topIdx, triangle->X[topIdx], triangle->Y[topIdx]);
    printf("right vertex index is %d at (%f,%f)\n", rightIdx, triangle->X[rightIdx], triangle->Y[rightIdx]);

    double minX = F441(triangle->X[leftIdx]);
    double maxX = C441(triangle->X[rightIdx]);
    double minY = F441(triangle->Y[leftIdx]);
    double maxY = C441(triangle->Y[topIdx]);

    printf("X floor is %f ceil is %f\n", minX, maxX);
    printf("Y floor is %f ceil is %f\n", minY, maxY);

    printf("Scanlines go from %1f to %1f\n", minY, maxY);

    double x1 = triangle->X[topIdx];
    double y1 = triangle->Y[topIdx];
    double x2 = triangle->X[leftIdx];
    double y2 = triangle->Y[leftIdx];
    double x3 = triangle->X[rightIdx];
    double y3 = triangle->Y[rightIdx];

    double slopeRight = (y3 - y1)/(x3 - x1);
    if (slopeRight == INFINITY) {printf("right infinity achieved\n");}
    double slopeLeft = (y2 - y1)/(x2 - x1);
    if (slopeLeft == INFINITY) {printf("left infinity achieved\n");}

    printf("Right slope is %f\n", slopeRight);
    printf("Left slope is %f\n", slopeLeft);

    printf("Scalines go from %f to %f\n", minY, maxY);

    for (int i = 0; i < 3; i++) {
        int x = 999 - triangle->X[i];
        int y = triangle->Y[i];

        printf("inserting pixel at pixels[%d][%d]\n", x, y);

        if (x >= 1000 || y >= 1000)
            continue;
        img->pixels[x][y] = pixel;
    }


    for (int i = (int)minY; i < (int)maxY; i++) {

    }
}

void writeImage(Image image, FILE *fp)
{
    fwrite(&image, sizeof(Image), 1, fp);
}


FILE* setFile(int width, int height, int colorRange)
{
    char *filename = "proj1B_out.pnm";
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("file open failure\n");
        return 0;
    }
    else {
        printf("file open success\n");
    }

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", colorRange);

    return fp;
}

int main(int argc, char* argv[])
{
    printf("triangulator running\n");

    int width = 1000;
    int height = 1000;
    int colorRange = 255;

    TriangleList *tl = GetTriangles();

    Image img;

    for (int i; i < 1000; i++) {
        for (int j; j < 1000; j++) {
            img.pixels[i][j] = black;
        }
    }

    char *filename = "proj1B_out.pnm";
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("file open failure\n");
        return 0;
    }
    else {
        printf("file open success\n");
    }

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", colorRange);


//some useful code that goes in your main loop:
    for (int i = 0 ; i < tl->numTriangles; i++) {
        RasterizeGoingUpTriangle(tl->triangles+i, &img);
    }

    writeImage(img, fp);
    fclose(fp);
    return 0;
}
