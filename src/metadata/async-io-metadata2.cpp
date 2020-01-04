#include "metadata2.hpp"


namespace flac_bindings {

    using namespace v8;
    using namespace node;

    AsyncFlacIOWork::AsyncFlacIOWork(
        std::function<bool(void*, FLAC__IOCallbacks)> f,
        const char* name,
        Local<Object> &obj
    ): PromisifiedAsyncBackgroundTask<bool, FlacIOWorkRequest*>(
        [this, f] (auto &c) {
            this->ptr1 = std::make_tuple(&this->cbk1, &c);
            return f(&this->ptr1, this->cbk1.generateIOCallbacks());
        },
        std::bind(&AsyncFlacIOWork::doAsyncWork, this, std::placeholders::_1, std::placeholders::_2),
        name,
        booleanToJs<bool>
    ), cbk1(obj), cbk2() { }

    AsyncFlacIOWork::AsyncFlacIOWork(
        std::function<bool(void*, FLAC__IOCallbacks, void*, FLAC__IOCallbacks)> f,
        const char* name,
        Local<Object> &obj1,
        Local<Object> &obj2
    ): PromisifiedAsyncBackgroundTask<bool, FlacIOWorkRequest*>(
        [this, f] (auto &c) {
            this->ptr1 = std::make_tuple(&this->cbk1, &c);
            this->ptr2 = std::make_tuple(&this->cbk2, &c);
            return f(&this->ptr1, this->cbk1.generateIOCallbacks(), &this->ptr2, this->cbk2.generateIOCallbacks());
        },
        std::bind(&AsyncFlacIOWork::doAsyncWork, this, std::placeholders::_1, std::placeholders::_2),
        name,
        booleanToJs<bool>
    ), cbk1(obj1), cbk2(obj2) { }

    AsyncFlacIOWork::IOCallbacks::IOCallbacks() {}

    AsyncFlacIOWork::IOCallbacks::IOCallbacks(Local<Object> &obj) {
        this->readCallback = newCallback(Nan::Get(obj, Nan::New("read").ToLocalChecked()));
        this->writeCallback = newCallback(Nan::Get(obj, Nan::New("write").ToLocalChecked()));
        this->seekCallback = newCallback(Nan::Get(obj, Nan::New("seek").ToLocalChecked()));
        this->tellCallback = newCallback(Nan::Get(obj, Nan::New("tell").ToLocalChecked()));
        this->eofCallback = newCallback(Nan::Get(obj, Nan::New("eof").ToLocalChecked()));
        this->closeCallback = newCallback(Nan::Get(obj, Nan::New("close").ToLocalChecked()));
    }

    AsyncFlacIOWork::IOCallbacks::~IOCallbacks() {
        if(readCallback) delete readCallback;
        if(writeCallback) delete writeCallback;
        if(seekCallback) delete seekCallback;
        if(tellCallback) delete tellCallback;
        if(eofCallback) delete eofCallback;
        if(closeCallback) delete closeCallback;
    }

    template<typename NumType>
    static std::function<void (Local<Value> result)> processNumberResult(
        AsyncFlacIOWork::ExecutionContext &c,
        const char* op,
        std::function<void(NumType)> setter
    ) {
        return [op, &c, setter] (Local<Value> result) {
            auto maybeReturnValue = numberFromJs<int>(result);
            if(maybeReturnValue.IsNothing()) {
                std::string errorMessage = std::string(op) + " - Expected number or bigint as return value";
                c.reject(Nan::TypeError(errorMessage.c_str()));
                return;
            }

            setter(maybeReturnValue.FromJust());
        };
    }

