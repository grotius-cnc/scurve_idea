#ifndef CURVE_H
#define CURVE_H

//! Author      : Skynet Cyberdine
//! Date        : 12-2022
//! Licence     : GPL2

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <vector>

//! See example() function at bottom of file for implementation example.

//! Structure to hold curve results.
struct CURVE_RESULT{
    //! Displacement result.    [mm]
    float sr=0;
    //! Velocity result.        [mm/s]
    float vr=0;
    //! Acceleration result.    [mm/s2]
    float ar=0;
    //! Curve time.             [s]
    float ct=0;


    //! Empty placeholders for pid class.
    float vo=0;
    float ve=0;
    float am=0;
    float acs=0;
    float ace=0;
};

//! Copy struct.
typedef CURVE_RESULT LINEAR_RESULT;
typedef CURVE_RESULT STEADY_RESULT;

//! Class to calculate a scurve or traditional linear curve.
//!
//! Note:
//!     If velocity begin > velocity end, curve solution is of type dcc. Deceleration.
//!     If velocity begin < velocity end, curve solution is of type acc. Acceleration.
//!
//! For a normal acc scurve, defenitions:
//!
//!     Concave             : The first part of the scurve, curve up.
//!     Convex              : The second part of the curve, curve down.
//!     Inflection point    : This is half the scurve, separating the convave & convex period's. Where acceleration is at maximum value.
//!
class curve{
public:
    //! Empty constructor.
    curve(){}
    //! Inputs:
    //! Velocity_start      [mm/s]
    //! Velocity_end        [mm/s]
    //! Acceleration_max    [mm/s2]
    //! Acceleration_end    [mm/s2]
    //!
    //! Note :  If acceleration_start or end input > 0, the velocity values will match the acceleration inputs.
    //!
    //!         In some scenario's you could calculate a velocity shift value to compensate velocity values when acceleration inputs are > 0.
    //!
    //! Expansion :
    //!
    //!         You can add a linear transition stage into the scurve.
    //!         You could stich it between the concave & convex period. At inflection point.
    //!         The question is, wich ratio to use. In a previous work i used a gain value 0-100% to construct such a scurve.
    //!
    void set(float velocity_start, float velocity_end, float acceleration_max, float acceleration_start, float acceleration_end){
        //! Parameters stored into private variables.
        vo=velocity_start;
        ve=velocity_end;
        am=acceleration_max;
        acs=acceleration_start;
        ace=acceleration_end;
        //! Init curve, calculate curve features.
        init();
        //! Calculate curve spec's for concave & convex period.
        concave();
        convex();
    }
    //! Get result of acc-dcc scurve.
    CURVE_RESULT getSCurveResult(float at_time){
        return CurveResult(at_time);
    }
    //! Get result as traditional linear acc-dcc curve.
    LINEAR_RESULT getLinearResult(float at_time){
        return LinearResult(at_time);
    }
    //! Lineair at speed result.
    STEADY_RESULT getLinearAtSpeedResult(float at_time){
        return LinearAtSpeedResult(at_time);
    }

private:
    //! Inputs.
    float vo=0;         //! Velocity start. [mm/s]
    float am=0;         //! Acceleration max. [mm/s2]
    float acs=0;        //! Acceleration start. [mm/s2]
    //! When acs is used, the velocity begin "vo" will be adjusted to match the acceleration start "acs" value.
    float ace=0;        //! Acceleration end. [mm/s2]
    //! When ace is used, the velocity end "ve" will be adjusted to match the acceleration end "ace" value.

    //! Outputs.
    bool mirror=false;  //! If ve<vo it is a deceleration curve. Curve is to be mirrored in function.
    float as=0;         //! Acceleration at inflection point, 2*am. [mm/s2]
    float ttot=0;       //! Total time vo to ve. Not influenced by acs, ace. [s]
    float ltot=0;       //! Total displacment vo to ve. Not influenced by acs, ace. [mm]
    float jm=0;         //! Jerk max. [-]
    float ts=0;         //! Time start. If acceleration start "acs" is used, this has a value. [s]
    float te=0;         //! Time end. If acceleration end "ace" is used, this has a value. [s]
    float th=0;         //! Time half. time total "ttot" /2.

