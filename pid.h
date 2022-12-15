#ifndef PID_H
#define PID_H

#include <iostream>
#include <curve.h>
#include <thread>
#include <unistd.h>

std::vector<double> tr,vr,ar,sr;
std::vector<double> tr1,vr1,ar1,sr1;
std::vector<double> tr2,vr2,ar2,sr2;
class pid{
public:
    //! Empty constructor.
    pid(){}

    void run(){
        thread = new std::thread(&pid::update,this);
        thread->detach(); //! Execute the thread independent from other programs.
    }

    void update(){
        while(1){
            //! Use timer delay ..
            usleep(1000000*0.001);
            std::cout<<"time:"<<time<<std::endl;
        }
    }

    struct result{
        double acceleration=0;
        double time=0;
    };

    void jog(){
        //! 1. On   Acc up to vm.
        //! 2.      Continue at steady at vm.
        //!
        //! 3. Off  Dcc to ve=0;


    }


    void full_acc_curve(double acs=0, double ace=-2, double vo=0, double ve=10, double am=2, double a_step=0.001, double time_step=0.001, bool debug=1){

            //! Get the acceleration time.
            double acc_t1=get_acceleration_transition_time(acs,am,a_step,time_step,0);
            double acc_t3=get_acceleration_transition_time(am,ace,a_step,time_step,0);

            //! Get the velocity shift for period t1 ,t2 t3.
            double v_t1=get_velocity_end(0,acs,am,a_step,time_step,0);
            double v_t3=get_velocity_end(0,am,ace,a_step,time_step,0);
            double v_t2=(ve-vo)-v_t1-v_t3;

            if(debug){
                //! Velocity shift total.
                std::cout<<"velocity total t1-t2-t3:"<<(ve-vo)<<std::endl;

                std::cout<<"acceleration time t1:"<<acc_t1<<std::endl;
                std::cout<<"acceleration time t2:"<<0<<std::endl;
                std::cout<<"acceleration time t3:"<<acc_t3<<std::endl;

                std::cout<<"velocity shift t1:"<<v_t1<<std::endl;
                std::cout<<"velocity shift t2:"<<v_t2<<std::endl;
                std::cout<<"velocity shift t3:"<<v_t3<<std::endl;
            }

            //! Curve can not reach am.
            if(v_t2<0){
                std::cout<<"impossible to reach am, recalculate am to fit curve."<<std::endl;

                double am_t1_t3=get_acceleration_max(vo,(vo+ve)/2,am,a_step,time_step,0);
                std::cout<<"acceleration max t1-t3:"<<am_t1_t3<<std::endl;

                //! Get the velocity shift for period t1 ,t2 t3.
                double v_t1=get_velocity_end(0,acs,am_t1_t3,a_step,time_step,0);
                double v_t3=get_velocity_end(0,am_t1_t3,ace,a_step,time_step,0);
                double v_t2=(ve-vo)-v_t1-v_t3;

                std::cout<<"velocity shift t1:"<<v_t1<<std::endl;
                std::cout<<"velocity shift t2:"<<v_t2<<std::endl;
                std::cout<<"velocity shift t3:"<<v_t3<<std::endl;
            }
    }

    double get_acceleration_max(double vo=0, double ve=2, double am=2, double a_step=0.001, double time_step=0.001, bool debug=1){

        double t=time_step;
        double v=0;
        double a=0;
        double time=0;

        //! Velocity up.
        if(vo<ve){
            while(1){
                if(vo>=ve){
                    return a;
                }
                time+=t;
                a+=a_step;
                if(a>am){
                    a=am;
                }

                v=vo+a*t;
                vo=v;

                if(debug){
                    std::cout<<"velocity up, time:"<<time<<" v:"<<v<<" a:"<<a<<std::endl;
                }
            }
        }

        //! Velocity down.
        if(vo>ve){
            //! Negative acceleration allowed.
            am=-abs(am);
            while(1){
                if(ve>=vo){
                    return a;
                }
                time+=t;
                a-=a_step;
                if(a<am){
                    a=am;
                }

                v=vo+a*t;
                vo=v;

                if(debug){
                    std::cout<<"velocity down, time:"<<time<<" v:"<<v<<" a:"<<a<<std::endl;
                }
            }
        }

        return a;
    }

