/*!
 * \file worker.h
 * \date 2021/02/02 15:36
 *
 * \author zhangxuebing
 * Contact: zhangxuebing@orientsec.com.cn
 * \brief 异步写入
 */
#ifndef WORKER_H_
#define WORKER_H_
#include <mutex>
#include <memory>
#include <thread>
#include <functional>
#include <fstream>
#include <ctime>
#include <condition_variable>
#include "safequeue.h"

typedef std::function<void()> Callback;
class CLogCustomer {
public:
	~CLogCustomer();
	void Insert(Callback msg_);
	static std::unique_ptr<CLogCustomer> CreateCustomer(const int cpuindex = -1);
private:
	CLogCustomer(const CLogCustomer&);
	CLogCustomer& operator=(const CLogCustomer&);
	CLogCustomer() {};
	void DoDone() {
		done_ = true;
	}
	void Run(const int cpuindex);

	CSafeQueue<Callback> mq_;
	std::thread thd_;
	bool done_{ false };
};

class CLogWorker {
public:
	CLogWorker();
	~CLogWorker();
	void Save(std::string&& msg, std::time_t t);
private:
	void CreateLogFile();
	void WriteToFile(std::string& msg, std::time_t& t);
	std::ofstream& FileStream() { return *(outptr_.get()); }
	std::unique_ptr<std::ofstream> outptr_;
	std::unique_ptr<CLogCustomer> customer_{ nullptr };
	std::string file_path_;
	int file_length = 0;
	std::string now_date_{ "" };
};
#endif // WORKER_H_