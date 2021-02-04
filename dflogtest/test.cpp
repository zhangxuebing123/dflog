#include "logging.h"
#include <thread>
#include <iostream>
#include <windows.h>
#include <process.h>

int main(int argc, char** argv) {
	auto& log = CDFLogger::GetLogger();
	log.SetFileName("test");
	log.BindWorkerThreadToCPU(0);
	log.SetLogRecord(DETAIL);
	log.ShowConsole(true);
	log.InitLog();

	// 	std::thread t0([&] {
	// 		SetThreadAffinityMask(GetCurrentThread(), 1 << 1);
	// 		while (true) {
	// 		}
	// 	});
	// 	std::thread t1([&] {
	// 		SetThreadAffinityMask(GetCurrentThread(), 1 << 2);
	// 		while (true) {
	// 		}
	// 	});
	// 	SetThreadAffinityMask(GetCurrentThread(), 1 << 3);
	while (true) {
		LOG_DEBUG << argv[0];
	}
	return 0;
}