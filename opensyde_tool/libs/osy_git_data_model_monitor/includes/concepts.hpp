#ifndef __CONCEPTS_HPP__
#define __CONCEPTS_HPP__

#include <type_traits>

// --------------------------------------------------------------------- //
// use owner concept without smart pointer overhead
// although the unique_pointer overhead should be minimal
template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
using owner = T;

#endif // __CONCEPTS_HPP__