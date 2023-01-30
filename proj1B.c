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
    int middleIdx;
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
       tl->triangles[i].middleIdx = -1;
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

//global logging variable
int log_var = 0;

//given 3 vertices, determine type of triangle and orientation of vertices
void determineTriangle(Triangle *triangle) {
    if (log_var == 1) {
        printf("triangle color rgb values are %d %d %d \n", triangle->color[0], triangle->color[1], triangle->color[2]);
    }

    if (log_var == 1) {
        printf("triangle vertexes are (%f,%f) (%f,%f) (%f,%f)\n", triangle->X[0], triangle->Y[0], triangle->X[1], triangle->Y[1], triangle->X[2], triangle->Y[2]);
    }

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
        if (triangle->X[leftIdx] > triangle->X[i]) {
            leftIdx = i;
        }
    }

    //determine mid index for arbitrary triangle
    if (triangle->Y[bottomIdx] < triangle->Y[0] < triangle->Y[topIdx]) {triangle->middleIdx = 0;}
    if (triangle->Y[bottomIdx] < triangle->Y[1] < triangle->Y[topIdx]) {triangle->middleIdx = 1;}
    if (triangle->Y[bottomIdx] < triangle->Y[2] < triangle->Y[topIdx]) {triangle->middleIdx = 2;}

    triangle->leftIdx = leftIdx;
    triangle->rightIdx = rightIdx;
    triangle->topIdx = topIdx;
    triangle->bottomIdx = bottomIdx;

    if (log_var == 1) {printf("left vertex index is %d at (%f,%f)\n", leftIdx, triangle->X[leftIdx], triangle->Y[leftIdx]);}
    if (log_var == 1) {printf("top vertex index is %d at (%f,%f)\n", topIdx, triangle->X[topIdx], triangle->Y[topIdx]);}
    if (log_var == 1) {printf("right vertex index is %d at (%f,%f)\n", rightIdx, triangle->X[rightIdx], triangle->Y[rightIdx]);}
    if (log_var == 1) {printf("bottom vertex index is %d at (%f,%f)\n", bottomIdx, triangle->X[bottomIdx], triangle->Y[bottomIdx]);}
    if (log_var == 1) {
        if (triangle->middleIdx == -1) {printf("middle index is %d, no middle vertex", triangle->middleIdx);}
        else {printf("middle vertex index is %d at (%f,%f)\n",
                     triangle->middleIdx, triangle->X[triangle->middleIdx], triangle->Y[triangle->middleIdx]);}
    }

    //arbitrary
    if (triangle->middleIdx != -1) {
        if (log_var == 1) {printf("triangle is arbitrary\n");}
        triangle->triangleType = 2;
    }

    //flat bottom
    else if (bottomIdx == leftIdx || (bottomIdx == rightIdx)) {
        if (log_var == 1) {printf("triangle is flat bottom\n");}
        triangle->triangleType = 0;
    }

    //flat top
    else if (topIdx == leftIdx || (topIdx == rightIdx)) {
        if (log_var == 1) {printf("triangle is flat top\n");}
        triangle->triangleType = 1;
    }
}

