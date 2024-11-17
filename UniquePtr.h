#pragma once
#ifndef _MY_SMART_PTR_
#define _MY_SMART_PTR_
#include <iostream>
#include <utility>
namespace mystl {
	template<typename T>
	class _unique_ptr 
	{
		/*
		unique_ptr的需求：
			构造：
				允许直接构造和移动构造（右值）
				禁止拷贝构造
		
		*/
	private:
		T* ptr;

	public:
		explicit _unique_ptr(T* p = nullptr) : ptr(p) {};

		_unique_ptr(const _unique_ptr&) = delete;

		_unique_ptr& operator = (const _unique_ptr&) = delete;

		_unique_ptr(_unique_ptr&& other) noexcept : ptr(other.ptr)
		{
			other.ptr = nullptr;
		};

		_unique_ptr& operator = (_unique_ptr&& other) noexcept
		{
			if (this != &other)
			{
				delete ptr;
				ptr = other.ptr;
				other.ptr = nullptr;
			}
			return *this;
		}

		~_unique_ptr()
		{
			delete ptr;
		}

		T& operator*() const
		{
			return *ptr;
		}

		T* operator->() const
		{
			return ptr;
		}

		T* get() const
		{
			return ptr;
		}

		T* release()
		{
			T* tmp = ptr;
			ptr = nullptr;
			return tmp;
		}

		void reset(T* p = nullptr)
		{
			if (ptr != p)
			{
				delete ptr;
				ptr = p;
			}
		}
	};

	template<typename T>
	class _shared_ptr
	{
	private:
		T* ptr;
		int* ref_count;

	public:
		//允许拷贝，但要增加计数
		_shared_ptr(T* p) : ptr(p), ref_count(new int(1)) {};

		_shared_ptr(const _shared_ptr& other) :ptr(other.ptr), ref_count(other.ref_count)
		{
			(*ref_count)++;
		}

		_shared_ptr& operator=(const _shared_ptr& other) 
		{

		}
	};
}

#endif // !_MY_SMART_PTR_



