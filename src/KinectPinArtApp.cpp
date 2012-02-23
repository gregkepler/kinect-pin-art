#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/CinderMath.h"

#include "cinder/params/Params.h"
#include "cinder/Camera.h"

#include "cinder/qtime/MovieWriter.h"
#include "cinder/Utilities.h"

#include "Kinect.h"

using namespace ci;
using namespace ci::app;
using namespace std;


// TODO: define the window size in a #define variable
// TODO: store the depth surface in a channel instead of a Surface/Texture (Channel 32f)
//          Use channel.getValue( mLoc ); to get the gray value for that location of the image

#define FRAME_MAX 600

class KinectPinArtApp : public AppBasic {
public:
	void prepareSettings( Settings* settings );
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void keyDown( KeyEvent event );
	
	gl::Texture mDepthTexture;
	ImageSourceRef depthImage;
	Surface mDepthSurface;
	Surface mColorSurface;
	
	Kinect mKinect;
    int cols;
    int rows;
    vector<int> curDepth;
    
    // PARAMS
	params::InterfaceGl	mParams;
    
    // CAMERA
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	float				mCameraDistance;
	Vec3f				mEye, mCenter, mUp;
    float               mRot;
    
    qtime::MovieWriter movie;
    bool                recording;
    int                 mFrameCount;
};


void KinectPinArtApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 640, 480 );
    settings->setFrameRate( 60.0f );
}


void KinectPinArtApp::setup()
{
	console() << "There are " << Kinect::getNumDevices() << " Kinects connected." << std::endl;
	
    cols = 110;
    rows = 71;
    curDepth.resize(cols*rows, 0);
	
	mKinect = Kinect( Kinect::Device() );
    
    // SETUP CAMERA
	mCameraDistance = 400.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 2000.0f );
    
    // SETUP PARAMS
	mParams = params::InterfaceGl( "Flocking", Vec2i( 200, 160 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation, "open" );
	//mParams.addSeparator();
	//mParams.addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1500.0 step=50.0 keyIncr=a keyDecr=d" );
    recording = false;
    
}

void KinectPinArtApp::mouseDown( MouseEvent event )
{
}

void KinectPinArtApp::update()
{
    // UPDATE CAMERA
	mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate(mSceneRotation);
    
    
    
	if( mKinect.checkNewDepthFrame() ){
        
		depthImage = mKinect.getDepthImage();
		mDepthSurface = depthImage;
		
        
		if( mKinect.checkNewVideoFrame() )
			mColorSurface = mKinect.getVideoImage();
		
        
        
		// Get an array of depth data
		//int[] depthValues = kinect.depthMap();
		// mKinect.getDepthData();
		//depthData = mKinect.getDepthData();
		//uint16_t col3Row2Depth = depthData.get()[2*640+3];
		
		
		
		if(mDepthSurface){
			
		}
        
	}
}

void KinectPinArtApp::draw()
{
	//int i, j;
	//int amt;
	
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	if( mDepthSurface){
		//gl::draw( mDepthSurface,Vec2i( 0, 0 ));
        
        // go through the pixels of the rgb image and get the depth
        // if the depth is of a certain amount, show it
        
        /*
         
         Area area( 0, 0, mDepthSurface.getWidth(), mDepthSurface.getHeight() );
         Surface::Iter iter = mDepthSurface.getIter( area );
         
         
         while( iter.line() ) {
         while( iter.pixel() ) {
         if(iter.r() > 120){
         iter.r() = iter.g() = iter.b() = iter.r();
         Vec2i pos = iter.getPos();
         
         
         if(mColorSurface){
         //console() << mColorSurface.getPixel(pos);
         //Area colorArea( pos.x, pos.y, 1, 1);
         ColorA8u pixCol = mColorSurface.getPixel(pos);
         iter.r() = pixCol.r;
         iter.g() = pixCol.g;
         iter.b() = pixCol.b;					}
         }else{
         iter.r() = iter.g() = iter.b() = 0;
         }
         
         }
         }
         
         gl::draw( mDepthSurface,Vec2i( 0, 0 ));
         }*/
        
        
        // if the depth data is over a certain threshold
        
        // then show that pixel
        
        
        
        
        // STEPS
        // create a grid of cylinders
        // for each cyinder's corresponding x and y, get the depth value (or average depth value)
        // set teh z position based on that depth value
        // if there is no depth value, then set it all the way back (set a default number for that)
        
        
        glEnable (GL_DEPTH_TEST);
        
        double cylW = 2;
        int cols = 110;
        int rows = 71;
        int amt = 0;
        float maxW = cols * cylW*2;
        float maxH = rows * cylW*2;
        
        glPushMatrix();
        glRotatef(180, 0, 0, 1);
        //glTranslated(-getWindowSize().x/2, -getWindowSize().y/2, 0);
        glTranslated(-(maxW/2), -(maxH/2), 0);
        
        
        //cols
        for(int i=0; i<cols; i++){
            // rows
            for(int j=0; j<rows; j++){
                
                bool even = (j%2==0);
                float xPos, yPos, zPos, depthVal, oldDepth;
                
                
                if(i == (cols-1) && even){
                    continue; 
                }
                
                oldDepth = curDepth[amt];
                
                if(even){
                    xPos =((cylW*2) * i);
                }else{
                    xPos =((cylW*2) * i) - cylW; 
                }
                
                yPos = (j*(cylW*2));
                
                Vec2i pos = Vec2i(xPos, yPos);
                Color8u pixCol = mDepthSurface.getPixel(pos);
                
                int d = pixCol.r;
                //console() << "depth: " << pixCol << endl;
                
                //if d>
                
                //depthVal = -50+ (pixCol.r * 150);
                depthVal = lmap<float>(pixCol.r, 0, 255, -50, 150);
                if(depthVal < 16){
                    depthVal = 16;
                }else if(depthVal > 230){
                    depthVal = 230;
                }
                
                zPos = oldDepth + (depthVal - oldDepth) * .8;
                //zPos = depthVal;
                
                glPushMatrix();
                
                glTranslated(xPos, yPos, zPos);
                
                
                //glTranslated(0, 0, -40);
                glRotatef(90, 1, 0, 0);
                glColor3f(0.5, 0.5, 0.5);
                gl::drawCylinder(cylW, cylW, 100, 20);
                
                glRotatef(-90, 1, 0, 0);
                glTranslated(0, 0, 100);
                glColor3f(1.0, 1.0, 1.0);
                //glColor3f(pixCol.r, pixCol.g, pixCol.b);
                
                gl::drawSolidCircle(Vec2f(0,0), cylW);
                
                glPopMatrix();
                
                curDepth[amt] = zPos;
                amt++;
            }
        }
        glPopMatrix();
        
        
        
    }
    
    // DRAW PARAMS WINDOW
	params::InterfaceGl::draw();
    
    
    
    if(recording){
        movie.addFrame( copyWindowSurface(), 0.1f);
        mFrameCount++;
        if(mFrameCount > FRAME_MAX){
            movie.finish();
            recording = false;
        }
    }
}


void KinectPinArtApp::keyDown( KeyEvent event ) 
{
    if( event.getChar() == 'm' ){
        if(recording){
            recording = false;
            movie.finish();
        }else{
            recording = true;
            mFrameCount = 0;
            
            qtime::MovieWriter::Format format;
            format.setCodec( qtime::MovieWriter::CODEC_JPEG );
            format.setQuality( 0.5f );
            movie = qtime::MovieWriter( getDocumentsDirectory() + "KinectPinArt_1.mov", 640, 480, format );
        }
    } 
}


CINDER_APP_BASIC( KinectPinArtApp, RendererGl )
