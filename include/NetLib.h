#pragma once

#include <cstddef>      // size_t
#include <cinttypes>    // uint8_t, ...
#include <string>       // std::string
#include <exception>    // std::exception
#include <functional>   // std::function
#include <system_error> // std::error_code

/// <summary>
/// Great care was taken that the asio headers are only included in the source file. Keeping the asio headers
/// away from the NetLib headers reduces compile time significantly and makes it compatible with virtually everything.
/// (No issues with winsock2 library). This is achieved by outsourcing the member variables to a struct pointer, 
/// wrapped in the IncompleteTypeWrapper to make it safe.
/// </summary>

#define NETLIB_DEFAULT_UDP_BUFFER_SIZE 1024

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

    /// <summary>
    /// <para>Sets the log level for the NetLib. Available:</para>
	/// <para>NetLib::LOG_LEVEL_TRACE</para>
	/// <para>NetLib::LOG_LEVEL_DEBUG</para>
	/// <para>NetLib::LOG_LEVEL_INFO</para>
	/// <para>NetLib::LOG_LEVEL_WARN</para>
	/// <para>NetLib::LOG_LEVEL_ERROR</para>
	/// <para>NetLib::LOG_LEVEL_CRITICAL</para>
    /// </summary>
    void SetLogLevel(enum LogLevel logLevel);

    


	// ==================================
	// ===      UDPClient Class       ===
	// ==================================

    struct UDPClientMembers;

	class UDPClient {
	public:
		UDPClient(const std::string& ipAddress, uint16_t port);
		~UDPClient();

		size_t send(uint8_t* data, size_t length);
		size_t send(const char* data);
		size_t send(const std::string& data);

    private:
        void logPacket(uint8_t* data, size_t length, const std::string& ipAddress, uint16_t port);

        IncompleteTypeWrapper<UDPClientMembers> members;
	};






	// ==================================
	// ===      UDPServer Class       ===
	// ==================================

	struct UDPServerMembers;

	class UDPServer {
	public:
		UDPServer(
			std::function<void(uint8_t* packet, size_t packetSize)> callback, 
			uint16_t port, 
			size_t bufferSize = NETLIB_DEFAULT_UDP_BUFFER_SIZE
		);

		UDPServer(
			std::function<void(uint8_t* packet, size_t packetSize, const std::string& remoteHost, uint16_t remotePort)> callback, 
			uint16_t port, size_t 
			bufferSize = NETLIB_DEFAULT_UDP_BUFFER_SIZE
		);

		~UDPServer();

	private:
		void Initialize(uint16_t port, size_t bufferSize);
		void OnReceive(const std::error_code& error, size_t bytes);
		void StartAsyncListener();
		void ListenerThread();

		void logPacket(uint8_t* data, size_t length, const std::string& ipAddress, uint16_t port);

		IncompleteTypeWrapper<UDPServerMembers> members;

	};


}






























/*

#pragma once

#include "Battery/pch.h"
#include "Battery/Core/Config.h"
#include "Battery/Utils/TypeUtils.h"

struct ALLEGRO_FILE;

namespace Battery {





	










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