    double get_velocity_end(double vo=10, double acs=0, double ace=2, double a_step=0.001, double time_step=0.001, bool debug=1){
        double t=time_step;
        double v=0;
        double a=0;
        double time=0;

        //! Acceleration period.
        if(acs<ace){
            while(1){
                if(acs+a_step>=ace){
                    break;
                }
                time+=t;
                acs+=a_step;

                a=acs;
                v=vo+a*t;
                vo=v;
                if(debug){
                    std::cout<<std::fixed<<"acc period, time:"<<time<<" acs:"<<acs<<std::endl;
                }
            }
        }

        //! Deceleration period.
        if(acs>ace){
            while(1){
                if(acs-a_step<=ace){
                    break;
                }
                time+=t;
                acs-=a_step;

                a=acs;
                v=vo+a*t;
                vo=v;
                if(debug){
                    std::cout<<std::fixed<<"dcc period, time:"<<time<<" acs:"<<acs<<std::endl;
                }
            }
        }

        if(debug){
            std::cout<<"time:"<<time<<std::endl;
        }
        return vo;
    }

    //! Velocity transition, including linear transition stage to respect acceleration max "am".
    //! Will try to end with max acceleration.
    result get_scurve_velocity_transition_period(double vo=10, double ve=0, double acs=0, double am=2, double a_step=0.001, double time_step=0.001, bool debug=1){

        result r;

        double time=0;

        double v=0;
        double va=vo;
        double vb=ve;
        double vc=vo;
        double vd=ve;
        double a=acs;
        double t=time_step;

        am=abs(am); //! User may input negative acceleration value.

        //! Velocity up.
        if(va<vb){
            while(1){
                if(va>=vb){
                    break;
                }
                time+=t;
                a+=a_step;
                if(a>am){
                    a=am;
                }

                v=va+a*t;
                va=v;

                if(debug){
                    std::cout<<"velocity up, time:"<<time<<" v:"<<v<<" a:"<<a<<std::endl;
                }
            }
        }

        //! Velocity down.
        if(vc>vd){
            //! Negative acceleration allowed.
            am=-abs(am);
            while(1){
                if(vd>=vc){
                    break;
                }
                time+=t;
                a-=a_step;
                if(a<am){
                    a=am;
                }

                v=vc+a*t;
                vc=v;

                if(debug){
                    std::cout<<"velocity down, time:"<<time<<" v:"<<v<<" a:"<<a<<std::endl;
                }
            }
        }

        r.time=time;
        r.acceleration=a;
        return r;
    }

    //! Calculate period for deceleration.s.
    //! velocity start. [mm/s]
    //! velocity end. [mm/s]
    //! acceleration start. [mm/s2]
    //! acceleration end. [mm/s2]
    //! acceleration step. [mm/s2]
    //!     - The power of the curve.
    //! time_step. [s]
    //! Return : time [s]
    float get_acceleration_transition_time(double acceleration_start=2, double acceleration_end=-2, double acceleration_step=0.001, double time_step=0.001, bool debug=0){

        double t=time_step;
        double a_step=acceleration_step;
        double acs=acceleration_start;
        double ace=acceleration_end;
        double time=0;

        //! Acceleration period.
        if(acs<ace){
            while(1){
                if(acs+a_step>=ace){
                    break;
                }
                time+=t;
                acs+=a_step;
                if(debug){
                    std::cout<<std::fixed<<"acc period, time:"<<time<<" acs:"<<acs<<std::endl;
                }
            }
        }

        //! Deceleration period.
        if(acs>ace){
            while(1){
                if(acs-a_step<=ace){
                    break;
                }
                time+=t;
                acs-=a_step;
                if(debug){
                    std::cout<<std::fixed<<"dcc period, time:"<<time<<" acs:"<<acs<<std::endl;
                }
            }
        }
        if(debug){
            std::cout<<"time:"<<time<<std::endl;
        }
        return time;
    }

private:
    //! Thread.
    std::thread *thread;

};

#endif




//! Velocity up to max a=4.
//void velocity_up(){


//        double vo=0;
//        double ve=0;
//        double a=0;             //! Start acc.
//        double a_step=0.001;
//        double am=6;            //! Max acc.
//        double t_step=0.001;
//        double s=0;
//        double t=0;
//        double time=0;

//        while(a<am){
//            time+=0.001;
//            a+=a_step;
//            t=t_step;
//            ve=vo+a*t;
//            s+=vo*t + 0.5*a*(t*t);
//            std::cout<<"s:"<<s<<" ve:"<<ve<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//            vo=ve;

//            vr.push_back(ve);
//            tr.push_back(time);
//            ar.push_back(a);
//            sr.push_back(s);
//        }

//        vo=0;
//        a=3;
//        s=0;

//        for(double i=0; i<time; i+=t_step){
//            t=t_step;
//            ve=vo+a*t;
//            s+=vo*t + 0.5*a*(t*t);
//            std::cout<<"control s:"<<s<<" ve:"<<ve<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//            vo=ve;

//            vr1.push_back(ve);
//            tr1.push_back(i);
//            ar1.push_back(a);
//            sr1.push_back(s);
//        }

