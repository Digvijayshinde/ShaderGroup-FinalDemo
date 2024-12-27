

// standard headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include"../../Source/Globals.h"
#include "../Headers/AudioPlayer.h"


bool gbPlayback = false;
static int songId = 0;
static unsigned int sourceid;
static ALCdevice* device = NULL;
static ALCcontext* context = NULL;
static unsigned char playing_stated = 0;

int initializeAudio(const char* audio_file)
{
    // function declaration 
    char* loadWAV(const char* , int* , int* , int*  , int* );
    void uninitializeAudio(void);

    int ret = -1;

    // code
    // check and stop, if already initialized
    uninitializeAudio();

    if(NULL == (device = alcOpenDevice(NULL)))
    {
       // AMC::Log::GetInstance()->WriteLogFile(__FUNCTION__, AMC::LOG_INFO, L"open Audio device failed\n");
        uninitializeAudio();
        return (ret);

    }
    else if(NULL == (context = alcCreateContext(device, NULL)))
    {
       // AMC::Log::GetInstance()->WriteLogFile(__FUNCTION__, AMC::LOG_INFO, L"alcCreateContext() failed\n");
        uninitializeAudio();
        return (ret);
    }
    else if(ALC_FALSE == alcMakeContextCurrent(context))
    {
       // AMC::Log::GetInstance()->WriteLogFile(__FUNCTION__, AMC::LOG_INFO, L"alcMakeContextCurrent() failed\n");
        uninitializeAudio();
        return (ret);
    }
    else
    {
        unsigned int bufferid, format;
        int channel, samplerate, bitspersecond, size=10000;
        char* data = loadWAV(audio_file, &channel, &samplerate, &bitspersecond, &size);
        if(!data)
        {
          //  AMC::Log::GetInstance()->WriteLogFile(__FUNCTION__, AMC::LOG_INFO, L"wav loading failed\n");
            uninitializeAudio();
            return (ret);
        }

        alGenBuffers(1, &bufferid);
        if(channel == 1)
        {
            if(bitspersecond == 8)
            {
                format = AL_FORMAT_MONO8;
            }
            else
            {
                format = AL_FORMAT_MONO16;
            }
        }
        else // channel 2
        {
            if(bitspersecond == 8)
            {
                format = AL_FORMAT_STEREO8;
            }
            else
            {
                format = AL_FORMAT_STEREO16;
            }
        }

        alBufferData(bufferid, format, data, size, samplerate);
        // NOTE: deleting sourceid result in stopping playback, declare this global scope
        // unsigned int sourceid;
        alGenSources(1, &sourceid);
        alSourcei(sourceid, AL_BUFFER, bufferid);
        // alSourcePlay(sourceid);
        // alDeleteSources(1, &sourceid);
        alDeleteBuffers(1, &bufferid);
        free(data);
        ret = 0;
    }

    return (ret);
}

char* loadWAV(const char* filename, int* channel, int* samplerate, int*  bitspersec, int* size)
{
    // function declaration
    int convertToInt(char* , int );

    // code
    char buffer[4] = {0};
    FILE* fp = NULL;
    fopen_s(&fp, filename, "rb");
    if (NULL == fp)
        fprintf(gpFile, "Failed to audio");
    else if (4 != fread(buffer, 1, 4, fp)) // riff
        fprintf(gpFile, "file read failed\n");
    else if (0 != strncmp("RIFF", buffer, 4))
        fprintf(gpFile, "invalid read\n");
    else if (4 != fread(buffer, 1, 4, fp)) // 
        fprintf(gpFile, "line: %d read failed\n");
    else if (4 != fread(buffer, 1, 4, fp)) // wave
        fprintf(gpFile, "wav read failed\n");
    else if (4 != fread(buffer, 1, 4, fp)) // "fmt "
        fprintf(gpFile, "fmt read failed\n");
    else if (4 != fread(buffer, 1, 4, fp)) // "10 00"
        fprintf(gpFile, "16 read failed\n");
    else if (2 != fread(buffer, 1, 2, fp)) // "00 00"
        fprintf(gpFile, "1 read failed\n");
    else if (2 != fread(buffer, 1, 2, fp)) // "01 00"
        fprintf(gpFile, "1-2 read failed\n");
    else if (0 == (*channel = convertToInt(buffer, 2))) // get channel
        fprintf(gpFile, "channel get failed: %d\n");
    else if (4 != fread(buffer, 1, 4, fp))
        fprintf(gpFile, "wav read failed\n");
    else if (0 == (*samplerate = convertToInt(buffer, 4)))
        fprintf(gpFile, "samplerate conversion failed: %d\n");
    else if (4 != fread(buffer, 1, 4, fp)) // fmt
        fprintf(gpFile, "line: %d read failed\n");
    else if (2 != fread(buffer, 1, 2, fp)) // 16
        fprintf(gpFile, "line: %d read failed\n");
    else if (2 != fread(buffer, 1, 2, fp)) // 1
        fprintf(gpFile, "line: %d read failed\n");
    else if(0 == (*bitspersec = convertToInt(buffer, 2))) // 1
        fprintf(gpFile, "bitspersec conversion failed:");
    else if(4 != fread(buffer, 1, 4, fp)) // data
        fprintf(gpFile, "line: %d read failed\n");
    else if(4 != fread(buffer, 1, 4, fp)) // 
        fprintf(gpFile, "line: %d read failed\n");
    else if(0 == (*size = convertToInt(buffer, 4))) // 1
        fprintf(gpFile, "size conversion failed: %d\n");
    else{
        char* data = (char*)malloc(*size);
        if (data == nullptr) {
            fprintf(gpFile, "Failed to allocate");
        }
        if (*size != fread(data, 1, *size, fp)) // 
            fprintf(gpFile, "Failed to readline");
        fclose(fp);    
        return data;
    }
    return (NULL);
}

