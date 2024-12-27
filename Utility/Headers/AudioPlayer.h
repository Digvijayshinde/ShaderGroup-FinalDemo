
#include"../../Source/Globals.h"
// Alsa header files
#include <al.h>
#include <alc.h>



#define AUDIO_DIR   "Media/Audio/"
#define NUM_AUDIO 2

static const char* szAudios[] = {
    "ver8.wav",
    "HawanMantra.wav"
    "\0",
};

int initializeAudio(const char*);
void WndProcForAudio(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);
int playSong(int);

void playAudio(void);

void pauseAudio(void);

void resumeAudio(void);

void stopAudio(void);

void AudioKeyboard(char key, UINT keycode = VK_NONAME);

FLOAT getCurrentPosition(ALuint source);

void skipForward(ALuint source, FLOAT seconds);

void rewind(ALuint source, FLOAT seconds);

void uninitializeAudio(void);

void seekAudio(FLOAT seconds);
void toggleMute();

void togglePlayback();