    void AsyncFlacIOWork::doAsyncWork(AsyncFlacIOWork::ExecutionContext &c, FlacIOWorkRequest* const* wPtr) {
        auto async = (Nan::AsyncResource*) c.getTask()->getAsyncResource();
        std::function<void (Local<Value> result)> processResult;
        Nan::MaybeLocal<Value> result;
        Nan::TryCatch tryCatch;
        auto w = *wPtr;
        AsyncFlacIOWork::IOCallbacks* io = (AsyncFlacIOWork::IOCallbacks*) w->cbks;

        switch(w->type) {
            case FlacIOWorkRequest::Read: {
                Local<Value> args[] = {
                    WrapPointer(w->ptr, *w->bytes).ToLocalChecked(),
                    numberToJs(w->sizeOfItem),
                    numberToJs(w->nitems)
                };
                result = io->readCallback->Call(3, args, async);
                processResult = processNumberResult<size_t>(c, "FlacIO::Read", [w] (size_t v) { *w->bytes = v; });
                break;
            }

            case FlacIOWorkRequest::Write: {
                Local<Value> args[] = {
                    WrapPointer(w->ptr, *w->bytes).ToLocalChecked(),
                    numberToJs(w->sizeOfItem),
                    numberToJs(w->nitems)
                };
                result = io->writeCallback->Call(3, args, async);
                processResult = processNumberResult<size_t>(c, "FlacIO::Write", [w] (size_t v) { *w->bytes = v; });
                break;
            }

            case FlacIOWorkRequest::Seek: {
                Local<String> dir;
                switch(*w->genericReturn) {
                    case SEEK_SET: dir = Nan::New("set").ToLocalChecked(); break;
                    case SEEK_CUR: dir = Nan::New("cur").ToLocalChecked(); break;
                    case SEEK_END: dir = Nan::New("end").ToLocalChecked(); break;
                    default: dir = Nan::New("set").ToLocalChecked(); break;
                }

                Local<Value> args[] = { numberToJs(*w->offset), dir };
                result = io->seekCallback->Call(2, args, async);
                processResult = processNumberResult<int>(c, "FlacIO::Seek", [w] (int v) { *w->genericReturn = v; });
                break;
            }

            case FlacIOWorkRequest::Tell: {
                result = io->tellCallback->Call(0, nullptr, async);
                processResult = processNumberResult<int64_t>(c, "FlacIO::Tell", [w] (int64_t v) { *w->offset = v; });
                break;
            }

            case FlacIOWorkRequest::Eof: {
                result = io->eofCallback->Call(0, nullptr, async);
                processResult = processNumberResult<bool>(c, "FlacIO::Eof", [w] (bool v) { *w->genericReturn = v; }); //TODO
                break;
            }

            case FlacIOWorkRequest::Close: {
                result = io->closeCallback->Call(0, nullptr, async);
                processResult = processNumberResult<int>(c, "FlacIO::Close", [w] (int v) { *w->genericReturn = v; });
                break;
            }
        }

        if(tryCatch.HasCaught()) {
            c.reject(tryCatch.Exception());
            w->notifyWorkDone();
        } else if(!result.IsEmpty()) {
            auto theGoodResult = result.ToLocalChecked();
            if(theGoodResult->IsPromise()) {
                auto promise = theGoodResult.As<Promise>();
                c.defer(promise, nullptr, [processResult, w] (auto &c, auto _, auto &info) {
                    if(processResult && !info[0].IsEmpty()) {
                        processResult(info[0]);
                    }

                    w->notifyWorkDone();
                }, [w] (auto &c, auto _, auto &info) { c.reject(info[0]); w->notifyWorkDone(); });
            } else {
                if(processResult && !result.IsEmpty()) processResult(result.ToLocalChecked());
                w->notifyWorkDone();
            }
        } else {
            w->notifyWorkDone();
        }
    }


