#include "waterQualityDataApp.h"
#include "ofUtils.h"

#include <stdlib.h>
#include <math.h>

//--------------------------------------------------------------
// INDEXED FILES (may need to change, since these are hardcoded)
// Check console window to see output
// The two variables are: int numCSVFiles;     int currentFileIndex;


#define FILE_BASINS                     (0)

// Index Num: 0 — Filename: 01_test


//--------------------------------------------------------------
// INPUT CSV: formatted records: we use the same ones for each data set
// Description, Type

#define INPUT_COL_DESCRIPTION       (0)

#define INPUT_COL_LAT               (1)
#define INPUT_COL_LNG               (2)
#define INPUT_COL_ALT               (3)

#define DEFAULT_SIZE                 (5)
//#define DEFAULT_SQUARE               (true)

#define GRID_SPACING_INCREMENT (.025)     // .25mm
#define GRID_SPACING_START_VAULE (.55)     // .25mm

#define POWER_INCREMENT (.05)


float MULTIPLIER_INCREMENT = .1f;

//--------------------------------------------------------------
void waterQualityDataApp::setup(){
    screenWidth = ofGetWindowWidth();
    screenHeight = ofGetWindowHeight();
    
    cout << "width = " << screenWidth << endl;
    cout << "height = " << screenHeight << endl;
    
    canvasWidth = screenWidth;
    canvasHeight = screenHeight;
    
    //newMexicoMap.loadImage("water_basins_inverted.jpg");
    
    
	numData = 0;
    data = NULL;
    
    gridSpacing = GRID_SPACING_START_VAULE;         // 5mm
    numCSVFiles = 0;
    currentFileIndex = 0;
    numGridLines = 0;
    

    
    loadCSVFiles();
    
    bEasyNav = false;
    bDraw3d = false;
    
    // set centerpoint
    ofVec3f target( (canvasWidth-CANVAS_MARGIN)/2,(canvasHeight-CANVAS_MARGIN)/2,0);
    cam.setTarget(target);
}

//--------------------------------------------------------------
void waterQualityDataApp::update(){
	ofBackground(0, 0, 0);

}

//--------------------------------------------------------------
void waterQualityDataApp::draw(){
    
    int xAdjust = 0;
    int yAdjust = 0;

    
    if( bEasyNav )
        cam.begin();
    
    
    ofFill();
    //ofSetColor( ofColor::black ) ;
    //newMexicoMap.draw(0 - xAdjust,0 -yAdjust);
    
    //return;
    
    
    // draws data (skip for now)
    
    for( int i = 1; i < numData; i++ ) {
         ofSetHexColor(0x000000);
        
        
        (data+i)->draw(-xAdjust, -yAdjust,gridSpacing, has3DData );
        
        //has3DData = true;
        
        if( has3DData ) {
            // 3d way
            if( i < (numData-1) )
                ofLine( (data+i)->x, (data+i)->y, (data+i)->z, (data+i+1)->x, (data+i+1)->y, (data+i+1)->z );
        }
        else {
            // 2d way
            if( i < (numData-1) )
                ofLine( (data+i)->x, (data+i)->y, (data+i+1)->x, (data+i+1)->y);

        }
    }
    
    if( bEasyNav )
        cam.end();
    
    ofDrawBitmapString( csvFiles[currentFileIndex].getFileName(), ofPoint(CANVAS_MARGIN,canvasHeight-CANVAS_MARGIN));
    
    
    
}

