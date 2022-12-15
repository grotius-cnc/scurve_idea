#ifndef JOG_H
#define JOG_H

#include <thread>
#include <iostream>

class jog{
public:
    jog(){}

    //! Set values.
    void set(double velocity_max=10, double acceleration_max=2, double acceleration_interval=0.001, double time_interval=0.001){
        myVelmax=velocity_max;
        myAccmax=acceleration_max;
        myTimeInterval=time_interval;
        myAccInterval=acceleration_interval;
    }

    void reset(){
        myVel=0;
        myAcc=0;
        myTimer=0;
        myInit=0;
        myInitAccPeriods=0;
        myInitDccPeriod=0;
    }

    //! Update values.
    void update(){

        myTimer+=myTimeInterval;
        myTime+=myTimeInterval;
        //! std::cout<<"time:"<<myTime<<std::endl;
        if(myJogPlus){
            myInit=1;
            myInitDccPeriod=0;
            if(!myInitAccPeriods){

                myInitAccPeriods=1;
                myTime=0;
                // myVel=0; //! Enable starting from a velocity.
                // myAcc=0;
                periods_acc_t1_t2_t3();
            }
            double t=myTime;

            //! Perform periods.
            if(t<=myT1){
                myPeriod=1;
                myAcc+=myAccInterval;
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t1:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>myT1 && t<myT1+myT2){
                myPeriod=2;
                //myAcc=myAccmax;
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t2:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>=myT1+myT2 && t<=myT1+myT2+myT3){
                myPeriod=3;
                myAcc-=myAccInterval;
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t3:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>myT1+myT2+myT3){
                myPeriod=10;
                //myAcc=0;
                // myVel=myVelmax;
                //std::cout<<"t10:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }

        }
        if(!myJogPlus && myInit){
            myInitAccPeriods=0;
            if(!myInitDccPeriod){
                periods_dcc_t4();
                periods_dcc_t5_t6_t7();
                myTime=0;
                myInitDccPeriod=1;
            }
            double t=myTime;

            //! Perform periods.
            if(t<=myT4){
                myPeriod=4;
                myAcc-=myAccInterval;
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t4:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>myT4 && t<myT4+myT5){
                myPeriod=5;
                myAcc-=myAccInterval;
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t5:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>=myT4+myT5 && t<=myT4+myT5+myT6){
                myPeriod=6;
                //myAcc=-std::abs(myAccmax);
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t6:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>myT4+myT5+myT6 && t<myT4+myT5+myT6+myT7){
                myPeriod=7;
                myAcc+=myAccInterval;
                myVel=myVel+myAcc*myTimeInterval;
                //std::cout<<"t7:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }
            if(t>=myT4+myT5+myT6+myT7){
                myPeriod=8;
                myAcc=0;
                myVel=0;
                //std::cout<<"t8:"<<t<<" v:"<<myVel<<" a:"<<myAcc<<std::endl;
            }

        }
        if(myJogMin){

        }


    }

    double Velocity(){
        return myVel;
    }
    double Acceleration(){
        return myAcc;
    }
    double Position(){
        return myPos;
    }
    double Time(){
        return myTimer;
    }
    int Period(){
        return myPeriod;
    }

    //! Input commands.
    void jog_plus_start(){
        myJogPlus=1;
    }
    void jog_plus_stop(){
        myJogPlus=0;
    }
    void jog_min_start(){
        myJogMin=1;
    }
    void jog_min_stop(){
        myJogMin=0;
    }
private:
    double myVelmax=0;
    double myAccmax=0;
    double myTimeInterval=0;
    double myAccInterval=0;
    bool myJogPlus=0;
    bool myJogMin=0;
    double myVel=0;
    double myAcc=0;
    double myPos=0;
    double myTime=0;
    double myTimer=0;
    bool myInitAccPeriods=0;
    bool myInitDccPeriod=0;
    bool myInit=0;
    int myPeriod=0;

    //! Dcc periods.
    //! When user releases button while in acc period, t4 is used to go to acc=0.
    //! t4 is from acceleration end to acceleration zero.
    double myT1=0, myT2=0, myT3=0;
    double myT4=0;
    double myV4=0;
    double myT5=0, myT6=0, myT7=0;

    void periods_acc_t1_t2_t3(){

        double vo=myVel;
        double vm=myVelmax;
        double a=myAcc;
        double am=myAccmax;
        double astep=myAccInterval;
        double tstep=myTimeInterval;

        double t=0;
        double v=0;
        double ttot=0;
        double t1=0, t2=0, t3=0;
        double v1=0, v2=0, v3=0;

        //! Period t1, t3.
        while(1){
            ttot+=tstep;
            t=tstep;
            a+=astep;
            v=vo+a*t;
            vo=v;

            if(a+astep>am){
                break;
            }

            if(v>=vm/2){ //! If curve cannot reach max am, stop period at half velocity.
                break;
            }
        }
        t1=t3=ttot;
        v1=v3=v;
        v2=vm-v1-v3;

        //! Period t2.
        if(v2<0){
            v2=0;
        }
        if(v2>0){
            t=0;
            v=0;
            ttot=0;
            vo=v1;
            while(v<v1+v2){
                ttot+=tstep;
                t=tstep;
                v=vo+a*t;
                vo=v;
            }
            t2=ttot;
            v2=v;
        }

        myT1=t1;
        myT2=t2;
        myT3=t3;
    }

    void periods_dcc_t4(){

        myPeriod=4;

        double vo=myVel;
        double a=myAcc;
        double astep=myAccInterval;
        double tstep=myTimeInterval;

        double t=0;
        double v=0;
        double ttot=0;
        myT4=0;

        //std::cout<<"dcc t4 input acc:"<<myAcc<<" vel:"<<myVel<<std::endl;

        if(a==0){
            myV4=myVel;
            myT4=0;
            return;
        }

        //! Period t4, to acc 0.
        while(1){
            if(a>0){
                ttot+=tstep;
                t=tstep;
                a-=astep;
                v=vo+a*t;
                vo=v;
                // std::cout<<"a:"<<a<<std::endl;
            }
            if(a<=0){
                break;
            }
        }
        myV4=v;
        myT4=ttot;
        //std::cout<<"dcc t4 to acc 0, time:"<<myT4<<" ve:"<<myV4<<std::endl;
    }

    void periods_dcc_t5_t6_t7(){

        double vo=0;
        double vm=myV4;
        double a=0;
        double am=myAccmax;
        double astep=myAccInterval;
        double tstep=myTimeInterval;

        double t=0;
        double v=0;
        double ttot=0;
        double t1=0, t2=0, t3=0;
        double v1=0, v2=0, v3=0;

        //! Period t1, t3.
        while(1){
            ttot+=tstep;
            t=tstep;
            a+=astep;
            v=vo+a*t;
            vo=v;

            if(a+astep>am){
                break;
            }
            //! Mirror curve start calculating in period 5.
            if(v>=vm/2){ //! If curve cannot reach max am, stop period at half velocity.
                break;
            }
        }
        t1=t3=ttot;
        v1=v3=v;
        v2=vm-v1-v3;

        //! Period t2.
        if(v2<0){
            v2=0;
        }
        if(v2>0){
            t=0;
            v=0;
            ttot=0;
            vo=v1;
            while(v<v1+v2){
                ttot+=tstep;
                t=tstep;
                v=vo+a*t;
                vo=v;
            }
            t2=ttot;
            v2=v;
        }
        myT5=t1;
        myT6=t2;
        myT7=t3;
    }
};

#endif



























