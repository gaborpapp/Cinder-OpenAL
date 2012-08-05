#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

#include "OpenALAudio.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class OpenALTest : public AppBasic
{
	public:
		void setup();

		void keyDown( KeyEvent event );

		void draw();

	private:

		mndl::openal::OpenALAudio mAudio;
		unsigned mSampleId;
};

void OpenALTest::setup()
{
	mSampleId = mAudio.load( getAssetPath( "sample.ogg" ) );
}

void OpenALTest::draw()
{
	gl::clear( Color::black() );

	gl::drawStringCentered( "Press any key to play the sample", getWindowSize() / 2.0 );
}

void OpenALTest::keyDown( KeyEvent event )
{
	switch ( event.getCode() )
	{
		case KeyEvent::KEY_ESCAPE:
			quit();
			break;

		default:
			mAudio.play( mSampleId );
			break;
	}
}

CINDER_APP_BASIC( OpenALTest, RendererGl() )

