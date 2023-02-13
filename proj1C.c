#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_ROWS 1000
#define NUM_COLS 1000

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
   double         Z[3];
   double         color[3][3]; // color[2][0] is for V2, red channel
} Triangle;

typedef struct
{
   int numTriangles;
   Triangle *triangles;
} TriangleList;

char *
ReadTuple3(char *tmp, double *v1, double *v2, double *v3)
{
    tmp++; /* left paren */
    *v1 = atof(tmp);
    while (*tmp != ',')
       tmp++;
    tmp += 2; // comma+space
    *v2 = atof(tmp);
    while (*tmp != ',')
       tmp++;
    tmp += 2; // comma+space
    *v3 = atof(tmp);
    while (*tmp != ')')
       tmp++;
    tmp++; /* right paren */
    return tmp;
}

TriangleList *
Get3DTriangles()
{
   FILE *f = fopen("tris_w_r_rgb.txt", "r");
   if (f == NULL)
   {
       fprintf(stderr, "You must place the tris_w_r_rgb.txt file in the current directory.\n");
       exit(EXIT_FAILURE);
   }
   fseek(f, 0, SEEK_END);
   int numBytes = ftell(f);
   fseek(f, 0, SEEK_SET);
   if (numBytes != 13488634)
   {
       fprintf(stderr, "Your tris_w_r_rgb.txt file is corrupted.  It should be 13488634 bytes, but you have %d.\n", numBytes);
       exit(EXIT_FAILURE);
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

   if (numTriangles != 42281)
   {
       fprintf(stderr, "Issue with reading file -- can't establish number of triangles.\n");
       exit(EXIT_FAILURE);
   }

   TriangleList *tl = (TriangleList *) malloc(sizeof(TriangleList));
   tl->numTriangles = numTriangles;
   tl->triangles = (Triangle *) malloc(sizeof(Triangle)*tl->numTriangles);

   for (int i = 0 ; i < tl->numTriangles ; i++)
   {
       double x1, y1, z1, x2, y2, z2, x3, y3, z3;
       double r[3], g[3], b[3];
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
       tmp = ReadTuple3(tmp, &x1, &y1, &z1);
       tmp += 3; /* space+equal+space */
       tmp = ReadTuple3(tmp, r+0, g+0, b+0);
       tmp += 2; /* comma+space */
       tmp = ReadTuple3(tmp, &x2, &y2, &z2);
       tmp += 3; /* space+equal+space */
       tmp = ReadTuple3(tmp, r+1, g+1, b+1);
       tmp += 2; /* comma+space */
       tmp = ReadTuple3(tmp, &x3, &y3, &z3);
       tmp += 3; /* space+equal+space */
       tmp = ReadTuple3(tmp, r+2, g+2, b+2);
       tmp++;    /* newline */

       tl->triangles[i].X[0] = x1;
       tl->triangles[i].X[1] = x2;
       tl->triangles[i].X[2] = x3;
       tl->triangles[i].Y[0] = y1;
       tl->triangles[i].Y[1] = y2;
       tl->triangles[i].Y[2] = y3;
       tl->triangles[i].Z[0] = z1;
       tl->triangles[i].Z[1] = z2;
       tl->triangles[i].Z[2] = z3;
       tl->triangles[i].color[0][0] = r[0];
       tl->triangles[i].color[0][1] = g[0];
       tl->triangles[i].color[0][2] = b[0];
       tl->triangles[i].color[1][0] = r[1];
       tl->triangles[i].color[1][1] = g[1];
       tl->triangles[i].color[1][2] = b[1];
       tl->triangles[i].color[2][0] = r[2];
       tl->triangles[i].color[2][1] = g[2];
       tl->triangles[i].color[2][2] = b[2];
       //printf("Read triangle (%f, %f, %f) / (%f, %f, %f), (%f, %f, %f) / (%f, %f, %f), (%f, %f, %f) / (%f, %f, %f)\n", x1, y1, z1, r[0], g[0], b[0], x2, y2, z2, r[1], g[1], b[1], x3, y3, z3, r[2], g[2], b[2]);
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

//init the z buffer
double z_buffer[NUM_ROWS][NUM_COLS] = {{-1.0}};

int log_var = 0;

int determineTriangle(Triangle *triangle, int triangleNum)
{
    int leftIdx = 0;
    int topIdx = 0;
    int rightIdx = 0;
    int botIdx = 0;
    int midIdx = 0;
    int top = 0;
    int bot = 0;
    for (int i = 0; i < 3; i++) {
        if (triangle->Y[topIdx] < triangle->Y[i]) {
            topIdx = i;
        }
        if (triangle->Y[botIdx] > triangle->Y[i]) {
            botIdx = i;
        }
    }

    midIdx = 3 - botIdx - topIdx;

    if (triangle->Y[botIdx] < triangle->Y[midIdx] < triangle->Y[topIdx]) {
        if (log_var >= 1) {printf("triangle is arbitrary\n");}
        return 3;
    }

    for (int i = 0; i < 3; i++) {
        if (triangle->Y[topIdx] > triangle->Y[i]) {top++;}
        if (triangle->Y[botIdx] < triangle->Y[i]) {bot++;}
    }

    //flat bottom
    if (top == 2){
        if (log_var >= 1) {printf("triangle is flat bottom\n");}
        return 1;
    }

    //flat top
    else if (bot == 2) {
        if (log_var >= 1) {printf("triangle is flat top\n");}
        return 2;
    }
    return 0;
}

void RasterizeArbitraryTriangle(Triangle *triangle, Image *img, int triangleNum, double z_buffer[][NUM_COLS]) {
    if (log_var == 1) {printf("%s called\n", __func__);}
    int topIdx = 0;
    int midIdx = 0;
    int botIdx = 0;

    //set pixel to be inserted
    Pixel pixel = {
    .red = C441(triangle->color[0][0] * 255),
    .green = C441(triangle->color[0][1] * 255),
    .blue = C441(triangle->color[0][2] * 255) };

    double leftEnd = 0;
    double rightEnd = 0;
    double leftInterpol = 0;
    double rightInterpol = 0;
    double topBotSlope = 0;
    double topMidSlope = 0;
    double botMidSlope = 0;
    double topBotIntercept = 0;
    double topMidIntercept = 0;
    double botMidIntercept = 0;
    double t_left = 0;
    double t_right = 0;
    double f_left_end = 0;
    double f_right_end = 0;

    for (int i = 0; i < 3; i++) {
        if (triangle->Y[i] < triangle->Y[botIdx]) { botIdx = i;}
        if (triangle->Y[i] > triangle->Y[topIdx]) { topIdx = i;}
    }

    midIdx = 3 - topIdx - botIdx;    // Little trick to find mid

    //set bottom and top of rasterizartion
    double minY = triangle->Y[botIdx];
    double maxY = triangle->Y[topIdx];

    if (log_var == 1) {
        printf("Working on triangle %d\n", triangleNum);
        printf("Triangle: (%f, %f, %f) / (%f, %f, %f,),\n (%f, %f, %f) / (%f, %f, %f,),\n (%f, %f, %f) / (%f, %f, %f,)\n",
               triangle->X[topIdx], triangle->Y[topIdx], triangle->Z[topIdx], triangle->color[topIdx][0], triangle->color[topIdx][1], triangle->color[topIdx][2],
               triangle->X[midIdx], triangle->Y[midIdx], triangle->Z[midIdx], triangle->color[midIdx][0], triangle->color[midIdx][1], triangle->color[midIdx][2],
               triangle->X[botIdx], triangle->Y[botIdx], triangle->Z[botIdx], triangle->color[botIdx][0], triangle->color[botIdx][1], triangle->color[botIdx][2]);
    }


    /*---If 3 points in a line rather than being a triangle, skip---*/
    if (triangle->Y[topIdx] == triangle->Y[botIdx]
        && triangle->Y[topIdx] == triangle->Y[midIdx])
        return;
    if (triangle->X[topIdx] - triangle->X[botIdx]
        && triangle->X[topIdx] == triangle->X[midIdx])
        return;

    if (log_var == 1) { printf("scanlines go from %d to %d\n", (int)C441(minY), (int)F441(maxY)); }

    topBotSlope = (triangle->Y[topIdx] - triangle->Y[botIdx])
    / (triangle->X[topIdx] - triangle->X[botIdx]);
    topBotIntercept = -topBotSlope * triangle->X[botIdx] + triangle->Y[botIdx];

    topMidSlope = (triangle->Y[topIdx] - triangle->Y[midIdx])
    / (triangle->X[topIdx] - triangle->X[midIdx]);
    topMidIntercept = -topMidSlope * triangle->X[midIdx] + triangle->Y[midIdx];

    botMidSlope = (triangle->Y[midIdx] - triangle->Y[botIdx])
    / (triangle->X[midIdx] - triangle->X[botIdx]);
    botMidIntercept = -botMidSlope * triangle->X[botIdx] + triangle->Y[botIdx];

    //if (log_var == 1) {
    //    printf("topBotSlope: %f\n topBotIntercept: %f\n", topBotSlope, topBotIntercept);
    //    printf("topMidSlope: %f\n topMidIntercept: %f\n", topMidSlope, topMidIntercept);
    //    printf("botMidSlope: %f\n botMidIntercept: %f\n", botMidSlope, botMidIntercept);
    //}

    for (int i = C441(minY); i <= F441(maxY); i++) {
        // Assuming top-bot line is on the right
        // The ternary operator (shorthand if/else) statements here are just to check for vertical line
        rightEnd = triangle->X[topIdx] == triangle->X[botIdx] ? triangle->X[topIdx] : (i - topBotIntercept) / topBotSlope;
        t_right = triangle->X[topIdx] == triangle->X[botIdx] ? 0 : (rightEnd - triangle->X[topIdx]) / (triangle->X[botIdx] - triangle->X[topIdx]);
        f_right_end = triangle->Z[topIdx] + (t_right * (triangle->Z[botIdx] - triangle->Z[topIdx]));

        // Going down and not flat bot, we work with mid-bot line
        if (i < triangle->Y[midIdx] && triangle->Y[midIdx] != triangle->Y[botIdx]) {
            // Assuming mid-bot line is on the right
            leftEnd = triangle->X[midIdx] == triangle->X[botIdx] ? triangle->X[midIdx] : (i - botMidIntercept) / botMidSlope;
            t_left = triangle->X[midIdx] == triangle->X[botIdx] ? 1 : (leftEnd - triangle->X[botIdx]) / (triangle->X[midIdx] - triangle->X[botIdx]);
            f_left_end = triangle->Z[botIdx] + t_left *(triangle->Z[midIdx] - triangle->Z[botIdx]);
        }
        // Going up and not flat top, we work with mid-top line
        else if (i >= triangle->Y[midIdx] && triangle->Y[midIdx] != triangle->Y[topIdx]) {
            // Assuming mid-top line is on the right
            leftEnd = triangle->X[midIdx] == triangle->X[topIdx] ? triangle->X[midIdx] : (i - topMidIntercept) / topMidSlope;
            t_left = triangle->X[midIdx] == triangle->X[topIdx] ? 1 : (leftEnd - triangle->X[midIdx]) / (triangle->X[topIdx] - triangle->X[midIdx]);
            f_left_end = triangle->Z[midIdx] + t_left *(triangle->Z[topIdx] - triangle->Z[midIdx]);
        }
        // This else here means if there's a flat top triangle, don't scan top. And if there's a flat bot triangle, don't scan bot
        else
            return;

        // Now if left intercept ends up greater than right intercept, swap them two
        if (leftEnd > rightEnd) {
            double tmp = leftEnd;
            leftEnd = rightEnd;
            rightEnd = tmp;

            double tmp_t = f_left_end;
            f_left_end = f_right_end;
            f_right_end = tmp;
        }

        if (log_var == 1) {
            printf("    Rasterizing along row %d with left end = %f (Z: %f, RGB = (%f/%f/%f)\n    right end = %f (Z: %f, RGB = (%f/%f/%f)\n",
                   i, leftEnd, f_left_end, 0.0, 0.0, 0.0, rightEnd, f_right_end, 0.0, 0.0, 0.0);
        }

        for ( int c = C441(leftEnd); c <= F441(rightEnd); c++) {

            //interpolate z(r,c) from z(leftEnd) and z(rightEnd)
            double t_cur_pixel = (c - leftEnd) / (rightEnd - leftEnd);
            double f_cur_pixel = f_left_end + t_cur_pixel * (f_right_end - f_left_end);

            int row = NUM_ROWS - i - 1;
            int col = c;
            if (log_var == 1) {
                printf("        Got fragment r = %d, c = %d z = %f, color = %f/%f/%f\n",
                       row, col, f_cur_pixel, 0.0, 0.0, 0.0);
            }
            //if z(r,c) > depthBuffer(r,c), exit if less than current z_buffer pixel
            if (row >= NUM_ROWS || col >= NUM_COLS || row < 0 || col < 0) {
                if (log_var == 1) {printf("row = %d | col = %d\n",row,col);}
                continue;
            }
            if (f_cur_pixel < z_buffer[row][col]) {
                printf("%f < %f", f_cur_pixel, z_buffer[row][col]);
                continue;
            }
            if (log_var == 2) {
                printf("inserting pixel at pixels[%d][%d]\n", row, col);
                printf("inserting pixel at z_buffer[%d][%d]\n", row, col);
            }
            img->pixels[row][col] = pixel;
            z_buffer[row][col] = f_cur_pixel;

        }
    }
}

int main(int argc, char* argv[])
{
    Pixel black = {.red = 0, .green = 0, .blue = 0};


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

    TriangleList *tl = Get3DTriangles();

    Image img;

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_COLS; j++) {
            img.pixels[i][j] = black;
        }
    }

    char *filename = "proj1C_out.pnm";
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

    printf("Rasterizing %d triangles\n", tl->numTriangles);

    //int len = tl->numTriangles;
    int len = 1;

    for (int i = 0 ; i < len; i++) {
        Triangle *curTriangle = tl->triangles+i;
        int triType = determineTriangle(curTriangle, i);
        switch (triType) {
            case 1:
                downTriangleCount++;
                break;
            case 2:
                upTriangleCount++;
                break;
            case 3:
                arbTriangleCount++;
                RasterizeArbitraryTriangle(curTriangle, &img, i, z_buffer);
                break;
            default:
                printf("triangle type failure\n");
                failTriangleCount++;
                break;
        }
    }

    double totalCount = 100 * ((upTriangleCount + downTriangleCount + arbTriangleCount) / tl->numTriangles);

    printf("Triangles counted:\n");
    printf("Up: %d\n",upTriangleCount);
    printf("Down: %d\n",downTriangleCount);
    printf("Arbitrary: %d\n",arbTriangleCount);
    printf("Rasterized %f percent of triangles\n", totalCount);

    fwrite(&img, sizeof(img), 1, fp);

    fclose(fp);
    return 0;
}
