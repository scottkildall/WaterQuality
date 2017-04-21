#pragma once

#include "ofMain.h"
#include "datum.h"

#include "ofxCsv.h"
using namespace wng;


#define CANVAS_RATIO (1)
//#define CANVAS_WIDTH (600)                  // mm of final cuts, this number can be scaled
//#define CANVAS_HEIGHT (CANVAS_WIDTH)        // keep same as width to prevent distortion
#define CANVAS_MARGIN (20)


class waterQualityDataApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
        void drawDescription();
		
		void keyPressed(int key);
	/*
        void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
*/
		
		
    private:
        void setDataVariables( datum *d, float dataSize );
    
        void adjustMinMaxXY( float &minX, float &maxX, float &minY, float &maxY );
    
        float getMercatorX(float lon, float width);
        float getMercatorY(float lat, float height, float width);
        float getSinusoidalX(float lat, float lon, float width);
        float getSinusoidalY(float lat, float lon, float height, float width );
    
    
        float adjustX(float x, float min, float max);
        float adjustY(float y, float min, float max);
        float map(float x, float in_min, float in_max, float out_min, float out_max);
    
        void loadCSVFiles();
        void loadCSVData(string filename);
    
        void generateOutputName(char *buffer );
    
        
        datum *data;
        long numData;
    
        vector <ofFile> csvFiles;
        int numCSVFiles;
        int currentFileIndex;
    
        void filterCSV();
    
        //-- grid lines, always horizontal lines
        float gridSpacing;      // in cm, space between each horizontol line
        float gridStart;        // in cm, the start horizontal grid line
        float gridEnd;         // in cm, the end horizontal grid line
        int numGridLines;
    
        // screen variables
        int screenWidth;
        int screenHeight;
        int canvasWidth;
        int canvasHeight;
    
    
        bool bEasyNav;
        bool bDraw3d;
    
        bool has3DData;
    
        // camera
     ofEasyCam cam;
};


