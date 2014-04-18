/*
 * Terrain Generation function declarations
 *
 * Based on code from http://www.lighthouse3d.com/opengl/terrain/
 * to use simplex noise values to calculate the terrain heights rather
 * than a TGA image file.
 */

#define TERRAIN_ERROR_INVALID_PARAM		-5
#define TERRAIN_ERROR_LOADING_IMAGE		-4
#define TERRAIN_ERROR_MEMORY_PROBLEM	-3
#define	TERRAIN_ERROR_NOT_SAVED			-2
#define TERRAIN_ERROR_NOT_INITIALISED	-1
#define TERRAIN_OK						 0

int generateTerrain(int normals, int turbulencePasses, float octaves, float persistence, float amplitude);
int terrainCreateDL(int lighting, int texture);
void terrainDestroy();
int terrainScale(float min,float max);
float terrainGetHeight(int x, int z);
int terrainSimulateLighting(int sim);
void terrainLightPosition(float x, float y, float z,float w);
void terrainDiffuseColor(float r, float g, float b);
void terrainAmbientColor(float r, float g, float b);
int terrainDim(float stepWidth, float stepLength);
