//
//  datum.h
//  vectorGraphicsExample
//
//  Created by Scott Kildall on 6/16/14.
//
//

#ifndef __baddata__datum_
#define __baddata__datum_

#include <iostream>
#include <string.h>
#include "ofMain.h"

class datum {
    
public:
    
    datum();
    ~datum();
    
    void setMinRadius( float _min );
    
    void setPower( float _power ) { power = _power; }
    float getPower() { return power; }
    
    void setVars(float _x, float _y, float _z, float _pop );
    void draw(int xAdjust, int yAdjust, float gridSpacing, Boolean draw3d);
 
    void setSize(float _s);
    float getSize() { return s; }           // this is the radius
    
    void setSizeMultiplier(float _multiplier);
    float getSizeMultiplier() { return multiplier; }
    
    void setDescription(string _desc);
    void setType(string _typ);
    string getType() { return typ; }
    
    float getParams(float &xParam, float &yParam, float &radiusParam );
    
    
    float x;
    float y;
    float z;
    
private:
    
    
    float s;    // size
    int shapeStyle;
    
    float minRadius;
    float power;            // > 1 means bigger circles have a larger relative weight
                            // < 1 means smaller cirlces have a larger relative weight
    float multiplier;
    
    unsigned long id;
    string desc;
    string typ;
    Boolean bGeneratedLines;
    
};


#endif /* defined(__baddata__datum_) */
