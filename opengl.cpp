#include <math.h>
#include <mainwindow.h>
#include <iostream>
#include <opengl.h>
#include <vector>

double velocity=0;
double max_velocity=0;
double acceleration=0;
double max_acceleration=0;
std::vector<int> velvec;
std::vector<int> accvec;

opengl::opengl(QWidget *parent)
    : QOpenGLWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
}

opengl::~opengl()
{
    //! destructor
}

void opengl::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_STIPPLE);
    setMouseTracking(true);
}

void opengl::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void opengl::reset(){
    velvec.clear();
    accvec.clear();
}

void opengl::paintGL()
{
    glViewport(0, 0, this->width(), this->height());
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(), 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double vel_ratio=(this->height()-10)/max_velocity;
    double acc_ratio=(this->height()-10)/max_acceleration;

    velvec.insert(velvec.begin(),this->height()-(velocity*vel_ratio)-5);
    if(velvec.size()>uint(this->width())){
        velvec.pop_back();
    }

    accvec.insert(accvec.begin(),this->height()-((acceleration*acc_ratio)/2));
    if(accvec.size()>uint(this->width())){
        accvec.pop_back();
    }

    glColor4d(255,255,0,255);
    glBegin(GL_LINE_STRIP);
    for(uint i=0; i<velvec.size(); i++){
        glVertex2d(this->width()-i,velvec.at(i));
    }
    glEnd();

    glColor4d(0,255,0,255);
    glBegin(GL_LINE_STRIP);
    for(uint i=0; i<accvec.size(); i++){
        glVertex2d(this->width()-i,accvec.at(i)-(0.5*this->height()));
    }
    glEnd();
}

void opengl::setMaxValues(double theMaxVelocity, double theMaxAcceleration){
    max_velocity=theMaxVelocity;
    max_acceleration=theMaxAcceleration;
}
void opengl::setValues(double theVelocity, double theAcceleration){
    velocity=theVelocity;
    acceleration=theAcceleration;
}