    //! Concave netto.
    float s1=0;         //! Netto displacment of concave period. [mm]
    float vh=0;         //! Velocity at end of concave period. [mm/s]
    float t1=0;         //! Netto time of concave period. [t]
    //! Convex netto.
    float s2=0;         //! Netto displacment of convex period. [mm]
    float ve=0;         //! Velocity end of convex period. [mm/s]
    float t2=0;         //! Netto time of convexperiod. [t]

    //! Concave + convex netto.
    float s12=0;        //! Netto displacement concave + convex period.
    float t12=0;        //! Netto time concave + convex period.

    //! Velocity shift.
    //! When acs > 0 , time start "ts" has value, and gives a velocity start, different from vo input. Normalize this start velocity to match vo input.
    float vshift=0;

    void init(){

        if(ve<vo){
            mirror=true;
            //! Swap velocity start, end.
            float temp_vo=vo;
            vo=ve;
            ve=temp_vo;
            //! Swap acceleration start, end.
            double temp_acs=acs;
            acs=ace;
            ace=temp_acs;
        }
        //! Check limits.
        if(am<0.1){
            am=0.1;
        }
        if(acs>(2*am)){
            acs=2*am;
        }
        if(ace>2*am){
            ace=2*am;
        }

        //! Max acceleration at inflection point. [2*A]
        as=2*am;
        //! Time acc period. Document page 3, observations : T=2*Delta_v/as
        ttot=2*(ve-vo)/as;
        //! Half time.
        th=ttot/2;
        //! Lenght acc period. Document page 4 (5.16)
        ltot=((ve*ve)-(vo*vo))/as;
        //! Max jerk. Document page 2.
        jm=2*as/ttot;

        //! If scurve has acceleration start value. Find start time[t] on concave period. a=jm*t.
        if(acs>0){
            ts=acs/jm;
        }
        //! Curve has acceleration end value. Find end Time[t] on concvex period. a=jm*t.
        if(ace>0){
            te=(as-ace)/jm;
        } else {
            te=ttot/2;
        } 

    }

    void concave(){
        //! Concave period.
        float t=ts;                     //! at start of curve.
        float sa=vo*t+jm*(t*t*t)/6;     //! Netto displacment concave front.
        t=th;                           //! at end of curve.
        vh=vo+jm*(t*t)/2;               //! velocity end.
        float sb=vo*t+jm*(t*t*t)/6;     //! Bruto displacement concave front.
        s1=sb-sa;                       //! Netto displacment convace back.
        t1=th-ts;                       //! Netto time concave back.
    }

    void convex(){
        float t=te;
        ve=vh + as*t - jm*(t*t)/2;                  //! Convex velocity end.
        s2=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;        //! Netto displacement convex front.
        t2=te;                                      //! Netto time convex front.

        //! Curve time concave+convex period.
        t12=t1+t2;
        //! Curve displacment concave+convex period.
        s12=s1+s2;
    }

    //! At speed linear result
    STEADY_RESULT LinearAtSpeedResult(float at_time){

        STEADY_RESULT r;

        //! formula's lineair steady:
        //! s=vo+ v*t, t=s/v

        float v=ve;         //! Velocity end, cq. max.
        float t=at_time;    //! Request at time stamp.

        r.vr=ve;            //! Assuming we are at velocity max for this move.
        r.sr=v*t;           //! Displacement result
        r.ar=0;             //! No acceleration at steady move.
        r.ct=0;             //! No curve time for steady move.

        return r;
    }

    //! Acc-dcc traditional linear curve.
    LINEAR_RESULT LinearResult(float at_time){

        LINEAR_RESULT r;

        //! Curve time for linear acceleration.
        r.ct=(ve-vo)/am;

        if(mirror){
            //! Mirror time.
            at_time=r.ct-at_time;
        }

        //! formula's lineair acceleration:
        //! v*v=(vo*vo) + 2*a*s, s=vo*t + 0.5*a*(t*t), t=sqrt(s*2/a) ,v=vo+a*t
        float t=at_time;
        float a=am;                 //! Acceleration max. [mm/s2]
        r.vr=vo+a*t;                //! Velocity result. [mm/s]
        r.sr=vo*t + 0.5*a*(t*t);    //! Displacement result. [mm]
        r.ar=a;                     //! Acceleration result [mm/s2]

        return r;
    }

