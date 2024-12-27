#pragma once
#include "../Utility/Headers/stb_image.h"
#include<windows.h>
#include<stdio.h>    //for FILE 
#include<stdlib.h>        //for exit();
#include"../vmath.h" 
#include<gl/glew.h>	//this line must be before the gl.h
#include<GL/gl.h>
#include <gl/wglew.h>
#include <iostream>

#include <vector>
#include <string>

using namespace vmath;
using namespace std;
//Common header file
#include "../Renderres/Headers/Renderers.h"					//Renderer header file common for all renderes.
#include "../Utility/Headers/TextureManager.h"				
#include "../Utility/Headers/ShaderManager.h"      

//OpenGL Library
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")

//Video rendering library
#pragma comment(lib, "Libraries/ffmpeg/lib/avcodec.lib")
#pragma comment(lib, "Libraries/ffmpeg/lib/avdevice.lib")
#pragma comment(lib, "Libraries/ffmpeg/lib/avfilter.lib")
#pragma comment(lib, "Libraries/ffmpeg/lib/avformat.lib")
#pragma comment(lib, "Libraries/ffmpeg/lib/avutil.lib")
#pragma comment(lib, "Libraries/ffmpeg/lib/swresample.lib")
#pragma comment(lib, "Libraries/ffmpeg/lib/swscale.lib")






