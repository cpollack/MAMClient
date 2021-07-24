#include "stdafx.h"
#include "Widget.h"
#include "VideoFrame.h"
#include "Global.h"

CVideoFrame::CVideoFrame(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;
}

CVideoFrame::CVideoFrame(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	//if (vWidget.HasMember("SkinImage")) SkinImage = vWidget["SkinImage"].GetString();
}

CVideoFrame::~CVideoFrame() {
	cleanup();
}

void CVideoFrame::cleanup() {
	//Free video and audio assets
	if (VideoFrame) SDL_DestroyTexture(VideoFrame);

	// Free the YUV frame
	av_frame_free(&vFrame);
	av_frame_free(&aFrame);
	if (yPlane) delete[] yPlane;
	if (uPlane) delete[] uPlane;
	if (vPlane) delete[] vPlane;

	sws_freeContext(sws_ctx);

	// Close the codec
	avcodec_close(vCodecCtx);
	avcodec_close(aCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);
}

void CVideoFrame::Render() {
	if (!Visible) return;
	
	if (!Ready) return;
	//Started, Playing, Paused ?

	int fps = 30;
	int ticksPerFrame = 1000 / fps;

	int curTick = SDL_GetTicks();
	int elapsed = curTick - lastFrame;
	/*if (lastFrame > 0 && elapsed < ticksPerFrame) {
		SDL_RenderCopy(renderer, VideoFrame, NULL, &widgetRect);
		return;
	}*/
	lastFrame = curTick;

	bool frameHandled = false;
	uvPitch = vCodecCtx->width / 2;

	//new test
	/*int vidStreamCount = packets.size() ? 1 : 0;
	while (true) {
		if (av_read_frame(pFormatCtx, &packet) >= 0) {
			if (packet.stream_index == videoStream) vidStreamCount++;
			packets.push_back(packet);
			if (vidStreamCount > 1) break;
		}
		else break;
	}
	if (packets.size() == 0) {
		if (Repeat) {
			repeatCount++;
			av_seek_frame(pFormatCtx, videoStream, 0, AVSEEK_FLAG_FRAME);
		}
		else {
			if (repeatCount == 0) {
				repeatCount++;
				PlaybackFinished = true;
			}
		}
	}
	else {
		vidStreamCount = 0;
		while (packets.size()) {
			packet = packets[0];

			// Update Video Frame
			if (packet.stream_index == videoStream) {
				if (vidStreamCount > 1) break;
				packets.erase(packets.begin());

				// Decode video frame
				avcodec_send_packet(vCodecCtx, &packet);
				avcodec_receive_frame(vCodecCtx, vFrame);

				AVFrame frame;
				frame.data[0] = yPlane;
				frame.data[1] = uPlane;
				frame.data[2] = vPlane;
				frame.linesize[0] = vCodecCtx->width;
				frame.linesize[1] = uvPitch;
				frame.linesize[2] = uvPitch;

				// Convert the image into YUV format that SDL uses
				sws_scale(sws_ctx, (uint8_t const* const*)vFrame->data,
					vFrame->linesize, 0, vCodecCtx->height, frame.data,
					frame.linesize);

				SDL_UpdateYUVTexture(VideoFrame, NULL, yPlane, vCodecCtx->width, uPlane, uvPitch, vPlane, uvPitch);

				frameHandled = true;
				skipFrame = true;
			}

			//Update Audio Frame
			if (packet.stream_index == audioStream) {
				packets.erase(packets.begin());
				if (!avcodec_send_packet(aCodecCtx, &packet)) {
					avcodec_receive_frame(aCodecCtx, aFrame);
					SDL_QueueAudio(audioDevice, aFrame->data[0], aFrame->linesize[0]);
				}
			}

			// Free the packet that was allocated by av_read_frame
			av_packet_unref(&packet);
		}
	}*/


	// Tutorial 5 - how to sync audio and video
	// http://dranger.com/ffmpeg/ffmpegtutorial_all.html
	// Not a priority fix for now

	while (true) {
		if (av_read_frame(pFormatCtx, &packet) >= 0) {
			// Update Video Frame
			if (packet.stream_index == videoStream) {
				// Decode video frame
				avcodec_send_packet(vCodecCtx, &packet);
				avcodec_receive_frame(vCodecCtx, vFrame);

				AVFrame frame;
				frame.data[0] = yPlane;
				frame.data[1] = uPlane;
				frame.data[2] = vPlane;
				frame.linesize[0] = vCodecCtx->width;
				frame.linesize[1] = uvPitch;
				frame.linesize[2] = uvPitch;

				// Convert the image into YUV format that SDL uses
				sws_scale(sws_ctx, (uint8_t const * const *)vFrame->data,
					vFrame->linesize, 0, vCodecCtx->height, frame.data,
					frame.linesize);

				SDL_UpdateYUVTexture(VideoFrame, NULL, yPlane, vCodecCtx->width, uPlane, uvPitch, vPlane, uvPitch);

				frameHandled = true;
				skipFrame = true;
			}

			//Update Audio Frame
			if (packet.stream_index == audioStream) {
				if (!avcodec_send_packet(aCodecCtx, &packet)) {
					avcodec_receive_frame(aCodecCtx, aFrame);
					SDL_QueueAudio(audioDevice, aFrame->data[0], aFrame->linesize[0]);
				}
			}

			// Free the packet that was allocated by av_read_frame
			av_packet_unref(&packet);
			if (frameHandled) break;
		}
		else {
			//End of playback
			if (Repeat) {
				repeatCount++;
				av_seek_frame(pFormatCtx, videoStream, 0, AVSEEK_FLAG_FRAME);
			}
			else {
				if (repeatCount == 0) {
					repeatCount++;
					PlaybackFinished = true;
				}
				break;
			}
		}
	}
	
	SDL_RenderCopy(renderer, VideoFrame, NULL, &widgetRect);
}