    //! Acc-dcc scurve at_time algoritme.
    CURVE_RESULT CurveResult(float at_time){

        CURVE_RESULT r;
        r.ct=t12;   //! Total curve time is netto concave + netto convex period.

        if(mirror){ //! If dcc curve.
            //! Mirror time.
            at_time=t12-at_time;
        }


        if(at_time<=t12){ //! Time in the scope of curve.

            if(at_time<=t1){ //! Time in the period of concave. (curve up)

                //! ts is relative startpos if start acc is used.
                float t=at_time+ts;
                r.sr=vo*t+jm*(t*t*t)/6;                     //! Displacement.
                r.sr-=vo*ts+jm*(ts*ts*ts)/6;                //! Substract front acc.
                r.vr=vo+jm*(t*t)/2;                         //! Velocity.
                r.ar=jm*t;                                  //! Acceleration.

                if(mirror==1){                              //! **** Scurve type : dcc.
                    r.sr=s12-r.sr;                           //! **** Dcc curve, mirror displacment.
                }
            }
            if(at_time>t1){ //! Time in the period convex. (curve down)

                float t=at_time-t1;                         //! Time starting at zero for the convex period. Substract concave period "t1".
                r.sr=vh*t + as*(t*t)/2 - jm*(t*t*t)/6;      //! Displacment given a time.
                r.sr+=s1;                                   //! Add displacement concave period.
                r.vr=vh + as*t - jm*(t*t)/2;                //! Velocity result convex period. Vh is velocity end of concave period.
                r.ar=as-jm*t;                               //! Acceleration result. As is acceleration at inflection point.

                if(mirror==1){                              //! If dcc curve.
                    r.sr=s12-r.sr;                          //! Mirror displacement.
                }
            }
        }
        return r;
    }
};

//! Example implementation of scurve and or traditional linear curve.
void example_1(){

    float am=2;         //! Acceleration max.   [mm/s2]
    float vo=0;         //! Velocity begin.     [mm/s]
    float ve=10;        //! Velocity end.       [mm/s]
    float acs=0;        //! Acceleration start. [mm/s2]
    float ace=0;        //! Acceleration end.   [mm/s2]
    float at_time=0;    //! at_time request.    [s]

    class curve *curve=new class curve();
    curve->set(vo,ve,am,acs,ace);

    //! This will initialize the curves at time 0, and returns the "total curve time" for you in the struct result r, rl.
    CURVE_RESULT rc= curve->getSCurveResult(0);
    LINEAR_RESULT rl= curve->getLinearResult(0);

    std::cout<<"scurve time: "<<rc.ct<<std::endl;
    std::cout<<"linear curve time: "<<rl.ct<<std::endl;
    std::cout<<""<<std::endl;

    //! Curve time examination given a time step 0.1 [s]
    for(float i=0; i<=rc.ct; i+=0.1){
        //! Scurve implementation.
        rc= curve->getSCurveResult(i);
        std::cout<<std::fixed<<"scurve t: "<<i<<"\t s: "<<rc.sr<<"\t v: "<<rc.vr<<"\t a: "<<rc.ar<<std::endl;
    }
    rc= curve->getSCurveResult(rc.ct);
    std::cout<<std::fixed<<"scurve t: "<<rc.ct<<"\t s: "<<rc.sr<<"\t v: "<<rc.vr<<"\t a: "<<rc.ar<<std::endl;
    std::cout<<""<<std::endl;

    //! Curve time examination given a time step 0.1 [s]
    for(float i=0; i<=rl.ct; i+=0.1){
        //! Traditional linear curve implementation.
        rl= curve->getLinearResult(i);
        std::cout<<std::fixed<<"linear t: "<<i<<"\t s: "<<rl.sr<<"\t v: "<<rl.vr<<"\t a: "<<rl.ar<<std::endl;
    }
    rl= curve->getLinearResult(rl.ct);
    std::cout<<std::fixed<<"linear t: "<<rl.ct<<"\t s: "<<rl.sr<<"\t v: "<<rl.vr<<"\t a: "<<rl.ar<<std::endl;
}