void RasterizeGoingUpTriangle(Triangle *triangle, Image *img)
{
    if (log_var == 1) {printf("%s called\n", __func__);}
    Pixel pixel = {.red = triangle->color[0], .green = triangle->color[1], .blue = triangle->color[2]};

    double minX = triangle->X[triangle->leftIdx];
    double maxX = triangle->X[triangle->rightIdx];
    double minY = C441(triangle->Y[triangle->leftIdx]);
    double maxY = F441(triangle->Y[triangle->topIdx]);

    if (log_var == 1) {printf("X floor is %f ceil is %f\n", minX, maxX);}
    if (log_var == 1) {printf("Y floor is %f ceil is %f\n", minY, maxY);}

    if (log_var == 1) {printf("Scanlines go from %1f to %1f\n", minY, maxY);}

    //variables for left and right endpoint
    double leftEnd = -1;
    double rightEnd = -1;
    double leftB = 0;
    double rightB = 0;

    int updateLeft = -1;
    int updateRight = -1;

    double slopeRight = (triangle->Y[triangle->rightIdx] - triangle->Y[triangle->topIdx])
        / (triangle->X[triangle->rightIdx] - triangle->X[triangle->topIdx]);

    //error checking for vertical slope, sets right end as the max x coordinate
    if (slopeRight == INFINITY || slopeRight == -INFINITY) {
        rightEnd = maxX;
        if (log_var == 1) {printf("right infinity achieved, leftEnd = %f\n", rightEnd);}
        updateRight = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        rightB = -slopeRight * triangle->X[triangle->rightIdx] + triangle->Y[triangle->rightIdx];
        updateRight = 1;
    }

    double slopeLeft = (triangle->Y[triangle->leftIdx] - triangle->Y[triangle->topIdx])
        / (triangle->X[triangle->leftIdx] - triangle->X[triangle->topIdx]);

    //error checking for vertical slope, sets left end as the max x coordinate
    if (slopeLeft == INFINITY || slopeLeft == -INFINITY) {
        leftEnd = minX;
        if (log_var == 1) {printf("left infinity achieved, leftEnd = %f\n", leftEnd);}
        updateLeft = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        leftB = -slopeLeft * triangle->X[triangle->leftIdx] + triangle->Y[triangle->leftIdx];
        updateLeft = 1;
    }

    if (log_var == 1) {printf("Right slope is %f\n", slopeRight);}
    if (log_var == 1) {printf("Left slope is %f\n", slopeLeft);}

    if (log_var == 1) {printf("Scalines go from %f to %f\n", minY, maxY);}
    for (int i = (int)minY; i <= (int)maxY; i++) {
        if (updateRight == 1) {rightEnd = (((double)i - rightB) / slopeRight);}
        if (updateLeft == 1) {leftEnd = (((double)i - leftB) / slopeLeft);}
        rightEnd = F441(rightEnd);
        leftEnd = C441(leftEnd);
        if (log_var == 1) {printf("Scanline %d: intercepts go from %d to %d\n", i, (int)leftEnd, (int)rightEnd);}

        for (int c = (int)leftEnd; c <= (int)rightEnd; c++) {
            int x = 1000 - i - 1;
            int y = c;
            if (x >= 1000 || y >= 1000 || x < 0 || y < 0) {
                if (log_var == 1) {printf("x = %d | y = %d\n",x,y);}
                continue;
            }
            if (x == 0 && y == 99)
            if (log_var == 1) {printf("inserting pixel at pixels[%d][%d]\n", x, y);}
            img->pixels[x][y] = pixel;
        }
    }
}

void RasterizeGoingDownTriangle(Triangle *triangle, Image *img)
{
    if (log_var == 1) {printf("%s called\n", __func__);}
    Pixel pixel = {.red = triangle->color[0], .green = triangle->color[1], .blue = triangle->color[2]};

    double minX = triangle->X[triangle->leftIdx];
    double maxX = triangle->X[triangle->rightIdx];
    double minY = C441(triangle->Y[triangle->bottomIdx]);
    double maxY = F441(triangle->Y[triangle->topIdx]);

    if (log_var == 1) {printf("X floor is %f ceil is %f\n", minX, maxX);}
    if (log_var == 1) {printf("Y floor is %f ceil is %f\n", minY, maxY);}

    if (log_var == 1) {printf("Scanlines go from %1f to %1f\n", minY, maxY);}

    //variables for left and right endpoint
    double leftEnd = -1;
    double rightEnd = -1;
    double leftB = 0;
    double rightB = 0;

    int updateLeft = -1;
    int updateRight = -1;

    double slopeRight = (triangle->Y[triangle->rightIdx] - triangle->Y[triangle->bottomIdx])
        / (triangle->X[triangle->rightIdx] - triangle->X[triangle->bottomIdx]);

    //error checking for vertical slope, sets right end as the max x coordinate
    if (slopeRight == INFINITY || slopeRight == -INFINITY) {
        rightEnd = maxX;
        if (log_var == 1) {printf("right infinity achieved, leftEnd = %f\n", rightEnd);}
        updateRight = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        rightB = -slopeRight * triangle->X[triangle->rightIdx] + triangle->Y[triangle->rightIdx];
        updateRight = 1;
    }

    double slopeLeft = (triangle->Y[triangle->leftIdx] - triangle->Y[triangle->bottomIdx])
        / (triangle->X[triangle->leftIdx] - triangle->X[triangle->bottomIdx]);

    //error checking for vertical slope, sets left end as the max x coordinate
    if (slopeLeft == INFINITY || slopeLeft == -INFINITY) {
        leftEnd = minX;
        if (log_var == 1) {printf("left infinity achieved, leftEnd = %f\n", leftEnd);}
        updateLeft = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        leftB = -slopeLeft * triangle->X[triangle->leftIdx] + triangle->Y[triangle->leftIdx];
        updateLeft = 1;
    }

    if (log_var == 1) {printf("Right slope is %f\n", slopeRight);}
    if (log_var == 1) {printf("Left slope is %f\n", slopeLeft);}

    if (log_var == 1) {printf("Scalines go from %f to %f\n", minY, maxY);}
    for (int i = (int)minY; i <= (int)maxY; i++) {
        if (updateRight == 1) {rightEnd = (((double)i - rightB) / slopeRight);}
        if (updateLeft == 1) {leftEnd = (((double)i - leftB) / slopeLeft);}
        rightEnd = F441(rightEnd);
        leftEnd = C441(leftEnd);
        if (log_var == 1) {printf("Scanline %d: intercepts go from %d to %d\n", i, (int)leftEnd, (int)rightEnd);}

        for (int c = (int)leftEnd; c <= (int)rightEnd; c++) {
            int x = 1000 - i - 1;
            int y = c;
            if (x >= 1000 || y >= 1000 || x < 0 || y < 0) {
                if (log_var == 2) {printf("x = %d | y = %d\n",x,y);}
                continue;
            }
            if (x == 0 && y == 99)
            if (log_var == 2) {printf("inserting pixel at pixels[%d][%d]\n", x, y);}
            img->pixels[x][y] = pixel;
        }
    }
}

