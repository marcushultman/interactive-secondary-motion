/*
* Copyright (c) 2015, Marcus Hultman
*/
#pragma once

#include <stdio.h>
#include <sstream>

static unsigned int s_width = 0, s_height = 0;
static unsigned int s_dataSize;
static unsigned char* s_data;
static bool s_rec = false;

static FILE* s_proc;

static unsigned int s_initialized = 0;
static std::string s_cmd = "ffmpeg -r 20 -f rawvideo -pix_fmt rgba -s $size -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip $output";

void ffmpegSetScreenSize(unsigned int w, unsigned int h)
{
	s_width = w; s_height = h;
	s_dataSize = s_width * s_height * sizeof(int);
	s_data = new unsigned char[s_dataSize];

	std::stringstream size;
	size << s_width << "x" << s_height;
	s_cmd.replace(s_cmd.find("$size"), 5, size.str());
	s_initialized++;
}

void ffmpegSetOutput(const char* output)
{
	s_cmd.replace(s_cmd.find("$output"), 7, output);
	s_initialized++;
}

void ffmpegStart(){
	s_rec = true;
}
void ffmpegStop(){
	s_rec = false;
}
void ffmpegToggle(){
	s_rec = !s_rec;
}

bool* ffmpegRec(){
	return &s_rec;
}

bool ffmpegOpen()
{
	if (s_initialized < 2)
		return false;
#if WIN32
	s_proc = _popen(s_cmd.c_str(), "wb");
#else
	s_proc = popen(s_cmd.c_str(), "wb");
#endif
	return true;
}
void ffmpegClose()
{
	ffmpegStop();
	delete [] s_data;
	s_data = 0;
#if WIN32
	_pclose(s_proc);
#else
	pclose(s_proc);
#endif
}

void ffmpegUpdate()
{
	if (!s_rec)
		return;

	glReadPixels(0, 0, s_width, s_height,
		GL_RGBA, GL_UNSIGNED_BYTE, s_data);
	fwrite(s_data, s_dataSize, 1, s_proc);
}