    typedef std::tuple<void*, AsyncFlacIOWork::ExecutionContext*> FlacIOArg;
    static size_t flacIORead(void* ptr, size_t size, size_t numberOfMembers, void* c) {
        FlacIOArg& ctx = *(FlacIOArg*) c;
        AsyncFlacIOWork::ExecutionContext* ec = std::get<1>(ctx);
        FlacIOWorkRequest* req = new FlacIOWorkRequest(FlacIOWorkRequest::Read);
        size_t bytes = size * numberOfMembers;

        req->cbks = std::get<0>(ctx);
        req->ptr = ptr;
        req->bytes = &bytes;
        req->nitems = numberOfMembers;
        req->sizeOfItem = size;

        ec->sendProgress(&req);
        req->waitForWorkDone();

        delete req;
        return bytes / size;
    }

    static size_t flacIOWrite(const void* ptr, size_t size, size_t numberOfMembers, void* c) {
        FlacIOArg& ctx = *(FlacIOArg*) c;
        AsyncFlacIOWork::ExecutionContext* ec = std::get<1>(ctx);
        FlacIOWorkRequest* req = new FlacIOWorkRequest(FlacIOWorkRequest::Write);
        size_t bytes = size * numberOfMembers;

        req->cbks = std::get<0>(ctx);
        req->ptr = (void*) ptr;
        req->bytes = &bytes;
        req->nitems = numberOfMembers;
        req->sizeOfItem = size;

        ec->sendProgress(&req);
        req->waitForWorkDone();

        delete req;
        return bytes / size;
    }

    static int flacIOSeek(void* c, int64_t offset, int whence) {
        FlacIOArg& ctx = *(FlacIOArg*) c;
        AsyncFlacIOWork::ExecutionContext* ec = std::get<1>(ctx);
        FlacIOWorkRequest* req = new FlacIOWorkRequest(FlacIOWorkRequest::Seek);
        int ret = whence;

        req->cbks = std::get<0>(ctx);
        req->offset = &offset;
        req->genericReturn = &ret;

        ec->sendProgress(&req);
        req->waitForWorkDone();

        delete req;
        return ret;
    }

    static int64_t flacIOTell(void* c) {
        FlacIOArg& ctx = *(FlacIOArg*) c;
        AsyncFlacIOWork::ExecutionContext* ec = std::get<1>(ctx);
        FlacIOWorkRequest* req = new FlacIOWorkRequest(FlacIOWorkRequest::Tell);
        int64_t offset = -1;

        req->cbks = std::get<0>(ctx);
        req->offset = &offset;

        ec->sendProgress(&req);
        req->waitForWorkDone();

        delete req;
        return offset;
    }

    static int flacIOEof(void* c) {
        FlacIOArg& ctx = *(FlacIOArg*) c;
        AsyncFlacIOWork::ExecutionContext* ec = std::get<1>(ctx);
        FlacIOWorkRequest* req = new FlacIOWorkRequest(FlacIOWorkRequest::Eof);
        int ret = 0;

        req->cbks = std::get<0>(ctx);
        req->genericReturn = &ret;

        ec->sendProgress(&req);
        req->waitForWorkDone();

        delete req;
        return ret;
    }

    static int flacIOClose(void* c) {
        FlacIOArg& ctx = *(FlacIOArg*) c;
        AsyncFlacIOWork::ExecutionContext* ec = std::get<1>(ctx);
        FlacIOWorkRequest* req = new FlacIOWorkRequest(FlacIOWorkRequest::Close);
        int ret = 0;

        req->cbks = std::get<0>(ctx);
        req->genericReturn = &ret;

        ec->sendProgress(&req);
        req->waitForWorkDone();

        delete req;
        return ret;
    }


    FLAC__IOCallbacks AsyncFlacIOWork::IOCallbacks::generateIOCallbacks() {
        return {
            this->readCallback ? flacIORead : nullptr,
            this->writeCallback ? flacIOWrite : nullptr,
            this->seekCallback ? flacIOSeek : nullptr,
            this->tellCallback ? flacIOTell : nullptr,
            this->eofCallback ? flacIOEof : nullptr,
            this->closeCallback ? flacIOClose : nullptr
        };
    }

}