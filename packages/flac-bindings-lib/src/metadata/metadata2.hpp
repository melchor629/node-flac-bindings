#include "../utils/async.hpp"
#include "../utils/converters.hpp"
#include "../utils/pointer.hpp"
#include <FLAC/callback.h>
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  struct FlacIOWorkRequest {
    enum Type { Read, Write, Seek, Tell, Eof, Close } type;
    void* cbks;
    void* ptr;
    size_t* bytes;
    size_t nitems;
    size_t sizeOfItem;
    int64_t* offset;
    int* genericReturn;

    FlacIOWorkRequest() {}
    FlacIOWorkRequest(FlacIOWorkRequest::Type type): type(type) {}
  };

  class AsyncFlacIOWork: public AsyncBackgroundTask<bool, FlacIOWorkRequest> {
    struct IOCallbacks {
      FunctionReference readCallback;
      FunctionReference writeCallback;
      FunctionReference seekCallback;
      FunctionReference tellCallback;
      FunctionReference eofCallback;
      FunctionReference closeCallback;

      IOCallbacks();
      IOCallbacks(const Object& obj);
      ~IOCallbacks();

      FLAC__IOCallbacks generateIOCallbacks();
    } cbk1, cbk2;

    pointer::BufferReference<uint8_t> sharedBufferRef;
    std::tuple<IOCallbacks*, AsyncFlacIOWork::ExecutionProgress*> ptr1;
    std::tuple<IOCallbacks*, AsyncFlacIOWork::ExecutionProgress*> ptr2;

    void doAsyncWork(
      const Napi::Env&,
      AsyncFlacIOWork::ExecutionProgress&,
      const std::shared_ptr<FlacIOWorkRequest>&);

  public:
    AsyncFlacIOWork(
      std::function<bool(FLAC__IOHandle, FLAC__IOCallbacks)> f,
      const char* name,
      const Object& obj,
      std::function<void(const Napi::Env&, bool)> checkStatus);

    AsyncFlacIOWork(
      std::function<bool(FLAC__IOHandle, FLAC__IOCallbacks, FLAC__IOHandle, FLAC__IOCallbacks)> f,
      const char* name,
      const Object& obj1,
      const Object& obj2,
      std::function<void(const Napi::Env&, bool)> checkStatus);
  };
}