//        std::cout<<"s:"<<sr.back()<<" ve:"<<vr.back()<<" a:"<<ar.back()<<" t:"<<tr.back()<<" time:"<<time<<std::endl;


//}


//! Fit.
//! End curve with exact velocity.
//            if(v>(vm/2)){
//                double vremove=v-(vm/2);
//                //! Acc?    v=a*t; -> a=v/t
//                double tr=vremove/a;
//                t-=tr;
//                v=vo+a*t;
//            }




//! Goto max vel, then to vel 0.
//void test_1(){

//    double vo=0;
//    double vm=25;
//    double a=0;             //! Start acc.
//    double a_step=0.001;
//    double am=2;            //! Max acc.
//    double t_step=0.001;
//    double s=0;
//    double t=0;
//    double v=0;
//    double time=0;
//    int acc_steps=0;
//    int steady_steps=0;

//    //! Concave period.
//    while(vo<(vm/2)){

//        time+=0.001;
//        if(a<am){
//            a+=a_step;
//        }
//        t=t_step;
//        v=vo+a*t;

//        s+=vo*t + 0.5*a*(t*t);
//        std::cout<<std::fixed<<"s:"<<s<<" ve:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//        vo=v;

//        if(a<am){
//            vr.push_back(v);
//            tr.push_back(time);
//            ar.push_back(a);
//            sr.push_back(s);
//            acc_steps++;
//        }

//        if(a>=am){
//            vr2.push_back(v);
//            tr2.push_back(time);
//            ar2.push_back(a);
//            sr2.push_back(s);
//            steady_steps++;
//        }
//    }

//    //! Convex steady period.
//    while(1){

//        time+=0.001;
//        if(steady_steps>0){
//            t=t_step;
//            v=vo+a*t;

//            s+=vo*t + 0.5*a*(t*t);
//            std::cout<<std::fixed<<"s:"<<s<<" ve:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//            vo=v;

//            vr2.push_back(v);
//            tr2.push_back(time);
//            ar2.push_back(a);
//            sr2.push_back(s);

//            steady_steps--;
//        }
//        if(steady_steps==0){
//            break;
//        }
//    }
//    //! Convex period.
//    while(a>0){

//        time+=0.001;
//        a-=a_step;
//        t=t_step;
//        v=vo+a*t;

//        s+=vo*t + 0.5*a*(t*t);
//        std::cout<<std::fixed<<"s:"<<s<<" ve:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//        vo=v;

//        vr2.push_back(v);
//        tr2.push_back(time);
//        ar2.push_back(a);
//        sr2.push_back(s);
//    }
//}



//! Goto max acc, then acc 0.
//void test_0(){


//    double vo=0;
//    double vm=10;
//    double a=0;             //! Start acc.
//    double a_step=0.001;
//    double am=4;            //! Max acc.
//    double t_step=0.001;
//    double s=0;
//    double t=0;
//    double v=0;
//    double time=0;

//    while(a<am){
//        time+=0.001;
//        a+=a_step;
//        t=t_step;
//        v=vo+a*t;
//        s+=vo*t + 0.5*a*(t*t);
//        //std::cout<<std::fixed<<"s:"<<s<<" ve:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//        vo=v;

//        vr.push_back(v);
//        tr.push_back(time);
//        ar.push_back(a);
//        sr.push_back(s);
//    }

//    while(a>0){
//        time+=0.001;
//        a-=a_step;
//        t=t_step;
//        v=vo+a*t;
//        s+=vo*t + 0.5*a*(t*t);
//        //std::cout<<std::fixed<<"s:"<<s<<" v:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//        vo=v;

//        vr.push_back(v);
//        tr.push_back(time);
//        ar.push_back(a);
//        sr.push_back(s);
//    }

//    std::cout<<std::fixed<<"s:"<<s<<" v:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//}


//! Deceleration, using scurve & linear transition period. Using am at end of curve.
//! vo=velocity start.
//! ve=velocity end.
//! acs=acceleration start.
//! am=acceleration max.
//void dcc(float vo=10, float ve=1, float acs=0, float am=2){

//    double t=0.001;
//    double a=0;
//    double s=0;
//    double v=0;
//    double time=0;

//    a=acs;

//    while(vo>ve){
//        time+=t;
//        a-=t;
//        if(a<-abs(am)){ //! Limit dcc to -am.
//            a=-abs(am);
//        }
//        v=vo+a*t;
//        s+=vo*t + 0.5*a*(t*t);
//        std::cout<<std::fixed<<"s:"<<s<<" ve:"<<v<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
//        //! s:34.412801 ve:0.999209 a:-2.000000 t:0.001000 time:5.500000
//        vo=v;

//        vr2.push_back(v);
//        tr2.push_back(time);
//        ar2.push_back(a);
//        sr2.push_back(s);
//    }
//}
