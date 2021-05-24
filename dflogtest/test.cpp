#include "logging.h"
#include <thread>
#include <iostream>
#include <windows.h>
#include <process.h>

int main(int argc, char** argv) {
	auto& log = CDFLogger::GetLogger();
	log.SetFileName("test");
	log.BindWorkerThreadToCPU(3);
	log.SetLogRecord(DETAIL);
	log.ShowConsole(true);
	log.InitLog();

	/*std::thread t0([&] {
		while (true) {
			LOG_DEBUG << argv[0];
			LOG_INFO << argv[0];
			LOG_WARNING << argv[0];
			LOG_FATAL << argv[0];
		}
	});*/
	//std::thread t1([&] {
	//	while (true) {
	//		LOG_DEBUG << argv[0];
	//		LOG_INFO << argv[0];
	//		LOG_WARNING << argv[0];
	//		LOG_FATAL << argv[0];
	//	}
	//});
	//std::thread t2([&] {
	//	while (true) {
	//		LOG_DEBUG << argv[0];
	//		LOG_INFO << argv[0];
	//		LOG_WARNING << argv[0];
	//		LOG_FATAL << argv[0];
	//	}
	//});
	//std::thread t3([&] {
	//	while (true) {
	//		LOG_DEBUG << argv[0];
	//		LOG_INFO << argv[0];
	//		LOG_WARNING << argv[0];
	//		LOG_FATAL << argv[0];
	//	}
	//});
	while (true) {
		LOG_DEBUG << argv[0];
		LOG_INFO << argv[0];
		LOG_WARNING << argv[0];
		LOG_FATAL << argv[0];
	}
	return 0;
}