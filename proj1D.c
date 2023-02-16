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
    double          A[4][4];     // A[i][j] means row i, column j
} Matrix;


void
PrintMatrix(Matrix m)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        printf("(%.7f %.7f %.7f %.7f)\n", m.A[i][0], m.A[i][1], m.A[i][2], m.A[i][3]);
    }
}

Matrix
ComposeMatrices(Matrix M1, Matrix M2)
{
    Matrix m_out;
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
        {
            m_out.A[i][j] = 0;
            for (int k = 0 ; k < 4 ; k++)
                m_out.A[i][j] += M1.A[i][k]*M2.A[k][j];
        }
    return m_out;
}

void
TransformPoint(Matrix m, const double *ptIn, double *ptOut)
{
    ptOut[0] = ptIn[0]*m.A[0][0]
             + ptIn[1]*m.A[1][0]
             + ptIn[2]*m.A[2][0]
             + ptIn[3]*m.A[3][0];
    ptOut[1] = ptIn[0]*m.A[0][1]
             + ptIn[1]*m.A[1][1]
             + ptIn[2]*m.A[2][1]
             + ptIn[3]*m.A[3][1];
    ptOut[2] = ptIn[0]*m.A[0][2]
             + ptIn[1]*m.A[1][2]
             + ptIn[2]*m.A[2][2]
             + ptIn[3]*m.A[3][2];
    ptOut[3] = ptIn[0]*m.A[0][3]
             + ptIn[1]*m.A[1][3]
             + ptIn[2]*m.A[2][3]
             + ptIn[3]*m.A[3][3];
}



typedef struct
{
    double          near, far;
    double          angle;
    double          position[3];
    double          focus[3];
    double          up[3];
} Camera;


double SineParameterize(int curFrame, int nFrames, int ramp)
{
    int nNonRamp = nFrames-2*ramp;
    double height = 1./(nNonRamp + 4*ramp/M_PI);
    if (curFrame < ramp)
    {
        double factor = 2*height*ramp/M_PI;
        double eval = cos(M_PI/2*((double)curFrame)/ramp);
        return (1.-eval)*factor;
    }
    else if (curFrame > nFrames-ramp)
    {
        int amount_left = nFrames-curFrame;
        double factor = 2*height*ramp/M_PI;
        double eval =cos(M_PI/2*((double)amount_left/ramp));
        return 1. - (1-eval)*factor;
    }
    double amount_in_quad = ((double)curFrame-ramp);
    double quad_part = amount_in_quad*height;
    double curve_part = height*(2*ramp)/M_PI;
    return quad_part+curve_part;
}

Camera
GetCamera(int frame, int nframes)
{
    double t = SineParameterize(frame, nframes, nframes/10);
    Camera c;
    c.near = 5;
    c.far = 200;
    c.angle = M_PI/6;
    c.position[0] = 40*sin(2*M_PI*t);
    c.position[1] = 40*cos(2*M_PI*t);
    c.position[2] = 40;
    c.focus[0] = 0;
    c.focus[1] = 0;
    c.focus[2] = 0;
    c.up[0] = 0;
    c.up[1] = 1;
    c.up[2] = 0;
    return c;
}

Matrix GetViewTransform(Camera c)
{
    Matrix m;

    /* YOU IMPLEMENT THIS */

    return m;
}

Matrix
GetCameraTransform(Camera c)
{
    Matrix rv;

    /* YOU IMPLEMENT THIS */

    return rv;
}

Matrix
GetDeviceTransform()
{
    Matrix rv;

    /* YOU IMPLEMENT THIS */

    return rv;
}



typedef struct
{
   double         X[3];
   double         Y[3];
   double         Z[3];
   double         color[3][3]; // color[2][0] is for V2, red channel
#ifdef NORMALS
   double         normals[3][3]; // normals[2][0] is for V2, x-component
#endif
} Triangle;

typedef struct
{
   int numTriangles;
   Triangle *triangles;
} TriangleList;

char *
Read3Numbers(char *tmp, double *v1, double *v2, double *v3)
{
    *v1 = atof(tmp);
    while (*tmp != ' ')
       tmp++;
    tmp++; /* space */
    *v2 = atof(tmp);
    while (*tmp != ' ')
       tmp++;
    tmp++; /* space */
    *v3 = atof(tmp);
    while (*tmp != ' ' && *tmp != '\n')
       tmp++;
    return tmp;
}

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
}/*------------------------END STARTER CODE-------------------------------------------*/

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

