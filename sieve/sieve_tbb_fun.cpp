/**
 * @file sieve_tbb_fun.cpp
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
 * Demo program sieve of Eratosthenes, TBB implementation
 */

#include <tbb/tbb.h>

#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include "sieve.hpp"
#include "sieve_fun.hpp"
#include "timer.hpp"

using namespace std::placeholders;

/**
 * Generate a (thread safe) sequence of integers, starting at 0
 * @return integer, value one greater than previously returned
 */
class input_body_tbb {
  const size_t block_size_;
  const size_t n_;
  std::atomic<size_t> p{0};

 public:
  input_body_tbb(size_t block_size, size_t n)
      : block_size_{block_size}
      , n_{n}
      , p{0} {
  }
  size_t operator()(oneapi::tbb::flow_control& fc) {
    if (debug)
      std::cout << "input_body_tbb " << p << std::endl;
    if (p * block_size_ >= n_) {
      fc.stop();
      return 0UL;
    }
    return p++;
  }
};

template <class bool_t>
auto sieve_tbb_block(size_t n, size_t block_size, size_t width) {
  size_t sqrt_n = static_cast<size_t>(std::ceil(std::sqrt(n)));

  /* Generate base set of sqrt(n) primes to be used for subsequent sieving */
  auto first_sieve = sieve_seq<bool_t>(sqrt_n);
  std::vector<size_t> base_primes = sieve_to_primes(first_sieve);

  /* Store vector of list of primes (each list generated by separate task chain) */
  std::vector<std::shared_ptr<std::vector<size_t>>> prime_list(n / block_size + 2);
  prime_list[0] = std::make_shared<std::vector<size_t>>(base_primes);

  input_body_tbb gen{block_size, n};

  tbb::flow::graph g;

  tbb::flow::input_node<size_t> inp{g, std::ref(gen)};
  tbb::flow::function_node<size_t, part_info<bool_t>> rng{g, width, std::bind(gen_range<bool_t>, _1, block_size, sqrt_n, n)};
  tbb::flow::function_node<part_info<bool_t>, part_info<bool_t>> fn{
      g, width, std::bind(range_sieve<bool_t>, _1, std::cref(base_primes))};
  tbb::flow::function_node<part_info<bool_t>, prime_info> flt{g, width, sieve_to_primes_part<bool_t>};
  tbb::flow::function_node<prime_info> outp{g, width, std::bind(output_body, _1, std::ref(prime_list))};

  tbb::flow::make_edge(inp, rng);
  tbb::flow::make_edge(rng, fn);
  tbb::flow::make_edge(fn, flt);
  tbb::flow::make_edge(flt, outp);

  inp.activate();

  g.wait_for_all();

  return prime_list;
}

int main(int argc, char* argv[]) {
  size_t number = 100'000'000;
  size_t block_size = 100;
  size_t width = std::thread::hardware_concurrency();

  if (argc >= 2) {
    number = std::stol(argv[1]);
  }
  if (argc >= 3) {
    block_size = std::stol(argv[2]);
  }
  if (argc >= 4) {
    width = std::stol(argv[3]);
  }

  auto using_bool_tbb_block = timer_2(sieve_tbb_block<bool>, number, block_size * 1024, width);
  auto using_char_tbb_block = timer_2(sieve_tbb_block<char>, number, block_size * 1024, width);

  std::cout << "Time using bool tbb block: " << duration_cast<std::chrono::milliseconds>(using_bool_tbb_block).count() << "\n";
  std::cout << "Time using char tbb block: " << duration_cast<std::chrono::milliseconds>(using_char_tbb_block).count() << "\n";
}