//! Scurve example using velocity shift when acs>0.
//! If acs>0 the vo is shifted to vo input.
void example_2(){

    float am=2;         //! Acceleration max.   [mm/s2]
    float vo=5;         //! Velocity begin.     [mm/s]
    float ve=15;        //! Velocity end.       [mm/s]
    float acs=2;        //! Acceleration start. [mm/s2]
    float ace=0;        //! Acceleration end.   [mm/s2]
    float at_time=0;    //! at_time request.    [s]

    class curve *curve=new class curve();
    curve->set(vo,ve,am,acs,ace);

    //! This will initialize the curves at time 0, and returns the "total curve time" for you in the struct result r, rl.
    CURVE_RESULT r= curve->getSCurveResult(0);

    std::cout<<"curve time: "<<r.ct<<std::endl;

    std::cout<<"velocity  : "<<r.vr<<std::endl;
    std::cout<<""<<std::endl;

     float velocity_shift=0; // r.vr-vo;
   // std::cout<<"velocity shift : "<<velocity_shift<<std::endl;

    //! Curve time examination given a time step 0.1 [s]
    for(float i=0; i<=r.ct; i+=0.1){
        //! Scurve implementation.
        r= curve->getSCurveResult(i);
        std::cout<<std::fixed<<"scurve t: "<<i<<"\t s: "<<r.sr<<"\t v: "<<r.vr-velocity_shift<<"\t a: "<<r.ar<<std::endl;
    }
    //! Scurve implementation.
    r= curve->getSCurveResult(r.ct);
    std::cout<<std::fixed<<"scurve t: "<<r.ct<<"\t s: "<<r.sr<<"\t v: "<<r.vr-velocity_shift<<"\t a: "<<r.ar<<std::endl;
}

//! Scurve example using time interval to calculate displacement values.
//! If acs>0 it uses a velocity shift to match vo input at curve start.
void example_3(){

    float am=2;         //! Acceleration max.   [mm/s2]
    float vo=5;         //! Velocity begin.     [mm/s]
    float ve=15;        //! Velocity end.       [mm/s]
    float acs=2;        //! Acceleration start. [mm/s2]
    float ace=0;        //! Acceleration end.   [mm/s2]
    float at_time=0;    //! at_time request.    [s]

    class curve *curve=new class curve();
    curve->set(vo,ve,am,acs,ace);

    //! This will initialize the curves at time 0, and returns the "total curve time" for you.
    CURVE_RESULT r= curve->getSCurveResult(0);
    std::cout<<"curve time: "<<r.ct<<std::endl;

    r= curve->getSCurveResult(r.ct);
    std::cout<<"curve displacment: "<<r.sr<<std::endl;
    std::cout<<""<<std::endl;

    //! Reset to zero to calculate velocity shift.
    r= curve->getSCurveResult(0);

    float velocity_shift=r.vr-vo;
    std::cout<<"velocity shift : "<<velocity_shift<<std::endl;

    std::cout<<"start situation:"<<std::endl;
    std::cout<<std::fixed<<"scurve t: "<<0.000000<<"\t displacement step: "<<r.sr<<"\t displacement total: "<<r.sr<<"\t v: "<<r.vr-velocity_shift<<"\t a: "<<r.ar<<std::endl;
    std::cout<<"for loop:"<<std::endl;

    float displacement=0;
    float lenght_a=0, lenght_b=0, lenght_ab=0;
    float time_interval=0.5; //! [s]

    //! Curve time examination given a time step 0.1 [s]
    for(float i=0; i<r.ct-time_interval; i+=time_interval){
        //! Scurve implementation.
        r= curve->getSCurveResult(i);
        lenght_a=r.sr;

        r= curve->getSCurveResult(i+time_interval);
        lenght_b=r.sr;

        lenght_ab=lenght_b-lenght_a;
        displacement+=lenght_ab;
        std::cout<<std::fixed<<"scurve t: "<<i+time_interval<<"\t displacement step: "<<lenght_ab<<"\t displacement total: "<<displacement<<"\t v: "<<r.vr-velocity_shift<<"\t a: "<<r.ar<<std::endl;
    }
    r= curve->getSCurveResult(r.ct);
    lenght_ab=r.sr-lenght_b;
    displacement+=lenght_ab;
    std::cout<<"final step"<<std::endl;
    std::cout<<std::fixed<<"scurve t: "<<r.ct<<"\t displacement step: "<<lenght_ab<<"\t displacement total: "<<displacement<<"\t v: "<<r.vr-velocity_shift<<"\t a: "<<r.ar<<std::endl;
}

