#include "logging.h"
#include "util.h"
#include "worker.h"

#include <thread>
#include <iomanip>
#include <assert.h>

CLogWorker* g_log_worker = nullptr;
std::string SplitFileName(const std::string& str) {
	size_t found;
	found = str.find_last_of("(/\\");
	return str.substr(found + 1);
}

std::thread::id  GetPID() {
	return std::this_thread::get_id();
}

CDFLogger& CDFLogger::GetLogger() {
	static CDFLogger logger;
	return logger;
}

void CDFLogger::InitLog() {
	g_log_worker = new CLogWorker();
	assert(g_log_worker != nullptr);
	logger_init_ = true;
}

CDFLogger::CDFLogger() {}

CDFLogger::~CDFLogger() {
	delete g_log_worker;
	g_log_worker = nullptr;
}

CLogMessage::CLogMessage(const std::string& file, const int line, const std::string& function, const std::string& level)
	:file_(file), line_(line), function_(function), level_(level), timestamp_(util::systemNow()) {
}

CLogMessage::~CLogMessage() {
	std::ostringstream oss;
	struct tm* ptm = localtime(&timestamp_);
	oss << "[ " << util::timeToString5(ptm) << " " << util::timeToString3(ptm) << "."
		<< std::setfill('0') << std::setw(3) << util::utcmicrosecond() / 1000 % 1000;
	oss << " " << std::setfill(' ') << std::left << std::setw(8) << level_ << " ";
	if (CDFLogger::GetLogger().log_record_info_ == DETAIL)
		oss << SplitFileName(file_) << " L: " << line_ << " ";
	oss << std::setw(6) << GetPID() << " ] ";
	oss << stream_.str();
	oss << "\n";
	g_log_worker->Save(oss.str(), timestamp_);
}