void CVideoFrame::HandleEvent(SDL_Event& e) {
	//
}

void CVideoFrame::Step() {
	if (PlaybackFinished && !Repeat) {
		SDL_Event e;
		PlaybackComplete(e);
		PlaybackFinished = false;
	}
}

void CVideoFrame::SetVideo(std::string filePath) {
	if (filePath.length() == 0) return;
	if (FilePath.compare(filePath) == 0) return;

	//Check and clear existing video?
	if (FilePath.length()) cleanup();

	FilePath = filePath;
	widgetRect = { X, Y, Width, Height };
	loadVideo();
}

void CVideoFrame::SetRepeat(bool repeat) {
	Repeat = repeat;
}

void CVideoFrame::loadVideo() {
	repeatCount = 0;
	Ready = false;
	PlaybackFinished = false;
	lastFrame = 0;
	skipFrame = false;
	getNext = true;

	SDL_ClearQueuedAudio(audioDevice);

	ERROR_CODE = 0;
	int avError;
	avError = avformat_open_input(&pFormatCtx, FilePath.c_str(), NULL, NULL);
	if (avError) {
		ERROR_CODE = VF_ERR_OPENINPUT;
		std::cout << "AV Open Input Error: " << avError << std::endl;
		return;
	}

	// Retrieve stream information
	avError = avformat_find_stream_info(pFormatCtx, NULL);
	if (avError) {
		ERROR_CODE = VF_ERR_FINDSTREAM;
		std::cout << "AV Open Input Error: " << avError << std::endl;
		return;
	}

	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, FilePath.c_str(), 0);

	// Find the first video stream
	videoStream = -1;
	audioStream = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
		int decoderType = avcodec_find_decoder(pFormatCtx->streams[i]->codecpar->codec_id)->type;
		if (decoderType == AVMEDIA_TYPE_VIDEO) {
			pFormatCtx->streams[i]->r_frame_rate;
			videoStream = i;
		}
		if (decoderType == AVMEDIA_TYPE_AUDIO) {
			audioStream = i;
		}
		if (videoStream && audioStream) break;
	}
	if (videoStream == -1) {
		ERROR_CODE = VF_ERR_STREAM_VIDEO;
		std::cout << "AV Video stream not Found.\n";
		return; // Didn't find a video stream
	}
	/*if (audioStream == -1) {
		ERROR_CODE = VF_ERR_STREAM_AUDIO;
		std::cout << "AV Audio stream not Found.\n";
		return; // Didn't find a video stream
	}*/

	initVideoStream();
	initAudioStream();

	if (!ERROR_CODE) Ready = true;
}

