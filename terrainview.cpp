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
   theta = 0;
   phi   = 0;

   //  Initialize color/texture toggle
   texture = false;

   // Lighting
   emission  =   0;  // Emission intensity (%)
   ambient   =  30;  // Ambient intensity (%)
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
   
   // Field of view
   fov = 60.0;

   // shadows
   framebuf = 0;
}

void Project(double fov,double asp,double dim)
{
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

void TerrainView::lightingEnabled(int enabled)
{
   //  Enable lighting
   if (enabled)
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
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_NORMALIZE);
   }
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

   initializeShadowMap();
}

void TerrainView::initializeShadowMap()
{
   unsigned int shadowtex; //  Shadow buffer texture id
   int n;

   //  Make sure multi-textures are supported
   glGetIntegerv(GL_MAX_TEXTURE_UNITS,&n);
   if (n<2) 
   {
      fprintf(stderr, "Multiple textures not supported\n");
      exit(-1);
   }

   //  Get maximum texture buffer size
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&shadowdim);

   //  Limit texture size to maximum buffer size
   glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&n);
   if (shadowdim > n) 
      shadowdim = n;

   //  Limit texture size to 2048 for performance
   if (shadowdim>2048) 
      shadowdim = 2048;
   if (shadowdim<512) 
   {
      fprintf(stderr, "Shadow map dimensions too small: %d\n",shadowdim);
      exit(-1);
   }

   //  Do Shadow textures in MultiTexture 1
   glActiveTexture(GL_TEXTURE1);

   //  Allocate and bind shadow texture
   glGenTextures(1,&shadowtex);
   glBindTexture(GL_TEXTURE_2D,shadowtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowdim, shadowdim, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   //  Map single depth value to RGBA (this is called intensity)
   glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);

   //  Set texture mapping to clamp and linear interpolation
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   //  Set automatic texture generation mode to Eye Linear
   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

   // Switch back to default textures
   glActiveTexture(GL_TEXTURE0);

   // Attach shadow texture to frame buffer
   glGenFramebuffers(1,&framebuf);
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowtex, 0);
   //  Don't write or read to visible color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   //  Make sure this all worked
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   {
      fprintf(stderr, "Error setting up frame buffer\n");
      exit(-1);
   }
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   int err = glGetError();
   if (err) 
      fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),"initializeShadowMap");

   //  Create shadow map
   calculateShadowMap();
}

void TerrainView::calculateShadowMap()
{
   double Lmodel[16];  //  Light modelview matrix
   double Lproj[16];   //  Light projection matrix
   double Tproj[16];   //  Texture projection matrix
   double Ldist;       //  Distance from light to scene center

   //  Save transforms and modes
   glPushMatrix();
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  No write to color buffer and no smoothing
   glShadeModel(GL_FLAT);
   glColorMask(0,0,0,0);
   // Overcome imprecision
   glEnable(GL_POLYGON_OFFSET_FILL);

   //  Turn off lighting
   lightingEnabled(0);

   //  Light distance
   Ldist = sqrt(lightPos[0]*lightPos[0] + lightPos[1]*lightPos[1] + lightPos[2]*lightPos[2]);
   if (Ldist<1.1*dim) Ldist = 1.1*dim;

   //  Set perspective view from light position
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(114.6*atan(dim/Ldist),1,Ldist-dim,Ldist+dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(lightPos[0],lightPos[1],lightPos[2] , 0,0,0 , 0,1,0);
   //  Size viewport to desired dimensions
   glViewport(0,0,shadowdim,shadowdim);

   // Redirect traffic to the frame buffer
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);

   // Clear the depth buffer
   glClear(GL_DEPTH_BUFFER_BIT);
   // Draw all objects that can cast a shadow
   scene(0);

   //  Retrieve light projection and modelview matrices
   glGetDoublev(GL_PROJECTION_MATRIX,Lproj);
   glGetDoublev(GL_MODELVIEW_MATRIX,Lmodel);

   // Set up texture matrix for shadow map projection,
   // which will be rolled into the eye linear
   // texture coordinate generation plane equations
   glLoadIdentity();
   glTranslated(0.5,0.5,0.5);
   glScaled(0.5,0.5,0.5);
   glMultMatrixd(Lproj);
   glMultMatrixd(Lmodel);

   // Retrieve result and transpose to get the s, t, r, and q rows for plane equations
   glGetDoublev(GL_MODELVIEW_MATRIX,Tproj);
   Svec[0] = Tproj[0];    Tvec[0] = Tproj[1];    Rvec[0] = Tproj[2];    Qvec[0] = Tproj[3];
   Svec[1] = Tproj[4];    Tvec[1] = Tproj[5];    Rvec[1] = Tproj[6];    Qvec[1] = Tproj[7];
   Svec[2] = Tproj[8];    Tvec[2] = Tproj[9];    Rvec[2] = Tproj[10];   Qvec[2] = Tproj[11];
   Svec[3] = Tproj[12];   Tvec[3] = Tproj[13];   Rvec[3] = Tproj[14];   Qvec[3] = Tproj[15];

   // Restore normal drawing state
   glShadeModel(GL_SMOOTH);
   glColorMask(1,1,1,1);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopAttrib();
   glPopMatrix();
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   int err = glGetError();
   if (err) 
      fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),"calculateShadowMap");
}

