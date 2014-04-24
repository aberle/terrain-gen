//
//  OpenGL Noise Draw Widget
//

#ifndef DRAWNOISE_H
#define DRAWNOISE_H

#include <QGLWidget>
#include <QDialog>
#include <QString>
#include <QGLShaderProgram>
#include <iostream>
#include "simplexnoise.h"

class DrawNoise : public QGLWidget
{
Q_OBJECT
// Private data
private:
    QGLShaderProgram shader;  //  Shader
    float noiseData[NOISE_WIDTH*NOISE_HEIGHT];

// Public access to this widget
public:
    DrawNoise(QWidget* parent=0);                     // Constructor
    QSize sizeHint() const {return QSize(200,200);}   // Default size of widget

// Communications with other widgets
public slots:
    void reDraw(float* values);

// Need to reimplement these methods inhereted from QGLWidget
protected:
    void initializeGL();                     // Initialize widget
    void resizeGL(int width, int height);    // Resize widget
    void paintGL();                          // Draw widget
};

#endif
