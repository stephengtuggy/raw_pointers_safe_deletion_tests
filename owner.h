/*
 * The MIT License (MIT)
 *
 * raw_pointers_safe_deletion_tests
 *
 * Copyright (C) 2024 Stephen G. Tuggy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef RAW_POINTERS_SAFE_DELETION_TESTS_OWNER_H
#define RAW_POINTERS_SAFE_DELETION_TESTS_OWNER_H

#include <memory>
#include <type_traits>

using std::shared_ptr;
using std::unique_ptr;

template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
using owner = T;

#endif //RAW_POINTERS_SAFE_DELETION_TESTS_OWNER_H
