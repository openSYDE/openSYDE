#pragma once

/* strict names */
// 2021-03-29 STW: Do not use option "strict names" as it leads to tool crashes e.g. if signals start with a digit
// #define OPTION_USE_STRICT_NAMES

/* regex support by C++11 or BOOST */
#define OPTION_USE_CPP11_REGEX
