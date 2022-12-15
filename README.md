# scurve_idea
scurve math idea.

https://user-images.githubusercontent.com/44880102/207896934-33f941af-10ad-4d07-887c-703fb2690885.mp4



An idea for creating a scurve, born out of a problem using the scurve math discribed in this paper :

https://github.com/grotius-cnc/s-curve-motion-planning/blob/main/Ch5.pdf

        The problem is using a acceleration start 
        and-or end value and starting a new scurve from there. 
        
        The max jerk "jm" is calculated with 
        "vo" (velocity start) and "ve" (velocity end) 
        values for a full curve. 
        
        Above implementation can not make solutions 
        for curve parts where "ve" (velocity end) is not known. 
        
        As a result the jerk max "jm" can not be calculated. 

        Below is a small example how to create the concave part of a scurve, 
        using a start acceleration and not knowing the velocity end.

        
In red:

        double vo=0;
        double ve=0;
        double a=0;             //! Start acc.
        double a_step=0.001;
        double am=6;            //! Max acc.
        double t_step=0.001;
        double s=0;
        double t=0;
        double time=0;

        while(a<am){
            time+=0.001;
            a+=a_step;
            t=t_step;
            ve=vo+a*t;
            s+=vo*t + 0.5*a*(t*t);
            std::cout<<"s:"<<s<<" ve:"<<ve<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
            vo=ve;
        }

In green, traditional linear acc curve to show the difference :

        vo=0;
        a=3;
        s=0;

        for(double i=0; i<time; i+=t_step){
            t=t_step;
            ve=vo+a*t;
            s+=vo*t + 0.5*a*(t*t);
            std::cout<<"control s:"<<s<<" ve:"<<ve<<" a:"<<a<<" t:"<<t<<" time:"<<time<<std::endl;
            vo=ve;
        }


        - Left = displacement.
        - Mid  = acceleration.
        - Right= velocity.

![screen](https://user-images.githubusercontent.com/44880102/206876530-09892f41-f0e9-4f09-a9f4-94f3ba34a687.jpg)

        For info:
        The above result is only showing the concave part of the scurve.
        When the convex part is added to the graph, the displacment value of red will problably win the game.
        
