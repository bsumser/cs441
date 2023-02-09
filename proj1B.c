#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_ROWS 1344
#define NUM_COLS 1786

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
GetTriangles(int small_read)
{
   FILE *f = fopen("tris.txt", "r");
   if (f == NULL)
   {
       fprintf(stderr, "You must place the tris.txt file in the current directory.\n");
       exit(EXIT_FAILURE);
   }
   fseek(f, 0, SEEK_END);
   int numBytes = ftell(f);
   fseek(f, 0, SEEK_SET);
   if (numBytes != 241511792)
   {
       fprintf(stderr, "Your tris.txt file is corrupted.  It should be 241511792 bytes, but you only have %d.\n", numBytes);
       exit(EXIT_FAILURE);
   }

   if (small_read == 1)
   {
       numBytes = 10000;
   }

   char *buffer = (char *) malloc(numBytes);
   if (buffer == NULL)
   {
       fprintf(stderr, "Unable to allocate enough memory to load file.\n");
       exit(EXIT_FAILURE);
   }

   fread(buffer, sizeof(char), numBytes, f);

   char *tmp = buffer;
   int numTriangles = atoi(tmp);
   while (*tmp != '\n')
       tmp++;
   tmp++;

   if (numTriangles != 2566541)
   {
       fprintf(stderr, "Issue with reading file -- can't establish number of triangles.\n");
       exit(EXIT_FAILURE);
   }

   if (small_read == 1)
       numTriangles = 100;

   TriangleList *tl = (TriangleList *) malloc(sizeof(TriangleList));
   tl->numTriangles = numTriangles;
   tl->triangles = (Triangle *) malloc(sizeof(Triangle)*tl->numTriangles);

   for (int i = 0 ; i < tl->numTriangles ; i++)
   {
       double x1, y1, x2, y2, x3, y3;
       int    r, g, b;
/*
 * Weird: sscanf has a terrible implementation for large strings.
 * When I did the code below, it did not finish after 45 minutes.
 * Reading up on the topic, it sounds like it is a known issue that
 * sscanf fails here.  Stunningly, fscanf would have been faster.
 *     sscanf(tmp, "(%lf, %lf), (%lf, %lf), (%lf, %lf) = (%d, %d, %d)\n%n",
 *              &x1, &y1, &x2, &y2, &x3, &y3, &r, &g, &b, &numRead);
 *
 *  So, instead, do it all with atof/atoi and advancing through the buffer manually...
 */
       tmp++,
       x1 = atof(tmp);
       while (*tmp != ',')
          tmp++;
       tmp += 2; // comma+space
       y1 = atof(tmp);
       while (*tmp != ')')
          tmp++;
       tmp += 4; // right-paren+comma+space+left-paren
       x2 = atof(tmp);
       while (*tmp != ',')
          tmp++;
       tmp += 2; // comma+space
       y2 = atof(tmp);
       while (*tmp != ')')
          tmp++;
       tmp += 4; // right-paren+comma+space+left-paren
       x3 = atof(tmp);
       while (*tmp != ',')
          tmp++;
       tmp += 2; // comma+space
       y3 = atof(tmp);
       while (*tmp != ')')
          tmp++;
       tmp += 5; // right-paren+space+equal+space+left-paren
       r = atoi(tmp);
       while (*tmp != ',')
          tmp++;
       tmp += 2; // comma+space
       g = atoi(tmp);
       while (*tmp != ',')
          tmp++;
       tmp += 2; // comma+space
       b = atoi(tmp);
       while (*tmp != '\n')
          tmp++;
       tmp++; // onto next line

       tl->triangles[i].X[0] = x1;
       tl->triangles[i].X[1] = x2;
       tl->triangles[i].X[2] = x3;
       tl->triangles[i].Y[0] = y1;
       tl->triangles[i].Y[1] = y2;
       tl->triangles[i].Y[2] = y3;
       tl->triangles[i].color[0] = r;
       tl->triangles[i].color[1] = g;
       tl->triangles[i].color[2] = b;
       //printf("Read triangle %f, %f, %f, %f, %f, %f, %d, %d, %d\n", x1, y1, x2, y2, x3, y3, r, g, b);

       tl->triangles[i].leftIdx = 0;
       tl->triangles[i].rightIdx = 0;
       tl->triangles[i].topIdx = 0;
       tl->triangles[i].bottomIdx = 0;
       tl->triangles[i].middleIdx = -1;
       tl->triangles[i].triangleType = -1; // variable for unset triangle type
   }

   free(buffer);
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
    Pixel pixels[NUM_ROWS][NUM_COLS];
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
void determineTriangle(Triangle *triangle, int triangleNum) {
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
    int top = 0;
    int bot = 0;
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


    for (int i = 0; i < 3; i++) {
        if ((triangle->Y[bottomIdx] < triangle->Y[i]) && (triangle->Y[i] < triangle->Y[topIdx]))
        {triangle->middleIdx = i;}
    }

    for (int i = 0; i < 3; i++) {
        if (triangle->Y[topIdx] > triangle->Y[i]) {top++;}
        if (triangle->Y[bottomIdx] < triangle->Y[i]) {bot++;}
    }

    triangle->leftIdx = leftIdx;
    triangle->rightIdx = rightIdx;
    triangle->topIdx = topIdx;
    triangle->bottomIdx = bottomIdx;

    //arbitrary
    if (triangle->middleIdx != -1) {
        if (log_var == 1) {printf("triangle is arbitrary\n");}
        triangle->triangleType = 2;
    }

    //flat bottom
    else if (top == 2){
        if (log_var == 1) {printf("triangle is flat bottom\n");}
        triangle->triangleType = 0;
        for (int i = 0; i < 3; i++) {
            if (triangle->Y[i] < triangle->Y[triangle->rightIdx] && triangle->X[i] > triangle->X[triangle->rightIdx]) {
                triangle->rightIdx = i;
            }
            if (triangle->Y[i] < triangle->Y[triangle->leftIdx] && triangle->X[i] < triangle->X[triangle->rightIdx]) {
                triangle->leftIdx = i;
            }
        }
    }

    //flat top
    else if (bot == 2) {
        if (log_var == 1) {printf("triangle is flat top\n");}
        triangle->triangleType = 1;
        for (int i = 0; i < 3; i++) {
            if (triangle->Y[i] > triangle->Y[triangle->rightIdx] && triangle->X[i] > triangle->X[triangle->rightIdx]) {
                triangle->rightIdx = i;
            }
            if (triangle->Y[i] > triangle->Y[triangle->leftIdx] && triangle->X[i] < triangle->X[triangle->rightIdx]) {
                triangle->leftIdx = i;
            }
        }
    }

    if (log_var == 1) {
        printf("left vertex index is %d at (%f,%f)\n", triangle->leftIdx, triangle->X[triangle->leftIdx], triangle->Y[triangle->leftIdx]);
        printf("top vertex index is %d at (%f,%f)\n", triangle->topIdx, triangle->X[triangle->topIdx], triangle->Y[triangle->topIdx]);
        printf("right vertex index is %d at (%f,%f)\n", triangle->rightIdx, triangle->X[triangle->rightIdx], triangle->Y[triangle->rightIdx]);
        printf("bottom vertex index is %d at (%f,%f)\n", triangle->bottomIdx, triangle->X[triangle->bottomIdx], triangle->Y[triangle->bottomIdx]);
        printf("middle vertex index is %d at (%f,%f)\n", triangle->middleIdx, triangle->X[triangle->middleIdx], triangle->Y[triangle->middleIdx]);
    }
}

void RasterizeGoingUpTriangle(Triangle *triangle, Image *img, int triangleNum)
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

    double slopeLeft = 0;
    double slopeRight = 0;

    int updateLeft = -1;
    int updateRight = -1;


    //error checking for vertical slope, sets right end as the max x coordinate
    if (triangle->X[triangle->rightIdx] == triangle->X[triangle->bottomIdx]) {
        rightEnd = maxX;
        updateRight = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        slopeRight = (triangle->Y[triangle->rightIdx] - triangle->Y[triangle->bottomIdx])
        / (triangle->X[triangle->rightIdx] - triangle->X[triangle->bottomIdx]);
        rightB = -slopeRight * triangle->X[triangle->rightIdx] + triangle->Y[triangle->rightIdx];
        updateRight = 1;
    }

    //error checking for vertical slope, sets left end as the max x coordinate
    if (triangle->X[triangle->leftIdx] == triangle->X[triangle->bottomIdx]) {
        leftEnd = minX;
        updateLeft = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        slopeLeft = (triangle->Y[triangle->leftIdx] - triangle->Y[triangle->bottomIdx])
        / (triangle->X[triangle->leftIdx] - triangle->X[triangle->bottomIdx]);
        leftB = -slopeLeft * triangle->X[triangle->leftIdx] + triangle->Y[triangle->leftIdx];
        updateLeft = 1;
    }

    if (log_var == 1) {printf("Right slope is %f\n", slopeRight);}
    if (log_var == 1) {printf("Left slope is %f\n", slopeLeft);}

    if (log_var == 1) {printf("Scalines go from %f to %f\n", minY, maxY);}
    for (int i = minY; i <= maxY; i++) {
        if (maxY - minY > 5) {
            printf("too many scanlines at triangle %d\n", triangleNum);
            abort();
        }
        if (updateRight == 1) {rightEnd = ((i - rightB) / slopeRight);}
        if (updateLeft == 1) {leftEnd = ((i - leftB) / slopeLeft);}
        rightEnd = F441(rightEnd);
        leftEnd = C441(leftEnd);
        if (log_var == 2) {printf("Scanline %d: intercepts go from %d to %d\n", i, (int)leftEnd, (int)rightEnd);}

        for ( int c = leftEnd; c <= rightEnd; c++) {
            if (rightEnd - leftEnd > 5) {
                printf("too many pixels at triangle %d scanline %d\n", triangleNum, i);
                abort();
            }
            int row = NUM_ROWS - i - 1;
            int col = c;
            if (row >= NUM_ROWS || col >= NUM_COLS || row < 0 || col < 0) {
                if (log_var == 1) {printf("row = %d | col = %d\n",row,col);}
                continue;
            }
            if (log_var == 1) {printf("inserting pixel at pixels[%d][%d]\n", row, col);}
            img->pixels[row][col] = pixel;

        }
    }
}

