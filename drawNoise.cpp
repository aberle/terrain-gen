//
//  OpenGL DrawNoise widget
//
#include <QtOpenGL>
#include <GL/glut.h>
#include "drawNoise.h"

extern float* terrainHeights;

//
//  Constructor
//
DrawNoise::DrawNoise(QWidget* parent)
    : QGLWidget(parent)
{
   
   // Nothing
}

//
// Initialize
//
void DrawNoise::initializeGL()
{
   // Nothing
}

void DrawNoise::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, NOISE_HEIGHT, 0, NOISE_WIDTH, 0, 1);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glEnable(GL_TEXTURE_2D);

   //  Load terrain noise image
   QPixmap img("terrain.bmp");
   int tex = bindTexture(img,GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, tex);

   glColor3f(1, 1, 1);
   glBegin(GL_QUADS); 

   glTexCoord2f(1,1); glVertex3f(NOISE_WIDTH, NOISE_HEIGHT, 0.0f); 
   glTexCoord2f(1,0); glVertex3f(NOISE_WIDTH, 0.0f,         0.0f); 
   glTexCoord2f(0,0); glVertex3f(0.0f,        0.0f,         0.0f); 
   glTexCoord2f(0,1); glVertex3f(0.0f,        NOISE_HEIGHT, 0.0f); 
   
   glEnd();
   glDisable(GL_TEXTURE_2D);

}

void DrawNoise::reDraw()
{
   updateGL();
}