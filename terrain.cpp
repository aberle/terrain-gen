/*
 * Terrain Generation function definitions
 *
 * Based on code from http://www.lighthouse3d.com/opengl/terrain/
 * to use simplex noise values to calculate the terrain heights rather
 * than a TGA image file.
 */

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "terrain.h"
#include "simplexnoise.h"
#include <QtOpenGL>

static int terrainGridWidth ,terrainGridLength;
static float *terrainHeights = NULL;
static float *terrainColors = NULL;
static float *terrainNormals = NULL;
static float terrainStepLength = 1.0;
static float terrainStepWidth = 1.0;

float n1[3] = {0.0,0.0,0.0},n2[3] = {0.0,0.0,0.0},n3[3] = {0.0,0.0,0.0},n4[3] = {0.0,0.0,0.0};

static void terrainComputeNormals();
static void terrainNormalize(float *v);

static float *terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) 
{
    float *auxNormal,v1[3],v2[3];
        
    v1[0] = (x2-x1) * terrainStepWidth; 
    v1[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
            + terrainHeights[z2 * terrainGridWidth + x2];
    v1[2] = (z2-z1) * terrainStepLength; 


    v2[0] = (x3-x1) * terrainStepWidth; 
    v2[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
            + terrainHeights[z3 * terrainGridWidth + x3];
    v2[2] = (z3-z1) * terrainStepLength; 

    auxNormal = (float *)malloc(sizeof(float)*3);

    auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
    auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
    auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];

    return(auxNormal);
}

static void terrainNormalize(float *v) 
{
    double d;
    
    d = sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));

    v[0] = v[0] / d;
    v[1] = v[1] / d;
    v[2] = v[2] / d;
}

static void terrainAddVector(float *a, float *b) 
{
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
}

void terrainComputeNormals() 
{

    float *norm1,*norm2;
    int i,j,k;
    
    if (terrainNormals == NULL)
        return;


    for(i = 0; i < terrainGridLength; i++)
        for(j = 0; j < terrainGridWidth; j++) {
            norm1 = NULL;
            norm2 = NULL;

            // Normals for the four corners
            if (i == 0 && j == 0) 
            {
                norm1 = terrainCrossProduct(0,0, 0,1, 1,0); 
                terrainNormalize(norm1);                
            }
            else if (j == terrainGridWidth-1 && i == terrainGridLength-1) 
            {
                norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i); 
                terrainNormalize(norm1);                
            }
            else if (j == 0 && i == terrainGridLength-1) 
            {
                norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i); 
                terrainNormalize(norm1);                
            }
            else if (j == terrainGridWidth-1 && i == 0) 
            {
                norm1 = terrainCrossProduct(j,i, j,i+1, j-1,i); 
                terrainNormalize(norm1);                
            }

            // Normals for the borders
            else if (i == 0) 
            {
                norm1 = terrainCrossProduct(j,0, j-1,0, j,1);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,0,j,1,j+1,0);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
            }
            else if (j == 0) 
            {
                norm1 = terrainCrossProduct(0,i, 1,i, 0,i-1);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(0,i, 0,i+1, 1,i);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
            }
            else if (i == terrainGridLength-1) 
            {
                norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,i, j+1,i, j,i-1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
            }
            else if (j == terrainGridWidth-1) 
            {
                norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,i, j-1,i, j,i+1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
            }

            // Normals for the inner vertices using 8 neighbors
            else 
            {
                norm1 = terrainCrossProduct(j,i, j-1,i, j-1,i+1);
                terrainNormalize(norm1);
                norm2 = terrainCrossProduct(j,i, j-1,i+1, j,i+1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
                norm2 = terrainCrossProduct(j,i, j,i+1, j+1,i+1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
                norm2 = terrainCrossProduct(j,i, j+1,i+1, j+1,i);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
                norm2 = terrainCrossProduct(j,i, j+1,i, j+1,i-1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
                norm2 = terrainCrossProduct(j,i, j+1,i-1, j,i-1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
                norm2 = terrainCrossProduct(j,i, j,i-1, j-1,i-1);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
                norm2 = terrainCrossProduct(j,i, j-1,i-1, j-1,i);
                terrainNormalize(norm2);
                terrainAddVector(norm1,norm2);
                free(norm2);
            }

            terrainNormalize(norm1);
            norm1[2] = - norm1[2];

            for (k = 0; k< 3; k++) 
                terrainNormals[3*(i*terrainGridWidth + j) + k] = norm1[k];

            free(norm1);
        }
}

int generateTerrain(int normals, int turbulencePasses, float octaves, float persistence, float amplitude)
{
    int i, j;
    float pointHeight;

    // If terrain already exists, destroy it
    if (terrainHeights != NULL)
        terrainDestroy();
    
    // Set the width and length of the terrain
    terrainGridLength = NOISE_HEIGHT;
    terrainGridWidth = NOISE_WIDTH;

    // Allocate memory for the terrain
    terrainHeights = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
    if (terrainHeights == NULL)
        return(TERRAIN_ERROR_MEMORY_PROBLEM);

    // Allocate memory for the normals
    if (normals) 
    {
        terrainNormals = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float) * 3);
        if (terrainNormals == NULL)
            return(TERRAIN_ERROR_MEMORY_PROBLEM);
    }
    else
        terrainNormals = NULL;

    terrainColors = NULL;

    // Prepare noise image
    QImage *img = new QImage(512, 512, QImage::Format_RGB32);
    QRgb value = qRgb(255,255,0);
    float colorFactor;

    // Fill height array with noise values
    for (i = 0 ; i < terrainGridLength; i++)
    {
        for (j = 0;j < terrainGridWidth; j++) 
        {
            // Compute the height using 2d simplex noise
            pointHeight = turbulence(i, j, turbulencePasses, octaves, persistence, amplitude);
            terrainHeights[i*terrainGridWidth + j] = pointHeight;

            // Clamp image representation to 8 bit color
            colorFactor = (pointHeight+1.0)/2.0;
            if (colorFactor > 1.0)
                colorFactor = 1.0;
            if (colorFactor < 0.0)
                colorFactor = 0.0;


            // Calculate RGB representation of height
            value = qRgb(255*colorFactor, 255*colorFactor, 255*colorFactor);
            
            if (colorFactor < 0.0 || colorFactor > 1.0)
                printf("colorFactor: %f\n", colorFactor);

            // Set the appropriate pixel
            img->setPixel(i, j, value);
        }
    }

    // Save image to disk
    img->save("terrain.bmp");
    
    // If we want normals then compute them
    if (normals)
        terrainComputeNormals();
    
    return(TERRAIN_OK); 
}

