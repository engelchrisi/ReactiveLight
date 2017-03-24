#ifndef __DYNAMICBUFFER_
#define __DYNAMICBUFFER_

#include "IClearable.h"

// ReSharper disable CppCStyleCast
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr

// ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
template <class T, int FIXED_BUF_ELEMS>
class VolatileDynamicBufferData
{
	public:
	static int getFixedBufElems()
	{
		return FIXED_BUF_ELEMS;
	}

	protected:
	T			  mFixedBuf[FIXED_BUF_ELEMS]; // >= 1: 0 array is not allowed!

	volatile T*	  mDynBuf/*[mDynBufElems]*/;
	volatile int  mDynBufElems;

	volatile int  mCurrentElems; // either for mFixedBuf or
};

// ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
template <class T, int FIXED_BUF_ELEMS>
class DynamicBufferData
{
	public:
	static int getFixedBufElems()
	{
		return FIXED_BUF_ELEMS;
	}

	protected:
	T		mFixedBuf[FIXED_BUF_ELEMS]; // >= 1: 0 array is not allowed!

	T*		mDynBuf/*[mDynBufElems]*/;
	int		mDynBufElems;

	int		mCurrentElems; // either for mFixedBuf or
};


template < class BASE, typename T, int GROW_BY_ELEMS>
class DynamicBuffer : protected BASE, public IClearable {
	friend class DynamicBufferTest;

	public:
	typedef T ELEM_TYPE;

	DynamicBuffer() {
		this->mDynBuf = NULL;
		this->mDynBufElems = this->mCurrentElems = 0;
		memset(this->mFixedBuf, 0, sizeof(this->mFixedBuf));
	}

	DynamicBuffer(const DynamicBuffer& other)
	{
		*this = other;
	}

	DynamicBuffer& operator=(const DynamicBuffer& other)
	{
		doClear(true);
		
		write(other.getData(), other.getUsedElems());

		return *this;
	}

	virtual ~DynamicBuffer() {
		doClear(false);
	}

	static int getMinDynBufElems()
	{
		return GROW_BY_ELEMS;
	}

	bool isEmpty() const {
		return this->mCurrentElems <= 0;
	}


	void clear(bool keepAllocated) override
	{
		doClear(keepAllocated);
	}

	const T* getData() const {
		return (T*)(this->mDynBuf == NULL? this->mFixedBuf : this->mDynBuf);
	}

	T* getData() {
		return (T*)(this->mDynBuf == NULL? this->mFixedBuf : this->mDynBuf);
	}
	
	const T& operator[](int index) const
	{
		if (index >= this->mCurrentElems)
		{
			T NULL_ELEMENT;
			return NULL_ELEMENT; // TODO
		}
		return const_cast<DynamicBuffer*>(this)->getData()[index];
	}
	
	T& operator[](int index)
	{
		if (index >= this->mCurrentElems)
		{
			T NULL_ELEMENT;
			return NULL_ELEMENT; // TODO
		}

		return this->getData()[index];
	}

	int getUsedElems() const {
		return this->mCurrentElems;
	}

	size_t getUsedBytes() const {
		return this->mCurrentElems * sizeof(T);
	}

	int getAllocatedElems() const
	{
		return this->mDynBufElems;
	}

	size_t getAllocatedBytes() const
	{
		return getUsedElems() * sizeof(T);
	}

	void write(const T* buf, int nElems)
	{
		assureNewElements(nElems);

		size_t newSize = nElems * sizeof(T);
		memcpy((T*)getData() + this->mCurrentElems, buf, newSize);
		this->mCurrentElems += nElems;
	}

	void write(T elem)
	{
		write(&elem, 1);
	}

	private:
	void doClear(bool keepAllocated)
	{
		if (!keepAllocated)
		{
			if (this->mDynBuf != NULL) {
				free((void*)this->mDynBuf);
				this->mDynBuf = NULL; // TODO: resize to GROW_BY_ELEMS or not?
				this->mDynBufElems = 0;
			}
		}
		this->mCurrentElems = 0;
	}

	void assureNewElements(int newElems) {
		if (this->mCurrentElems + newElems <= _countof(this->mFixedBuf)) {
			return;
		}

		if (this->mCurrentElems + newElems <= this->mDynBufElems) {
			return;
		}

		const int roundedNewElems = ((newElems / GROW_BY_ELEMS)+1) * GROW_BY_ELEMS;

		if (this->mDynBuf == NULL)
		{
			this->mDynBufElems = _countof(this->mFixedBuf) + roundedNewElems;

			size_t newBufSize = this->mDynBufElems * sizeof(T);
			this->mDynBuf = static_cast<T*>(malloc(newBufSize));
			memcpy((void*)this->mDynBuf, this->mFixedBuf, this->mCurrentElems * sizeof(T));
		}
		else
		{
			this->mDynBufElems += roundedNewElems;

			size_t newBufSize = this->mDynBufElems * sizeof(T);
			this->mDynBuf = static_cast<T*>(realloc((void*)this->mDynBuf, newBufSize));
		}
	}
};

#endif