unsigned char isBigEndian(void)
{
    int a = 1;
    return !((char*)&a)[0];
}

int convertToInt(char* buffer, int length)
{
    // function declaration
    unsigned char isBigEndian(void);

    // code
    int a = 0;
    if(!isBigEndian())
    {
        for(int i = 0; i < length; i++)
        {
            ((char*)&a)[i] = buffer[i];
        }
    }
    else
    {
        for(int i = 0; i < length; i++)
        {
            ((char*)&a)[3-i] = buffer[i];
        }
    }
    return a;
}

void playAudio()
{
    playing_stated = 1;

    alSourcePlay(sourceid);
}

void pauseAudio()
{
    if(playing_stated)
        alSourcePause(sourceid);
}

void resumeAudio()
{
    if(playing_stated)
        alSourcePlay(sourceid);
}

void stopAudio()
{
    alSourceStop(sourceid);
    playing_stated = 0;
}

int playSong(int songId)
{
    // code
    #ifdef AUDIO_ENABLE
 
      
        char audiopath[128] = { 0 };
        snprintf(audiopath, sizeof(audiopath), "%s%s", AUDIO_DIR, szAudios[songId]);
        if (initializeAudio(audiopath))
        {
            fprintf(gpFile,"Failed to laod audio");
            return (-1);
        }
        playAudio();
        gbPlayback = true;
    #endif // AUDIO_ENABLE
    return (0);
}

void togglePlayback()
{
    // code
    if (gbPlayback)
    {
        pauseAudio();
        gbPlayback = false;
    }
    else
    {
        resumeAudio();
        gbPlayback = true;
    }
}

void stopSong()
{

    // code
    if (gbPlayback) {
        stopAudio();
        gbPlayback = false;
    }

}

void AudioKeyboard(char key, UINT keycode)
{
    switch (key)
    {
        case 't':
            togglePlayback();
        break;
        case 'n':
            songId++;
            if (songId > NUM_AUDIO-1 )
                songId = 0;
            playSong(songId);
           
        break;
        case 'b':
            songId--;
            if (songId < 0)
                songId = NUM_AUDIO - 1;
            playSong(songId);
        break;
        case 's':
            stopSong();
            songId = -1;
            break;
        default:
            break;
    }

    switch (keycode)
    {
        case VK_SPACE:
            songId = 0;
            playSong(songId);
        break;
        case VK_RIGHT:
            skipForward(sourceid, 5.0f);
        break;
        case VK_LEFT:
            rewind(sourceid, 5.0f);
        break;
        default:
        break;
    }

}

FLOAT getCurrentPosition(ALuint source)
{
    FLOAT currentPosition = 0.0f;
    alGetSourcef(source, AL_SEC_OFFSET, &currentPosition);
    return currentPosition;
}

void seekAudio(FLOAT seconds)
{
    alSourcef(sourceid, AL_SEC_OFFSET, seconds);
}

void toggleMute()
{

}

void skipForward(ALuint source, FLOAT seconds)
{
    FLOAT currentPosition = getCurrentPosition(source);
    FLOAT newPosition = currentPosition + seconds;
    // Optionally, check if newPosition exceeds the duration of the audio and adjust accordingly
    // For example, let's assume 'duration' is the total length of the audio in seconds
    // float duration = getTotalDurationOfAudio(source); // Implement this function as needed
    // if (newPosition > duration) newPosition = duration;
    alSourcef(source, AL_SEC_OFFSET, newPosition);
}

void rewind(ALuint source, FLOAT seconds)
{
    FLOAT currentPosition = getCurrentPosition(source);
    FLOAT newPosition = currentPosition - seconds;
    if (newPosition < 0) newPosition = 0;
    alSourcef(source, AL_SEC_OFFSET, newPosition);
}

void uninitializeAudio(void)
{
    if(sourceid)
    {
        alDeleteSources(1, &sourceid);
        sourceid = 0;
    }
    if(context)
    {
        alcDestroyContext(context);
        context = NULL;
    }
    if(device)
    {
        alcCloseDevice(device);
        device = NULL;
    }
}


void WndProcForAudio(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {

    switch (wParam) {
    case 't':
        AudioKeyboard('t');
        break;
    case 'n':
        AudioKeyboard('n');
        break;
    }
}
