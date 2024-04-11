cmake_minimum_required(VERSION 3.1)

include_directories("C:\\Program Files\\OpenSSL-Win64\\include")
link_directories("C:\\Program Files\\OpenSSL-Win64\\lib")
add_executable(author-validation validation.cpp)
