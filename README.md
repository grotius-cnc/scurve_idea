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

Notes:

        The attached code is working, but not stable during runtime.
        
Formula for scurve, this should be a linear acceleration formula, but it can behave like scurve also :

        v=vo+a*t
        
        To make it work as scurve :
        
        In the concave and convex periods each cycle the "a" is incremented with a value. "a_step".
        
        
        
        
        
