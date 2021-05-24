/*!
 * \file util.h
 * \date 2021/02/03 9:01
 *
 * \author zhangxuebing
 * Contact: zhangxuebing@orientsec.com.cn
 * \brief tools
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <time.h>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <stdint.h>
#include <ctime>

#ifdef _WIN32
#define _WINSOCKAPI_
#include <windows.h>
#include <WinSock2.h>
#include <shlwapi.h>
#include <direct.h>
#include <process.h>
#include <corecrt_io.h>
#else
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#endif

#define  MAX_PATH_NUM  1024

namespace util {
#ifdef _WIN32
#ifndef H_GETTIMEOFDAY
#define H_GETTIMEOFDAY
	//@refer evpp/gettimeofday.h

	inline int gettimeofday(struct timeval* tp, void* tzp) {
		uint64_t  intervals;
		FILETIME  ft;

		GetSystemTimeAsFileTime(&ft);

		/*
		* A file time is a 64-bit value that represents the number
		* of 100-nanosecond intervals that have elapsed since
		* January 1, 1601 12:00 A.M. UTC.
		*
		* Between January 1, 1970 (Epoch) and January 1, 1601 there were
		* 134744 days,
		* 11644473600 seconds or
		* 11644473600,000,000,0 100-nanosecond intervals.
		*
		* See also MSKB Q167296.
		*/

		intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
		intervals -= 116444736000000000;

		tp->tv_sec = (long)(intervals / 10000000);
		tp->tv_usec = (long)((intervals % 10000000) / 10);

		return (0);
	}
