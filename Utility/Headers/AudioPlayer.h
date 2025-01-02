
#include"../../Source/Globals.h"
// Alsa header files
#include <al.h>
#include <alc.h>



#define AUDIO_DIR   "Media/Audio/"
#define NUM_AUDIO 8

static const char* szAudios[] = {
    "Scene2-ChanakyaInsult.wav",
    "HawanMantraAndChanakyaPratigya.wav",
    "SceneEnd.wav",
    "ChanakyaMeetChandragupt-Final.wav",
    "Scene0SkyViewVer2FadeOut.wav",
    "StrategyRoom.wav",
    "BattleField.wav",
    "FinalAudio.wav"
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

