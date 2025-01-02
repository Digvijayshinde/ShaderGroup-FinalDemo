#pragma once

//ImGui  required  header
#include "../../Libraries/ImGui/imgui.h"
#include "../../Libraries/ImGui/imgui_impl_win32.h"
#include "../../Libraries/ImGui/imgui_impl_opengl3.h"
#include "../../../Scene/Headers/Scene.h"				//scene header file

int ImGuiInitialization();
void displayPrepareForImGui();
void displayImGui();
void renderingImGui(Scene scene);
void UiForSceneOne();
void UiForSceneTwo();
void UiForSceneThree();
