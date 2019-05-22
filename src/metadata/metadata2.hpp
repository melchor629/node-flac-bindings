#include <nan.h>

#include "../utils/async.hpp"
#include "../utils/defs.hpp"

extern "C" {
    typedef void* FLAC__IOHandle;
    typedef struct FLAC__IOCallbacks {
        size_t(*read)(void *ptr, size_t size, size_t nmemb, FLAC__IOHandle handle);
        size_t(*write)(const void *ptr, size_t size, size_t nmemb, FLAC__IOHandle handle);
        int(*seek)(FLAC__IOHandle handle, int64_t offset, int whence);
        int64_t(*tell)(FLAC__IOHandle handle);
        int(*eof)(FLAC__IOHandle handle);
        int(*close)(FLAC__IOHandle handle);
    } FLAC__IOCallbacks;
}

namespace flac_bindings {
    struct FlacIOWorkRequest: SyncronizableWorkRequest {
        enum Type { Read, Write, Seek, Tell, Eof, Close } type;
        void* cbks;
        void* ptr;
        size_t* bytes;
        size_t nitems;
        size_t sizeOfItem;
        int64_t* offset;
        int* genericReturn;

        FlacIOWorkRequest(): SyncronizableWorkRequest() {}
        FlacIOWorkRequest(FlacIOWorkRequest::Type type): SyncronizableWorkRequest(), type(type) {}
    };

    class AsyncFlacIOWork: public PromisifiedAsyncBackgroundTask<bool, FlacIOWorkRequest> {
        struct IOCallbacks {
            Nan::Callback* readCallback = nullptr;
            Nan::Callback* writeCallback = nullptr;
            Nan::Callback* seekCallback = nullptr;
            Nan::Callback* tellCallback = nullptr;
            Nan::Callback* eofCallback = nullptr;
            Nan::Callback* closeCallback = nullptr;

            IOCallbacks();
            IOCallbacks(v8::Local<v8::Object> &obj);
            ~IOCallbacks();

            FLAC__IOCallbacks generateIOCallbacks();
        } cbk1, cbk2;

        std::tuple<IOCallbacks*, AsyncFlacIOWork::ExecutionContext*> ptr1;
        std::tuple<IOCallbacks*, AsyncFlacIOWork::ExecutionContext*> ptr2;

        void doAsyncWork(AsyncFlacIOWork::ExecutionContext &, const FlacIOWorkRequest*);

    public:
        AsyncFlacIOWork(
            std::function<bool(void*, FLAC__IOCallbacks)> f,
            const char* name,
            v8::Local<v8::Object> &obj
        );

        AsyncFlacIOWork(
            std::function<bool(void*, FLAC__IOCallbacks, void*, FLAC__IOCallbacks)> f,
            const char* name,
            v8::Local<v8::Object> &obj1,
            v8::Local<v8::Object> &obj2
        );
    };
}