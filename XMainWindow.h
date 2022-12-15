#ifndef XMAINWINDOW_H
#define XMAINWINDOW_H

/*
        Copyright (c) 2022 Skynet Cyberdyne

        This software is provided 'as-is', without any express or implied
        warranty. In no event will the authors be held liable for any damages
        arising from the use of this software.

        Permission is granted to anyone to use this software for any purpose,
        excluding commercial applications, and to alter it and redistribute it
        freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
           claim that you wrote the original software. If you use this software
           in a product, an acknowledgment in the product documentation would
           be appreciated but is not required.

        2. Altered source versions must be plainly marked as such, and must not
           be misrepresented as being the original software.

        3. This notice may not be removed or altered from any source
           distribution.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <XWidget.h>
#include <XImage.h>
#include <XWindow.h>
#include <XMouse.h>
#include <XKey.h>
#include <XFrame.h>
#include <XScissor.h>
#include <XOpenGLArc.h>
#include <XOpenGLCircle.h>
#include <XOpenGLLine.h>
#include <XOpenGLPoint.h>
#include <XOpenGLSpline.h>
#include <XRectangular.h>

#include <curve.h>
#include <pid.h>

class XMainWindow : public XWidget {
public:
    XMainWindow(){ }
    XMainWindow(std::string theDialogName, uint theStartupWidth, uint theStartupHeight):myDialogName(theDialogName),myWidth(theStartupWidth),myHeight(theStartupHeight){ }
    ~XMainWindow(){}

    void run(){
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw window creation
        // --------------------
        window=glfwCreateWindow(myWidth, myHeight, myDialogName.c_str(), NULL, NULL);
        setWindowPointer(window);

        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetWindowOpacity(window,1.0); // 0-1. 1=no opacy.
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }

        glfwSetKeyCallback(window, KeyCallback);
        glfwSetCharCallback(window, CharacterCallback);
        glfwSetScrollCallback(window, ScrollCallBack);
        glfwSetCursorPosCallback(window, MousePosCallBack);
        glfwSetMouseButtonCallback(window, MouseButtonCallBack);
        glfwSetWindowCloseCallback(window, WindowCloseCallBack);
        glfwSetWindowSizeLimits(window,0,0,GLFW_DONT_CARE,GLFW_DONT_CARE);

        setScissorWindow(Window());
        enableScissor(1);

        //  example_2();

        pid *p=new pid();
       // p->test_1();
        //p->dcc();
       // p->get_scurve_acc_dcc_period();
       // p->get_scurve_velocity_transition_period();
        p->full_acc_curve();

        while (!glfwWindowShouldClose(window)){
            glfwMakeContextCurrent(window); // Solves window flickering.

            glfwGetFramebufferSize(window, &myWidth, &myHeight);
            //! Set the XWindow size.
            this->setWindowDimension(myWidth,myHeight);

            //! Set the scissor size.
            mySize->setSize({{0,0,0},float(myWidth),float(myHeight)});
            setScissorSize(mySize->Size());
            //! ScissorSize().printSize("ScissorSize Mainwindow:");

            glViewport(0, 0, myWidth, myHeight);
            glClear(GL_COLOR_BUFFER_BIT);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, myWidth, myHeight, 0, -10000, 10000);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            if(glfwGetTime()>1){ // Reset every 1000 Ms.
                glfwSetTime(0);
            }
            //! Timer for blinking mouse cursor when using text editor widgets. This function has to be done called once each project.
            Mouse.setTime(glfwGetTime()*1000);

            //! draw content.
            drawWidgetVec();


            float scale_x=10;
            float scale_y=10;

            if(sr.size()>1){
                //! Scurve.
                for(uint i=0; i<sr.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(1,0,0);
                    glVertex2d(tr.at(i)*scale_x,myHeight-sr.at(i)*scale_y-50);
                    glVertex2d(tr.at(i+1)*scale_x,myHeight-sr.at(i+1)*scale_y-50);
                    glEnd();
                }

                for(uint i=0; i<ar.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(1,0,0);
                    glVertex2d(200+tr.at(i)*scale_x,myHeight-ar.at(i)*scale_y-50);
                    glVertex2d(200+tr.at(i+1)*scale_x,myHeight-ar.at(i+1)*scale_y-50);
                    glEnd();
                }

                for(uint i=0; i<vr.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(1,0,0);
                    glVertex2d(400+tr.at(i)*scale_x,myHeight-vr.at(i)*scale_y-50);
                    glVertex2d(400+tr.at(i+1)*scale_x,myHeight-vr.at(i+1)*scale_y-50);
                    glEnd();
                }
            }

            if(sr1.size()>1){
                //! control
                for(uint i=0; i<sr1.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(0,1,0);
                    glVertex2d(tr1.at(i)*scale_x,myHeight-sr1.at(i)*scale_y-50);
                    glVertex2d(tr1.at(i+1)*scale_x,myHeight-sr1.at(i+1)*scale_y-50);
                    glEnd();
                }

                for(uint i=0; i<ar1.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(0,1,0);
                    glVertex2d(200+tr1.at(i)*scale_x,myHeight-ar1.at(i)*scale_y-50);
                    glVertex2d(200+tr1.at(i+1)*scale_x,myHeight-ar1.at(i+1)*scale_y-50);
                    glEnd();
                }

                for(uint i=0; i<vr1.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(0,1,0);
                    glVertex2d(400+tr1.at(i)*scale_x,myHeight-vr1.at(i)*scale_y-50);
                    glVertex2d(400+tr1.at(i+1)*scale_x,myHeight-vr1.at(i+1)*scale_y-50);
                    glEnd();
                }
            }


            if(sr2.size()>1){
                //! control
                for(uint i=0; i<sr2.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(0,0,1);
                    glVertex2d(tr2.at(i)*scale_x,myHeight-sr2.at(i)*scale_y-50);
                    glVertex2d(tr2.at(i+1)*scale_x,myHeight-sr2.at(i+1)*scale_y-50);
                    glEnd();
                }

                for(uint i=0; i<ar2.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(0,0,1);
                    glVertex2d(200+tr2.at(i)*scale_x,myHeight-ar2.at(i)*scale_y-50);
                    glVertex2d(200+tr2.at(i+1)*scale_x,myHeight-ar2.at(i+1)*scale_y-50);
                    glEnd();
                }

                for(uint i=0; i<vr2.size()-1; i++){
                    glBegin(GL_LINES);
                    glColor3f(0,0,1);
                    glVertex2d(400+tr2.at(i)*scale_x,myHeight-vr2.at(i)*scale_y-50);
                    glVertex2d(400+tr2.at(i+1)*scale_x,myHeight-vr2.at(i+1)*scale_y-50);
                    glEnd();
                }
            }


            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        enableScissor(0);

        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }
private:
    GLFWwindow* window;
    std::string myDialogName="XScurve";
    int myWidth=500, myHeight=250;
    XSize *mySize=new XSize();
};
#endif























