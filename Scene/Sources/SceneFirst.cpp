#include"../../Source/Globals.h"
#include "../../Scene/Headers/Scene.h"
#include "../../Utility/Headers/ShaderManager.h"
#include "../../Utility/Headers/VideoRendering.h"
#include "../../Utility/Headers/AudioPlayer.h"

//variable declaration
static ShaderManager VideoRendershaderTexture;
static int initSongForSceneSecond = 0;

void Scene::WndProcForSceneOne(HWND hwnd, UINT imsg, WPARAM wParam, LPARAM lParam) {
	//code
}

int Scene::initialiseSceneFirst() {
	////code
	//	//code
	std::vector<std::string> shaderName;
	shaderName.push_back("Shaders\\CommomTexture\\CommonTexture.vert");
	shaderName.push_back("Shaders\\CommomTexture\\CommonTexture.frag");
	GLuint retVal = VideoRendershaderTexture.loadShaderAndCompileShader(shaderName);

	if (retVal != 0)
		return -1;


	if (!video_reader_open(&vr_state, "Media/Video/AMCBanner.mp4")) {
		PrintLog("unable to open video file \n");
		return 1;
	}
	else
		PrintLog("Video file open Successfully \n");


	constexpr int ALIGNMENT = 128;
	frame_width = vr_state.width;
	frame_height = vr_state.height;


	frame_data = (uint8_t*)malloc(frame_width * frame_height * 4);
	if (frame_data == NULL)
		PrintLog("allocating memory to file is failed \n");

	else
		PrintLog("allocation memory for video rendering is successfully \n");



	glGenTextures(1, &video_texture);
	glBindTexture(GL_TEXTURE_2D, video_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}
void Scene::displaySceneFirst() {
	//code
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	
	if (initSongForSceneSecond == 0) {
		if (AUDIO_ENABLE) {
			initSongForSceneSecond = 1;
			//playSong(1);
		}
	}
	//modelMatrix = genrateModelMatrix(transformationVector.translationVector, transformationVector.rotationVector, transformationVector.scaleVector);

	VideoRendershaderTexture.useProgram();
	VideoRendershaderTexture.setMat4("u_modelMatrix", modelMatrix);
	VideoRendershaderTexture.setMat4("u_viewMatrix", viewMatrix);
	VideoRendershaderTexture.setMat4("u_projectionMatrix", prespectiveProjectionMatrix);
	VideoRendershaderTexture.setInt("isFullScreen", 1);
	VideoRendershaderTexture.setInt("isFlipRequired", 1);

	//activating texture 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, video_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);

	VideoRendershaderTexture.setInt("u_textureSampler", 0);

	renderes->renderCube();

	//unuse the shader progarm object
	VideoRendershaderTexture.unUseProgram();
	glBindTexture(GL_TEXTURE_2D, 0);

}
void Scene::updateSceneFirst() {
		bool video_reader_read_frame(VideoReaderState * state, uint8_t * frame_buffer, int64_t * pts);

	int64_t pts;

	if (!video_reader_read_frame(&vr_state, frame_data, &pts)) {

		PrintLog("unable to laod the video file \n");
	}
}
void Scene::unitializeSceneFirst() {
		//unitialize the video rendering struct.
	if (vr_state.sws_scaler_ctx) {
		sws_freeContext(vr_state.sws_scaler_ctx);
		vr_state.sws_scaler_ctx = NULL;
	}
	if (vr_state.av_format_ctx) {
		avformat_close_input(&vr_state.av_format_ctx);
		vr_state.av_format_ctx = NULL;
	}
	if (vr_state.av_format_ctx) {
		avformat_free_context(vr_state.av_format_ctx);
		vr_state.av_format_ctx = NULL;
	}
	if (vr_state.av_frame) {
		av_frame_free(&vr_state.av_frame);
		vr_state.av_frame = NULL;
	}
	if (vr_state.av_packet) {
		av_packet_free(&vr_state.av_packet);
		vr_state.av_packet = NULL;
	}
	if (vr_state.av_codec_ctx) {
		avcodec_free_context(&vr_state.av_codec_ctx);
		vr_state.av_codec_ctx = NULL;
	}
	if (frame_data) {
		free(frame_data);
		frame_data = NULL;
	}
	//unitialize all shader
	VideoRendershaderTexture.uninitializeShader();
}