//--------------------------------------------------------------
void waterQualityDataApp::keyPressed(int key){
    if( key == ' ')
        bEasyNav = !bEasyNav;
    
     if( key == '.' ) {
        // next CSV file
        currentFileIndex++;
        if( currentFileIndex == numCSVFiles )
            currentFileIndex = 0;
         
        loadCSVData(csvFiles[currentFileIndex].getFileName());
        
    }
    else if( key == ',' ) {
        // previous CSV file
        currentFileIndex--;
        if( currentFileIndex == -1 )
            currentFileIndex = numCSVFiles - 1;
        
        loadCSVData(csvFiles[currentFileIndex].getFileName());
    }
    
   
   
    
    
    
    // oLD STUFF
    /*
     
     
    
    else if( key == '1' ) {
        if( gridSpacing > GRID_SPACING_INCREMENT )
            gridSpacing -= GRID_SPACING_INCREMENT;
    }
    
    else if( key == '2' )
        gridSpacing += GRID_SPACING_INCREMENT;
    
    else if( key == '3' ) {
        if( numInsideSkipSpirals > 0 )
            numInsideSkipSpirals--;
    }
    
    else if( key == '4')
        numInsideSkipSpirals++;
    
    else if( key == '5' ) {
        if( numOutsideAddSpirals > 0 )
            numOutsideAddSpirals--;
    }

    
    else if( key == '6' )
        numOutsideAddSpirals++;
    */
    
    // 5 and 6 to decrement/increment other stuff
}

/*
    Load each CSV file here, or cycle to the next one
*/
void waterQualityDataApp::loadCSVFiles() {
    
    
    //-- load files into vector array
    ofDirectory dir(ofToDataPath("csvfiles"));
    numCSVFiles = dir.listDir();
    dir.sort();
    cout << "num CSV files = " << numCSVFiles << endl;
    cout << "file list:"  << endl;
    
    csvFiles = dir.getFiles();
    
    for(int i=0; i< numCSVFiles; ++i)
        cout <<  "Index Num: " << i << " — Filename: " << csvFiles[i].getFileName() << endl;
    
    //-- load first CSV - will crash if we have no CSV files
    loadCSVData(csvFiles[currentFileIndex].getFileName());
}

void waterQualityDataApp::loadCSVData(string filename) {
    // Generate pathname into CSV diretor and load into an ofxCsv object
    ofxCsv csv;
    string path = ofToDataPath("csvfiles/");
    path.append(filename);
    csv.loadFile(path);
    
    numData = csv.numRows;
    data = new datum[numData];
    
    float x;
    float y;
    float z;
    
    // Set these out of range
    float minLat = 9999;
    float maxLat = -9999;
    float minLng = 9999;
    float maxLng = -9999;
    float lat, lng, alt;
    
    float dataSize = DEFAULT_SIZE;
    
    
    float minY = 9999;
    float maxY = -9999;
    float minX = 9999;
    float maxX = -9999;
    
    int minYIndex = 0;
    int maxYIndex = 0;
    
    int firstDataRow = 1;   // set to 0 if no header
    
    cout << "NUM COLS:" << "\n";
     cout << csv.numCols << "\n";
    cout << csv.numRows << "\n";

    
    float totalAlt = 0.0;
    float avgAlt = 0.0;
    
    //-- get minX, maxX, minY, maxY values
    for(int i=firstDataRow; i<numData; i++) {
        // extract the x and y, invert lat for perfect mapping
        lng = ofToFloat(csv.data[i][INPUT_COL_LNG]);
        lat = ofToFloat(csv.data[i][INPUT_COL_LAT]);
        
        // note: csv.numCols is incorrect, so we try...catch instead
        if( csv.data[i].size()  > 3 ) {
            alt = ofToFloat(csv.data[i][INPUT_COL_ALT]);
            
            cout << "avgAlt = " << alt << "\n";
            totalAlt += alt;
            cout << "totalAlt = " << totalAlt << "\n";
            has3DData = true;       // this will set it for each row, but otherwise we need a
        }
        else {
            alt = 0.0;
            has3DData = false;
        }

        
        x = getMercatorX(lng, canvasWidth);
        y = getMercatorY(lat, canvasHeight, canvasWidth);
        
        
        if( x < minX )
            minX = x;
        if( x > maxX )
            maxX = x;
        
        if( y < minY ) {
            minY = y;
            //minYIndex = i;
        }
        if( y > maxY ) {
            maxY = y;
            //maxYIndex = i;
        }
    }
    
    avgAlt = totalAlt / (numData -firstDataRow);
    cout << "avgAlt = " << avgAlt << "\n";
    
    cout << "minY line # = " << minYIndex + 1 << endl;
    cout << "maxY line # = " << maxYIndex + 1 << endl;
    
    cout << "minX = " << minX << endl;
    cout << "maxX = " << maxX << endl;
    cout << "minY = " << minY << endl;
    cout << "maxY = " << maxY << endl;
    adjustMinMaxXY( minX, maxX, minY, maxY );
    
    for(int i=1; i<numData; i++) {
        // extract the x and y, invert lat for perfect mapping
        lng = ofToFloat(csv.data[i][INPUT_COL_LNG]);
        lat = ofToFloat(csv.data[i][INPUT_COL_LAT]);
        
        
        // note: csv.numCols is incorrect, so we try...catch instead
        if( csv.data[i].size()  > 3 ) {
            alt = ofToFloat(csv.data[i][INPUT_COL_ALT]);
            z = alt - avgAlt;
            has3DData = true;       // this will set it for each row, but otherwise we need a
        }
        else {
            alt = 0.0;
            has3DData = false;
        }
    
        x = getMercatorX(lng, canvasWidth);
        y = getMercatorY(lat, canvasHeight, canvasWidth);

        
        x = adjustX(x,minX,maxX);
        y = adjustY(y,minY,maxY);
        
        
        string description = csv.getString(i, INPUT_COL_DESCRIPTION);
        //float dataSize =  ofToFloat(csv.data[i][INPUT_COL_SIZE]);
        
        float dataSize = 1;
        float zAdjust = 10;
        (data+i)->setVars(x, y, z * zAdjust, dataSize);
        
        setDataVariables( data+i, dataSize );
    }
    
    csv.clear();
}