void TerrainView::scene(int light)
{
   lightingEnabled(light);

   // Draw terrain
   glPushMatrix();

   glScalef(scale*0.05,scale*0.05,scale*0.05);

   glColor3f(0.00, 0.75, 1.00);
   glCallList(terrainDL);
   glPopMatrix();
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

   initializeGLFunctions();

   //  Enable Z-buffer depth testing
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPolygonOffset(4,0);

   initTerrain(turbulencePasses, 1.0, 1.0, 1.0);

   //  Build shader
   if (!shader.addShaderFromSourceFile(QGLShader::Vertex,"shadow.vert"))
      exit(-1);
   if (!shader.addShaderFromSourceFile(QGLShader::Fragment,"shadow.frag"))
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
   asp= height ? width / (float)height : 1;

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
   double t = glutGet(GLUT_ELAPSED_TIME)/50000.0;
   double az = fmod(90*t,1440.0);

   //  Set light position
   lightPos[0] = 0.5*cos(az);
   lightPos[2] = 0.5*sin(az);

   calculateShadowMap();

   updateGL();
}

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

void TerrainView::mouseMoveEvent(QMouseEvent* event) 
{
   static int lastX = 0;
   static int lastY = 0;

   int newX = event->pos().x();
   int newY = event->pos().y();

   int deltaX = newX - lastX;
   int deltaY = newY - lastY;

   theta += deltaX*.001;
   phi += deltaY*.001;

   //printf("delta x: %d\n", deltaX);
   //printf("delta y: %d\n\n", deltaY);

   lastX = newX;
   lastY = newY;
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
   //glLoadIdentity();

   //  Disable lighting
   glDisable(GL_LIGHTING);

   //  Perspective projection
   Project(60,asp,dim);

   //  Eye position
   float eX = -1*dim*sin(theta)*cos(phi);
   float eY = +1*dim           *sin(phi);
   float eZ = +1*dim*cos(theta)*cos(phi);

   gluLookAt(eX,eY,eZ, // eye vector
             0, 0, 0,  // look at vector
             0, 1, 0); // up vector
   
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(0,0,0); //x, y, z

   // Enable shader program
   shader.bind();

   int id = shader.uniformLocation("tex");
   if (id>=0) glUniform1i(id,0);
   id = shader.uniformLocation("depth");
   if (id>=0) glUniform1i(id,1);

   // Set up the eye plane for projecting the shadow map on the scene
   glActiveTexture(GL_TEXTURE1);
   glTexGendv(GL_S,GL_EYE_PLANE,Svec);
   glTexGendv(GL_T,GL_EYE_PLANE,Tvec);
   glTexGendv(GL_R,GL_EYE_PLANE,Rvec);
   glTexGendv(GL_Q,GL_EYE_PLANE,Qvec);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
   glActiveTexture(GL_TEXTURE0);

   shader.release();

   // Undo transofrmations
   glPopMatrix();

   // Draw the terrain with lighting
   scene(1);

   //  Draw light position as sphere
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(lightPos[0],lightPos[1],lightPos[2]);
   glutSolidSphere(0.02,10,10);
   glPopMatrix();

   int n;
      //  Orthogonal view (right half)
      Project(0,asp,1);

      //  Disable any manipulation of textures
      glActiveTexture(GL_TEXTURE1);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
      //  Display texture by drawing quad
      glEnable(GL_TEXTURE_2D);
      glColor3f(1,1,1);
      glBegin(GL_QUADS);
      glMultiTexCoord2f(GL_TEXTURE1,0,0);glVertex2f(-1,-1);
      glMultiTexCoord2f(GL_TEXTURE1,1,0);glVertex2f(+1,-1);
      glMultiTexCoord2f(GL_TEXTURE1,1,1);glVertex2f(+1,+1);
      glMultiTexCoord2f(GL_TEXTURE1,0,1);glVertex2f(-1,+1);
      glEnd();
      glDisable(GL_TEXTURE_2D);

   //  Emit message to display
   emit message("Theta angle at "+QString::number(theta)+" degrees\nPhi angle at "+QString::number(phi)+" degrees");
}
