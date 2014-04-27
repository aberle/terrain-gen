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
   // Set intial display angle
   theta = -1.5;
   phi   =  0.7;

   // Initialize color/texture toggle
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
   moon = 0;
   
   scale = 0.2;
   usingTexture = 0;

   // noise params
   turbulencePasses = 32;
   octaves = 1.0;
   persistence = 1.0;
   amplitude = 1.0;

   // camera
   dim = 6.0;
   fov = 60.0; // field of view

   // mouse
   lastX = -999;
   lastY = -999;
   zoom = 1.0;
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
// Set scale (TerrainView size)
//
void TerrainView::setScale(int new_scale)
{
   // Set scale
   scale = float(float(new_scale)/200);
   // Request redisplay
   updateGL();
}

//
// Set theta (horizontal rotation angle)
//
void TerrainView::setTheta(int angle)
{
   //  Set angle
   theta = angle;
   //  Request redisplay
   updateGL();
}

//
// Set phi (vertical rotation angle)
//
void TerrainView::setPhi(int angle)
{
   //  Set angle
   phi = angle;
   //  Request redisplay
   updateGL();
}

//
// Turn lights on/off
//
void TerrainView::reGenTerrain()
{
   initTerrain(turbulencePasses, octaves, persistence, amplitude);
   updateGL();
}

//
// Initialize
//
void TerrainView::initializeGL()
{
   int argc = 1;
   char* argv[2] = {(char*)"tGen", NULL};
   glutInit(&argc, argv);

   // Enable Z-buffer depth testing
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPolygonOffset(4,0);

   initTerrain(turbulencePasses, 1.0, 1.0, 1.0);

   // Build shaders
   if (!shader.addShaderFromSourceFile(QGLShader::Vertex,"light.vert"))
      exit(-1);
   if (!shader.addShaderFromSourceFile(QGLShader::Fragment,"terrain_texture.frag"))
      exit(-1);
   if (!shader.link())
      exit(-1);

   if (!shader2.addShaderFromSourceFile(QGLShader::Vertex,"light.vert"))
      exit(-1);
   if (!shader2.addShaderFromSourceFile(QGLShader::Fragment,"water.frag"))
      exit(-1);
   if (!shader2.link())
      exit(-1);

   // Set skybox color
   skyColor[0] = 0.0;
   skyColor[1] = 0.50;
   skyColor[2] = 0.75;

   // Load terrain textures
   glActiveTexture(GL_TEXTURE0);
   glEnable(GL_TEXTURE_2D);
   QPixmap img0("rock.jpg");
   rock_texture = bindTexture(img0,GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, rock_texture);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glEnable(GL_TEXTURE_2D);
   QPixmap img1("grass.jpg");
   grass_texture = bindTexture(img1,GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, grass_texture);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE2);
   glEnable(GL_TEXTURE_2D);
   QPixmap img2("snow.jpg");
   snow_texture = bindTexture(img2,GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, snow_texture);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glDisable(GL_TEXTURE_2D);

   setCamera();
}

//
// Camera settings
// 
void TerrainView::setCamera()
{
   // Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   // Undo previous transformations
   glLoadIdentity();
   // Perspective transformation
   if (fov)
      gluPerspective(fov,asp,dim/16,16*dim);
   // Orthogonal transformation
   else
      glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
   // Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   // Undo previous transformations
   glLoadIdentity();
}

//
//  Set projection when window is resized
//
void TerrainView::resizeGL(int width, int height)
{
   // Window aspect ratio
   asp = height ? width / (float)height : 1; 
   
   // Viewport is whole screen
   glViewport(0,0,width,height);

   // Perspective camera
   setCamera();
}

void TerrainView::idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000000.0;
   double az = fmod(90*t,2880.0);

   //  Set light position
   lightPos[1] = fabs(3*sin(az));
   lightPos[2] = 3*cos(az);

   // Alternate between sun and moon
   if (3*sin(az) < 0.0)
   {
      moon = 1;
      lightPos[2] = -3*cos(az);

      // Sky color isn't dynamic at night
      skyColor[0] = 0.094;
      skyColor[1] = 0.188;
      skyColor[2] = 0.376;
   }
   else // Daytime: skybox is redder at sunrise/set, blue at midday
   {
      moon = 0;
      skyColor[0] = fabs(cos(az)) - 0.10;
      skyColor[1] = (1.5 - fabs(cos(az))) * 0.40;
      skyColor[2] = 1.0 - fabs(cos(az));
   }

   // Dim sky color based on light position
   skyColor[0] *= 1.0 - fabs(lightPos[2]/3);
   skyColor[1] *= 1.0 - fabs(lightPos[2]/3);
   skyColor[2] *= 1.0 - fabs(lightPos[2]/3);

   // Redraw the scene
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
   zoom  = (float)event->delta()/100.0;
   fov  -= zoom;
   
   if (fov > 180.0)
   {
      fov = 180.0;
   }
   if (fov < 0.0)
   {
      fov = 0.0;
   }

   setCamera();
   updateGL();
}

