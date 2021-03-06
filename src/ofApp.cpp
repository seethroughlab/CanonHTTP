#include "ofApp.h"
#include <Poco/Net/DNS.h>

// --------------------------------------------------------
ofApp::~ofApp(){
    ofLogNotice("ofApp") << endl << "Shutting down..." << endl;
    camera.close();
    s->stop();
    delete s;
}

//--------------------------------------------------------------
string ofGetEnv( std::string const & key ) {
    char * val = getenv( key.c_str() );
    return val == NULL ? string("") : string(val);
}

//--------------------------------------------------------------
long long ofEpochTime() {
    Poco::Timestamp epoch(0);
    Poco::Timestamp now;
    Poco::Timestamp::TimeDiff diffTime = (now - epoch);
    long long millisSince1970 = (long long) (diffTime/1000);
    return millisSince1970;
}

//--------------------------------------------------------------
string ofApp::basename() {
    return path.toString();
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetDataPathRoot("../Resources/data/");
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(0);
    ofSetCircleResolution(50);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    
    
    nextConnectAttempt = ofGetElapsedTimef();
    windowShaped=false;
    
    ofSetWindowTitle("CanonHTTP v"+string(VERSION));
    
    font.load("fonts/OpenSans-Bold.ttf", 48);
    
    string hostname = Poco::Net::DNS::hostName();
    station = hostname.substr(0, hostname.find('.'));
    
    path = Poco::Path::home();
    path.pushDirectory("Pictures");
    path.pushDirectory("CanonMedia");
    ofDirectory::createDirectory(path.toString(), false, true);
    
    
    CanonRequestHandlerFactory* factory = new CanonRequestHandlerFactory();
    s = new Poco::Net::HTTPServer(factory, ServerSocket(PORT), new HTTPServerParams);
    s->start();
}

//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    
    stringstream basename;
    basename << station << "-" << ofEpochTime();
    path.setFileName(basename.str());
    
    if(!camera.isConnected() && now>nextConnectAttempt) {
        camera.setup();
        nextConnectAttempt = now + 5;
    } else
        camera.update();
    
    if(!windowShaped && camera.isLiveDataReady()) {
        ofSetWindowShape(camera.getWidth(), camera.getHeight());
        windowShaped=true;
    }
}

//--------------------------------------------------------------
void ofApp::drawCentered(string s, int y) {
    int w = font.getStringBoundingBox(s, 0, 0).getWidth();
    int x = (ofGetWidth()/2.0) - (w/2.0);
    font.drawString(s, x, y);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(camera.isLiveDataReady()) {
        
        ofSetColor(ofColor::white);
        camera.draw(0, 0);
        
        stringstream status;
        status << camera.getWidth() << "x" << camera.getHeight() << " @ " <<
        (int) ofGetFrameRate() << " app-fps / " <<
        (int) camera.getFrameRate() << " cam-fps / " <<
        (camera.getBandwidth() / (1<<20)) << " MiB/s" << endl;
        status << basename();
        ofDrawBitmapStringHighlight(status.str(), 10, 20);
    }
    
    if(camera.isConnected()) {
        if(camera.isRecordingMovie() && ofGetElapsedTimeMillis() % 1000 > 500) {
            ofSetColor(ofColor::red);
            ofDrawCircle(ofGetWidth()-30, 30, 20);
        }
    } else {
        ofSetColor(ofColor::red);
        drawCentered("Camera Not Attached", ofGetHeight()*0.5);
    }
    
    ofSetColor(ofColor::white);
    int y = 70;
    for(int i = 0; i<messages.size(); i++) {
        ofDrawBitmapString(messages[i], 10, y);
        y += 14;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='s') {
        ofLogNotice() << "Attemping to connect to camera";
        camera.setup();
    }
    if(key=='c') {
        ofLogNotice() << "Closing camera";
        camera.close();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    string message = ofGetTimestampString()+" "+msg.message;
    messages.push_back(message);
    if(messages.size() > 30) {
        messages.erase(messages.begin());
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
