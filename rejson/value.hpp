#ifndef REJSON_VALUE_HPP_
#define REJSON_VALUE_HPP_

#include <rejson/export.h>

#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rejson {

class Value;

using Int = int;
using Bool = bool;
using Real = double;
using String = std::string;
using Null = std::nullptr_t;

using Array = std::vector<Value>;
using KeyValuePair = std::pair<std::string, Value>;
using Object = std::unordered_map<std::string, Value>;

template <class T>
struct to_json;

enum class ValueType {
	Null, Int, Real, Bool, String, Object, Array
};

class REJSON_EXPORT Value
{
	using value_storage_t = boost::variant<
		boost::blank, Int, Real, Bool, String,
		boost::recursive_wrapper<Object>,
		boost::recursive_wrapper<Array>
	>;

public:
	Value() noexcept;
	Value(Null n) noexcept;

	Value(Int i);
	Value(Real f);
	Value(Bool b);
	Value(Array a);
	Value(String s);
	Value(Object o);

	Value(const char * s);

	Value(const Value & other);
	Value(Value && other);

	template <class T, std::enable_if_t<
		(sizeof(to_json<T>) > 0)
	>... >
	Value(const T & t);

	template <class M, std::enable_if_t<
		std::is_constructible<std::string, typename M::key_type>::value
		&& std::is_constructible<Value, typename M::mapped_type>::value
	>... >
	Value(const M & m);

	template <class V, std::enable_if_t<
		std::is_constructible<Value, typename V::value_type>::value
	>... >
	Value(const V & v);

	ValueType type() const;

	bool is_int() const;
	bool is_real() const;
	bool is_null() const;
	bool is_bool() const;
	bool is_array() const;
	bool is_string() const;
	bool is_object() const;

	Int as_int() const;
	Real as_real() const;
	Bool as_bool() const;

	Array as_array() &&;
	Array & as_array() &;
	const Array & as_array() const &;

	String as_string() &&;
	String & as_string() &;
	const String & as_string() const &;

	Object as_object() &&;
	Object & as_object() &;
	const Object & as_object() const &;

	void swap(Value & other);

	Value & operator=(Array && a);
	Value & operator=(String && s);
	Value & operator=(Object && o);

	Value & operator=(const Value & other);
	Value & operator=(Value && other);

private:
	value_storage_t value_;
};

template <class T, std::enable_if_t<
	(sizeof(to_json<T>) > 0)
>... >
Value::Value(const T & t)
	: Value { to_json<T>()(t) } {}

template <class M, std::enable_if_t<
	std::is_constructible<std::string, typename M::key_type>::value
	&& std::is_constructible<Value, typename M::mapped_type>::value
>... >
Value::Value(const M & m)
	: Value { Object(m.begin(), m.end()) } {}

template <class V, std::enable_if_t<
	std::is_constructible<Value, typename V::value_type>::value
>... >
Value::Value(const V & v)
	: Value { Array(v.begin(), v.end()) } {}

}

#endif
