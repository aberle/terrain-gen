//
//  OpenGL TerrainView widget
//
#include <QtOpenGL>
#include "terrainview.h"
#include "terrain.h"
#include <GL/glut.h>

extern float *terrainHeights;

//
//  Constructor
//
TerrainView::TerrainView(QWidget* parent)
    : QGLWidget(parent)
{
   //  Set intial display angle
   theta = 1.5;
   phi   = 0.7;

   //  Initialize color/texture toggle
   texture = false;

   emission  =   0;  // Emission intensity (%)
   ambient   =   5;  // Ambient intensity (%)
   diffuse   =  80;  // Diffuse intensity (%)
   specular  =   0;  // Specular intensity (%)
   shininess =   0;  // Shininess (power of two)
   light     =   1;  // Lighting enabled
   lightPos[0] = 0.0;// Light position
   lightPos[1] = 0.75;
   lightPos[2] = 0.0;
   lightPos[3] = 1.0;
   
   shaders   =   1;  // Shaders on/off
   scale = 0.2;
   usingTexture = 0;

   // noise params
   turbulencePasses = 32;
   octaves = 1.0;
   persistence = 1.0;
   amplitude = 1.0;

   // camera
   dim = 6.0;

   // mouse
   lastX = -999;
   lastY = -999;
   zoom = 0.0;
}

void TerrainView::initTerrain(int turbulencePasses, float octaves, float persistence, float amplitude)
{
   int createNormals = 1;
   if (generateTerrain(createNormals, turbulencePasses, octaves, persistence, amplitude) != TERRAIN_OK)
   {
      printf("Error generating noise-based terrain\n");
      exit(-1);
   }
   terrainScale(0,100);
   terrainDL = terrainCreateDL(light,usingTexture);
}


//
//  Set scale (TerrainView size)
//
void TerrainView::setScale(int new_scale)
{
   //  Set scale
   scale = float(float(new_scale)/200);
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
void TerrainView::reGenTerrain()
{
   initTerrain(turbulencePasses, octaves, persistence, amplitude);
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
   int argc = 1;
   char* argv[2] = {(char*)"tGen", NULL};
   glutInit(&argc, argv);

   //  Enable Z-buffer depth testing
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPolygonOffset(4,0);

   initTerrain(turbulencePasses, 1.0, 1.0, 1.0);

   //  Build shader
   if (!shader.addShaderFromSourceFile(QGLShader::Vertex,"light.vert"))
      exit(-1);
   if (!shader.addShaderFromSourceFile(QGLShader::Fragment,"checkers.frag"))
      exit(-1);
   if (!shader.link())
      exit(-1);
}

//
//  Set projection when window is resized
//
void TerrainView::resizeGL(int width, int height)
{
   //  Window aspect ratio
   float asp= height ? width / (float)height : 1;

   // Field of view
   float fov = 60.0;

   //  Viewport is whole screen
   glViewport(0,0,width,height);

   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (fov)
      gluPerspective(fov,asp,dim/16,16*dim);
   //  Orthogonal transformation
   else
      glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void TerrainView::idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/240000.0;
   double az = fmod(90*t,2880.0);

   //  Set light position
   lightPos[1] = 3*sin(az);
   lightPos[2] = 3*cos(az);

   if (lightPos[1] < 0.0)
   {
      diffuse   =  60;  // Diffuse intensity (%)
   }
   else
   {
      diffuse   =  80;  // Diffuse intensity (%)
   }

   

   updateGL();
}

//
// Slots to set noise params
//
void TerrainView::setTurbulence(QString val)
{
   int value = val.toInt();
   turbulencePasses = value;
}
void TerrainView::setOctaves(QString val)
{
   double value = val.toDouble();
   octaves = value;
}
void TerrainView::setPersistence(QString val)
{
   double value = val.toDouble();
   persistence = value;
}
void TerrainView::setAmplitude(QString val)
{
   double value = val.toDouble();
   amplitude = value;
}

// 
// Mouse stuff
//
void TerrainView::mouseReleaseEvent(QMouseEvent* event)
{
   // Special flag to indicate that the last coordinates should be forgotten
   lastX = -999;
   lastY = -999;
}
void TerrainView::mouseMoveEvent(QMouseEvent* event) 
{
   // Find where the cursor is
   int newX = event->pos().x();
   int newY = event->pos().y();
   
   // The mouse has just been clicked, start from scratch
   if (lastX == -999 && lastY == -999)
   {
      lastX = newX;
      lastY = newY;
   }

   // Move the camera based on how far the mouse has moved
   int deltaX = newX - lastX;
   int deltaY = newY - lastY;
   theta += deltaX * 0.001;
   phi   += deltaY * 0.001;

   // Store the new coordinates
   lastX = newX;
   lastY = newY;
}
void TerrainView::wheelEvent(QWheelEvent* event)
{
   //printf("scroll delta: %d\n", event->delta());
   zoom += event->delta()/1000.0;
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

   //  Eye position
   float eX = -1*dim*sin(theta)*cos(phi);//*zoom;
   float eY = +1*dim           *sin(phi);//*zoom;
   float eZ = +1*dim*cos(theta)*cos(phi);//*zoom;

   // Point the camera
   gluLookAt(eX,eY,eZ,        // eye position vector
             0, 0, 0,         // look at vector
             0, cos(phi), 0); // up vector
   
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
      glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
   }
   else
     glDisable(GL_LIGHTING);

   shaders = 1;

   if (shaders)
   {
      shader.bind();
      int loc = shader.uniformLocation("party");
      if (loc>=0)
      {
        shader.setUniformValue(loc,1);
      }
      else
      {
        printf("failed to share uniform with shader!\n");
      }

      //  Undo transofrmations
      glPopMatrix();

      // Draw the terrain
      glPushMatrix();

      glRotated(theta,0,1,0);
      glRotated(phi,1,0,0);

      glScalef(scale*0.05,scale*0.05,scale*0.05);

      glColor3f(0.00, 0.75, 1.00);
      glCallList(terrainDL);
      glPopMatrix();

      shader.release();
   }

   //  Draw light position as sphere
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(lightPos[0],lightPos[1],lightPos[2]);
   glutSolidSphere(0.02,10,10);
   glPopMatrix();

   //  Emit message to display
   emit message("Theta angle at "+QString::number(theta)+" degrees\nPhi angle at "+QString::number(phi)+" degrees");
}
