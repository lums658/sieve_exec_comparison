
add_executable       (sieve_async_fun.exe     sieve_async_fun.cpp)
target_link_libraries(sieve_async_fun.exe     PUBLIC)

add_executable       (sieve_cc_fun.exe        sieve_cc_fun.cpp)
target_link_libraries(sieve_cc_fun.exe        PUBLIC concurrencpp)

add_executable       (sieve_direct_fun.exe    sieve_direct_fun.cpp)
target_link_libraries(sieve_direct_fun.exe    PUBLIC)

add_executable       (sieve_p2300_fun.exe     sieve_p2300_fun.cpp)
target_link_libraries(sieve_p2300_fun.exe     PUBLIC kirkshoop)

add_executable       (sieve_tbb_fun.exe       sieve_tbb_fun.cpp)
target_link_libraries(sieve_tbb_fun.exe       PUBLIC TBB::tbb)

add_executable       (sieve_unifex_fun.exe    sieve_unifex_fun.cpp)
target_link_libraries(sieve_unifex_fun.exe    PUBLIC unifex)
