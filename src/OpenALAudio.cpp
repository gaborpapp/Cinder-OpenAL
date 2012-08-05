#include <vorbis/vorbisfile.h>

#include "cinder/app/AppBasic.h"

#include "OpenALAudio.h"

using namespace ci;
using namespace std;

namespace mndl { namespace openal {

OpenALAudio::OpenALAudio( unsigned poly ) :
	mPoly( poly ),
	mNextSource( 0 )
{
	// initialization
	mDevice = alcOpenDevice( NULL ); // select the "preferred device"

	if ( mDevice == NULL )
		throw OpenALAudioExc( "Could not open audio device" );

	mContext = alcCreateContext( mDevice, NULL );
	if ( mContext == NULL )
		throw "Could not open context " + string( alGetString( alcGetError( mDevice ) ) );
	alcMakeContextCurrent( mContext );

	alListener3f( AL_POSITION, 0.f, 0.f, 0.f );

	// generate the sources
	for ( int i = 0; i < mPoly; ++i )
	{
		unsigned id;
		alGenSources( 1, &id );
		mSources.push_back( id );
	}
}

OpenALAudio::~OpenALAudio()
{
    // TODO: clean up buffers and sources
    // alDeleteBuffers( 1, &bufferId );
	alDeleteSources( mPoly, &(mSources[ 0 ]) );

	alcDestroyContext( mContext );
	alcCloseDevice( mDevice );
}

void OpenALAudio::play( unsigned bufferId, float volume /* = 1.f */, bool looping /* = false */ )
{
	// find the next free source
	int source = mNextSource;
    for ( int i = 0; i < mPoly; ++i, ++source )
	{
		if ( source >= mPoly )
			source = 0;

        // query the state of the source
		ALint state;
        alGetSourcei( mSources[ source ], AL_SOURCE_STATE, &state );
		if ( state != AL_PLAYING )
		{
			break;
		}
	}
	unsigned sourceId = mSources[ source ];

	alSourceStop( sourceId );
	alSource3f( sourceId, AL_POSITION, 0.f, 0.f, 0.f );
	alSourcei( sourceId, AL_BUFFER, bufferId );
	alSourcef( sourceId, AL_GAIN, volume );
	alSourcei( sourceId, AL_LOOPING, looping ? AL_TRUE : AL_FALSE );

	alSourcePlay( sourceId );
	mNextSource++;
	if ( mNextSource >= mPoly )
		mNextSource = 0;
}

unsigned OpenALAudio::load( const fs::path &filename )
{
    ALuint bufferId = 0;
    alGenBuffers( 1, &bufferId );

    FILE *file = fopen( filename.string().c_str(), "rb" );
    if ( !file )
		throw OpenALAudioExc( "Error opening " + filename.string() );

	string extension = filename.extension().string();

	if ( extension == ".ogg" )
	{
		loadOgg( filename, file, bufferId );
	}
	else
	{
		throw OpenALAudioExc( "Unknown file format " + extension );
	}

	return bufferId;
}

void OpenALAudio::loadOgg( const fs::path &filename, FILE *f, ALuint buffer )
{
    vorbis_info *info;
    OggVorbis_File oggFile;

    if ( ov_open( f, &oggFile, NULL, 0 ) != 0 )
	{
		fclose( f );
        throw OpenALAudioExc( "Error opening ogg " + filename.string() );
	}

    info = ov_info( &oggFile, -1 );

	int64_t len = ov_pcm_total( &oggFile, -1 ) * info->channels * 2; // always 16 bit
	char *data = new char[ len ];

	int bs = -1;
	int64_t todo = len;
	char *dst = data;
#ifdef CINDER_LITTLE_ENDIAN
	const int oggEndian = 0;
#else
	const int oggEndian = 1;
#endif

	while ( todo )
	{
		long read = ov_read( &oggFile, dst, todo, oggEndian, 2, 1, &bs );
		todo -= read;
		dst += read;
	}

	alBufferData( buffer, ( info->channels == 1 ) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
				  data, len, info->rate);

	delete data;
}


} } // mndl::openal

