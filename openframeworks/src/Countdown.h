#ifndef __COUNTDOWN_H__
#define __COUNTDOWN_H__

class Countdown : public ofThread  {
    
public:
    
    Countdown() {
        iSeconds = 3;
    }
    
    void threadedFunction() {
        
        iStartTime = ofGetElapsedTimeMillis();
        
        while(isThreadRunning()) {
            
            float timer = ofGetElapsedTimeMillis() - iStartTime;
            
            if (timer >= 1000) { // one second
                iSeconds--;
                
                iStartTime = ofGetElapsedTimeMillis(); // reset start time
            }
        }
    }
    
    inline int getSeconds()  { return this->iSeconds; }
    inline void setSeconds(int seconds) { this->iSeconds = seconds; }

private:
    
    int iSeconds; // countdown seconds 3 .. 2 .. 1 ;P
    int iStartTime;
    
};

#endif // __COUNTDOWN_H__