void swap(double *value1, double *value2)
{
    double tmp;
    tmp = *value1;
    *value1 = *value2;
    *value2 = tmp;
}

void RasterizeArbitraryTriangle(Triangle *triangle, Image *img, int triangleNum, double **z_buffer) {
    if (log_var == 1) {printf("%s called\n", __func__);}
    //set pixel to be inserted
    Pixel pixel = {
    .red = C441(triangle->color[0][0] * 255),
    .green = C441(triangle->color[0][1] * 255),
    .blue = C441(triangle->color[0][2] * 255) };

    double red_left = 0;
    double green_left = 0;
    double blue_left = 0;
    double red_right = 0;
    double green_right = 0;
    double blue_right = 0;

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

    double minY = 9999999.0;
    double maxY = -9999999.0;
    int topIdx, midIdx, botIdx;
    for (int i = 0; i < 3; i++) {   // Loop through 3 set of points
        if (triangle->Y[i] > maxY) {    // If current Y bigger than maxY
            topIdx = i;                    // Update top index
            maxY = triangle->Y[i];      // Update maxY
        }
        if (triangle->Y[i] < minY){     // If current Y smaller than minY
            botIdx = i;                    // Update bot index
            minY = triangle->Y[i];      // Update minY
        }
    }
    midIdx = 3 - topIdx - botIdx;    // Little trick to find mid

    //set bottom and top of rasterizartion
    minY = triangle->Y[botIdx];
    maxY = triangle->Y[topIdx];

    if (log_var == 1) {
        printf("Working on triangle %d\n", triangleNum);
        printf("Triangle: (%f, %f, %f) / (%f, %f, %f,),\n (%f, %f, %f) / (%f, %f, %f,),\n (%f, %f, %f) / (%f, %f, %f,)\n",
               triangle->X[topIdx], triangle->Y[topIdx], triangle->Z[topIdx], triangle->color[topIdx][0], triangle->color[topIdx][1], triangle->color[topIdx][2],
               triangle->X[midIdx], triangle->Y[midIdx], triangle->Z[midIdx], triangle->color[midIdx][0], triangle->color[midIdx][1], triangle->color[midIdx][2],
               triangle->X[botIdx], triangle->Y[botIdx], triangle->Z[botIdx], triangle->color[botIdx][0], triangle->color[botIdx][1], triangle->color[botIdx][2]);
    }

    /*---If 3 points in a line rather than being a triangle, skip---*/
    if (triangle->Y[topIdx] == triangle->Y[botIdx]
        && triangle->Y[topIdx] == triangle->Y[midIdx]) {
        printf("3 in a row detected (y axis) at triangle %d\n", triangleNum);
        return;
    }
    if (triangle->X[topIdx] == triangle->X[botIdx]
        && triangle->X[topIdx] == triangle->X[midIdx]) {
        printf("3 in a row detected (x axis) at triangle %d\n", triangleNum);
        return;
    }

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
        t_right = triangle->X[topIdx] == triangle->X[botIdx] ? (i - triangle->Y[topIdx]) / (triangle->Y[botIdx] - triangle->Y[topIdx]) : (rightEnd - triangle->X[topIdx]) / (triangle->X[botIdx] - triangle->X[topIdx]);
        f_right_end = triangle->Z[topIdx] + (t_right * (triangle->Z[botIdx] - triangle->Z[topIdx]));

        //interpolate the colors using t_right
        red_right = triangle->color[topIdx][0] + (t_right * (triangle->color[botIdx][0] - triangle->color[topIdx][0]));
        green_right = triangle->color[topIdx][1] + (t_right * (triangle->color[botIdx][1] - triangle->color[topIdx][1]));
        blue_right = triangle->color[topIdx][2] + (t_right * (triangle->color[botIdx][2] - triangle->color[topIdx][2]));

        // Going down and not flat bot, we work with mid-bot line
        if (i < triangle->Y[midIdx] && triangle->Y[midIdx] != triangle->Y[botIdx]) {
            // Assuming mid-bot line is on the right
            leftEnd = triangle->X[midIdx] == triangle->X[botIdx] ? triangle->X[midIdx] : (i - botMidIntercept) / botMidSlope;
            t_left = triangle->X[midIdx] == triangle->X[botIdx] ? (i - triangle->Y[botIdx]) / (triangle->Y[midIdx] - triangle->Y[botIdx]) : (leftEnd - triangle->X[botIdx]) / (triangle->X[midIdx] - triangle->X[botIdx]);
            f_left_end = triangle->Z[botIdx] + t_left *(triangle->Z[midIdx] - triangle->Z[botIdx]);

            //interpolate the colors using t_left
            red_left = triangle->color[botIdx][0] + (t_left * (triangle->color[midIdx][0] - triangle->color[botIdx][0]));
            green_left = triangle->color[botIdx][1] + (t_left * (triangle->color[midIdx][1] - triangle->color[botIdx][1]));
            blue_left = triangle->color[botIdx][2] + (t_left * (triangle->color[midIdx][2] - triangle->color[botIdx][2]));
        }
        // Going up and not flat top, we work with mid-top line
        else if (i >= triangle->Y[midIdx] && triangle->Y[midIdx] != triangle->Y[topIdx]) {
            // Assuming mid-top line is on the right
            leftEnd = triangle->X[midIdx] == triangle->X[topIdx] ? triangle->X[midIdx] : (i - topMidIntercept) / topMidSlope;
            t_left = triangle->X[midIdx] == triangle->X[topIdx] ? (i - triangle->Y[midIdx]) / (triangle->Y[topIdx] - triangle->Y[midIdx]) : (leftEnd - triangle->X[midIdx]) / (triangle->X[topIdx] - triangle->X[midIdx]);
            f_left_end = triangle->Z[midIdx] + t_left *(triangle->Z[topIdx] - triangle->Z[midIdx]);

            //interpolate the colors using t_left
            red_left = triangle->color[midIdx][0] + (t_left * (triangle->color[topIdx][0] - triangle->color[midIdx][0]));
            green_left = triangle->color[midIdx][1] + (t_left * (triangle->color[topIdx][1] - triangle->color[midIdx][1]));
            blue_left = triangle->color[midIdx][2] + (t_left * (triangle->color[topIdx][2] - triangle->color[midIdx][2]));
        }
        // This else here means if there's a flat top triangle, don't scan top. And if there's a flat bot triangle, don't scan bot
        else
            return;

        // Now if left intercept ends up greater than right intercept, swap them two
        if (leftEnd > rightEnd) {
            //if (log_var >= 1) { printf("swapped at %d\n", triangleNum); }
            //double tmp = leftEnd;
            //leftEnd = rightEnd;
            //rightEnd = tmp;
            swap(&leftEnd, &rightEnd);

            //double tmp_t = f_left_end;
            //f_left_end = f_right_end;
            //f_right_end = tmp_t;
            swap(&f_left_end, &f_right_end);
            swap(&red_left, &red_right);
            swap(&green_left, &green_right);
            swap(&blue_left, &blue_right);

        }

        if (log_var == 1) {
            printf("    Rasterizing along row %d with left end = %f (Z: %f, RGB = (%f/%f/%f)\n    right end = %f (Z: %f, RGB = (%f/%f/%f)\n",
                   i, leftEnd, f_left_end, red_left, green_left, blue_left, rightEnd, f_right_end, red_right, green_right, blue_right);
        }

        for ( int c = C441(leftEnd); c <= F441(rightEnd); c++) {

            //interpolate z(r,c) from z(leftEnd) and z(rightEnd)
            double t_cur_pixel = (c - leftEnd) / (rightEnd - leftEnd);
            double f_cur_pixel = f_left_end + t_cur_pixel * (f_right_end - f_left_end);
            double red_inter = (red_left + t_cur_pixel * (red_right - red_left));
            double green_inter = (green_left + t_cur_pixel * (green_right - green_left));
            double blue_inter = (blue_left + t_cur_pixel * (blue_right - blue_left));

            pixel.red = C441(red_inter * 255);
            pixel.green = C441(green_inter * 255);
            pixel.blue = C441(blue_inter * 255);

            int row = NUM_ROWS - i - 1;
            int col = c;
            if (log_var == 1) {
                printf("        Got fragment r = %d, c = %d z = %f, color = %f/%f/%f\n",
                       row, col, f_cur_pixel, red_inter, green_inter, blue_inter);
            }
            if (row >= NUM_ROWS || col >= NUM_COLS || row < 0 || col < 0) {
                if (log_var == 1) {printf("row = %d | col = %d\n",row,col);}
                continue;
            }

            //if z(r,c) > depthBuffer(r,c), exit if less than current z_buffer pixel
            if (f_cur_pixel < z_buffer[row][col]) {
                if (log_var == 2) { printf("%f < %f\n", f_cur_pixel, z_buffer[row][col]); }
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

void TransformAndRenderTriangles(Camera c, TriangleList *tl, Image *img, double **z_buffer)
{
    if (log_var == 1) {printf("%s called\n", __func__);}
    double arbTriangleCount = 0;

    printf("Rasterizing %d triangles\n", tl->numTriangles);

    int len = tl->numTriangles;
    //int len = 10;

    for (int i = 0 ; i < len; i++) {
        Triangle *curTriangle = tl->triangles+i;
        arbTriangleCount++;
        RasterizeArbitraryTriangle(curTriangle, img, i, z_buffer);
    }

    double totalCount = (arbTriangleCount / tl->numTriangles);

    printf("Triangles counted:\n");
    printf("Arbitrary: %f\n",arbTriangleCount);
    printf("Rasterized %f percent of triangles\n", totalCount);

}

void InitializeScreen(Image *img, int num_rows, int num_cols)
{
    Pixel black = {.red = 0, .green = 0, .blue = 0};

    if (log_var == 1) {printf("%s called\n", __func__);}
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            img->pixels[i][j] = black;
        }
    }
    printf("%s initialized img of %d x %d", __func__, num_rows, num_cols);
}

Image* AllocateScreen(int num_rows, int num_cols)
{
    if (log_var == 1) {printf("%s called\n", __func__);}
    Pixel black = {.red = 0, .green = 0, .blue = 0};
    Image *img = malloc(sizeof(*img));

    printf("loop started\n");
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            img->pixels[i][j] = black;
        }
    }
    printf("%s initialized img of %d x %d", __func__, num_rows, num_cols);
    return img;
}

