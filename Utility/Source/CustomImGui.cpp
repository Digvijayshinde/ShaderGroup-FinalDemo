#include "../../Source/Globals.h"
#include"../Headers/CustomImGui.h"

//ImGui initialization
int ImGuiInitialization() {

	//code
	if (ImGui::CreateContext() == NULL) {
		fprintf(gpFile, "Failed to  imgui load context\n");
		return -1;
	}
	if (ImGui_ImplWin32_InitForOpenGL(gHwnd) == FALSE) {
		fprintf(gpFile, "Filed to load init for opengl");
		return -1;
	}
	if (ImGui_ImplOpenGL3_Init("#version 460") == FALSE) {
		fprintf(gpFile, "Filed to ImGui_ImpOpenGL3_Init");
		return -1;
	}
	ImGui::StyleColorsDark();
	return 0;
}

void displayImGui() {


	switch (currentSceneNumber) {
	case 1:
		UiForSceneOne();
		break;
	case 2:
		UiForSceneTwo();
		break;
	case 3:
		UiForSceneThree();
		break;
	}
}
void displayPrepareForImGui() {

	//Change
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

}
void renderingImGui() {

	ImGui::BulletText("Position: X = %.2f, Y = %.2f, Z = %.2f", freeCamera->position[0], freeCamera->position[1], freeCamera->position[2]);
	ImGui::BulletText("Yaw: %.2f", freeCamera->yaw);
	ImGui::BulletText("Pitch: %.2f", freeCamera->pitch);
	ImGui::SliderFloat("Speed", &freeCamera->movementSpeed, 0.1, 100.0f);
	ImGui::SliderFloat("Sensitivity", &freeCamera->mouseSensitivity, 0.1, 1.0f);

	ImGui::Render();


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void UiForSceneOne() {

	ImGui::Begin("Scene 1");
	{
	
	}
	ImGui::End();

}

void UiForSceneTwo() {

	ImGui::Begin("Scene 2");
	{

	}
	ImGui::End();

}

void UiForSceneThree() {

	ImGui::Begin("Scene 3");
	{

	}
	ImGui::End();

}


