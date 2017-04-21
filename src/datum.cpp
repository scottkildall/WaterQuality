//
//  datum.cpp
//  vectorGraphicsExample
//
//  Created by Scott Kildall on 6/16/14.
//
//

#include "datum.h"
#include "ofMath.h"


//-- elliptical spiral experiment, set to 1 for ELLIPSES, 0 for CIRCLES
#define ELLIPSE_SPIRALS (0)
#define ELLIPSE_SPIRALS_DIVISOR (2)

datum::datum() {
    id = 0;
    s = 0;
    x = 0;
    y = 0;
    
    power = .9;
    
    minRadius = 0;
    
    bGeneratedLines = false;
 
    multiplier = 1;
}
        
datum::~datum() {
    ;
}

void datum::setVars(float _x, float _y, float _z, float _s ) {
    x = _x;
    y = _y;
    z = _z;
    
    
        //    if( z == 0.0 )
//        z = 50 - ofRandom(100);
    s = _s;
}


//-- figure out shapes here
//-- add xAdjust and yAdjust
void datum::draw(int xAdjust, int yAdjust, float gridSpacing, Boolean draw3d ) {
    
    
    ofFill();
    ofSetColor( ofColor::white ) ;
    
    float radius = s * multiplier;
    
    //radius = pow(s,power);
    
    radius = 2;
    
    if( draw3d )
        // 3D way
        ofCircle( x + xAdjust, y + yAdjust, z, 5 /*radius*100*/ ) ;
    
    else
        // 2d way
        ofCircle( x + xAdjust, y + yAdjust, radius ) ;
    
    return;
}

void datum::setMinRadius( float _min ) {
    minRadius = _min;
}

void datum::setSize(float _s) {
    //s = sqrt(_s);
    
    //-- check for minumums, add real code here
    //-- this seems to map to a 1.5 minimum etch, which is a little weird...
    if( s < minRadius )
        s = minRadius;
    
    s = _s;
}

void datum::setSizeMultiplier(float _multiplier) {
    multiplier = _multiplier;
}

void datum::setDescription(string _desc) {
    desc = _desc;
}

void datum::setType(string _typ) {
    typ = _typ;
}



float datum::getParams(float &xParam, float &yParam, float &radiusParam ) {
    xParam = x;
    yParam = y;
    radiusParam = s;
}