void CVideoFrame::initVideoStream() {
	// Get a pointer to the codec context for the video stream
	AVCodecParameters *pCodecParOrig = NULL;
	
	vStream = pFormatCtx->streams[videoStream];
	pCodecParOrig = vStream->codecpar;
	//vStream->r_frame_rate.num = 60;
	vStream->time_base.num = 60;

	// Find the decoder for the video stream
	AVCodec *pCodec = NULL;
	pCodec = avcodec_find_decoder(pCodecParOrig->codec_id);
	if (pCodec == NULL) {
		ERROR_CODE = VF_ERR_VIDEO_CODECNOTSUPPORTED;
		fprintf(stderr, "Unsupported codec!\n");
		return; // Codec not found
	}

	// Copy context
	vCodecCtx = avcodec_alloc_context3(pCodec);
	if (avcodec_parameters_to_context(vCodecCtx, pCodecParOrig) != 0) {
		ERROR_CODE = VF_ERR_VIDEO_CODECCOPY;
		fprintf(stderr, "Couldn't copy codec context");
		return; // Error copying codec context
	}

	// Open codec
	if (avcodec_open2(vCodecCtx, pCodec, NULL) < 0) {
		ERROR_CODE = VF_ERR_VIDEO_CODECOPEN;
		fprintf(stderr, "Couldn't open codec");
		return; // Could not open codec
	}

	//AVRational framerate = vCodecCtx->framerate;

	// Allocate video frame
	vFrame = av_frame_alloc();

	// Allocate a place to put our YUV image on that screen
	VideoFrame = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_YV12,
		SDL_TEXTUREACCESS_STREAMING,
		vCodecCtx->width,
		vCodecCtx->height
	);
	if (!VideoFrame) {
		ERROR_CODE = VF_ERR_TEXTURE;
		fprintf(stderr, "SDL: could not create texture - exiting\n");
		return;
	}
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, VideoFrame);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, priorTarget);

	// initialize SWS context for software scaling
	sws_ctx = sws_getContext(vCodecCtx->width, vCodecCtx->height,
		vCodecCtx->pix_fmt, vCodecCtx->width, vCodecCtx->height,
		AV_PIX_FMT_YUV420P,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL);

	// set up YV12 pixel array (12 bits per pixel)
	yPlaneSz = vCodecCtx->width * vCodecCtx->height;
	uvPlaneSz = vCodecCtx->width * vCodecCtx->height / 4;
	yPlane = new Uint8[yPlaneSz];
	uPlane = new Uint8[uvPlaneSz];
	vPlane = new Uint8[uvPlaneSz];
	if (!yPlane || !uPlane || !vPlane) {
		ERROR_CODE = VF_ERR_VIDEO_PIXELBUFFERS;
		fprintf(stderr, "Could not allocate pixel buffers - exiting\n");
		return;
	}
}

void CVideoFrame::initAudioStream() {
	if (audioStream == -1) return;
	// Get a pointer to the codec context for the video stream
	AVCodecParameters *aCodecParOrig = NULL;
	aCodecParOrig = pFormatCtx->streams[audioStream]->codecpar;

	// Find the decoder for the video stream
	AVCodec *aCodec = NULL;
	aCodec = avcodec_find_decoder(aCodecParOrig->codec_id);
	if (aCodec == NULL) {
		ERROR_CODE = VF_ERR_AUDIO_CODECNOTSUPPORTED;
		fprintf(stderr, "Unsupported codec!\n");
		return; // Codec not found
	}

	// Copy context
	aCodecCtx = avcodec_alloc_context3(aCodec);
	if (avcodec_parameters_to_context(aCodecCtx, aCodecParOrig) != 0) {
		ERROR_CODE = VF_ERR_AUDIO_CODECCOPY;
		fprintf(stderr, "Couldn't copy codec context");
		return; // Error copying codec context
	}

	// Open codec
	if (avcodec_open2(aCodecCtx, aCodec, NULL) < 0) {
		ERROR_CODE = VF_ERR_AUDIO_CODECOPEN;
		fprintf(stderr, "Couldn't open codec");
		return; // Could not open codec
	}

	// Allocate video frame
	aFrame = av_frame_alloc();
}

void CVideoFrame::PlaybackComplete(SDL_Event& e) {
	auto iter = eventMap.find("PlaybackComplete");
	if (iter != eventMap.end()) iter->second(e);
}