#ifndef __DYNAMICPTRBUFFER_
#define __DYNAMICPTRBUFFER_

#include "DynamicBuffer.h"

// ReSharper disable CppCStyleCast
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr

template < class BASE, class T, int GROW_BY_ELEMS>
class DynamicPtrBuffer : public DynamicBuffer<BASE, T, GROW_BY_ELEMS>
{
	typedef  DynamicBuffer<BASE, T, GROW_BY_ELEMS> SUPER;
	friend class DynamicPtrBufferTest;
    
public:
    typedef T ELEM_TYPE;

	DynamicPtrBuffer() {
	}

	virtual ~DynamicPtrBuffer() {
		doClear();
	}

	virtual void clear(bool keepAllocated)
	{
		doClear();
		SUPER::clear(keepAllocated);
	}

private:
	void doClear()
	{
		ELEM_TYPE pData = NULL;
		ELEM_TYPE* ppData = this->getData();
		
		for (int i = 0; i < this->getUsedElems(); ++i)
		{
			pData = ppData[i];
			ppData[i] = NULL;
			delete pData;
		}
	}
};
#endif