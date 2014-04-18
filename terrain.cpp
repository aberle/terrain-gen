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
//#include "CSCIx229.h"

static int terrainGridWidth ,terrainGridLength;
static float *terrainHeights = NULL;
static float *terrainColors = NULL;
static float *terrainNormals = NULL;
static float terrainStepLength = 1.0;
static float terrainStepWidth = 1.0;

static float terrainLightPos[4] = {0.0,0.1,0.1,0.0};
static float terrainDiffuseCol[3] = {1.0,1.0,1.0};
static float terrainAmbientCol[3] = {0.04,0.04,0.04};
static int terrainSimLight = 1;

float n1[3] = {0.0,0.0,0.0},n2[3] = {0.0,0.0,0.0},n3[3] = {0.0,0.0,0.0},n4[3] = {0.0,0.0,0.0};

static void terrainComputeNormals();
static void terrainNormalize(float *v);


void terrainLightPosition(float x, float y, float z,float w) {

	terrainLightPos[0] = x;
	terrainLightPos[1] = y;
	terrainLightPos[2] = z;
	terrainLightPos[3] = w;

	/* normalise this vector to save time later */
	if (terrainLightPos[3] == 0.0)
		terrainNormalize(terrainLightPos);
}

void terrainDiffuseColor(float r, float g, float b) {

	terrainDiffuseCol[0] = r ;
	terrainDiffuseCol[1] = g;
	terrainDiffuseCol[2] = b;

}

void terrainAmbientColor(float r, float g, float b) {

	terrainAmbientCol[0] = r;
	terrainAmbientCol[1] = g;
	terrainAmbientCol[2] = b;
}



int terrainSimulateLighting(int sim) {

	terrainSimLight = sim;

	/* just in case we don't have normals already */
	if (terrainNormals == NULL) {
		terrainNormals = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float) * 3);
		terrainComputeNormals();
	}
	if (terrainNormals == NULL) 
		return(TERRAIN_ERROR_MEMORY_PROBLEM);
	else
		return(TERRAIN_OK);

}


static float *terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) {

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

static void terrainNormalize(float *v) {

	double d;
	
	d = sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));

	v[0] = v[0] / d;
	v[1] = v[1] / d;
	v[2] = v[2] / d;
}

