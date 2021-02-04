#include "worker.h"
#include "logging.h"
#include "util.h"
#include <iostream>

CLogCustomer::~CLogCustomer() {
	Callback quit_token = std::bind(&CLogCustomer::DoDone, this);
	Insert(quit_token);
	thd_.join();
}

void CLogCustomer::Insert(Callback msg_) {
	mq_.push(msg_);
}

void CLogCustomer::Run(const int cpuindex) {
	if (cpuindex >= 0) {
		if (!util::setcpu(cpuindex))
			std::cerr << "cpu set faild! " << cpuindex;
	}
	while (!done_) {
		Callback func;
		if (mq_.try_and_pop(func))
			func();
	}
}

std::unique_ptr<CLogCustomer> CLogCustomer::CreateCustomer(const int cpuindex) {
	std::unique_ptr<CLogCustomer> aPtr(new CLogCustomer());
	aPtr->thd_ = std::thread(&CLogCustomer::Run, aPtr.get(), cpuindex);
	return aPtr;
}

CLogWorker::CLogWorker() :outptr_(new std::ofstream) {
	auto now = util::systemNow();
	struct tm* ptm = localtime(&now);
	now_date_ = util::timeToString2(ptm);

	customer_ = CLogCustomer::CreateCustomer(CDFLogger::GetLogger().cpu_index_);
	if (!util::isdir(CDFLogger::GetLogger().file_path_)) {
		if (!util::cwd(file_path_)) {
			file_path_ = ".";
		} else {
#ifdef _WIN32
			file_path_ = file_path_ + "\\Log";
#else
			file_path_ = file_path_ + "/Log";
#endif // _WIN32
			if (!util::mkdir(file_path_.c_str()))
				file_path_ = ".";
		}
	} else {
		file_path_ = CDFLogger::GetLogger().file_path_;
	}
#ifdef _WIN32
	if (*(file_path_.end() - 1) != '\\')
		file_path_ = file_path_ + "\\";
#else
	if (*(file_path_.end() - 1) != '/')
		file_path_ = file_path_ + "/";
#endif // _WIN32
	CreateLogFile();
}

CLogWorker::~CLogWorker() {
	customer_.reset();
	customer_ = nullptr;
	if (outptr_ && outptr_->is_open())
		outptr_->close();
	outptr_.reset();
}

void CLogWorker::Save(std::string&& msg, std::time_t t) {
	if (!CDFLogger::GetLogger().logger_init_) {
		std::cout << msg;
		return;
	}
	customer_->Insert(std::bind(&CLogWorker::WriteToFile, this, msg, t));
}

void CLogWorker::CreateLogFile() {
	if (outptr_ && outptr_->is_open())
		outptr_->close();
	auto now = util::systemNow();
	struct tm* ptm = localtime(&now);
	std::string complete_file_with_path = file_path_ + CDFLogger::GetLogger().file_name_ + now_date_ + "_" + std::to_string(util::utcmicrosecond());
	std::ios_base::openmode mode = std::ios_base::out;
	mode |= std::ios_base::trunc;
	outptr_->open(complete_file_with_path, mode);
	if (!outptr_->is_open()) {
		std::ostringstream ss_error;
		ss_error << "FILE ERROR:  could not open log file:[" << complete_file_with_path << "]";
		ss_error << "\n\t\t std::ios_base state = " << outptr_->rdstate();
		std::cerr << ss_error.str().c_str() << std::endl << std::flush;
		outptr_->close();
		return;
	}
	std::ostringstream ss_entry;
	ss_entry << "created log file at: " << util::timeToString2(ptm) << " " << util::timeToString3(ptm) << "\n";
	*outptr_.get() << ss_entry.str() << std::flush;
	outptr_->fill('0');
}

void CLogWorker::WriteToFile(std::string& msg, std::time_t& t) {
	struct tm* ptm = localtime(&t);
	auto nowdata = util::timeToString2(ptm);
	if (nowdata != now_date_) {
		now_date_ = nowdata;
		file_length = 0;
		CreateLogFile();
	}
	if (file_length > CDFLogger::GetLogger().max_file_len_) {
		file_length = 0;
		CreateLogFile();
	}
	std::ofstream& out(FileStream());
	if (out)
		out << msg << std::flush;
	file_length += (int)msg.size();
	if (CDFLogger::GetLogger().show_to_console_)
		std::cout << msg;
}