//! Scurve example for jogging.
//!
//!     Scenario:  User holds jog button for x seconds, then releases the jog button, machine has to perform controlled stop from there.
//!
void example_4(){

    float am=2;         //! Acceleration max.   [mm/s2]
    float vo=0;         //! Velocity begin.     [mm/s]
    float ve=10;        //! Velocity end.       [mm/s]
    float acs=0;        //! Acceleration start. [mm/s2]
    float ace=0;        //! Acceleration end.   [mm/s2]
    float at_time=0;    //! at_time request.    [s]

    //! User holds jog button for x seconds.
    float jog_time=20.0; //! [s]
    //! Servo thread interval. 1 [ms] = 0.001 [s]
    float servo_interval=1.0; //! [s]

    class curve *curve=new class curve();
    curve->set(vo,ve,am,acs,ace);

    CURVE_RESULT r;
    r=curve->getSCurveResult(0);
    float acc_time=r.ct;
    float acc_displacement=0;
    float linear_displacement=0;
    float linear_time=0;
    bool dcc_stage=0;               //! Flag when we are in dcc stage, controlled stop. Then ve & vo has to be swapped only once.
    float dcc_time=0;
    float dcc_displacement=0;

    float timer=0;

    while(1){

        timer+=servo_interval;
        //! Acceleration stage.
        if(timer<acc_time){
            curve->set(vo,ve,am,acs,ace);
            r=curve->getSCurveResult(timer);
            acc_displacement=r.sr;
            std::cout<<std::fixed<<"jogging acc stage at time: "<<timer<<" vel: "<<r.vr<<" acc: "<<r.ar<<" displacement: "<<r.sr<<std::endl;
        }
        if(timer>acc_time){
            //! Exact values.
            r=curve->getSCurveResult(acc_time);
            acc_displacement=r.sr;
            //! std::cout<<"acc displacment:"<<acc_displacement<<std::endl;
        }
        //! At speed.
        if(timer>acc_time && timer<jog_time){
            r=curve->getLinearAtSpeedResult(timer-acc_time);
            std::cout<<std::fixed<<"jogging lin stage at time: "<<timer<<" vel: "<<r.vr<<" acc: "<<r.ar<<" displacement: "<<r.sr+acc_displacement<<std::endl;
            linear_displacement=r.sr;
        }
        if(timer>jog_time){
            //! Exact values.
            r=curve->getLinearAtSpeedResult(jog_time-acc_time);
            linear_displacement=r.sr;
            linear_time=jog_time-acc_time;
            //! std::cout<<"linear displacment:"<<linear_displacement<<std::endl;
        }

        //! Button release, controlled stop, dcc stage.
        if(timer>jog_time){
            if(!dcc_stage){
                vo=r.vr;        //! Use current velocity to start stop sequence.
                //! std::cout<<"velocity init stop sequence:"<<vo<<std::endl;
                ve=0;           //! Stop = velocity zero.
                curve->set(vo,ve,am,acs,ace);
                r=curve->getSCurveResult(0);
                dcc_time=r.ct;
                dcc_stage=1;
            }

            float t=timer-jog_time;
            if(t>dcc_time){
                //! Exact values.
                r=curve->getSCurveResult(dcc_time);
                std::cout<<std::fixed<<"jogging dcc stage at time: "<<acc_time+linear_time+dcc_time<<" vel: "<<r.vr<<" acc: "<<r.ar<<" displacement: "<<r.sr+acc_displacement+linear_displacement<<std::endl;
                std::cout<<"finished jogging."<<std::endl;
                break;
            }

            r=curve->getSCurveResult(t);
            std::cout<<std::fixed<<"jogging dcc stage at time: "<<timer<<" vel: "<<r.vr<<" acc: "<<r.ar<<" displacement: "<<r.sr+acc_displacement+linear_displacement<<std::endl;
        }
    }
}
#endif
















