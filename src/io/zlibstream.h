#pragma once

#include "inputstream.h"
#include "buffering.h"
#include <zlib.h>

namespace Impacto {
namespace Io {

class ZlibStream : public InputStream, public Buffering<ZlibStream> {
  friend class Buffering<ZlibStream>;

 public:
  ~ZlibStream();

  bool IsSeekSlow = true;

  static IoError Create(InputStream* baseStream, int64_t compressedOffset,
                        int64_t compressedSize, int64_t uncompressedSize,
                        InputStream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(InputStream** outStream) override;

 protected:
  static int64_t const ZlibStreamBufferSize = 128 * 1024;
  static int64_t const ZlibStreamInputBufferSize = 64 * 1024;

  ZlibStream() : Buffering(ZlibStreamBufferSize) {}
  ZlibStream(ZlibStream const& other) = default;

  IoError FillBuffer();

  InputStream* BaseStream;
  int64_t CompressedOffset;
  int64_t CompressedSize;

  uint8_t* InputBuffer;
  z_stream ZlibState;
};

}  // namespace Io
}  // namespace Impacto