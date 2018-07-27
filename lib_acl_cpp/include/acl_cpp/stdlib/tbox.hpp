#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include "thread_mutex.hpp"
#include "thread_cond.hpp"
#include "noncopyable.hpp"

namespace acl
{

/**
 * �����߳�֮�����Ϣͨ�ţ�ͨ���߳������������߳���ʵ��
 *
 * ʾ����
 *
 * class myobj
 * {
 * public:
 *     myobj(void) {}
 *     ~myobj(void) {}
 *
 *     void test(void) { printf("hello world\r\n"); }
 * };
 *
 * acl::tbox<myobj> tbox;
 *
 * void thread_producer(void)
 * {
 *     myobj* o = new myobj;
 *     tbox.push(o);
 * }
 *
 * void thread_consumer(void)
 * {
 *     myobj* o = tbox.pop();
 *     o->test();
 *     delete o;
 * }
 */

template<typename T>
class tbox : public noncopyable
{
public:
	tbox(void) : size_(0) , cond_(&lock_) {}

	~tbox(void)
	{
		for (typename std::list<T*>::iterator it = tbox_.begin();
			it != tbox_.end(); ++it)
		{
			delete *it;
		}
	}

	/**
	 * ������Ϣ����
	 * @param t {T*} �ǿ���Ϣ����
	 */
	void push(T* t)
	{
		lock_.lock();
		tbox_.push_back(t);
		size_++;
		lock_.unlock();
		cond_.notify();
	}

	/**
	 * ������Ϣ����
	 * @param wait_ms {int} >= 0 ʱ���õȴ���ʱʱ��(���뼶��)��
	 *  ������Զ�ȴ�ֱ��������Ϣ��������
	 * @param found {bool*} �ǿ�ʱ��������Ƿ�����һ����Ϣ������Ҫ����
	 *  �������ݿն���ʱ�ļ��
	 * @return {T*} �� NULL ��ʾ���һ����Ϣ���󣬷��� NULL ʱ����Ҫ����һ
	 *  ����飬��������� push ��һ���ն���NULL������������Ҳ���� NULL��
	 *  ����ʱ��Ȼ��Ϊ�����һ����Ϣ����ֻ����Ϊ�ն������ wait_ms ����
	 *  Ϊ -1 ʱ���� NULL ��Ȼ��Ϊ�����һ������Ϣ������� wait_ms ����
	 *  ���� 0 ʱ���� NULL����Ӧ�ü�� found ������ֵΪ true ���� false ��
	 *  �ж��Ƿ�����һ������Ϣ����
	 */
	T* pop(int wait_ms = -1, bool* found = NULL)
	{
		long long n = ((long long) wait_ms) * 1000;
		bool found_flag;
		lock_.lock();
		while (true)
		{
			T* t = peek(found_flag);
			if (found_flag) {
				lock_.unlock();
				if (found) {
					*found = found_flag;
				}
				return t;
			}

			if (wait_ms >= 0 && !cond_.wait(n, true)) {
				lock_.unlock();
				if (found) {
					*found = false;
				}
				return NULL;
			}
		}
	}

	/**
	 * ���ص�ǰ��������Ϣ�����е���Ϣ����
	 * @return {size_t}
	 */
	size_t size(void) const
	{
		return size_;
	}

public:
	void lock(void)
	{
		lock_.lock();
	}

	void unlock(void)
	{
		lock_.unlock();
	}

private:
	std::list<T*>     tbox_;
	size_t            size_;
	thread_mutex lock_;
	thread_cond  cond_;

	T* peek(bool& found_flag)
	{
		typename std::list<T*>::iterator it = tbox_.begin();
		if (it == tbox_.end()) {
			found_flag = false;
			return NULL;
		}
		found_flag = true;
		size_--;
		T* t = *it;
		tbox_.erase(it);
		return t;
	}
};

} // namespace acl
