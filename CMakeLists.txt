add_library(vofa STATIC)

target_sources(vofa PRIVATE ./vofa.c)
target_include_directories(vofa PUBLIC ./)