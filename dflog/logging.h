/*!
 * \file logging.h
 * \date 2021/02/01 17:06
 *
 * \author zhangxuebing
 * Contact: zhangxuebing@orientsec.com.cn
 * \brief 日志库
 */
#ifndef DF_LOGGING_H_
#define DF_LOGGING_H_

#include <string>
#include <sstream>
#include <ctime>

#if defined(_WIN32)
#ifdef DFLOG_DLL_EXPORT
#define DFLOG_EXPORT  __declspec(dllexport)
#else
#define DFLOG_EXPORT  __declspec(dllimport)
#endif
#else
#define DFLOG_EXPORT
#endif

#ifdef _WIN32
#define  __PRETTY_FUNCTION__  __FUNCTION__
#endif

const int DEBUG = 0, INFO = 1, WARNING = 2, FATAL = 3;
const int NORMAL = 0, DETAIL = 1;						// log record info

#define  MAX_LOG_FILE_LEN  1024 * 1024 * 10

class DFLOG_EXPORT CLogMessage {
public:
	CLogMessage(const std::string& file, const int line, const std::string& function, const std::string& level);
	~CLogMessage();
	std::ostringstream& MessageStream() { return stream_; }
private:
	const std::string file_;
	const int line_;
	const std::string function_;
	const std::string level_;
	std::time_t timestamp_;
	std::ostringstream stream_;
};

class CLogWorker;
class DFLOG_EXPORT CDFLogger {
public:
	friend class CLogMessage;
	friend class CLogWorker;
	static CDFLogger& GetLogger();
	void SetFileName(const char* filename) {					    // 文件前缀
		file_name_ = filename;
	}
	//const char* path
	// eg linux "/root/log"  window  "F:\\log\\"
	void SetFilePath(const char* path) {						    // 路径
		file_path_ = path;
	}
	void SetWriteLevel(const int level) {							// 写入文件的日志等级
		level_ = level;
	}
	// const int recordinfo
	// NORMAL	[ YYYY-MM-DD HH:MM::SS.xxx level pid ]
	// DETAIL	[ YYYY-MM-DD HH:MM::SS.xxx level  file line pid ]
	void SetLogRecord(const int recordinfo) {						// 日志记录前缀信息
		log_record_info_ = recordinfo;
	}
	void BindWorkerThreadToCPU(const int cpuindex) {				// 绑定日志写入线程到指定CPU
		cpu_index_ = cpuindex;
	}
	void ShowConsole(const bool b) {							    // 是否控制台显示
		show_to_console_ = b;
	}
	void InitLog();
private:
	CDFLogger();
	~CDFLogger();
	CDFLogger(const CDFLogger&);
	CDFLogger operator=(const CDFLogger&);
private:
	std::string file_name_{ "" };
	std::string file_path_{ "" };

	int level_{ INFO };
	int log_record_info_{ NORMAL };
	bool show_to_console_{ false };
	int max_file_len_{ MAX_LOG_FILE_LEN };
	int cpu_index_{ -1 };
	bool logger_init_{ false };
};

#define DF_LOG_DEBUG	CLogMessage(__FILE__,__LINE__,__FUNCTION__,"DEBUG")
#define DF_LOG_INFO		CLogMessage(__FILE__,__LINE__,__FUNCTION__,"INFO")
#define DF_LOG_WARNING	CLogMessage(__FILE__,__LINE__,__FUNCTION__,"WARNING")
#define DF_LOG_FATAL	CLogMessage(__FILE__,__LINE__,__FUNCTION__,"FATAL")

#define LOG(level) DF_LOG_##level.MessageStream()

#define LOG_DEBUG	LOG(DEBUG)
#define LOG_INFO	LOG(INFO)
#define LOG_WARNING LOG(WARNING)
#define LOG_FATAL   LOG(FATAL)
#endif // DF_LOGGING_H_
