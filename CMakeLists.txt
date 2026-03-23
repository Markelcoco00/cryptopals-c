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

add_executable(challenge4
    set1/challenge4_single_character_xor/main.c
)

add_executable(challenge5
    set1/challenge5_repeating_key_xor/main.c
)

add_executable(challenge6
    set1/challenge6_Break_repeating_key_xor/main.c
    set1/common/utils.c
)

add_executable(challenge9
    set2/challenge9_Implement_PKCS7_padding/main.c
)

add_executable(challenge15
    set2/challenge15_PKCS7_padding_validation/main.c
)
