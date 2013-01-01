/**
	File: ScopedPointer.h
	Author: Lars Woxberg | Rarosu

	A smart pointer class, owning the resource it points to, and deleting it when
	going out of scope. 
	
	Things to know:
	* Must own a dynamically created pointer
	* Cannot own arrays
	* If the inner pointer is replaced, the old pointer is first deleted
	* Can own NULL
	* Ownership can be transferred but not shared
*/

#ifndef SCOPED_POINTER_H
#define SCOPED_POINTER_H

template <typename T>
class ScopedPointer
{
public:
	/** Constructor - initialize to NULL */
	ScopedPointer();

	/** Constructor - initialize to resource */
	ScopedPointer(T* resource);

	/** Constructor - transfer ownership from pointer to this */
	ScopedPointer(ScopedPointer<T>& pointer);

	/** Destructor */
	~ScopedPointer();


	/** Assignment operator */
	ScopedPointer& operator=(T* resource);

	/** Assignment operator */
	ScopedPointer& operator=(ScopedPointer<T>& pointer);


	/** Access operators - to access the inner resource */
	T* operator->();
	const T* operator->() const;

	T& operator*();
	const T& operator*() const;


	/** Comparison operators */
	bool operator==(const T* rhs) const;
	bool operator!=(const T* rhs) const;


	/** Access the inner resource */
	T* Resource();
	const T* Resource() const;
private:
	T* m_resource;

	/** Non-transferrable */
	ScopedPointer(const ScopedPointer<T>& copy);
	ScopedPointer& operator=(const ScopedPointer<T>& copy);
};



/** IMPLEMENTATION */


template <typename T>
ScopedPointer<T>::ScopedPointer()
	: m_resource(NULL) {}

template <typename T>
ScopedPointer<T>::ScopedPointer(T* resource)
	: m_resource(resource) {}

template <typename T>
ScopedPointer<T>::ScopedPointer(ScopedPointer<T>& pointer)
{
	m_resource = pointer.m_resource;
	pointer.m_resource = NULL;
}

template <typename T>
ScopedPointer<T>::~ScopedPointer()
{
	delete m_resource;
}


template <typename T>
ScopedPointer<T>& ScopedPointer<T>::operator=(T* resource)
{
	if (resource != m_resource)
	{
		delete m_resource;
		m_resource = resource;
	}

	return *this;
}

template <typename T>
ScopedPointer& ScopedPointer<T>::operator=(ScopedPointer<T>& pointer)
{
	if (pointer != *this)
	{
		// Since it is a different pointer, it cannot own the same memory. Delete the
		// old memory and assume ownership of pointer's memory.
		delete m_resource;
		m_resource = pointer.m_resource;
		pointer.m_resource = NULL;
	}

	return *this;
}


template <typename T>
T* ScopedPointer<T>::operator->()
{
	return m_resource;
}

template <typename T>
const T* ScopedPointer<T>::operator->() const
{
	return m_resource;
}


template <typename T>
T& ScopedPointer<T>::operator*()
{
	return *m_resource;
}

template <typename T>
const T& ScopedPointer<T>::operator*() const
{
	return *m_resource;
}


template <typename T>
bool ScopedPointer<T>::operator==(const T* rhs) const
{
	return m_resource == rhs;
}

template <typename T>
bool ScopedPointer<T>::operator!=(const T* rhs) const
{
	return m_resource != rhs;
}


template <typename T>
T* ScopedPointer<T>::Resource()
{
	return m_resource;
}

template <typename T>
const T* ScopedPointer<T>::Resource() const
{
	return m_resource;
}

#endif