//-- this is fucked
/*
void waterQualityDataApp::loadCSVData(string filename) {
    // Generate pathname into CSV diretor and load into an ofxCsv object
    ofxCsv csv;
    string path = ofToDataPath("csvfiles/");
    path.append(filename);
    csv.loadFile(path);
    
    numData = csv.numRows;
    data = new datum[numData];
    
    float x;
    float y;
    float z;
    
    // Set these out of range
    float minLat = 9999;
    float maxLat = -9999;
    float minLng = 9999;
    float maxLng = -9999;
    float lat, lng, alt;
    
    float dataSize = DEFAULT_SIZE;
    
    
    // start at 1 to skip header
    
    //-- get minX, maxX, minY, maxY values
    
    float totalX = 0;
    float totalY = 0;
    float totalZ = 0;
    
    
    
    float minY = 9999;
    float maxY = -9999;
    float minX = 9999;
    float maxX = -9999;
    
    // Pass 1: calcuate averages for each X, y
    for(int i=1; i<numData; i++) {
        // extract the x and y, invert lat for perfect mapping
        //cout << csv.data[i][INPUT_COL_LNG] << "\n";
         
        lng = ofToFloat(csv.data[i][INPUT_COL_LNG])/100;
        lat = ofToFloat(csv.data[i][INPUT_COL_LAT])/100;
        alt = ofToFloat(csv.data[i][INPUT_COL_ALT]);
        
        //-- do other types of mapping here
        x = getMercatorX(lng, canvasWidth);
        y = getMercatorY(lat, canvasHeight, canvasWidth);
        z = alt;
        
        x = adjustX(x,minX,maxX);
        y = adjustY(y,minY,maxY);
        
        cout << "x =  " << lng << endl;

        
        totalX += x;
        totalY += y;
        totalZ += z;
    }
    
    float avgX = totalX/(numData-1);
    float avgY = totalY/(numData-1);
    float avgZ = totalZ/(numData-1);
    
    
    cout << "total x =  " << totalX << endl;
    cout << "total y =  " << totalY << endl;
    cout << "total z =  " << totalZ << endl;
    cout << "avg x =  " << avgX << endl;
    cout << "avg y =  " << avgY << endl;
    cout << "avg z =  " << avgZ << endl;
    
    
    
    cout << "min x =  " << minX << endl;
    cout << "max x =  " << maxX << endl;
    cout << "min y =  " << minY << endl;
    cout << "max y =  " << maxY << endl;
    
    adjustMinMaxXY( minX, maxX, minY, maxY );
    
    // Pass 2: use averages to calc min and max x + y
    for(int i=1; i<numData; i++) {
        // extract the x and y, invert lat for perfect mapping
        //cout << csv.data[i][INPUT_COL_LNG] << "\n";
        
        lng = ofToFloat(csv.data[i][INPUT_COL_LNG])/100;
        lat = ofToFloat(csv.data[i][INPUT_COL_LAT])/100;
        alt = ofToFloat(csv.data[i][INPUT_COL_ALT]);
        
        //-- do other types of mapping here
        //-- this mapping seems messed up
        
        x = getMercatorX(lng, canvasWidth);// - avgX;
        y = getMercatorY(lat, canvasHeight, canvasWidth);// - avgY;
        
        x = adjustX(x,minX,maxX);
        y = adjustY(y,minY,maxY);

        //x = getMercatorX(lng, canvasWidth);
        //y = getMercatorY(lat, canvasHeight, canvasWidth);
 
         z = alt - avgZ;
        //cout << "mapping mercator" << endl;
        
 
        
        //x *= 1000;
        //y *= 1000;
        //z *= 6;
 
        
        //float dataSize =  ofToFloat(csv.data[i][INPUT_COL_SIZE]);
        dataSize = .1;
        (data+i)->setVars(x, y, z, dataSize);
        (data +i)->setDescription(csv.data[i][INPUT_COL_DESCRIPTION]);
        //(data +i)->setType(csv.data[i][INPUT_COL_TYPE]);
        
        setDataVariables( data+i, dataSize );
        
        if( x < minX )
            minX = x;
        if( x > maxX )
            maxX = x;
    }
    
   

    csv.clear();
}


*/

