// Simple HTTP client for RevenueCat API calls
#include <windows.h>
#include <wininet.h>
#include <string>
#include <iostream>

#pragma comment(lib, "wininet.lib")

class SimpleHttpClient {
public:
    static std::string MakeGetRequest(const std::string& apiKey, const std::string& endpoint) {
        HINTERNET hInternet = InternetOpenA("RevenueCat-Windows-SDK/1.0", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
        if (!hInternet) {
            return "";
        }

        std::string fullUrl = "https://api.revenuecat.com/v1" + endpoint;
        HINTERNET hConnect = InternetOpenUrlA(hInternet, fullUrl.c_str(), nullptr, 0, INTERNET_FLAG_SECURE, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }

        // Add authorization header
        std::string authHeader = "Authorization: Bearer " + apiKey;
        HttpAddRequestHeadersA(hConnect, authHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);

        // Read response
        std::string response;
        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return response;
    }
};
