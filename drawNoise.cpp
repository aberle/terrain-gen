//
//  OpenGL DrawNoise widget
//
#include <QtOpenGL>
#include <GL/glut.h>
#include "drawNoise.h"
#include "terrain.h"

extern float* terrainHeights;

//
//  Constructor
//
DrawNoise::DrawNoise(QWidget* parent)
    : QGLWidget(parent)
{
   
   /*for (int i = 0; i < NOISE_WIDTH; ++i)
   {
      for (int j = 0; j < NOISE_HEIGHT; ++j)
      {
         noiseData[i*NOISE_WIDTH + j] = (float)i;
         
      }
   }*/

}

//
// Initialize
//
void DrawNoise::initializeGL()
{
   if (!shader.addShaderFromSourceFile(QGLShader::Fragment,"drawNoise.frag"))
      exit(-1);
   if (!shader.link())
      exit(-1);
}

void DrawNoise::resizeGL(int width, int height)
{
   //nothing
}

void DrawNoise::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   glColor3f(1.0, 0.0, 0.0);

   shader.bind();

   shader.setUniformValueArray("noise",noiseData,NOISE_WIDTH,1);

   glBegin(GL_QUADS);                      // Draw A Quad
     glVertex3f(-1.0f, 1.0f, 0.0f);              // Top Left
     glVertex3f( 1.0f, 1.0f, 0.0f);              // Top Right
     glVertex3f( 1.0f,-1.0f, 0.0f);              // Bottom Right
     glVertex3f(-1.0f,-1.0f, 0.0f);              // Bottom Left
   glEnd();

   shader.release();
}

void DrawNoise::reDraw(float* values)
{
   //nothing
}