#endif // H_GETTIMEOFDAY
#endif //_WIN32

	inline double utcsecond() {
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		return (double)(tv.tv_sec) + ((double)(tv.tv_usec)) / 1000000.0f;
	}

	inline uint64_t utcmicrosecond() {
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		return (uint64_t)(((uint64_t)(tv.tv_sec)) * 1000000 + tv.tv_usec);
	}

	inline struct timeval timevalconv(uint64_t time_us) {
		struct timeval tv;
		tv.tv_sec = (long)time_us / 1000000;
		tv.tv_usec = (long)time_us % 1000000;
		return tv;
	}

	template< class StringVector,
		class StringType,
		class DelimType>
		inline void StringSplit(
			const StringType& str,
			const DelimType& delims,
			unsigned int maxSplits,
			StringVector& ret) {
		if (str.empty()) {
			return;
		}

		unsigned int numSplits = 0;

		// Use STL methods
		size_t start, pos;
		start = 0;

		do {
			pos = str.find_first_of(delims, start);

			if (pos == start) {
				ret.push_back(StringType());
				start = pos + 1;
			} else if (pos == StringType::npos || (maxSplits && numSplits + 1 == maxSplits)) {
				// Copy the rest of the string
				ret.emplace_back(StringType());
				*(ret.rbegin()) = StringType(str.data() + start, str.size() - start);
				break;
			} else {
				// Copy up to delimiter
				//ret.push_back( str.substr( start, pos - start ) );
				ret.push_back(StringType());
				*(ret.rbegin()) = StringType(str.data() + start, pos - start);
				start = pos + 1;
			}

			++numSplits;
		} while (pos != StringType::npos);
	}

	inline void trim(std::string& msg) {
		auto rhs = msg.find_first_not_of(' ');
		if (rhs == msg.npos) {
			msg = "";
			return;
		}
		msg.erase(0, rhs);

		auto lhs = msg.find_last_not_of(' ');
		if (lhs == msg.npos) {
			msg = "";
			return;
		}
		msg.erase(msg.begin() + lhs + 1, msg.end());
	}
	inline const std::time_t systemNow() {
		const auto now = std::chrono::system_clock::now();
		return std::chrono::system_clock::to_time_t(now);
	}
	//mmdd
	inline const std::string timeToString1(struct tm* pTm) {
		std::stringstream ss;
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_mon + 1;
		ss.width(2);
		ss << pTm->tm_mday;
		return std::move(ss.str());
	}
	//yyyymmdd
	inline const std::string timeToString2(struct tm* pTm) {
		std::stringstream ss;
		ss << pTm->tm_year + 1900;
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_mon + 1;
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_mday;
		return std::move(ss.str());
	}
	//yyyy/mm/dd
	inline const std::string timeToString5(struct tm* pTm) {
		std::stringstream ss;
		ss << pTm->tm_year + 1900;
		ss << "/";
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_mon + 1;
		ss << "/";
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_mday;
		return std::move(ss.str());
	}
	//hh:mm:ss
	inline const std::string timeToString3(struct tm* pTm) {
		std::stringstream ss;
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_hour;
		ss << ":";
		ss.width(2);
		ss << pTm->tm_min;
		ss << ":";
		ss.width(2);
		ss << pTm->tm_sec;
		return std::move(ss.str());
	}
	//hhmmss
	inline const std::string timeToString4(struct tm* pTm) {
		std::stringstream ss;
		ss.fill('0');
		ss.width(2);
		ss << pTm->tm_hour;
		ss.width(2);
		ss << pTm->tm_min;
		ss.width(2);
		ss << pTm->tm_sec;
		return std::move(ss.str());
	}

	inline bool isdir(const std::string &path) {
#ifdef _WIN32
		return PathIsDirectoryA(path.c_str()) ? true : false;
#else
		DIR *pdir = opendir(path.c_str());
		if (pdir == NULL) {
			return false;
		} else {
			closedir(pdir);
			pdir = NULL;
			return true;
		}
#endif // _WIN32
	}

	inline bool mkdir(const std::string &path) {
		std::string tmp = path;
		if (0 == tmp.length()) {
			return false;
		}

		for (std::string::iterator iter = tmp.begin(); iter != tmp.end(); ++iter) {
			if (*iter == '\\') {
				*iter = '/';
			}
		}
		if (tmp.at(tmp.length() - 1) != '/') {
			tmp.append("/");
		}

		std::string::size_type pos = tmp.find('/');
		while (pos != std::string::npos) {
			std::string cur = tmp.substr(0, pos - 0);
			if (cur.length() > 0 && !isdir(cur)) {
				bool result = false;
#ifdef _WIN32
				result = CreateDirectoryA(cur.c_str(), NULL) ? true : false;
				if (!result) {
					return false;
				}
#else
				result = (::mkdir(cur.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
				if (!result) {
					if (EEXIST != errno) {
						return false;
					}
					result = true;
				}
				if (!result) {
					return false;
				}
#endif // _WIN32
			}
			pos = tmp.find('/', pos + 1);
		}
		return true;
	}

	inline bool lsdir(const std::string &path, std::vector<std::string>& files) {
#ifdef _WIN32
		_finddata_t fd;
		intptr_t handle;
		handle = _findfirst((path + "*.*").c_str(), &fd);
		if (handle < 0) {
			/* NOTE: The caller my lock the _keys_mtx */
			/* LOGFE("_findfirst err: %d", path.c_str()); */
			return false;
		}
		while (_findnext(handle, &fd) != -1) {
			files.push_back(fd.name);
		}
		_findclose(handle);
#else
		struct dirent *file = NULL;
		DIR *dir = opendir(path.c_str());
		if (!dir) {
			/* NOTE: The caller my lock the _keys_mtx */
			/* LOGFE("opendir err: %d", path.c_str()); */
			return false;
		}
		while ((file = readdir(dir)) != NULL) {
			files.push_back(file->d_name);
		}
		closedir(dir);
#endif // _WIN32
		return true;
	}

	inline bool cwd(std::string& str) {
		str.clear();
		char path[MAX_PATH_NUM] = {};
		if (!getcwd(path, MAX_PATH_NUM))
			return false;
		str.append(path);
		return true;
	}

	inline bool setcpu(const int index) {
#ifdef _WIN32
		auto rc = SetThreadAffinityMask(GetCurrentThread(), 1 << index);
		if (rc == 0)
			return false;
#else
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(index, &cpuset);
		auto rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
		if (rc != 0)
			return false;
#endif
		return true;
	}
}

#endif
