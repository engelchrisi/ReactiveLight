#pragma once

// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
class IClearable
{
public:
	virtual void clear(bool keepAllocated) = 0;
};
