#pragma once

#ifndef CONCURRENT_QUEUE_
#define CONCURRENT_QUEUE_

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue
{
public:

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto val = queue_.front();
		queue_.pop();
		return val;
	}

	bool pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		if (queue_.empty())
			return false;

		item = queue_.front();
		queue_.pop();
		return true;
	}

	bool empty()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		bool is_empty = queue_.empty();
		mlock.unlock();
		return is_empty;
	}

	int count()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		int count = queue_.size();
		mlock.unlock();
		return count;
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}
	Queue() = default;
	Queue(const Queue&) = delete;            
	Queue& operator=(const Queue&) = delete;

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

#endif