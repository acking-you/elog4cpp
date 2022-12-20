//
// Created by Alone on 2022-9-23.
//

#include "my-logger/count_down_latch.h"

USING_LBLOG_DETAIL

CountDownLatch::CountDownLatch(int count)
	: m_mtx(), m_cv(), m_count(count)
{

}

void CountDownLatch::wait()
{
	std::unique_lock<std::mutex> lock(m_mtx);
	while (m_count > 0)
	{
		m_cv.wait(lock);
	}
}

void CountDownLatch::countDown()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	--m_count;
	if (m_count == 0)
	{
		m_cv.notify_all();
	}
}

int CountDownLatch::getCount()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	return m_count;
}