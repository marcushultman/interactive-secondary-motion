// Copyright (c) 2015, Marcus Hultman

#pragma once

#include <string>
#include <vector>
#include <stdio.h>

const std::string kCmdTemplate =
    "ffmpeg -r 20 -f rawvideo -pix_fmt rgba -s $size -i - -threads 0 -preset fast -y -pix_fmt "
    "yuv420p -crf 21 -vf vflip $output";

class FFMPEGRecorder {
 public:
  FFMPEGRecorder() : _cmd{kCmdTemplate} {}
  ~FFMPEGRecorder() { ffmpegClose(); }

  void ffmpegSetScreenSize(unsigned int w, unsigned int h) {
    _width = w;
    _height = h;
    _data.resize(_width * _height);

    auto size = std::to_string(_width) + "x" + std::to_string(_height);
    _cmd.replace(_cmd.find("$size"), 5, size);
    _initialized++;
  }

  void ffmpegSetOutput(const char *output) {
    _cmd.replace(_cmd.find("$output"), 7, output);
    _initialized++;
  }

  void ffmpegStart() { _rec = true; }
  void ffmpegStop() { _rec = false; }
  void ffmpegToggle() { _rec = !_rec; }

  bool *ffmpegRec() { return &_rec; }

  void ffmpegOpen() {
    assert(_initialized == 2);
#if WIN32
    _proc = _popen(_cmd.c_str(), "wb");
#else
    _proc = popen(_cmd.c_str(), "wb");
#endif
  }
  void ffmpegClose() {
    ffmpegStop();
    _data.clear();
#if WIN32
    _pclose(_proc);
#else
    pclose(_proc);
#endif
  }

  void ffmpegUpdate() {
    if (!_rec) {
      return;
    }

    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, _data.data());
    fwrite(_data.data(), _data.size(), 1, _proc);
  }

 private:
  unsigned int _width = 0;
  unsigned int _height = 0;
  std::vector<unsigned char> _data;
  bool _rec = false;
  std::string _cmd;

  FILE *_proc;

  unsigned int _initialized = 0;
};