double** InitializBuffer(int num_rows, int num_cols)
{
    if (log_var == 1) {printf("%s called\n", __func__);}

    //init memory for z_buffer
    double **z_buffer = malloc(NUM_ROWS * sizeof *z_buffer);
    for (int i = 0; i < NUM_ROWS; i++)
        z_buffer[i] = malloc(NUM_ROWS * sizeof *z_buffer[i]);

    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < NUM_ROWS; j++) {
            z_buffer[i][j] = -1;
        }
    }
    return z_buffer;
}

void SaveImage(Image *img, int i)
{
    if (log_var == 1) {printf("%s called\n", __func__);}
    char *filename = (i == 0) ? "proj1E_frame0000.pnm" : (i == 250) ? "proj1E_frame0250.pnm" : (i == 500) ? "proj1E_frame0500.pnm" : (i == 750) ? "proj1E_frame0750.pnm" : "wrong_num.pnm";
    int colorRange = 255;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("file open failure\n");
        return;
    }
    else {
        printf("file open success\n");
    }

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", NUM_COLS, NUM_ROWS);
    fprintf(fp, "%d\n", colorRange);

    fwrite(img, sizeof(*img), 1, fp);
    printf("%s saved file named %s\n", __func__, filename);
    fclose(fp);
}


int main(int argc, char* argv[])
{
    printf("triangulator running\n");
    log_var = 0;
    if (argc != 1) {
        if (strcmp(argv[1], "-l") == 0) { log_var = 1; }
        if (strcmp(argv[1], "-log") == 0) { log_var = 2; }
    }
    printf("Level %d logging active\n", log_var);

    TriangleList *tl = Get3DTriangles();
    Image *img = AllocateScreen(NUM_ROWS, NUM_COLS);
    for (int i = 0 ; i < 1000 ; i++) {

        if (i % 250 != 0)
            continue;
        double **z_buffer = InitializBuffer(NUM_ROWS, NUM_COLS);
        InitializeScreen(img, NUM_ROWS, NUM_COLS);
        Camera c = GetCamera(i, 1000);
        TransformAndRenderTriangles(c, tl, img, z_buffer);
        SaveImage(img, i);
    }
    return 0;
}