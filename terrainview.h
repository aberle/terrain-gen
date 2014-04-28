//
//  OpenGL Terrain View Widget
//

#ifndef TERRAIN_GEN_H
#define TERRAIN_GEN_H

#include <QGLWidget>
#include <QString>
#include <QGLShaderProgram>
#include <QGLFunctions>
#include <iostream>

#include "simplexnoise.h"

class TerrainView : public QGLWidget
{
Q_OBJECT
//  Private data
private:
    // Shaders
    QGLShaderProgram shader;  // Terrain shader
    QGLShaderProgram shader2; // Water shader
    QGLShaderProgram shader3; // Clouds shader

    // Camera
    float  theta;       // Horizontal rotation angle
    float  phi;         // Vertical rotation angle
    float fov;          // Field of view (for perspective)
    double asp;         // Aspect ratio
    double dim;         // Size of world

    // Lighting
    float lightPos[4];
    int emission;       // Emission intensity (%)
    int ambient;        // Ambient intensity (%)
    int diffuse;        // Diffuse intensity (%)
    int specular;       // Specular intensity (%)
    int shininess;      // Shininess (power of two)
    float shinyvec[1];  // Shininess (value)

    // environment
    int moon;
    int timeScale;
    float waterHeight;

    // noise
    int turbulencePasses;
    double octaves;
    double persistence;
    double amplitude;

    // mouse movement
    int lastX;
    int lastY;
    float zoom;

    // skybox color
    float skyColor[3];

    // terrain
    int rock_texture;   // textures
    int grass_texture;
    int snow_texture;
    int sand_texture;
    int terrainDL;      // Terrain call list
    float scale;        // Size of terrain
    float blend;        // Texture blending amount
    float sand_low;     // Lower bound of sand texture
    float sand_high;    // Upper bound of sand texture

    // clouds
    float cloud_textures[2];
    float *cloudVerts;
    float cloudOpacity;
    int cloudDensity;

//  Public access to this widget
public:
    TerrainView(QWidget* parent=0);                      //  Constructor
    QSize sizeHint() const {return QSize(800,800);}   //  Default size of widget

//  Communications with other widgets
public slots:
    void setTheta(int angle);          // Slot to set horizontal angle
    void setPhi(int angle);            // Slot to set vertical angle
    void reGenTerrain();               // Slot to regenerate terrain
    void idle();                       // Slot for idle function
    void setTurbulence(QString val);   // Slot for setting turbulence factor
    void setOctaves(QString val);      // Slot for setting octaves
    void setPersistence(QString val);  // Slot for setting persistence
    void setAmplitude(QString val);    // Slot for setting amplitude
    void setTime(int new_time);        // Slot for changing time scale
    void initTerrain(int turbulencePasses, float octaves, float persistence, float amplitude); // Create terrain using noise
    void skyBox(double D);             // Draw skybox
    void setCamera();                  // Camera settings
    void initClouds();                 // Initialize cloud particle locations
    void drawClouds();                 // Draw cloud particles
    void setCloudOpacity(int opacity); // Set cloud alpha
    void setCloudDensity(int density); // Set number of clouds
    void setWaterHeight(int height);   // Set height of water plane
    void setBlend(int new_blend);      // Set texture blending amount for terrain
    void setSandLowerBound(int new_val);// Set lower bound of sand texture
    void setSandUpperBound(int new_val);// Set upper bound of sand texture

//  Need to reimplement these methods inhereted from QGLWidget
protected:
    void initializeGL();                       // Initialize widget
    void resizeGL(int width, int height);      // Resize widget
    void paintGL();                            // Draw widget
    void mouseMoveEvent(QMouseEvent* event);   // Capture mouse click and movement
    void mouseReleaseEvent(QMouseEvent* event);// Capture button release
    void wheelEvent(QWheelEvent* event);       // Capture scroll wheel movements
};

#endif
