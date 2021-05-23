#include "utils/async.hpp"
#include <napi.h>
#include <thread>

namespace flac_bindings {

  using namespace Napi;

  Promise testAsync(const CallbackInfo& info) {
    using namespace std::chrono_literals;
    std::string endMode = info[0].ToString();
    AsyncBackgroundTask<bool, char>::FunctionCallback asyncFunction = [endMode](auto& c) {
      for (char ch = '0'; ch <= '9'; ch++) {
        c.sendProgressAndWait(std::make_shared<char>(ch));
        if (c.isCompleted()) {
          return;
        }
      }

      if (endMode == "reject") {
        c.reject("Rejected :(");
      } else if (endMode == "resolve") {
        c.resolve(true);
      }
    };

    AsyncBackgroundTask<bool, char>::ProgressCallback asyncFUNction = [endMode](
                                                                        auto& env,
                                                                        auto& self,
                                                                        auto e) {
      HandleScope scope(env);
      auto task = self.getTask();
      auto func = task->Receiver().Get("cbk").template As<Function>();
      auto result = func.MakeCallback(env.Global(), {String::New(env, e.get(), 1)});
      if (result.IsPromise()) {
        self.defer(result.template As<Promise>(), [endMode = endMode](auto&, auto& info2, auto e) {
          if (*e == '9' && endMode == "exception") {
            throw Error::New(info2.Env(), "Thrown :(");
          }
        });
      } else if (*e == '9' && endMode == "exception") {
        throw Error::New(env, "Thrown :(");
      }
    };

    AsyncBackgroundTask<bool, char>* worker = new AsyncBackgroundTask<bool, char>(
      info.Env(),
      asyncFunction,
      asyncFUNction,
      "flac:testAsync",
      Boolean::New);
    worker->Receiver().Set("cbk", info[1]);
    worker->Queue();
    return worker->getPromise();
  }

}
