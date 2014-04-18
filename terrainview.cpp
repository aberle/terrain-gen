//
//  OpenGL TerrainView widget
//
#include <QtOpenGL>
#include "terrainview.h"
#include "terrain.h"

extern float *terrainHeights;

//
//  Constructor
//
TerrainView::TerrainView(QWidget* parent)
    : QGLWidget(parent)
{
   //  Set intial display angle
   theta = 0;
   phi   = 0;

   //  Initialize color/texture toggle
   texture = false;

   emission  =   0;  // Emission intensity (%)
   ambient   =  30;  // Ambient intensity (%)
   diffuse   = 100;  // Diffuse intensity (%)
   specular  =   0;  // Specular intensity (%)
   shininess =   0;  // Shininess (power of two)
   light     =   1;  // Lighting enabled
   shaders   =   1;  // Shaders on/off

   scale = 0.10;

   usingTexture = 0;
}

void TerrainView::initTerrain()
{
   int createNormals = 1;
   if (generateTerrain(createNormals) != TERRAIN_OK)
   {
      printf("Error generating noise-based terrain\n");
      exit(-1);
   }
   terrainScale(0,105);
   terrainDL = terrainCreateDL(light,usingTexture);
}


//
//  Set scale (TerrainView size)
//
void TerrainView::setScale(int new_scale)
{
   //  Set scale
   scale = float(float(new_scale)/100);
   //  Request redisplay
   updateGL();
}

//
//  Set theta (horizontal rotation angle)
//
void TerrainView::setTheta(int angle)
{
   //  Set angle
   theta = angle;
   //  Request redisplay
   updateGL();
}

//
//  Set phi (vertical rotation angle)
//
void TerrainView::setPhi(int angle)
{
   //  Set angle
   phi = angle;
   //  Request redisplay
   updateGL();
}

//
//  Turn lights on/off
//
void TerrainView::toggleLights()
{
   //  Toggle lights
   light = 1-light;
   //  Request redisplay
   updateGL();
}

//
//  Turn party mode on/off
//
void TerrainView::partyMode()
{
   party = 1-party;
   //  Request redisplay
   updateGL();
}

//
//  Turn shaders on/off
//
void TerrainView::toggleShaders()
{
   shaders = 1-shaders;
   //  Request redisplay
   updateGL();
}

//
//  Initialize
//
void TerrainView::initializeGL()
{
   //  Enable Z-buffer depth testing
   glEnable(GL_DEPTH_TEST);
   initTerrain();

   //  Build shader
   /*if (!shader.addShaderFromSourceFile(QGLShader::Vertex,"light.vert"))
      exit(-1);
   if (!shader.addShaderFromSourceFile(QGLShader::Fragment,"checkers.frag"))
      exit(-1);
   if (!shader.link())
      exit(-1);*/
}

//
//  Set projection when window is resized
//
void TerrainView::resizeGL(int width, int height)
{
   //  Window aspect ratio
   float asp= height ? width / (float)height : 1;

   //  Viewport is whole screen
   glViewport(0,0,width,height);

   //  Orthogonal projection to 2
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-2*asp, +2*asp, -2, +2, -2, +2);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
}

void TerrainView::idle()
{
   //updateGL();
}

void TerrainView::multiply(QString val)
{
   emit multMessage(QString::number(val.toInt()*2));
}

//
//  Draw the window
//
void TerrainView::paintGL()
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);

   //  Reset transformations
   glLoadIdentity();
   
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(0,0,0); //x, y, z

   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {0,2,0,1.0}; // or 5*cos, 5*sin
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1); // or 0
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
     glDisable(GL_LIGHTING);
   
   // Orthogonal projection
   glScaled(1,1,1);
   glRotated(theta,0,1,0);
   glRotated(phi,1,0,0);

   shaders = 0;

   /*
   if (shaders)
   {
      shader.bind();
      int loc = shader.uniformLocation("party");
      if (loc>=0)
      {
        shader.setUniformValue(loc,party);
      }
      else
      {
        printf("failed to share rando with shader!");
      }
      //  TerrainView
      glBegin(GL_QUADS);
      //  End
      glEnd();
      shader.release();
   }*/

   //  Undo transofrmations
   glPopMatrix();

   // Draw the terrain
   glPushMatrix();

   glRotated(theta,0,1,0);
   glRotated(phi,1,0,0);

   glScalef(scale*0.1,scale*0.1,scale*0.1);

   glColor3f(0.00, 0.75, 1.00);
   glCallList(terrainDL);
   glPopMatrix();

   //  Emit message to display
   emit message("Theta angle at "+QString::number(theta)+" degrees\nPhi angle at "+QString::number(phi)+" degrees");
}