/*
void waterQualityDataApp::loadCSVData(string filename) {
    // Generate pathname into CSV diretor and load into an ofxCsv object
    ofxCsv csv;
    string path = ofToDataPath("csvfiles/");
    path.append(filename);
    csv.loadFile(path);
    
    numData = csv.numRows;
    data = new datum[numData];
    
    float x;
    float y;
    
    // Set these out of range
    float minLat = 9999;
    float maxLat = -9999;
    float minLng = 9999;
    float maxLng = -9999;
    float lat, lng;
    
    float dataSize = DEFAULT_SIZE;
    
    
    float minY = 9999;
    float maxY = -9999;
    float minX = 9999;
    float maxX = -9999;
    
    int minYIndex = 0;
    int maxYIndex = 0;
    
    //-- get minX, maxX, minY, maxY values
    for(int i=1; i<numData; i++) {
        // extract the x and y, invert lat for perfect mapping
        lng = ofToFloat(csv.data[i][INPUT_COL_LNG]);
        lat = ofToFloat(csv.data[i][INPUT_COL_LAT]);
        
        //-- do other types of mapping here
        x = getMercatorX(lng, canvasWidth);
        y = getMercatorY(lat, canvasHeight, canvasWidth);
        
        if( x < minX )
            minX = x;
        if( x > maxX )
            maxX = x;
        
        if( y < minY ) {
            minY = y;
            minYIndex = i;
        }
        if( y > maxY ) {
            maxY = y;
            maxYIndex = i;
        }
    }
   
    adjustMinMaxXY( minX, maxX, minY, maxY );
    
    for(int i=1; i<numData; i++) {
        // extract the x and y, invert lat for perfect mapping
        lng = ofToFloat(csv.data[i][INPUT_COL_LNG]);
        lat = ofToFloat(csv.data[i][INPUT_COL_LAT]);
        
        //-- do other types of mapping here
        x = getMercatorX(lng, canvasWidth);
        y = getMercatorY(lat, canvasHeight, canvasWidth);
        
        
        
      //  string description = csv.getString(i, INPUT_COL_DESCRIPTION);
       // float dataSize =  ofToFloat(csv.data[i][INPUT_COL_SIZE]);
        
        dataSize = .1;
        //(data+i)->setVars(x, y, z, dataSize);
        (data+i)->setVars(x, y, 0, dataSize);   // no Z (for now)
        
        //(data +i)->setDescription(csv.data[i][INPUT_COL_DESCRIPTION]);
        
        setDataVariables( data+i, dataSize );
    }
    
    csv.clear();
}
*/