void RasterizeArbitraryTriangle(Triangle *triangle, Image *img) {
    if (log_var == 1) {printf("%s not implemented yet\n", __func__);}
}


void writeImage(Image image, FILE *fp)
{
    printf("%s called, writing the image file\n", __func__);
    fwrite(&image, sizeof(image), 1, fp);
}

FILE* setFile(int width, int height, int colorRange)
{
    char *filename = "proj1B_out.pnm";
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        if (log_var == 1) {printf("file open failure\n");}
        return 0;
    }
    else {
        if (log_var == 1) {printf("file open success\n");}
    }

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", colorRange);

    return fp;
}

int main(int argc, char* argv[])
{
    log_var = 0;
    if (strcmp(argv[1], "-l") == 0) { log_var = 1; }
    if (strcmp(argv[1], "-log") == 0) { log_var = 2; }
    printf("Level %d logging active\n", log_var);
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

    Triangle testTriangle = {
    .X[0] = 555.000000,
    .X[1] = 500.000000,
    .X[2] = 605.000000,
    .Y[0] = 500.000000,
    .Y[1] = 560.000000,
    .Y[2] = 500.000000,
    .color[0] = red.red,
    .color[1] = red.green,
    .color[2] = red.blue,

    .leftIdx = 0,
    .rightIdx = 0,
    .topIdx = 0,
    .bottomIdx = 0,
    .middleIdx = -1,
    .triangleType = -1};

    Triangle downTriangle = {
    .X[0] = 500.000000,
    .X[1] = 100.000000,
    .X[2] = 300.000000,
    .Y[0] = 500.000000,
    .Y[1] = 500.000000,
    .Y[2] = 100.000000,
    .color[0] = red.red,
    .color[1] = red.green,
    .color[2] = red.blue,

    .leftIdx = 0,
    .rightIdx = 0,
    .topIdx = 0,
    .bottomIdx = 0,
    .middleIdx = -1,
    .triangleType = -1};

    Triangle arbTriangle = {
    .X[0] = 500.000000,
    .X[1] = 100.000000,
    .X[2] = 400.000000,
    .Y[0] = 500.000000,
    .Y[1] = 200.000000,
    .Y[2] = 250.000000,
    .color[0] = red.red,
    .color[1] = red.green,
    .color[2] = red.blue,

    .leftIdx = 0,
    .rightIdx = 0,
    .topIdx = 0,
    .bottomIdx = 0,
    .middleIdx = -1,
    .triangleType = -1};

    // Some test cases checking functions are working properly
    Triangle *curTriangle = &arbTriangle;
    determineTriangle(curTriangle);
    if (curTriangle->triangleType == 2) { RasterizeArbitraryTriangle(curTriangle, &img); }

    //for (int i = 0 ; i < tl->numTriangles; i++) {
    //    Triangle *curTriangle = tl->triangles+i;
    //    determineTriangle(curTriangle);
    //    switch(curTriangle->triangleType) {
    //        case (0):
    //            if (log_var == 1) {printf("rasterizing flat bottom triangle\n");}
    //            RasterizeGoingUpTriangle(curTriangle, &img);
    //            break;
    //        case (1):
    //            if (log_var == 1) {printf("rasterizing flat top triangle\n");}
    //            RasterizeGoingDownTriangle(curTriangle, &img);
    //            break;
    //        case (2):
    //            if (log_var == 1) {printf("rasterizing arbitrary triangle\n");}
    //            RasterizeArbitraryTriangle(curTriangle, &img);
    //            break;
    //    }
    //}

    writeImage(img, fp);
    fclose(fp);
    return 0;
}
