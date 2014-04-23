//
//  OpenGL Terrain View Widget
//

#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include <QGLWidget>
#include <QString>
#include <QGLShaderProgram>
#include <iostream>
#include <QMatrix4x4>
#include <QGLFunctions>

class TerrainView : public QGLWidget, protected QGLFunctions
{
Q_OBJECT
//  Private data
private:
    float  theta;       //  Horizontal rotation angle
    float  phi;         //  Vertical rotation angle
    bool texture;       //  Color/Texture toggle
    int emission;       //  Emission intensity (%)
    int ambient;        //  Ambient intensity (%)
    int diffuse;        //  Diffuse intensity (%)
    int specular;       //  Specular intensity (%)
    int shininess;      //  Shininess (power of two)
    float shinyvec[1];  //  Shininess (value)
    int th;             //  Azimuth of view angle
    int ph;             //  Elevation of view angle
    int light;          //  Lighting
    double asp;         //  Aspect ratio
    double dim;         //  Size of world
    int party;          //  Party mode on/off!
    QGLShaderProgram shader;  //  Shader
    int shaders;        //  Shaders on/off
    float scale;        //  Size of terrain

    // Terrain
    int terrainDL;
    int usingTexture;
    int usingMap;

    //  Lighting
    float lightPos[4];

    // Noise
    int turbulencePasses;
    double octaves;
    double persistence;
    double amplitude;

    // Camera
    float fov;

    // Shadows
    int shadowdim;          // Dimensions of shadowmap
    unsigned int framebuf;  // Frame buffer
    double       Svec[4];   // Texture planes S
    double       Tvec[4];   // Texture planes T
    double       Rvec[4];   // Texture planes R
    double       Qvec[4];   // Texture planes Q

//  Public access to this widget
public:
    TerrainView(QWidget* parent=0);                      //  Constructor
    QSize sizeHint() const {return QSize(1920,1080);}   //  Default size of widget

//  Communications with other widgets
public slots:
    void setTheta(int angle);          //  Slot to set horizontal angle
    void setPhi(int angle);            //  Slot to set vertical angle
    void reGenTerrain();               //  Slot to regenerate terrain
    void idle();                       //  Slot for idle function
    void setTurbulence(QString val);   //  Slot for setting turbulence factor
    void setOctaves(QString val);      //  Slot for setting octaves
    void setPersistence(QString val);  //  Slot for setting persistence
    void setAmplitude(QString val);    //  Slot for setting amplitude
    void toggleShaders();              //  Slot for toggling shaders
    void setScale(int new_scale);      //  Slot for changing terrain size
    void initTerrain(int turbulencePasses, float octaves, float persistence, float amplitude);
    void initializeShadowMap();        //  Set up calculation of shadow map
    void calculateShadowMap();         //  Compute shadow map
    void lightingEnabled(int enabled); //  Turn lighting on/off
    void scene(int light);             //  Draw scene with lighting on or off
signals:
    void message(QString text);    //  Signal for messages
    void multMessage(QString text);

//  Need to reimplement these methods inhereted from QGLWidget
protected:
    void initializeGL();                   //  Initialize widget
    void resizeGL(int width, int height);  //  Resize widget
    void paintGL();                        //  Draw widget
    void mouseMoveEvent(QMouseEvent* event); // Capture mouse movement
};

#endif
