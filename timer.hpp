/**
 * @file timer.hpp
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2022 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * Demo program sieve of Eratosthenes, utility timing functions.
 */

#ifndef TILEDB_TIMER_HPP
#define TILEDB_TIMER_HPP

#include <chrono>
#include <iostream>
#include <utility>

template <class F, class... Args>
auto timer_1(F f, size_t max, Args&&... args) {
  auto start = std::chrono::high_resolution_clock::now();
  auto s = f(max, std::forward<Args>(args)...);
  auto stop = std::chrono::high_resolution_clock::now();

  auto a = sieve_to_primes(s);
  std::cout << a.size() << ": ";
  std::cout << a[0] << ", " << a[1] << " ... " << a[a.size()-2] << ", " << a[a.size()-1] << std::endl;

  return stop - start;
}

template <class F, class... Args>
auto timer_2(F f, size_t max, Args && ... args) {
  auto start = std::chrono::high_resolution_clock::now();
  auto s = f(max, std::forward<Args>(args)...);
  auto stop = std::chrono::high_resolution_clock::now();

  size_t num = 0;
  for (auto& j : s) {
    if (j) {
      num += j->size();
    }
  }
  std::cout << num << ": " << std::endl;

  return stop - start;
}

#endif // TILEDB_TIMER_HPP