// 
//  Draw skybox
//
void TerrainView::skyBox(double D)
{
   glColor3f(skyColor[0],skyColor[1],skyColor[2]);

   //  Sides
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();
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
   float eX = -1*dim*sin(theta)*cos(phi);// - sin(zoom);
   float eY = +1*dim           *sin(phi);// - zoom;
   float eZ = +1*dim*cos(theta)*cos(phi);// + sin(zoom);

   // Point the camera
   gluLookAt(eX,eY,eZ,        // eye position vector
             0, 0, 0,         // look at vector
             0, cos(phi), 0); // up vector
   
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT,GL_EMISSION,black);
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
      glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
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

   shader.bind();
   int loc = shader.uniformLocation("moon");
   if (loc >= 0)
   {
     shader.setUniformValue(loc, moon);
   }
   else
   {
     printf("failed to share moon with shader!\n");
   }

   loc = shader.uniformLocation("lightFactor");
   if (loc >= 0)
   {
      shader.setUniformValue(loc, lightPos[1]);
   }
   else
   {
     printf("failed to share lightFactor with shader!\n");
   }

   loc = shader.uniformLocation("rock_texture");
   if (loc >= 0)
   {
      shader.setUniformValue(loc, 0);
   }
   else
   {
      printf("failed to share rock_texture with shader!\n");
   }
   loc = shader.uniformLocation("grass_texture");
   if (loc >= 0)
   {
      shader.setUniformValue(loc, 1);
   }
   else
   {
      printf("failed to share grass_texture with shader!\n");
   }
   loc = shader.uniformLocation("snow_texture");
   if (loc >= 0)
   {
      shader.setUniformValue(loc, 2);
   }
   else
   {
      printf("failed to share snow_texture with shader!\n");
   }

   //  Undo transofrmations
   glPopMatrix();

   // Draw the terrain
   glPushMatrix();
   glScalef(0.01, 0.01, 0.01);
   glCallList(terrainDL);
   glPopMatrix();

   shader.release();

   // Water
   shader2.bind();

   loc = shader2.uniformLocation("moon");
   if (loc >= 0)
   {
     shader2.setUniformValue(loc, moon);
   }
   else
   {
     printf("failed to share moon with shader2!\n");
   }
   loc = shader2.uniformLocation("lightFactor");
   if (loc >= 0)
   {
      shader2.setUniformValue(loc, lightPos[1]);
   }
   else
   {
     printf("failed to share lightFactor with shader!\n");
   }

   // Enable blending to make water slightly transparent
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // Set water light parameters (crank up specular light to make it shiny)
   float waterSpecular[] = {0.01*70, 0.01*70, 0.01*70, 1.0};
   float waterDiffuse[]  = {0.01*50, 0.01*50, 0.01*50, 1.0};
   float waterAmbient[]  = {0.01*20, 0.01*20, 0.01*20, 1.0};

   glLightfv(GL_LIGHT0, GL_SPECULAR, waterSpecular);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, waterDiffuse);
   glLightfv(GL_LIGHT0, GL_AMBIENT, waterAmbient);
      
   glPushMatrix();
   glVertex3f(0.0, 0.25, 0.80);
   glBegin(GL_QUADS);
      
      // Split the water plane into 4 parts for better lighting 
      glVertex3f(-2.55,0.35,-2.55);
      glVertex3f(+0.0,0.35,-2.55);
      glVertex3f(+0.0,0.35,+0.0);
      glVertex3f(-2.55,0.35,+0.0);

      glVertex3f(-0.0,0.35,-2.55);
      glVertex3f(+2.55,0.35,-2.55);
      glVertex3f(+2.55,0.35,+0.0);
      glVertex3f(-0.0,0.35,+0.0);

      glVertex3f(-0.0,0.35,-0.0);
      glVertex3f(+2.55,0.35,-0.0);
      glVertex3f(+2.55,0.35,+2.55);
      glVertex3f(-0.0,0.35,+2.55);

      glVertex3f(-2.55,0.35,-0.0);
      glVertex3f(+0.0,0.35,-0.0);
      glVertex3f(+0.0,0.35,+2.55);
      glVertex3f(-2.55,0.35,+2.55);
   glEnd();
   glPopMatrix();

   shader2.release();

   // Draw light position as sphere
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(lightPos[0],lightPos[1],lightPos[2]);
   glutSolidSphere(0.02,10,10);
   glPopMatrix();

   // Skybox
   skyBox(dim);

   // Emit message to display
   emit message("Theta angle at "+QString::number(theta)+" degrees\nPhi angle at "+QString::number(phi)+" degrees");
}
