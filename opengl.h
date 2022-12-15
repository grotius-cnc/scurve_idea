#ifndef OPENGL_H
#define OPENGL_H

#include <QMainWindow>
#include <mainwindow.h>
#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QToolTip>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QtWidgets>

class opengl : public QOpenGLWidget{

public:
    opengl(QWidget *parent = nullptr);
    ~opengl();

    void reset();
    void setMaxValues(double theMaxVelocity, double theMaxAcceleration);
    void setValues(double theVelocity, double theAcceleration);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
};

#endif
