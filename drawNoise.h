//
//  OpenGL Noise Draw Widget
//

#ifndef DRAWNOISE_H
#define DRAWNOISE_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include "simplexnoise.h"

class DrawNoise : public QGLWidget
{
Q_OBJECT
// Private data
private:
    QGLShaderProgram shader;  //  Shader

// Public access to this widget
public:
    DrawNoise(QWidget* parent=0);                     // Constructor
    QSize sizeHint() const {return QSize(NOISE_HEIGHT,NOISE_WIDTH);}   // Default size of widget

// Communications with other widgets
public slots:
    void reDraw();

// Need to reimplement these methods inhereted from QGLWidget
protected:
    void initializeGL();                     // Initialize widget
    void paintGL();                          // Draw widget
};

#endif
