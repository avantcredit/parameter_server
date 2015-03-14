#include "ps.h"
namespace PS {

class HelloServer : public App {
 public:
  virtual void ProcessRequest(const MessagePtr& req) {
    std::cout << MyNodeID() <<  ": processing request " << req->task.time() <<
        " from " << req->sender << std::endl;
  }
};

class HelloWorker : public App {
 public:
  virtual void ProcessResponse(const MessagePtr& res) {
    std::cout << MyNodeID() << ": received response " << res->task.time() <<
        " from " << res->sender << std::endl;
  }

  virtual void Run() {
    WaitServersReady();

    int ts = Submit(Task(), kServerGroup);
    Wait(ts, kServerGroup);

    ts = Submit(Task(), kServerGroup);
    Wait(ts, kServerGroup);

    auto req = NewMessage();
    req->recver = kServerGroup;
    req->fin_handle = [this]() {
      std::cout << MyNodeID() << ": request " << LastResponse()->task.time() <<
      " is finished" << std::endl;
    };
    Wait(Submit(req), req->recver);
  }
};

App* App::Create(const std::string& conf) {
  if (IsWorker()) return new HelloWorker();
  if (IsServer()) return new HelloServer();
  return new App();
}

}  // namespace PS

int main(int argc, char *argv[]) {
  return PS::RunSystem(argc, argv);
}