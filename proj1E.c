#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_ROWS 1000
#define NUM_COLS 1000
int log_var = 0;

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

double *normalizeVector(double vec[3])
{
    //given 3 element vector, normalize and return
    double vec_mag = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    vec[0] /= vec_mag;
    vec[1] /= vec_mag;
    vec[2] /= vec_mag;

    return vec;
}

Matrix GetViewTransform(Camera c)
{
    /* YOU IMPLEMENT THIS */
    double cot = 1/tan(c.angle / 2);

    Matrix m = {{{cot, 0, 0, 0},
                 {0, cot, 0, 0},
                 {0, 0, (c.far + c.near) / (c.far - c.near), -1},
                 {0, 0, (2 * c.far * c.near) / (c.far - c.near), 0}}};

    printf("View Transform\n");
    PrintMatrix(m);
    return m;
}

Matrix
GetCameraTransform(Camera c)
{
    /* YOU IMPLEMENT THIS */

    double O[3] = {c.position[0], c.position[1], c.position[2]};
    double O_focus[3] = {c.position[0] - c.focus[0], c.position[1] - c.focus[1], c.position[2] - c.focus[2]};

    //u = Up cross (O - focus)
    double u[3] = {(c.up[1] * O_focus[2]) - (c.up[2] * O_focus[1]),
    (c.up[2] * O_focus[0]) - (c.up[0] * O_focus[2]),
    (c.up[0] * O_focus[1]) - (c.up[1] * O_focus[0])};

    normalizeVector(u);

    //v = (O-focus) cross u
    double v[3] = {(O_focus[1] * u[2]) - (O_focus[2] * u[1]),
    (O_focus[2] * u[0]) - (O_focus[0] * u[2]),
    (O_focus[0] * u[1]) - (O_focus[1] * u[0])};

    normalizeVector(v);

    //w = O_focus
    double w[3] = {O_focus[0], O_focus[1], O_focus[2]};
    normalizeVector(w);

    double t[3] = {0 - O[0], 0 - O[1], 0 - O[2]};

    double u_dot_t = u[0] * t[0] + u[1] * t[1] + u[2] * t[2];
    double v_dot_t = v[0] * t[0] + v[1] * t[1] + v[2] * t[2];
    double w_dot_t = w[0] * t[0] + w[1] * t[1] + w[2] * t[2];

    Matrix rv = {{{u[0], v[0], w[0], 0},
                {u[1], v[1], w[1], 0},
                {u[2], v[2], w[2], 0},
                {u_dot_t, v_dot_t, w_dot_t, 1}}};

    printf("Camera Frame: U = %f, %f, %f,\n", u[0], u[1], u[2]);
    printf("Camera Frame: V = %f, %f, %f,\n", v[0], v[1], v[2]);
    printf("Camera Frame: W = %f, %f, %f,\n", w[0], w[1], w[2]);
    printf("Camera Frame: O = %f, %f, %f,\n", O[0], O[1], O[2]);

    printf("Camera Transform\n");
    PrintMatrix(rv);

    return rv;
}

Matrix
GetDeviceTransform()
{
    /* YOU IMPLEMENT THIS */
    double n = NUM_ROWS;
    double m = NUM_COLS;
    Matrix rv = {{{n/2, 0, 0, 0},
                  {0, m/2, 0, 0},
                  {0, 0, 1, 0},
                  {n/2, m/2, 0, 1}}};
    return rv;
}


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

