#pragma once

extern "C" {
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
}

enum VideoFrameError {
	//Core
	VF_ERR_OPENINPUT = 1,
	VF_ERR_FINDSTREAM,
	VF_ERR_STREAM_VIDEO,
	VF_ERR_STREAM_AUDIO,
	VF_ERR_TEXTURE,
	
	//Video
	VF_ERR_VIDEO_CODECNOTSUPPORTED,
	VF_ERR_VIDEO_CODECCOPY,
	VF_ERR_VIDEO_CODECOPEN,
	VF_ERR_VIDEO_PIXELBUFFERS,

	//Audio
	VF_ERR_AUDIO_CODECNOTSUPPORTED,
	VF_ERR_AUDIO_CODECCOPY,
	VF_ERR_AUDIO_CODECOPEN
};

class CVideoFrame : public CWidget {
public:
	CVideoFrame(CWindow* window, std::string name, int x, int y);
	CVideoFrame(CWindow* window, rapidjson::Value& vWidget);
	~CVideoFrame();
	void cleanup();

	void Render();
	void HandleEvent(SDL_Event& e);
	void Step();

	void SetVideo(std::string filePath);
	void SetRepeat(bool repeat);

	void loadVideo();
	void initVideoStream();
	void initAudioStream();

	void Play();
	void Stop();
	void Pause();

	//Event Callbacks?
	void PlaybackComplete(SDL_Event& e); //Callback function when playback finishes and repeat is disabled

private:
	SDL_Texture* VideoFrame = NULL;
	std::string FilePath;

	bool Ready = false;
	bool Paused, Repeat;
	int repeatCount;

	int ERROR_CODE = 0;
	AVFormatContext *pFormatCtx;
	std::vector<AVPacket> packets;
	AVPacket packet;
	bool getNext = true;
	int frameRes;

	//Video
	int videoStream;
	AVStream* vStream = NULL;
	AVCodecContext *vCodecCtx = NULL;
	AVFrame *vFrame = NULL;
	struct SwsContext *sws_ctx = NULL;

	//Pixel Buffers
	Uint8 *yPlane, *uPlane, *vPlane;
	size_t yPlaneSz, uvPlaneSz;
	int uvPitch;

	//Audio
	int audioStream;
	AVCodecContext *aCodecCtx = NULL;
	AVFrame *aFrame = NULL;

	bool PlaybackFinished;
	int lastFrame = 0;
	bool skipFrame = false;
};