//-- based on current data index. At this point, the size of the data should be set from column 5
//-- we can override these settings
void waterQualityDataApp::setDataVariables( datum *d, float dataSize ) {
    float dSize;
    
    switch( currentFileIndex ) {
        case FILE_BASINS:
           
            d->setMinRadius(1);
            //cout<<dataSize<<"\n";
            if( dataSize < 1 )
                dataSize = 1;
            
            d->setSizeMultiplier(10);
            dSize = dataSize/14;
            if( dSize < 2 )
                dSize = 2;
            
            d->setSize(dSize);
            break;
    }
}


void waterQualityDataApp::adjustMinMaxXY( float &minX, float &maxX, float &minY, float &maxY ) {
    // Adjust ranges
    float xRange = maxX - minX;
    float yRange = maxY - minY;

    cout << "xRange = " << xRange << endl;
    cout << "yRange = " << yRange << endl;


    if( xRange < yRange ) {
        cout << "adjust X range" << endl;
        
        float r = yRange - xRange;
        minX -= r/2;
        maxX += r/2;
        
        cout << "r = " << r << endl;
    }
    else {
        cout << "adjust Y range" << endl;
        
        float r = xRange - yRange;
        minY -= r/2;
        maxY += r/2;
        
        cout << "r = " << r << endl;
    }


    cout << "minX = " << minX << endl;
    cout << "maxX = " << maxX << endl;
    cout << "minY = " << minY << endl;
    cout << "maxY = " << maxY << endl;
}


float waterQualityDataApp::getMercatorX(float lon, float width)
{
    // width is map width
    double x = fmod((width*(180+lon)/360), (width +(width/2)));
    
    return x;
}

float waterQualityDataApp::getMercatorY(float lat, float height, float width)
{
    // height and width are map height and width
    //double PI = 3.14159265359;
    double latRad = lat*PI/180;
    
    // get y value
    double mercN = log(tan((PI/4)+(latRad/2)));
    double y     = (height/2)-(width*mercN/(2*PI));
    return y;
}

// http://www.codeproject.com/Questions/626899/Converting-Latitude-And-Longitude-to-an-X-Y-Coordi

float waterQualityDataApp::getSinusoidalX(float lat, float lon, float width)
{
    // width is map width
    //double x = fmod((width*(180+lon)/360), (width +(width/2)));
    
    return width * cos(ofDegToRad(lat)) * cos(ofDegToRad(lon));
    //float x = width * cos(lat) * cos(lon)
    
    //return x;
}

float waterQualityDataApp::getSinusoidalY(float lat, float lon, float height, float width)
{
    //return 0.0;
    return height * cos(ofDegToRad(lat)) * sin(ofDegToRad(lon));
    
    //float y = height* cos(lat) * sin(lon)
}



float waterQualityDataApp::adjustX(float x, float min, float max) {
    return map(x, min, max, CANVAS_MARGIN, canvasWidth-CANVAS_MARGIN );
}

float waterQualityDataApp::adjustY(float y, float min, float max) {
    return map(y, min, max, CANVAS_MARGIN, canvasHeight-CANVAS_MARGIN );
}


float waterQualityDataApp::map(float m, float in_min, float in_max, float out_min, float out_max) {
    return (m - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