TriangleList *
Get3DTriangles()
{
   FILE *f = fopen("ws_tris.txt", "r");
   if (f == NULL)
   {
       fprintf(stderr, "You must place the ws_tris.txt file in the current directory.\n");
       exit(EXIT_FAILURE);
   }
   fseek(f, 0, SEEK_END);
   int numBytes = ftell(f);
   fseek(f, 0, SEEK_SET);
   if (numBytes != 3892295)
   {
       fprintf(stderr, "Your ws_tris.txt file is corrupted.  It should be 3892295 bytes, but you have %d.\n", numBytes);
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

   if (numTriangles != 14702)
   {
       fprintf(stderr, "Issue with reading file -- can't establish number of triangles.\n");
       exit(EXIT_FAILURE);
   }

   TriangleList *tl = (TriangleList *) malloc(sizeof(TriangleList));
   tl->numTriangles = numTriangles;
   tl->triangles = (Triangle *) malloc(sizeof(Triangle)*tl->numTriangles);

   for (int i = 0 ; i < tl->numTriangles ; i++)
   {
       for (int j = 0 ; j < 3 ; j++)
       {
           double x, y, z;
           double r, g, b;
           double normals[3];
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
           tmp = Read3Numbers(tmp, &x, &y, &z);
           tmp += 3; /* space+slash+space */
           tmp = Read3Numbers(tmp, &r, &g, &b);
           tmp += 3; /* space+slash+space */
           tmp = Read3Numbers(tmp, normals+0, normals+1, normals+2);
           tmp++;    /* newline */

           tl->triangles[i].X[j] = x;
           tl->triangles[i].Y[j] = y;
           tl->triangles[i].Z[j] = z;
           tl->triangles[i].color[j][0] = r;
           tl->triangles[i].color[j][1] = g;
           tl->triangles[i].color[j][2] = b;
#ifdef NORMALS
           tl->triangles[i].normals[j][0] = normals[0];
           tl->triangles[i].normals[j][1] = normals[1];
           tl->triangles[i].normals[j][2] = normals[2];
#endif
       }
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

    Matrix gt = GetCameraTransform(c);
    Matrix gv = GetViewTransform(c);
    Matrix gd = GetDeviceTransform();
    Matrix tm = ComposeMatrices(ComposeMatrices(gt, gv), gd);
    printf("Total transform\n");
    PrintMatrix(tm);

    //var for counting triangles rasterized
    double arbTriangleCount = 0;

    printf("Rasterizing %d triangles\n", tl->numTriangles);

    int len = tl->numTriangles;
    //int len = 2;

    for (int i = 0 ; i < len; i++) {
        Triangle *curTriangle = tl->triangles+i;
        for (int j = 0; j < 3; j++) {
            const double ptIn[4] = {curTriangle->X[j], curTriangle->Y[j], curTriangle->Z[j], 1};
            double ptOut[4] = {0, 0, 0, 0};
            TransformPoint(tm, ptIn, ptOut);
            if (ptOut[3] != 1)
            {
                ptOut[0] /= ptOut[3];
                ptOut[1] /= ptOut[3];
                ptOut[2] /= ptOut[3];
            }
            if (log_var >= 2) { printf("Transformed V%d from (%f, %f, %f) to (%f, %f, %f)\n", j, ptIn[0], ptIn[1], ptIn[2], ptOut[0], ptOut[1], ptOut[2]); }
            curTriangle->X[j] = ptOut[0];
            curTriangle->Y[j] = ptOut[1];
            curTriangle->Z[j] = ptOut[2];
        }
        RasterizeArbitraryTriangle(curTriangle, img, i, z_buffer);
        arbTriangleCount++;
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
    printf("%s initialized img of %d x %d\n", __func__, num_rows, num_cols);
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
    printf("%s initialized img of %d x %d\n", __func__, num_rows, num_cols);
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

    Image *img = AllocateScreen(NUM_ROWS, NUM_COLS);
    for (int i = 0 ; i < 1000 ; i++) {

        if (i % 250 != 0)
            continue;
        TriangleList *tl = Get3DTriangles();
        double **z_buffer = InitializBuffer(NUM_ROWS, NUM_COLS);
        InitializeScreen(img, NUM_ROWS, NUM_COLS);
        Camera c = GetCamera(i, 1000);
        TransformAndRenderTriangles(c, tl, img, z_buffer);
        SaveImage(img, i);
    }
    return 0;
}
