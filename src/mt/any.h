
#pragma once

#include "type.h"
#include <new>
#include <stdexcept>

namespace Meta
{ 

template<typename T, typename S>
struct AnyHelper;

//Type helper for small types, such as pointers
template<typename T>
struct AnyHelper<T, _True_>
{
	typedef typename std::is_pointer<T>::type is_pointer;
	typedef typename std::decay<T>::type T_dec;
	typedef typename Pointer<T_dec, is_pointer>::type T_no_cv;

	inline static void clone(const T **src, void **dest)
	{
		new (dest)T(*reinterpret_cast<T const*>(*src));
	}

	inline static T *cast(void **object)
	{
		return const_cast<T*>(reinterpret_cast<T_no_cv*>(object));
	}
};

//Type helper for bigger types
template<typename T>
struct AnyHelper<T, _False_>
{
	typedef typename std::is_pointer<T>::type is_pointer;
	typedef typename std::decay<T>::type T_dec;
	typedef typename Pointer<T_dec, is_pointer>::type T_no_cv;

	inline static void clone(const T **src, void **dest)
	{
		*dest = new T(**src);
	}

	inline static T *cast(void **object)
	{
		return const_cast<T*>(reinterpret_cast<T_no_cv*>(*object));
	}
};

class Any
 { public:
	  	Any();
   	Any(Any const& other);
   	Any(Any &&other);

	  	~Any();

	  	template<typename T>
	   Any(T const& value);

	  	template<typename T, std::size_t N>
	   Any(T(&value)[N]);

   	template<typename T>
	   void reset(T const& value);

		  void reset();

   	std::type_info const& type() const;

   private:
	   template<typename T>
	   friend T* any_cast(Any*);

	   TypeTable* _table;
	   void* _object;
 };

template<typename T>
Any::Any(T const& x) : _table(Table<T>::get()), _object(nullptr)
{
	const T *src = &x;
	AnyHelper<T, typename Table<T>::is_small>::clone(&src, &_object);
}

template<typename T, std::size_t N>
Any::Any(T(&x)[N]) : _table(Table<T*>::get()), _object(nullptr)
{
	new (&_object) T*(&x[0]);
}

template<typename T>
void Any::reset(T const& x)
{
	if (_table)
		_table->static_delete(&_object);
	_table = Table<T>::get();
	const T *src = &x;
	AnyHelper<T, typename Table<T>::is_small>::clone(&src, &_object);
}

template <typename T>
inline T* any_cast(Any* operand)
{
	if (operand && operand->_table == Table<T>::get())
		return AnyHelper<T, typename Table<T>::is_small>::cast(&operand->_object);

	return nullptr;
}

template <typename T>
inline T* any_cast(Any const* operand)
{
	return any_cast<T>(const_cast<Any*>(operand));
}

/*! Casts Any container to a given type T.
	\relates Any
	Use it as follows:
	\code{.cpp}
	Any a{5.0};
	double d = any_cast<double>(a);
	\endcode
*/
template <typename T>
inline T any_cast(Any& operand)
{
	typedef typename std::remove_reference<T>::type nonref;

	nonref* result = any_cast<nonref>(&operand);
	if (!result)
		throw std::runtime_error("Bad cast");
	return *result;
}

/*! Casts Any container to a given type T.
	\relates Any
	Use it as follows:
	\code{.cpp}
	Any a{5.0};
	double d = any_cast<double>(a);
	\endcode
*/
template <typename T>
inline T const& any_cast(Any const& operand)
{
	typedef typename std::remove_reference<T>::type nonref;
	return any_cast<nonref const&>(const_cast<Any&>(operand));
} 

}
