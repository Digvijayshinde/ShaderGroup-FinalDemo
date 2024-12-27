#include"../../Source/Common.h"

class Scene {
public:
	Scene() {};

	// scene 3
	void WndProcForSceneOpening(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);
	int initialiseSceneOpening();
	void displaySceneOpening();
	void updateSceneOpening();
	void unitializeSceneOpening();

	//scene 1
	void WndProcForSceneOne(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);
	int initialiseSceneFirst();
	void displaySceneFirst();
	void updateSceneFirst();
	void unitializeSceneFirst();

	//scene 2
	void WndProcForSceneTwo(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);
	int initialiseSceneSecond();
	void displaySceneSecond();
	void updateSceneSecond();
	void unitializeSceneSecond();

	// scene 3
	void WndProcForSceneThird(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);
	int initialiseSceneThird();
	void displaySceneThird();
	void updateSceneThird();
	void unitializeSceneThird();

	// scene End
	void WndProcForSceneEnd(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam);
	int initialiseSceneEnd();
	void displaySceneEnd();
	void updateSceneEnd();
	void unitializeSceneEnd();
};

