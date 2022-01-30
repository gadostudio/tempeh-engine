#ifndef _TEMPEH_LOGGER_LOGGER_HPP
#define _TEMPEH_LOGGER_LOGGER_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <string_view>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <tempeh/common/collections/capped_vector.hpp>

namespace Tempeh::Log
{
//#ifdef TEMPEH_EDITOR
	template<typename M>
	class EditorSink : public spdlog::sinks::base_sink<M>
	{
	protected:
        sbstd::collections::CappedVector<std::string> *m_log_messages;

		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override {};
    public:
        EditorSink(sbstd::collections::CappedVector<std::string> *log_messages):
            m_log_messages(log_messages)
        {
        }
	};
	using EditorSink_MT = EditorSink<std::mutex>;
	using EditorSink_ST = EditorSink<spdlog::details::null_mutex>;
//#endif

	class Logger
	{
	protected:
        inline static sbstd::collections::CappedVector<std::string> capped_log_messages{500, 500};
		inline static std::vector<spdlog::sink_ptr> sinks;
		inline static std::shared_ptr<spdlog::logger> logger;
	public:
		static void init(const char* logger_name);
		static std::shared_ptr<spdlog::logger>& get_logger() { return logger; }
//#ifdef TEMPEH_EDITOR
        static sbstd::collections::CappedVector<std::string>& get_capped_log_messages() { return capped_log_messages; }
//#endif
	};

#define LOG_TRACE(...)    ::Tempeh::Log::Logger::get_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Tempeh::Log::Logger::get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Tempeh::Log::Logger::get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Tempeh::Log::Logger::get_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Tempeh::Log::Logger::get_logger()->critical(__VA_ARGS__)

}

#endif
