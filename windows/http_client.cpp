#include "http_client.h"
#include <Windows.h>
#include <WinInet.h>
#include <iostream>

#pragma comment(lib, "wininet.lib")

namespace purchases_flutter
{

    HttpClient::HttpClient() : initialized_(false)
    {
        EnsureInitialized();
    }

    HttpClient::~HttpClient()
    {
        if (initialized_)
        {
            // WinInet cleanup is handled automatically
        }
    }

    void HttpClient::EnsureInitialized()
    {
        if (!initialized_)
        {
            initialized_ = true;
        }
    }

    HttpResponse HttpClient::Get(const std::string &url, const std::map<std::string, std::string> &headers)
    {
        return MakeRequest("GET", url, "", headers);
    }

    HttpResponse HttpClient::Post(const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers)
    {
        return MakeRequest("POST", url, body, headers);
    }

    HttpResponse HttpClient::Put(const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers)
    {
        return MakeRequest("PUT", url, body, headers);
    }

    HttpResponse HttpClient::MakeRequest(const std::string &method, const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers)
    {
        HttpResponse response;
        response.statusCode = 0;

        std::cout << "Making " << method << " request to: " << url << std::endl;

        HINTERNET hInternet = InternetOpenA("RevenueCat-Windows-SDK/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!hInternet)
        {
            response.statusCode = -1;
            response.body = "Failed to initialize WinINet";
            std::cout << "Error: " << response.body << std::endl;
            return response;
        }

        // Parse URL to get components
        std::string host = "api.revenuecat.com";
        std::string path = "/v1/subscribers/";
        size_t pathPos = url.find(path);
        if (pathPos != std::string::npos)
        {
            path = url.substr(pathPos);
        }

        std::cout << "Connecting to host: " << host << std::endl;
        std::cout << "Path: " << path << std::endl;

        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect)
        {
            InternetCloseHandle(hInternet);
            response.statusCode = -1;
            response.body = "Failed to connect to host";
            std::cout << "Error: " << response.body << std::endl;
            return response;
        }

        // Create HTTP request
        HINTERNET hRequest = HttpOpenRequestA(hConnect, method.c_str(), path.c_str(), NULL, NULL, NULL,
                                              INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
        if (!hRequest)
        {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            response.statusCode = -1;
            response.body = "Failed to create HTTP request";
            std::cout << "Error: " << response.body << std::endl;
            return response;
        }

        // Add headers
        std::string headerString = "";
        for (const auto &header : headers)
        {
            headerString += header.first + ": " + header.second + "\r\n";
        }

        if (!headerString.empty())
        {
            std::cout << "Adding headers: " << headerString << std::endl;
            HttpAddRequestHeadersA(hRequest, headerString.c_str(), static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);
        }

        // Send request
        std::cout << "Sending request..." << std::endl;
        if (!body.empty())
        {
            std::cout << "Request body: " << body << std::endl;
        }

        BOOL bResult = HttpSendRequestA(hRequest, NULL, 0,
                                        body.empty() ? NULL : (LPVOID)body.c_str(),
                                        body.empty() ? 0 : static_cast<DWORD>(body.length()));

        if (!bResult)
        {
            DWORD error = GetLastError();
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            response.statusCode = -1;
            response.body = "Failed to send HTTP request. Error code: " + std::to_string(error);
            std::cout << "Error: " << response.body << std::endl;
            return response;
        }

        // Get status code
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);
        if (HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL))
        {
            response.statusCode = statusCode;
            std::cout << "Status code: " << statusCode << std::endl;
        }

        // Read response
        const DWORD bufferSize = 4096;
        char buffer[bufferSize];
        DWORD bytesRead = 0;
        std::string responseBody;

        std::cout << "Reading response..." << std::endl;
        while (InternetReadFile(hRequest, buffer, bufferSize - 1, &bytesRead) && bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            responseBody += buffer;
        }

        response.body = responseBody;
        std::cout << "Response body: " << responseBody << std::endl;

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return response;
    }

} // namespace purchases_flutter
