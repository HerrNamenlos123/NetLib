
#include "NetLib.h"

#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN10		// This sets the asio winsock library to Windows 10
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
using asio::ip::udp;

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

// TODO: Make logging more fool-proof (and check if name already exists, prevent crashes)

#ifndef DEPLOY

#define LOG_SET_LOGLEVEL(...)			NetLib::logger->set_level(__VA_ARGS__)
#define INIT_LOGGER()			        {	if (!NetLib::logger) {	\
												spdlog::set_pattern("%^[%T] %n: %v%$"); \
												NetLib::logger = spdlog::stdout_color_mt("NetLib"); \
												LOG_SET_LOGLEVEL(spdlog::level::trace); \
											} \
										}

#define LOG_TRACE(...)					{ INIT_LOGGER(); NetLib::logger->trace(__VA_ARGS__);			 }
#define LOG_WARN(...)					{ INIT_LOGGER(); NetLib::logger->warn(__VA_ARGS__);				 }
#define LOG_DEBUG(...)					{ INIT_LOGGER(); NetLib::logger->debug(__VA_ARGS__);			 }
#define LOG_INFO(...)					{ INIT_LOGGER(); NetLib::logger->info(__VA_ARGS__);				 }
#define LOG_ERROR(...)					{ INIT_LOGGER(); NetLib::logger->error(__VA_ARGS__);			 }
#define LOG_CRITICAL(...)				{ INIT_LOGGER(); NetLib::logger->critical(__VA_ARGS__);			 }

#else

#define LOG_SET_LOGLEVEL(...)			{ ; }

#define LOG_TRACE(...)					{ ; }
#define LOG_WARN(...)					{ ; }
#define LOG_DEBUG(...)					{ ; }
#define LOG_INFO(...)					{ ; }
#define LOG_ERROR(...)					{ ; }
#define LOG_CRITICAL(...)				{ ; }

#endif

namespace NetLib {



	// ==========================
	// ===      Logging       ===
	// ==========================

	std::shared_ptr<spdlog::logger> logger;

    void SetLogLevel(enum LogLevel logLevel) {
        INIT_LOGGER();

        switch (logLevel) {
            case LOG_LEVEL_TRACE:       LOG_SET_LOGLEVEL(spdlog::level::trace);     break;
            case LOG_LEVEL_DEBUG:       LOG_SET_LOGLEVEL(spdlog::level::debug);     break;
            case LOG_LEVEL_INFO:        LOG_SET_LOGLEVEL(spdlog::level::info);      break;
            case LOG_LEVEL_WARN:        LOG_SET_LOGLEVEL(spdlog::level::warn);      break;
            case LOG_LEVEL_ERROR:       LOG_SET_LOGLEVEL(spdlog::level::err);       break;
            case LOG_LEVEL_CRITICAL:    LOG_SET_LOGLEVEL(spdlog::level::critical);  break;
        }
    }






	// ==================================
	// ===      NetLib::SendUDP       ===
	// ==================================