static void terrainAddVector(float *a, float *b) {

	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void terrainComputeNormals() {

	float *norm1,*norm2;
	int i,j,k;
	
	if (terrainNormals == NULL)
		return;


	for(i = 0; i < terrainGridLength; i++)
		for(j = 0; j < terrainGridWidth; j++) {
			norm1 = NULL;
			norm2 = NULL;

			/* normals for the four corners */
			if (i == 0 && j == 0) {
				norm1 = terrainCrossProduct(0,0, 0,1, 1,0);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == 0 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == 0) {
				norm1 = terrainCrossProduct(j,i, j,i+1, j-1,i);	
				terrainNormalize(norm1);				
			}

			/* normals for the borders */
			else if (i == 0) {
				norm1 = terrainCrossProduct(j,0, j-1,0, j,1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,0,j,1,j+1,0);
				terrainNormalize(norm2);
				terrainAddVector(norm1,norm2);
				free(norm2);
			}
			else if (j == 0) {
				norm1 = terrainCrossProduct(0,i, 1,i, 0,i-1);
				terrainNormalize(norm1);
				norm2 
					= terrainCrossProduct(0,i, 0,i+1, 1,i);
				terrainNormalize(norm2);
				terrainAddVector(norm1,norm2);
				free(norm2);
			}
			else if (i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm2);
				terrainAddVector(norm1,norm2);
				free(norm2);
			}
			else if (j == terrainGridWidth-1) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm2);
				terrainAddVector(norm1,norm2);
				free(norm2);
			}

			/* normals for the inner vertices using 8 neighbours */
			else {
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


int generateTerrain(int normals) 
{
	int i, j;
	float pointHeight;

	/* if a terrain already exists, destroy it. */
	if (terrainHeights != NULL)
		terrainDestroy();
	
	/* set the width and length of the terrain */
	terrainGridLength = 1024;
	terrainGridWidth = 1024;

	/* alocate memory for the terrain, and check for errors */
	terrainHeights = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_MEMORY_PROBLEM);

	/* allocate memory for the normals, and check for errors */
	if (normals) 
	{
		terrainNormals = (float *)malloc(terrainGridWidth * terrainGridLength * sizeof(float) * 3);
		if (terrainNormals == NULL)
			return(TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
			terrainNormals = NULL;

	terrainColors = NULL;

	/* fill arrays */
	for (i = 0 ; i < terrainGridLength; i++)
		for (j = 0;j < terrainGridWidth; j++) {

			/* compute the height using 2d simplex noise */
			pointHeight = turbulence(i, j, 100);
			terrainHeights[i*terrainGridWidth + j] = pointHeight;
		}
	
	/* if we want normals then compute them */
	if (normals)
		terrainComputeNormals();
	
	return(TERRAIN_OK); 
}

int terrainScale(float min,float max) {

	float amp,aux,min1,max1,height;
	int total,i;

	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_NOT_INITIALISED);

	if (min > max) {
		aux = min;
		min = max;
		max = aux;
	}

	amp = max - min;
	total = terrainGridWidth * terrainGridLength;

	min1 = terrainHeights[0];
	max1 = terrainHeights[0];
	for(i=1;i < total ; i++) {
		if (terrainHeights[i] > max1)
			max1 = terrainHeights[i];
		if (terrainHeights[i] < min1)
			min1 = terrainHeights[i];
	}
	for(i=0;i < total; i++) {
		height = (terrainHeights[i] - min1) / (max1-min1);
		terrainHeights[i] = height * amp - min;
	}
	if (terrainNormals != NULL)
		terrainComputeNormals();
	return(TERRAIN_OK);
}


int terrainDim(float stepWidth, float stepLength) {

	if (stepWidth > 0 && stepLength > 0) {
		terrainStepWidth = stepWidth;
		terrainStepLength = stepLength;

		if (terrainNormals != NULL)
			terrainComputeNormals();

		return(TERRAIN_OK);
	}
	else
		return(TERRAIN_ERROR_INVALID_PARAM);
}


static float terrainComputeLightFactor(int i,int j,int offseti, int offsetj) {
	
	float factor,v[3];

	if (terrainLightPos[3] == 0.0) /* directional light */
	factor = terrainNormals[3*(i * terrainGridWidth + j)] * terrainLightPos[0] +
				terrainNormals[3*(i * terrainGridWidth + j) +1] * terrainLightPos[1] +
				terrainNormals[3*(i * terrainGridWidth + j) +2] * terrainLightPos[2];
	else { /* positional light */
		v[0] = terrainLightPos[0] - (j + offsetj)*terrainStepWidth;
		v[1] = terrainLightPos[1] - terrainHeights[i*terrainGridWidth + j];
		v[2] = terrainLightPos[2] - (offseti -i) * terrainStepLength;
		terrainNormalize(v);
		factor = terrainNormals[3*(i * terrainGridWidth + j)] * v[0] +
				terrainNormals[3*(i * terrainGridWidth + j) +1] * v[1] +
				terrainNormals[3*(i * terrainGridWidth + j) +2] * v[2];
	}	
	if (factor < 0)
		factor = 0;
	return(factor);
}

int terrainCreateDL(int lighting, int texture) {

	GLuint terrainDL;
	float startW,startL,factor;
	int i,j;//, terrainTexture;

	// prevent unused var warning
	factor = texture = 0;

	/*switch (texture)
	{
		case 0:
			terrainTexture = LoadTexBMP("rock1.bmp");
			break;
		case 1:
			terrainTexture = LoadTexBMP("rock2.bmp");
			break;
		case 2:
			terrainTexture = LoadTexBMP("rock3.bmp");
			break;
		default:
			terrainTexture = LoadTexBMP("rock1.bmp");
			break;
	}*/

	startW = terrainGridWidth / 2.0 - terrainGridWidth;
	startL = - terrainGridLength / 2.0 + terrainGridLength;

	terrainDL = glGenLists(1);

	if (lighting)
		terrainSimLight = 0;

	glNewList(terrainDL,GL_COMPILE);
	for (i = 0 ; i < terrainGridLength-1; i++) {
		//glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, terrainTexture);

		glBegin(GL_TRIANGLE_STRIP);

		for(j=0; j < terrainGridWidth; j++) {
			if (terrainSimLight  && terrainColors != NULL) {
				factor = terrainComputeLightFactor(i+1,j,startL,startW);
				glColor3f(terrainColors[3*((i+1)*terrainGridWidth + j)] * factor + terrainAmbientCol[0],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+1] * factor + terrainAmbientCol[1],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+2] * factor + terrainAmbientCol[2]);
			}
			else if (terrainSimLight  && terrainColors == NULL) {
				factor = terrainComputeLightFactor(i+1,j,startL,startW);
				glColor3f(terrainDiffuseCol[0] * factor + terrainAmbientCol[0],
							terrainDiffuseCol[1] * factor + terrainAmbientCol[1],
							terrainDiffuseCol[2] * factor + terrainAmbientCol[2]);
			}
			else if (terrainColors != NULL) 
				glColor3f(terrainColors[3*((i+1)*terrainGridWidth + j)],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+1],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+2]);
	
			if (terrainNormals != NULL && lighting)
				glNormal3f(terrainNormals[3*((i+1)*terrainGridWidth + j)],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+1],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+2]);	
			
			glTexCoord2f((((startW + j)*terrainStepWidth)/terrainGridWidth)+.5, (((startL - (i+1))*terrainStepLength)/terrainGridLength) + .5);
			//printf("texcoords = %f, %f\n", (((startW + j)*terrainStepWidth)/terrainGridWidth)+.5, (((startL - (i+1))*terrainStepLength)/terrainGridLength) + .5);
			
			glVertex3f(
				(startW + j)*terrainStepWidth,// * stepW,
				terrainHeights[(i+1)*terrainGridWidth + (j)],
				(startL - (i+1))*terrainStepLength);// * stepL);					
			
			if (terrainSimLight && !lighting && terrainColors != NULL) {
				factor = terrainComputeLightFactor(i,j,startL,startW);
				glColor3f(terrainColors[3*(i*terrainGridWidth + j)] * factor + terrainAmbientCol[0],
						  terrainColors[3*(i*terrainGridWidth + j)+1] * factor + terrainAmbientCol[1],
						  terrainColors[3*(i*terrainGridWidth + j)+2] * factor + terrainAmbientCol[2]);
			}
			else if (terrainSimLight && !lighting && terrainColors == NULL) {
				factor = terrainComputeLightFactor(i,j,startL,startW);
				glColor3f(terrainDiffuseCol[0] * factor + terrainAmbientCol[0],
							terrainDiffuseCol[1] * factor + terrainAmbientCol[1],
							terrainDiffuseCol[2] * factor + terrainAmbientCol[2]);
			}
			else if (terrainColors != NULL) 
				glColor3f(terrainColors[3*(i*terrainGridWidth + j)],
						  terrainColors[3*(i*terrainGridWidth + j)+1],
						  terrainColors[3*(i*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL && lighting)
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
		glDisable(GL_TEXTURE_2D);
	}
	glEndList();

	return(terrainDL);
}

float terrainGetHeight(int x, int z) {

	int xt,zt;

	if (terrainHeights == NULL) 
			return(0.0);

	xt = x + terrainGridWidth /2;
	zt = terrainGridLength - (z + terrainGridLength /2);

	if ((xt > terrainGridWidth) || (zt > terrainGridLength) || (xt < 0) || (zt < 0))
		return(0.0);

	return(terrainHeights[zt * terrainGridWidth + xt]);
}


void terrainDestroy() {

	if (terrainHeights != NULL)
		free(terrainHeights);

	if (terrainColors != NULL)
		free(terrainColors);

	if (terrainNormals != NULL)
		free(terrainNormals);
}
