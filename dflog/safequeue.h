/*!
 * \file safequeue.h
 * \date 2021/02/02 11:04
 *
 * \author zhangxuebing
 * Contact: zhangxuebing@orientsec.com.cn
 * \brief 安全队列
 */

#ifndef SAFE_QUEUE_
#define SAFE_QUEUE_

#include <queue>
#include <mutex>
#include <exception>
#include <condition_variable>

template<typename T>
class CSafeQueue {
	std::queue<T> queue_;
	mutable std::mutex m_;
	std::condition_variable data_cond_;

	CSafeQueue& operator=(const CSafeQueue&);
	CSafeQueue(const CSafeQueue& other);

public:
	CSafeQueue() {}
	void push(T item) {
		{
			std::lock_guard<std::mutex> lock(m_);
			queue_.push(item);
		}
		data_cond_.notify_one();
	}
	bool try_and_pop(T& popped_item) {
		std::lock_guard<std::mutex> lock(m_);
		if (queue_.empty()) {
			return false;
		}
		popped_item = std::move(queue_.front());
		queue_.pop();
		return true;
	}
	void wait_and_pop(T& popped_item) {
		std::unique_lock<std::mutex> lock(m_);
		while (queue_.empty()) {
			data_cond_.wait(lock);
		}
		popped_item = std::move(queue_.front());
		queue_.pop();
	}
	bool empty() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.empty();
	}
	unsigned size() const {
		std::lock_guard<std::mutex> lock(m_);
		return queue_.size();
	}
};

#endif //SAFE_QUEUE_
