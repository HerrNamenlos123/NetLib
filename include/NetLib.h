#pragma once

#include <cstddef>      // size_t
#include <cinttypes>    // uint8_t, ...
#include <string>       // std::string
#include <exception>    // std::exception

namespace NetLib {

    template<typename T>
	class IncompleteTypeWrapper {
	public:
		IncompleteTypeWrapper(T* data) : data(data) {}
		~IncompleteTypeWrapper() { delete data; }
		T* operator->() { return data; }
		T* get() { return data; }

		IncompleteTypeWrapper(const IncompleteTypeWrapper&) = delete;
		IncompleteTypeWrapper& operator=(const IncompleteTypeWrapper&) = delete;

    private:
		T* data;
	};



	// ==========================
	// ===      Logging       ===
	// ==========================

    enum LogLevel {
        LOG_LEVEL_TRACE,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_CRITICAL
    };

    void SetLogLevel(enum LogLevel logLevel);

    


	// ==================================
	// ===      UDPClient Class       ===
	// ==================================

    struct UDPClientData;

	class UDPClient {
	public:
		UDPClient();
		UDPClient(UDPClient&& client);
		UDPClient(const char* ipAddress, uint16_t port);
		~UDPClient();

		size_t send(uint8_t* data, size_t length, const char* ipAddress, uint16_t port);
		size_t send(uint8_t* data, size_t length);
		size_t send(const char* data);
		size_t send(const std::string& data);

    private:
        void logPacket(uint8_t* data, size_t length, const char* ipAddress, uint16_t port);

        IncompleteTypeWrapper<UDPClientData> data;
	};

}






























/*

#pragma once

#include "Battery/pch.h"
#include "Battery/Core/Config.h"
#include "Battery/Utils/TypeUtils.h"

struct ALLEGRO_FILE;

namespace Battery {


	// ==================================
	// ===      UDPServer Class       ===
	// ==================================

	struct UDPServerData;
	// TODO: Make UDP parts good with IncompleteTypeWrapper
	class UDPServer {

		// This is a wrapper around a raw pointer which makes it possible to use an incomplete type in an std::unique_ptr. 
		// This class is not copy safe, which is why it is private within this class and only used within the std::unique_ptr.
		class UDPServerDataWrapper {
			UDPServerData* data = nullptr;
		public:
			UDPServerDataWrapper();
			~UDPServerDataWrapper();
			UDPServerData* get();
		};

		std::unique_ptr<UDPServerDataWrapper> data;

	public:
		UDPServer();
		UDPServer(std::function<void(uint8_t* packet, size_t packetSize)> callback,
				  uint16_t port, size_t bufferSize = BATTERY_DEFAULT_UDP_BUFFER_SIZE);
		~UDPServer();

		void Listen(std::function<void(uint8_t* packet, size_t packetSize)> callback,
					uint16_t port, size_t bufferSize = BATTERY_DEFAULT_UDP_BUFFER_SIZE);

	private:
		void OnReceive(const std::error_code& error, size_t bytes);
		void StartAsyncListener();
		void ListenerThread();

	};






	










	// =================================================
	// ===      File download / HTTP Utilities       ===
	// =================================================

	struct HttpResponse {
		std::string body;
		size_t status;
		std::string reason;

		HttpResponse(const std::string& body, size_t status, std::string reason) :
			body(body), status(status), reason(reason) {}
	};

	/// <summary>
	/// Splits an url into server hostname and server path. E.g: "https://www.google.at/my/page.html"
	///  -> "https://www.google.at" and "/my/page.html"
	/// </summary>
	std::pair<std::string, std::string> SplitUrl(const std::string& url);

	/// <summary>
	/// Do a HTTP GET request, redirects are automatically followed by default. Return value
	/// is empty when the server can't be reached, otherwise the body and HTTP code can be retrieved.
	/// </summary>
	std::optional<HttpResponse> GetHttpRequest(const std::string& url, bool followRedirect = true);

	/// <summary>
	/// Advanced HTTP GET request, use this only if you know what you're doing.
	/// </summary>
	std::optional<HttpResponse> GetHttpRequestChunked(
		const std::string& url,
		std::function<void()> onClearDataCallback,
		std::function<bool(const char*, size_t)> onReceiveCallback,
		std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback = std::nullopt,
		bool followRedirect = true);

	/// <summary>
	/// Download an online resource and return the buffer.
	/// OnProgress callback is optional and just for monitoring progress. Return false
	/// to cancel the download.
	/// </summary>
	std::string DownloadUrlToBuffer(
		const std::string& url, std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback = std::nullopt,
		bool followRedirect = true);

	/// <summary>
	/// Download an online resource and write it to disk under the given filename.
	/// OnProgress callback is optional and just for monitoring progress. Return false
	/// to cancel the download.
	/// </summary>
	bool DownloadUrlToFile(
		const std::string& url, const std::string& targetFile, bool binary = false,
		std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback = std::nullopt,
		bool followRedirect = true);
}*/