int terrainScale(float min,float max) 
{
    float amp,aux,min1,max1,height;
    int total,i;

    if (terrainHeights == NULL)
        return(TERRAIN_ERROR_NOT_INITIALISED);

    if (min > max) 
    {
        aux = min;
        min = max;
        max = aux;
    }

    amp = max - min;
    total = terrainGridWidth * terrainGridLength;

    min1 = terrainHeights[0];
    max1 = terrainHeights[0];
    for(i=1;i < total ; i++) 
    {
        if (terrainHeights[i] > max1)
            max1 = terrainHeights[i];
        if (terrainHeights[i] < min1)
            min1 = terrainHeights[i];
    }
    for(i=0;i < total; i++) 
    {
        height = (terrainHeights[i] - min1) / (max1-min1);
        terrainHeights[i] = height * amp - min;
    }
    if (terrainNormals != NULL)
        terrainComputeNormals();

    return(TERRAIN_OK);
}

int terrainDim(float stepWidth, float stepLength) 
{
    if (stepWidth > 0 && stepLength > 0) 
    {
        terrainStepWidth = stepWidth;
        terrainStepLength = stepLength;

        if (terrainNormals != NULL)
            terrainComputeNormals();

        return(TERRAIN_OK);
    }
    else
        return(TERRAIN_ERROR_INVALID_PARAM);
}

int terrainCreateDL() 
{
    GLuint terrainDL;
    float startW,startL;
    int i,j;

    startW = terrainGridWidth / 2.0 - terrainGridWidth;
    startL = - terrainGridLength / 2.0 + terrainGridLength;

    terrainDL = glGenLists(1);

    glNewList(terrainDL,GL_COMPILE);
    for (i = 0 ; i < terrainGridLength-1; i++) 
    {
        glBegin(GL_TRIANGLE_STRIP);

        for(j=0; j < terrainGridWidth; j++) 
        {
            if (terrainColors != NULL) 
                glColor3f(terrainColors[3*((i+1)*terrainGridWidth + j)],
                          terrainColors[3*((i+1)*terrainGridWidth + j)+1],
                          terrainColors[3*((i+1)*terrainGridWidth + j)+2]);
    
            if (terrainNormals != NULL)
                glNormal3f(terrainNormals[3*((i+1)*terrainGridWidth + j)],
                          terrainNormals[3*((i+1)*terrainGridWidth + j)+1],
                          terrainNormals[3*((i+1)*terrainGridWidth + j)+2]);    
            
            glTexCoord2f((((startW + j)*terrainStepWidth)/terrainGridWidth)+.5, (((startL - (i+1))*terrainStepLength)/terrainGridLength) + .5);
            //printf("texcoords = %f, %f\n", (((startW + j)*terrainStepWidth)/terrainGridWidth)+.5, (((startL - (i+1))*terrainStepLength)/terrainGridLength) + .5);
            
            glVertex3f(
                (startW + j)*terrainStepWidth,// * stepW,
                terrainHeights[(i+1)*terrainGridWidth + (j)],
                (startL - (i+1))*terrainStepLength);// * stepL);                    
            
            if (terrainColors != NULL) 
                glColor3f(terrainColors[3*(i*terrainGridWidth + j)],
                          terrainColors[3*(i*terrainGridWidth + j)+1],
                          terrainColors[3*(i*terrainGridWidth + j)+2]);
            if (terrainNormals != NULL)
                glNormal3f(terrainNormals[3*(i*terrainGridWidth + j)],
                           terrainNormals[3*(i*terrainGridWidth + j)+1],
                           terrainNormals[3*(i*terrainGridWidth + j)+2]);
            
            glTexCoord2f((((startW + j)*terrainStepWidth)/terrainGridWidth)+.5, (((startL - i)*terrainStepLength)/terrainGridLength)+.5);
            
            glVertex3f(
                (startW + j)*terrainStepWidth,// * stepW,
                terrainHeights[i*terrainGridWidth + j],
                (startL - i)*terrainStepLength);// * stepL);
        }
        glEnd();
    }
    glEndList();

    return(terrainDL);
}

float terrainGetHeight(int x, int z) 
{
    int xt,zt;

    if (terrainHeights == NULL) 
            return(0.0);

    xt = x + terrainGridWidth /2;
    zt = terrainGridLength - (z + terrainGridLength /2);

    if ((xt > terrainGridWidth) || (zt > terrainGridLength) || (xt < 0) || (zt < 0))
        return(0.0);

    return(terrainHeights[zt * terrainGridWidth + xt]);
}


void terrainDestroy() 
{
    if (terrainHeights != NULL)
        free(terrainHeights);

    if (terrainColors != NULL)
        free(terrainColors);

    if (terrainNormals != NULL)
        free(terrainNormals);
}
