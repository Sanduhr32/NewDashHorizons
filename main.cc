#include <drogon/drogon.h>

namespace drogon {
    using Callback = std::function<void(const HttpResponsePtr &)>;
}

int main() {
    //Load config file
    drogon::app().loadConfigFile("../config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().registerHandler("/stop", [](const drogon::HttpRequestPtr &req, drogon::Callback &&call) {
        auto response = drogon::HttpResponse::newHttpResponse();
        auto source = req->peerAddr();

//        if (!source.isIntranetIp()) {
//            response->setStatusCode(drogon::k401Unauthorized);
//            call(response);
//            return;
//        }

        response->setStatusCode(drogon::k200OK);
        response->setBody("shutting");
        call(response);
        drogon::app().quit();
    },{drogon::Get, "drogon::LocalHostFilter"});

    drogon::app().run();
    return 0;
}