	bool SendUDP(const std::string& ipAddress, uint16_t port, uint8_t* data, size_t length) {
		try {
			LOG_DEBUG("[SendUDP()]: Connecting to {}:{}", ipAddress, port);

			// Create the socket
			asio::io_service ioService;
			udp::socket socket(ioService);
			udp::endpoint remote_endpoint(udp::endpoint(asio::ip::address::from_string(ipAddress), port));
			socket.open(udp::v4());

			// Send the data
			size_t bytes = socket.send_to(asio::buffer(data, length), remote_endpoint);

#ifndef DEPLOY
			std::string str = "";
			for (size_t i = 0; i < length; i++) {
				str += std::to_string(data[i]);
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
			LOG_INFO("[SendUDP()]: Packet sent to {}:{} -> [{}] -> \"{}\"", ipAddress, port, str, std::string((const char*)data, length));
#endif

			// Close the socket
			socket.close();
			LOG_DEBUG("[SendUDP()]: Operation successful");

			return true;
		}
		catch (std::exception& e) {
			LOG_WARN("[SendUDP()]: ASIO Exception: {}", e.what());
		}

		return false;
	}

	bool SendUDP(const std::string& ipAddress, uint16_t port, const char* data) {
		return SendUDP(ipAddress, port, (uint8_t*)data, strlen(data));
	}

	bool SendUDP(const std::string& ipAddress, uint16_t port, const std::string& data) {
		return SendUDP(ipAddress, port, (uint8_t*)data.c_str(), data.length());
	}






	// ==================================
	// ===      UDPClient Class       ===
	// ==================================

	struct UDPClientMembers {
		asio::io_service ioService;
		udp::socket socket;
		udp::endpoint remote_endpoint;

		UDPClientMembers() : socket(ioService) {}
		~UDPClientMembers() = default;
	};

	UDPClient::UDPClient(const std::string& ipAddress, uint16_t port) : members(new UDPClientMembers()) {
		try {
			members->remote_endpoint = udp::endpoint(asio::ip::address::from_string(ipAddress), port);
			members->socket.open(udp::v4());
			LOG_DEBUG("[UDPClient]: Instance constructed, pointing to {}:{}", ipAddress, port);
		}
		catch (std::exception& e) {
			throw std::runtime_error(std::string("ASIO Exception: ") + e.what());
		}
	}

	UDPClient::~UDPClient() {
		members->socket.close();
		LOG_DEBUG("[UDPClient]: Instance destructed");
	}

	size_t UDPClient::send(uint8_t* data, size_t length) {

		try {
			size_t bytes = members->socket.send_to(asio::buffer(data, length), members->remote_endpoint);

#ifndef DEPLOY
			logPacket(data, length, members->remote_endpoint.address().to_string().c_str(), members->remote_endpoint.port());
#endif

			return bytes;
		}
		catch (std::exception& e) {
			throw std::runtime_error(std::string("ASIO Exception: ") + e.what());
		}
	}

	size_t UDPClient::send(const char* data) {
		return send((uint8_t*)data, strlen(data));
	}

	size_t UDPClient::send(const std::string& data) {
		return send(data.c_str());
	}

    void UDPClient::logPacket(uint8_t* data, size_t length, const std::string& ipAddress, uint16_t port) {
        std::string str = "";
        for (size_t i = 0; i < length; i++) {
            str += std::to_string(data[i]);
            str += ", ";
        }
        str.pop_back();
        str.pop_back();
		LOG_INFO("[UDPClient]: Packet sent to {}:{} -> [{}] -> \"{}\"", ipAddress, port, str, std::string((const char*)data, length));
    }







	// ==================================
	// ===      UDPServer Class       ===
	// ==================================

	struct UDPServerMembers {

		asio::io_service ioService;
		udp::socket socket;
		udp::endpoint remoteEndpoint;

		bool terminate = false;
		std::thread listenerThread;
		std::function<void(uint8_t* packet, size_t packetSize)> callback;
		std::function<void(uint8_t* packet, size_t packetSize, const std::string& remoteHost, uint16_t remotePort)> callbackWithHost;

		std::vector<uint8_t> buffer;
		size_t bufferSize = 0;

		UDPServerMembers(const udp::endpoint& endpoint) : socket(ioService, endpoint) {}
		~UDPServerMembers() = default;
	};

	UDPServer::UDPServer(std::function<void(uint8_t* packet, size_t packetSize)> callback, uint16_t port, size_t bufferSize) 
		: members(new UDPServerMembers(udp::endpoint(udp::v4(), port))) 
	{
		members->callback = callback;
		Initialize(port, bufferSize);
	}

	UDPServer::UDPServer(std::function<void(uint8_t* packet, size_t packetSize, const std::string& remoteHost, uint16_t remotePort)> callback, uint16_t port, size_t bufferSize) 
		: members(new UDPServerMembers(udp::endpoint(udp::v4(), port))) 
	{
		members->callbackWithHost = callback;
		Initialize(port, bufferSize);
	}

	UDPServer::~UDPServer() {
		LOG_DEBUG("[UDPServer]: Terminating UDP listener");

		// Set the terminate flag and wait until the listener thread returns
		members->terminate = true;
		members->socket.close();
		members->listenerThread.join();

		LOG_DEBUG("[UDPServer]: Instance destructed");
	}

	void UDPServer::Initialize(uint16_t port, size_t bufferSize) {
		try {
			LOG_DEBUG("[UDPServer]: Creating UDP listener ...");

			// Initialize the buffer
			members->bufferSize = bufferSize;
			members->buffer.clear();
			members->buffer.reserve(bufferSize);
			for (size_t i = 0; i < bufferSize; i++) {
				members->buffer.push_back(0);
			}
			memset(&members->buffer[0], 0, bufferSize);

			// Start the listener thread
			members->listenerThread = std::thread(std::bind(&UDPServer::ListenerThread, this));

			LOG_DEBUG("[UDPServer]: Instance constructed");
		}
		catch (std::exception& e) {
			throw std::runtime_error(std::string("ASIO Exception: ") + e.what());
		}
	}

	void UDPServer::OnReceive(const std::error_code& error, size_t bytes) {
		if (!error) {

			LOG_DEBUG("[UDPServer]: Packet received, calling client callback");
			std::string remoteHost = members->remoteEndpoint.address().to_string();

#ifndef DEPLOY
			logPacket(&members->buffer[0], bytes, remoteHost.c_str(), members->remoteEndpoint.port());
#endif

			if (members->callback) {
				members->callback(&members->buffer[0], bytes);
			}
			if (members->callbackWithHost) {
				members->callbackWithHost(&members->buffer[0], bytes, remoteHost, members->remoteEndpoint.port());
			}

		}
		else {

			if (members->terminate)		// Errors are ignored if thread is being terminated
				return;

			LOG_WARN("[UDPServer]: Error " + std::to_string(error.value()) + ": " + error.message());
		}

		// Start listening for the next packet
		StartAsyncListener();
	}

	void UDPServer::StartAsyncListener() {
		try {
			members->socket.async_receive_from(asio::buffer(&members->buffer[0], members->bufferSize), members->remoteEndpoint,
				std::bind(&UDPServer::OnReceive, this, std::placeholders::_1, std::placeholders::_2));
			LOG_DEBUG("[UDPServer]: Async listener started");
		}
		catch (std::exception& e) {
			throw std::runtime_error(std::string("ASIO Exception: ") + e.what());
		}
	}

	void UDPServer::ListenerThread() {

		LOG_DEBUG("[UDPServer]: Listener thread started");

		try {

			// Start listener once
			StartAsyncListener();

			// Main loop in the listener thread
			while (!members->terminate) {
				members->ioService.run_one();
			}

		}
		catch (std::exception& e) {
			LOG_CRITICAL(std::string("ASIO UDP Exception from listener thread: ") + e.what());
		}
		catch (...) {
			LOG_CRITICAL("[UDPServer]: Unknown exception from listener thread!");
		}

		LOG_DEBUG("[UDPServer]: Listener thread terminated");
	}

	void UDPServer::logPacket(uint8_t* data, size_t length, const std::string& ipAddress, uint16_t port) {
		std::string str = "";
		for (size_t i = 0; i < length; i++) {
			str += std::to_string(data[i]);
			str += ", ";
		}
		str.pop_back();
		str.pop_back();
		LOG_INFO("[UDPServer]: Packet received from {}:{} -> [{}] -> \"{}\"", ipAddress, port, str, std::string((const char*)data, length));
	}

}




























/*

#include "Battery/pch.h"
#include "Battery/Core/Exception.h"
#include "Battery/Log/Log.h"
#include "Battery/Utils/NetUtils.h"
#include "Battery/StringUtils.h"
#include "Battery/Utils/TimeUtils.h"
#include "Battery/Utils/FileUtils.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "Battery/Extern/httplib.h"
#include "Battery/Extern/magic_enum.hpp"

#pragma warning( disable : 4101 )		// Suppress warning about unused variable 'e' (Only in this .cpp file)

namespace Battery {

	// TODO: Fix UDPServer and Client with the IncompleteTypeWrapper, also for RobotinoLib

	// UDPServer

	struct UDPServerData {

		asio::io_service ioService;
		std::unique_ptr<udp::socket> socket;
		udp::endpoint remoteEndpoint;

		bool terminate = false;
		std::unique_ptr<std::thread> listenerThread;
		std::function<void(uint8_t* packet, size_t packetSize)> callback;

		std::unique_ptr<uint8_t[]> buffer;
		size_t bufferSize;

		UDPServerData() = default;
		~UDPServerData() = default;
	};

	// -> UDPServerDataWrapper Child class vvvvvv
	UDPServer::UDPServerDataWrapper::UDPServerDataWrapper() {
		data = new UDPServerData();
	}

	UDPServer::UDPServerDataWrapper::~UDPServerDataWrapper() {
		delete data;
	}

	UDPServerData* UDPServer::UDPServerDataWrapper::get() {
		return data;
	}
	// -> UDPServerDataWrapper Child class ^^^^^^

	UDPServer::UDPServer() {
		try {
			data = std::make_unique<UDPServerDataWrapper>();
			LOG_TRACE("UDPServer Instance constructed");
		}
		catch (std::exception& e) {
			throw Battery::Exception(std::string("ASIO Exception: ") + e.what());
		}
	}

	UDPServer::UDPServer(std::function<void(uint8_t* packet, size_t packetSize)> callback, uint16_t port, size_t bufferSize) {
		try {
			data = std::make_unique<UDPServerDataWrapper>();
			LOG_TRACE("UDPServer Instance constructed");
		}
		catch (std::exception& e) {
			throw Battery::Exception(std::string("ASIO Exception: ") + e.what());
		}
		Listen(callback, port, bufferSize);
	}

	UDPServer::~UDPServer() {
		LOG_TRACE("Terminating UDP listener");

		// Set the terminate flag and wait until the listener thread returns
		data->get()->terminate = true;
		data->get()->socket->close();
		data->get()->listenerThread->join();
		data->get()->listenerThread.reset();

		LOG_TRACE("UDPServer Instance destructed");
	}

	void UDPServer::Listen(std::function<void(uint8_t* packet, size_t packetSize)> callback, uint16_t port, size_t bufferSize) {
		try {
			LOG_TRACE("Creating UDP listener");

			// Setting all data members
			data->get()->socket = std::make_unique<udp::socket>(data->get()->ioService, udp::endpoint(udp::v4(), port));
			data->get()->bufferSize = bufferSize;
			data->get()->callback = callback;

			// Initialize the buffer
			data->get()->buffer.reset(new uint8_t[bufferSize]);
			for (size_t i = 0; i < bufferSize; i++) {
				data->get()->buffer[i] = 0;
			}

			// Start the listener thread
			data->get()->listenerThread = std::make_unique<std::thread>(std::bind(&UDPServer::ListenerThread, this));

			LOG_TRACE("UDPServer Instance constructed");
		}
		catch (std::exception& e) {
			throw Battery::Exception(std::string("ASIO Exception: ") + e.what());
		}
	}

	void UDPServer::OnReceive(const std::error_code& error, size_t bytes) {
		if (!error) {

			LOG_TRACE("UDP Packet received, calling client callback");
			data->get()->callback(data->get()->buffer.get(), bytes);

		}
		else {

			if (data->get()->terminate)	// Errors are ignored if thread is being terminated
				return;

			LOG_WARN("UDP Error " + std::to_string(error.value()) + ": " + error.message());
		}

		// Start listening for the next packet
		StartAsyncListener();
	}

	void UDPServer::StartAsyncListener() {
		try {
			data->get()->socket->async_receive_from(asio::buffer(data->get()->buffer.get(), data->get()->bufferSize), data->get()->remoteEndpoint,
				std::bind(&UDPServer::OnReceive, this, std::placeholders::_1, std::placeholders::_2));
			LOG_TRACE("UDP async listener started");
		}
		catch (std::exception& e) {
			throw Battery::Exception(std::string("ASIO Exception: ") + e.what());
		}
	}

	void UDPServer::ListenerThread() {

		LOG_TRACE("UDP listener thread started");

		try {

			// Start listener once
			StartAsyncListener();

			// Main loop in the listener thread
			while (!data->get()->terminate) {
				data->get()->ioService.run_one();
			}

		}
		catch (Battery::Exception& e) {
			LOG_CRITICAL(std::string("Battery::Exception from listener thread: ") + e.what());
		}
		catch (std::exception& e) {
			LOG_CRITICAL(std::string("ASIO UDP Exception from listener thread: ") + e.what());
		}
		catch (...) {
			LOG_CRITICAL("Unknown exception from listener thread!");
		}

		LOG_TRACE("UDP listener thread returned");
	}









	











	// =================================================
	// ===      File download / HTTP Utilities       ===
	// =================================================

	std::pair<std::string, std::string> SplitUrl(const std::string& url) {
		std::vector<std::string> urlFragments = Battery::StringUtils::SplitString(url, '/');

		if (urlFragments.size() < 3)
			return std::make_pair("", "");

		std::string hostname = urlFragments[0] + "//" + urlFragments[1];
		std::string pathname = "/";

		for (size_t i = 2; i < urlFragments.size(); i++) {
			pathname += urlFragments[i] + "/";
		}
		pathname.pop_back();

		return std::make_pair(hostname, pathname);
	}

	std::optional<httplib::Response> GetHttpRequestRaw(const std::string& url, bool followRedirect = true) {

		LOG_TRACE(__FUNCTION__"(): Accessing {}", url);
		std::pair<std::string, std::string> urlP = SplitUrl(url);

		if (urlP.first.empty() || urlP.second.empty()) {
			LOG_TRACE(__FUNCTION__"(): Error: Invalid url: server='{}', path='{}'", urlP.first, urlP.second);
			return std::nullopt;
		}

		httplib::Client cli(urlP.first);
		httplib::Result res = cli.Get(urlP.second.c_str());

		if (&res.value() == nullptr || res.error() != httplib::Error::Success) {
			LOG_TRACE(__FUNCTION__"(): Network failed");
			return std::nullopt;
		}
		LOG_TRACE(__FUNCTION__"(): Success");

		if (res->status == 302 && followRedirect) {
			std::string follow = res->get_header_value("location");
			LOG_TRACE(__FUNCTION__"(): Following redirect to {}", follow);
			return GetHttpRequestRaw(follow);
		}

		return std::make_optional(res.value());
	}

	std::optional<HttpResponse> GetHttpRequest(const std::string& url, bool followRedirect) {
		std::optional<httplib::Response> res = GetHttpRequestRaw(url, followRedirect);
		if (!res.has_value())
			return std::nullopt;

		return std::make_optional(HttpResponse(res->body, (size_t)res->status, res->reason));
	}

	std::optional<HttpResponse> GetHttpRequestChunkedImpl(const std::string& url,
		std::function<bool(const char*, size_t)> onReceiveCallback,
		std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback)
	{
		LOG_TRACE(__FUNCTION__"(): Accessing {}", url);
		std::pair<std::string, std::string> urlP = SplitUrl(url);

		if (urlP.first.empty() || urlP.second.empty()) {
			LOG_TRACE(__FUNCTION__"(): Error: Invalid url: server='{}', path='{}'", urlP.first, urlP.second);
			return std::nullopt;
		}

		httplib::Client cli(urlP.first);
		std::optional<httplib::Result> res;

		if (onProgressCallback.has_value()) {
			res = cli.Get(urlP.second.c_str(),
				[&](const char* data, size_t bytes) {
				return onReceiveCallback(data, bytes);
			},
				[&](uint64_t progress, uint64_t total) {
				return onProgressCallback.value()(progress, total);
			}
			);
		}
		else {
			res = cli.Get(urlP.second.c_str(),
				[&](const char* data, size_t bytes) {
				return onReceiveCallback(data, bytes);
			},
				[&](uint64_t progress, uint64_t total) {
				return true;
			}
			);
		}

		if (&res->value() == nullptr || res->error() != httplib::Error::Success) {
			LOG_TRACE(__FUNCTION__"(): Network failed");
			return std::nullopt;
		}

		LOG_TRACE(__FUNCTION__"(): Success");
		return std::make_optional(HttpResponse(res.value()->body, res.value()->status, res.value()->reason));
	}

	// TODO: Implement max redirect count
	std::optional<HttpResponse> GetHttpRequestChunked(
		const std::string& url,
		std::function<void()> onClearDataCallback,
		std::function<bool(const char*, size_t)> onReceiveCallback,
		std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback,
		bool followRedirect)
	{
		// Download the resource
		std::optional<HttpResponse> res = GetHttpRequestChunkedImpl(url, onReceiveCallback, onProgressCallback);
		if (!res.has_value()) {
			return std::nullopt;
		}

		// Handle redirect
		if (res->status == 302 && followRedirect) {
			LOG_TRACE(__FUNCTION__"(): Got redirected, requesting again");

			// Send the same request again without redirect
			std::optional<httplib::Response> res = GetHttpRequestRaw(url, false);
			if (!res.has_value()) {
				LOG_TRACE(__FUNCTION__"(): Second request failed");
				return std::nullopt;
			}

			// Now read the location of the redirect
			std::string follow = res->get_header_value("location");
			LOG_TRACE(__FUNCTION__"(): Followed redirect to {}, clearing data and restarting download", follow);
			onClearDataCallback();

			// And download the actual resource recursively (allowing several redirects)
			return GetHttpRequestChunked(follow, onClearDataCallback, onReceiveCallback, onProgressCallback);
		}

		return res;
	}

	std::string DownloadUrlToBuffer(
		const std::string& url, std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback,
		bool followRedirect) {

		std::string buffer = "";

		std::optional<HttpResponse> res = GetHttpRequestChunked(url,
			[&]() {
			buffer.clear();
		},
			[&](const char* data, size_t bytes) {
			buffer.append(data, bytes);
			return true;
		},
			onProgressCallback,
			true
			);

		if (!res.has_value())
			return "";

		return buffer;
	}

	bool DownloadUrlToFile(
		const std::string& url, const std::string& targetFile, bool binary,
		std::optional<std::function<bool(uint64_t, uint64_t)>> onProgressCallback,
		bool followRedirect) {

		LOG_TRACE(__FUNCTION__"(): Preparing directory and opening target file");
		Battery::PrepareDirectory(Battery::GetParentDirectory(targetFile));
		ALLEGRO_FILE* file = al_fopen(targetFile.c_str(), (binary ? "wb" : "w"));
		if (file == nullptr) {
			LOG_TRACE(__FUNCTION__"(): File failed, memory error...");
			return false;
		}

		std::optional<HttpResponse> res = GetHttpRequestChunked(url,
			[&]() {
				// Clear the file: Close and open it again
				al_fclose(file);
				file = al_fopen(targetFile.c_str(), (binary ? "wb" : "w"));
				if (file == nullptr) {
					LOG_TRACE(__FUNCTION__"(): File failed, memory error...");
					return false;
				}
				return true;
			},
			[&](const char* data, size_t bytes) {
				if (file)
					al_fwrite(file, data, bytes);

				return true;
			},
			onProgressCallback,
			true
		);

		al_fclose(file);

		if (!res.has_value()) {
			LOG_TRACE(__FUNCTION__"(): Download failed, removing file from disk");
			Battery::RemoveFile(targetFile);
			return false;
		}

		return true;
	}
}*/