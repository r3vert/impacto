#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

#include <queue>

#include "ffmpegstream.h"
#include "../impacto.h"
#include "../io/inputstream.h"
#include "../log.h"
#include "yuvframe.h"
#include "../texture/texture.h"
#include "../renderer2d.h"

namespace Impacto {
namespace Video {

class Clock {
 public:
  double Pts;
  double PtsDrift;
  double LastUpdated;
  double Speed;
  int Serial;
  bool Paused;

  Clock();
  void SyncTo(Clock* target);
  void Set(double pts, int serial);
  double Get();
};

class VideoPlayer {
 public:
  ~VideoPlayer();

  void Init();

  void Play(Io::InputStream* stream, bool loop);
  void Stop();

  void Update(float dt);
  void Render();
  void Read();
  void Decode(AVMediaType avType);
  void ProcessAudio();

  bool IsPlaying;
  FFmpegStream* VideoStream = 0;
  FFmpegStream* AudioStream = 0;

 private:
  void FillAudioBuffers();
  double GetTargetDelay(double duration);

  static int const FILESTREAMBUFFERSZ = 64 * 8192;
  uint8_t* FileStreamBuffer;
  SDL_mutex* Lock;

  uint64_t Time;

  AVFormatContext* FormatContext = 0;
  AVIOContext* IoContext = 0;
  SwsContext* ImgConvertContext;
  SwrContext* AudioConvertContext;
  Clock* VideoClock;
  Clock* ExternalClock;
  Clock* AudioClock;

  Clock* MasterClock;

  ALCuint ALSource;
  static int const AudioBufferSize = 256 * 8192;
  static int const AudioBufferCount = 3;
  ALuint BufferIds[AudioBufferCount];
  uint8_t HostBuffer[AudioBufferSize];
  int FirstFreeBuffer = 0;
  int FreeBufferCount = AudioBufferCount;
  int BufferStartPositions[AudioBufferCount];
  bool First = true;
  uint8_t** AudioBuffer;
  int AudioLinesize;

  bool IsInit = false;

  YUVFrame Test;

  bool Looping;
  double PreviousFrameTimestamp = 0.0;
  double FrameTimer = 0.0;
  double MaxFrameDuration;
};

}  // namespace Video
}  // namespace Impacto