void RasterizeGoingDownTriangle(Triangle *triangle, Image *img, int triangleNum)
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

    double slopeLeft = 0;
    double slopeRight = 0;

    int updateLeft = -1;
    int updateRight = -1;


    //error checking for vertical slope, sets right end as the max x coordinate
    if (triangle->X[triangle->rightIdx] == triangle->X[triangle->bottomIdx]) {
        rightEnd = maxX;
        updateRight = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        slopeRight = (triangle->Y[triangle->rightIdx] - triangle->Y[triangle->bottomIdx])
        / (triangle->X[triangle->rightIdx] - triangle->X[triangle->bottomIdx]);
        rightB = -slopeRight * triangle->X[triangle->rightIdx] + triangle->Y[triangle->rightIdx];
        updateRight = 1;
    }

    //error checking for vertical slope, sets left end as the max x coordinate
    if (triangle->X[triangle->leftIdx] == triangle->X[triangle->bottomIdx]) {
        leftEnd = minX;
        updateLeft = 0;
    }

    //solve y = mx + b for b with given slope, y and x values
    else {
        slopeLeft = (triangle->Y[triangle->leftIdx] - triangle->Y[triangle->bottomIdx])
        / (triangle->X[triangle->leftIdx] - triangle->X[triangle->bottomIdx]);
        leftB = -slopeLeft * triangle->X[triangle->leftIdx] + triangle->Y[triangle->leftIdx];
        updateLeft = 1;
    }

    if (log_var == 1) {printf("Right slope is %f\n", slopeRight);}
    if (log_var == 1) {printf("Left slope is %f\n", slopeLeft);}

    if (log_var == 1) {printf("Scalines go from %f to %f\n", minY, maxY);}
    for (int i = (int)minY; i <= (int)maxY; i++) {
        if ((int)maxY - (int)minY > 5) {
            printf("too many scanlines at triangle %d\n", triangleNum);
            abort();
        }
        if (updateRight == 1) {rightEnd = (((double)i - rightB) / slopeRight);}
        if (updateLeft == 1) {leftEnd = (((double)i - leftB) / slopeLeft);}
        rightEnd = F441(rightEnd);
        leftEnd = C441(leftEnd);
        if (log_var == 2) {printf("Scanline %d: intercepts go from %d to %d\n", i, (int)leftEnd, (int)rightEnd);}

        for ( int c = (int)leftEnd; c <= (int)rightEnd; c++) {
            if ((int)rightEnd - (int)leftEnd > 5) {
                printf("too many pixels at triangle %d scanline %d\n", triangleNum, i);
                abort();
            }
            int row = NUM_ROWS - i - 1;
            int col = c;
            if (row >= NUM_ROWS || col >= NUM_COLS || row < 0 || col < 0) {
                if (log_var == 1) {printf("row = %d | col = %d\n",row,col);}
                continue;
            }
            if (log_var == 1) {printf("inserting pixel at pixels[%d][%d]\n", row, col);}
            img->pixels[row][col] = pixel;

        }
    }
}

