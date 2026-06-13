#pragma once

#include <string>
#include <map>
#include <memory>
#include <flutter/encodable_value.h>

namespace purchases_flutter
{

    struct HttpResponse
    {
        int statusCode;
        std::string body;
        std::map<std::string, std::string> headers;
    };

    class HttpClient
    {
    public:
        HttpClient();
        ~HttpClient();

        HttpResponse Get(const std::string &url, const std::map<std::string, std::string> &headers = {});
        HttpResponse Post(const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers = {});
        HttpResponse Put(const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers = {});

    private:
        bool initialized_;
        void EnsureInitialized();
        HttpResponse MakeRequest(const std::string &method, const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers);
    };

} // namespace purchases_flutter
