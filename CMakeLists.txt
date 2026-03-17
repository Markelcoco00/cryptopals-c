cmake_minimum_required(VERSION 3.16)
project(cryptopals_c C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(challenge1
    set1/challenge1_hex_to_base64/main.c
)

add_executable(challenge2
    set1/challenge2_fixed_xor/main.c
)

add_executable(challenge3
    set1/challenge3_single_byte_xor/main.c
)