void RasterizeArbitraryTriangle(Triangle *triangle, Image *img, int triangleNum) {
    if (log_var == 1) {printf("%s called\n", __func__);}
    int topIdx = 0;
    int midIdx = 0;
    int botIdx = 0;

    //set pixel to be inserted
    Pixel pixel = {.red = triangle->color[0], .green = triangle->color[1], .blue = triangle->color[2]};

    double leftEnd = 0;
    double rightEnd = 0;
    double topBotSlope = 0;
    double topMidSlope = 0;
    double botMidSlope = 0;
    double topBotIntercept = 0;
    double topMidIntercept = 0;
    double botMidIntercept = 0;

    for (int i = 0; i < 3; i++) {
        if (triangle->Y[i] < triangle->Y[botIdx]) { botIdx = i;}
        if (triangle->Y[i] > triangle->Y[topIdx]) { topIdx = i;}
    }

    midIdx = 3 - topIdx - botIdx;    // Little trick to find mid

    //set bottom and top of rasterizartion
    double minY = triangle->Y[botIdx];
    double maxY = triangle->Y[topIdx];

    if (log_var == 1) {
        printf("top vertex index is %d at (%f,%f)\n", topIdx, triangle->X[topIdx], triangle->Y[topIdx]);
        printf("mid vertex index is %d at (%f,%f)\n", midIdx, triangle->X[midIdx], triangle->Y[midIdx]);
        printf("bot vertex index is %d at (%f,%f)\n", botIdx, triangle->X[botIdx], triangle->Y[botIdx]);
    }


    /*---If 3 points in a line rather than being a triangle, skip---*/
    if (triangle->Y[triangle->topIdx] == triangle->Y[triangle->bottomIdx]
        && triangle->Y[triangle->topIdx] == triangle->Y[triangle->middleIdx])
        return;
    if (triangle->X[triangle->topIdx] - triangle->X[triangle->bottomIdx]
        && triangle->X[triangle->topIdx] == triangle->X[triangle->middleIdx])
        return;

    if (log_var == 1) { printf("Rasterizing from %f to %f\n", minY, maxY); }

    topBotSlope = (triangle->Y[topIdx] - triangle->Y[botIdx])
    / (triangle->X[topIdx] - triangle->X[botIdx]);
    topBotIntercept = -topBotSlope * triangle->X[botIdx] + triangle->Y[botIdx];

    topMidSlope = (triangle->Y[topIdx] - triangle->Y[midIdx])
    / (triangle->X[topIdx] - triangle->X[midIdx]);
    topMidIntercept = -topMidSlope * triangle->X[midIdx] + triangle->Y[midIdx];

    botMidSlope = (triangle->Y[midIdx] - triangle->Y[botIdx])
    / (triangle->X[midIdx] - triangle->X[botIdx]);
    botMidIntercept = -botMidSlope * triangle->X[botIdx] + triangle->Y[botIdx];

    if (log_var == 1) {
        printf("topBotSlope: %f\n topBotIntercept: %f\n", topBotSlope, topBotIntercept);
        printf("topMidSlope: %f\n topMidIntercept: %f\n", topMidSlope, topMidIntercept);
        printf("botMidSlope: %f\n botMidIntercept: %f\n", botMidSlope, botMidIntercept);
    }
    if (maxY - minY > 5) {
        printf("too many scanlines at triangle %d\n", triangleNum);
        abort();
    }

    for (int i = C441(minY); i <= F441(maxY); i++) {
        // Assuming top-bot line is on the right
        // The ternary operator (shorthand if/else) statements here are just to check for vertical line
        rightEnd = triangle->X[topIdx] == triangle->X[botIdx] ? triangle->X[topIdx] : (i - topBotIntercept) / topBotSlope;

        // Going down and not flat bot, we work with mid-bot line
        if (i < triangle->Y[midIdx] && triangle->Y[midIdx] != triangle->Y[botIdx]) {
            // Assuming mid-bot line is on the right
            leftEnd = triangle->X[midIdx] == triangle->X[botIdx] ? triangle->X[midIdx] : (i - botMidIntercept) / botMidSlope;
        }
        // Going up and not flat top, we work with mid-top line
        else if (i >= triangle->Y[midIdx] && triangle->Y[midIdx] != triangle->Y[topIdx]) {
            // Assuming mid-top line is on the right
            leftEnd = triangle->X[midIdx] == triangle->X[topIdx] ? triangle->X[midIdx] : (i - topMidIntercept) / topMidSlope;
        }
        // This else here means if there's a flat top triangle, don't scan top. And if there's a flat bot triangle, don't scan bot
        else
            return;

        // Now if left intercept ends up greater than right intercept, swap them two
        if (leftEnd > rightEnd) {
            double tmp = leftEnd;
            leftEnd = rightEnd;
            rightEnd = tmp;
        }

        if (log_var == 1) { printf("Scanline %d goes from %f to %f \n", i, leftEnd, rightEnd); }

        for ( int c = C441(leftEnd); c <= F441(rightEnd); c++) {
            if (rightEnd - leftEnd > 5) {
                printf("too many pixels at triangle %d scanline %d\n", triangleNum, i);
                abort();
            }
            int row = NUM_ROWS - i - 1;
            int col = c;
            if (row >= NUM_ROWS || col >= NUM_COLS || row < 0 || col < 0) {
                if (log_var == 1) {printf("row = %d | col = %d\n",row,col);}
                continue;
            }
            if (log_var == 2) {printf("inserting pixel at pixels[%d][%d]\n", row, col);}
            img->pixels[row][col] = pixel;

        }
    }
}

