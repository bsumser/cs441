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

    //added in for tracking index pos
    int leftIdx;
    int rightIdx;
    int topIdx;
    int bottomIdx;
    int triangleType;

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

       tl->triangles[i].leftIdx = 0;
       tl->triangles[i].rightIdx = 0;
       tl->triangles[i].topIdx = 0;
       tl->triangles[i].bottomIdx = 0;
       tl->triangles[i].triangleType = -1; // variable for unset triangle type
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

//given 3 vertices, determine type of triangle and orientation of vertices
void determineTriangle(Triangle *triangle) {
    printf("triangle color rgb values are %d %d %d \n",
           triangle->color[0], triangle->color[1], triangle->color[2]);
    printf("triangle vertexes are (%f,%f) (%f,%f) (%f,%f)\n",
           triangle->X[0], triangle->Y[0], triangle->X[1], triangle->Y[1], triangle->X[2], triangle->Y[2]);

    int leftIdx = 0;
    int topIdx = 0;
    int rightIdx = 0;
    int bottomIdx = 0;
    for (int i = 0; i < 3; i++) {
        if (triangle->Y[topIdx] < triangle->Y[i]) {
            topIdx = i;
        }
        if ((triangle->X[rightIdx] < triangle->X[i]) && (i != topIdx)) {
            rightIdx = i;
        }
        if (triangle->Y[bottomIdx] >= triangle->Y[i]) {
            bottomIdx = i;
        }
    }
    leftIdx = 6 - (topIdx + rightIdx + 3);

    triangle->leftIdx = leftIdx;
    triangle->rightIdx = rightIdx;
    triangle->topIdx = topIdx;
    triangle->bottomIdx = bottomIdx;

    printf("left vertex index is %d at (%f,%f)\n", leftIdx, triangle->X[leftIdx], triangle->Y[leftIdx]);
    printf("top vertex index is %d at (%f,%f)\n", topIdx, triangle->X[topIdx], triangle->Y[topIdx]);
    printf("right vertex index is %d at (%f,%f)\n", rightIdx, triangle->X[rightIdx], triangle->Y[rightIdx]);
    printf("bottom vertex index is %d at (%f,%f)\n", bottomIdx, triangle->X[bottomIdx], triangle->Y[bottomIdx]);

    //flat bottom
    if (bottomIdx == leftIdx || (bottomIdx == rightIdx)) {
        printf("triangle is flat bottom\n");
        triangle->triangleType = 0;
    }

    //flat top
    else if (topIdx == leftIdx || (topIdx == rightIdx)) {
        printf("triangle is flat top\n");
        triangle->triangleType = 1;
    }

    //arbitrary
    else {
        printf("triangle is arbitrary\n");
        triangle->triangleType = 2;
    }
}

void RasterizeGoingUpTriangle(Triangle *triangle, Image *img)
{
    printf("%s called\n", __func__);
    Pixel pixel = {.red = triangle->color[0], .green = triangle->color[1], .blue = triangle->color[2]};


    double minX = F441(triangle->X[triangle->leftIdx]);
    double maxX = C441(triangle->X[triangle->rightIdx]);
    double minY = F441(triangle->Y[triangle->leftIdx]);
    double maxY = C441(triangle->Y[triangle->topIdx]);

    printf("X floor is %f ceil is %f\n", minX, maxX);
    printf("Y floor is %f ceil is %f\n", minY, maxY);

    printf("Scanlines go from %1f to %1f\n", minY, maxY);

    double x1 = triangle->X[triangle->topIdx];
    double y1 = triangle->Y[triangle->topIdx];
    double x2 = triangle->X[triangle->leftIdx];
    double y2 = triangle->Y[triangle->leftIdx];
    double x3 = triangle->X[triangle->rightIdx];
    double y3 = triangle->Y[triangle->rightIdx];

    //variables for left and right endpoint
    double leftEnd = -1;
    double rightEnd = -1;
    double leftB = 0;
    double rightB = 0;

    int updateLeft = -1;
    int updateRight = -1;

    double slopeRight = (y3 - y1)/(x3 - x1);
    if (slopeRight == INFINITY || slopeRight == -INFINITY) {
        rightEnd = maxX;
        printf("right infinity achieved, leftEnd = %f\n", rightEnd);
        updateRight = 0;
    }
    else {
        rightB = -slopeRight * triangle->X[triangle->rightIdx] + triangle->Y[triangle->rightIdx];
        updateRight = 1;
    }

    double slopeLeft = (y2 - y1)/(x2 - x1);
    if (slopeLeft == INFINITY || slopeLeft == -INFINITY) {
        leftEnd = minX;
        printf("left infinity achieved, leftEnd = %f\n", leftEnd);
        updateLeft = 0;
    }
    else {
        leftB = -slopeLeft * triangle->X[triangle->leftIdx] + triangle->Y[triangle->leftIdx];
        updateLeft = 1;
    }

    printf("Right slope is %f\n", slopeRight);
    printf("Left slope is %f\n", slopeLeft);

    printf("Scalines go from %f to %f\n", minY, maxY);
    for (int i = (int)minY; i < (int)maxY; i++) {
        if (updateRight == 1) {rightEnd = (((double)i - rightB) / slopeRight);}
        if (updateLeft == 1) {leftEnd = (((double)i - leftB) / slopeLeft);}
        printf("Scanline %d goes from %d to %d\n", i, (int)leftEnd, (int)rightEnd);

        for (int c = (int)leftEnd; c < (int)rightEnd; c++) {
            int x = 999 - i;
            int y = c;
            printf("inserting pixel at pixels[%d][%d]\n", x, y);
            img->pixels[x][y] = pixel;
            if ((x >= 1000 || y >= 1000) || (x < 0 || y < 1000)) {
                continue;
            }
        }
    }
}

void RasterizeGoingDownTriangle(Triangle *triangle, Image *img) {
    printf("%s not implemented yet\n", __func__);
}

void RasterizeArbitraryTriangle(Triangle *triangle, Image *img) {
    printf("%s not implemented yet\n", __func__);
}


void writeImage(Image image, FILE *fp)
{
    printf("%s called\n", __func__);
    fwrite(&image, sizeof(image), 1, fp);
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

    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
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


    int triangleFail = 0;
    for (int i = 0 ; i < tl->numTriangles; i++) {
        Triangle *curTriangle = tl->triangles+i;
        determineTriangle(curTriangle);
        switch(curTriangle->triangleType) {
            case (0):
                printf("rasterizing flat bottom triangle\n");
                RasterizeGoingUpTriangle(curTriangle, &img);
                break;
            case (1):
                printf("rasterizing flat top triangle\n");
                RasterizeGoingUpTriangle(curTriangle, &img);
                triangleFail++;
                break;
            case (2):
                printf("rasterizing arbitrary triangle\n");
                RasterizeArbitraryTriangle(curTriangle, &img);
                triangleFail++;
                break;
        }
    }

    writeImage(img, fp);
    fclose(fp);
    printf("Fail count = %d\n", triangleFail);
    return 0;
}
