#pragma once

template <typename T, int N>
class OrderedArray
{
public:
	typedef T& reference;
	typedef T* iterator;
	typedef const T* const_iterator;
	
	typedef size_t size_type;

private:
	T data[N];
	iterator currentEnd;
	
public:
	OrderedArray();

	void insert(const T& value);
	T& at(size_type n) { return data[n]; }
	reference operator[](size_type n) { return data[n]; }
	iterator erase(const_iterator position);
	
	iterator begin() noexcept { return data; };
	iterator end() noexcept { return currentEnd; };
};

template <typename T>
typename T::iterator begin(T& cont)
{
	return cont.begin();
}

template <typename T>
typename T::iterator end(T& cont)
{
	return cont.end();
}

template <typename T, int N>
OrderedArray<T, N>::OrderedArray()
	: currentEnd(data)
{
}

template <typename T, int N>
void OrderedArray<T, N>::insert(const T& value)
{
	//assert(currentEnd <= data + N);

	iterator insertPos = data;
	
	while (insertPos < currentEnd && *insertPos <= value)
	{
		++insertPos;
	}
	
	for (iterator it = currentEnd; it > insertPos; --it)
	{
		*it = *(it - 1);
	}
	
	*insertPos = value;
	++currentEnd;
}

template <typename T, int N>
typename OrderedArray<T, N>::iterator OrderedArray<T, N>::erase(const_iterator position)
{
	//assert(position < currentEnd);
	//assert(position >= data);
	
	for (iterator it = const_cast<iterator>(position); it < currentEnd - 1; ++it)
	{
		*it = *(it + 1);
	}
	
	--currentEnd;
	return const_cast<iterator>(position);
}