int main(int argc, char* argv[])
{
    log_var = 0;
    if (argc != 1) {
        if (strcmp(argv[1], "-l") == 0) { log_var = 1; }
        if (strcmp(argv[1], "-log") == 0) { log_var = 2; }
    }
    printf("Level %d logging active\n", log_var);
    printf("triangulator running\n");


    int upTriangleCount = 0;
    int downTriangleCount = 0;
    int arbTriangleCount = 0;
    int failTriangleCount = 0;

    int colorRange = 255;

    TriangleList *tl = GetTriangles(0);

    Image img;

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
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
    fprintf(fp, "%d %d\n", NUM_COLS, NUM_ROWS);
    fprintf(fp, "%d\n", colorRange);

    /*----------------------TEST TRIANGLES-----------------------------*/

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

    /*----------------------END TEST TRIANGLES-----------------------------*/

    // Some test cases checking functions are working properly
    //Triangle *curTriangle = &(arbTriangle);
    //determineTriangle(curTriangle);
    //if (curTriangle->triangleType == 2) { RasterizeArbitraryTriangle(curTriangle, &img); }
    //if (curTriangle->triangleType == 1) { RasterizeGoingDownTriangle(curTriangle, &img); }
    //if (curTriangle->triangleType == 0) { RasterizeGoingUpTriangle(curTriangle, &img); }

    for (int i = 0 ; i < tl->numTriangles; i++) {
        Triangle *curTriangle = tl->triangles+i;
        RasterizeArbitraryTriangle(curTriangle, &img, i);
        arbTriangleCount++;
    }

    double totalCount = 100 * ((upTriangleCount + downTriangleCount + arbTriangleCount) / 2500000);

    printf("Triangles counted:\n");
    printf("Up: %d\n",upTriangleCount);
    printf("Down: %d\n",downTriangleCount);
    printf("Arbitrary: %d\n",arbTriangleCount);
    printf("Rasterized %f percent of triangles\n", totalCount);

    fwrite(&img, sizeof(img), 1, fp);

    //3029
    fclose(fp);
    return 0;
}
