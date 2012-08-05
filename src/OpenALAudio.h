#include <cstdio>
#include <string>
#include <vector>

#include "cinder/Cinder.h"
#include "cinder/Filesystem.h"

#ifdef CINDER_MAC
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace mndl { namespace openal {

class OpenALAudioExc : public std::exception
{
	public:
		OpenALAudioExc( const std::string &log ) throw()
		{
			strncpy( mMessage, log.c_str(), 512 );
		}

		virtual const char* what() const throw()
		{
			return mMessage;
		}

	private:
		char mMessage[ 513 ];
};

class OpenALAudio
{
	public:
		OpenALAudio( unsigned poly = 10 );
		~OpenALAudio();

		unsigned load( const ci::fs::path &filename );
		void play( unsigned bufferId, float volume = 1.f, bool looping = false );

	private:
		unsigned mPoly;
		ALCcontext *mContext;
		ALCdevice *mDevice;
		std::vector< unsigned > mSources;
		unsigned mNextSource;

		void loadOgg( const ci::fs::path &filename, FILE *f, ALuint buffer );
};

} } // namespace mndl::openal
