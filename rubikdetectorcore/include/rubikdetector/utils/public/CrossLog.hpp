//
// Created by catalin on 30.07.2017.
//

#ifndef RUBIKDETECTOR_CROSSLOG_HPP
#define RUBIKDETECTOR_CROSSLOG_HPP

#ifdef __ANDROID__

#include <android/log.h>

#define LOG_VERBOSE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_VERBOSE, (LOG_TAG), ## __VA_ARGS__)
#define LOG_DEBUG(LOG_TAG, ...) __android_log_print(ANDROID_LOG_DEBUG, (LOG_TAG), ## __VA_ARGS__)
#define LOG_WARN(LOG_TAG, ...) __android_log_print(ANDROID_LOG_WARN, (LOG_TAG), ## __VA_ARGS__)
#define LOG_ERROR(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR, (LOG_TAG), ## __VA_ARGS__)

#else // Non-mobile platform

#include <iostream>

#define LOG_VERBOSE(LOG_TAG, ...) printf("\n" LOG_TAG ": " __VA_ARGS__)
#define LOG_DEBUG(LOG_TAG, ...) printf("\n" LOG_TAG ": " __VA_ARGS__)
#define LOG_WARN(LOG_TAG, ...) printf("\n" LOG_TAG ": " __VA_ARGS__)
#define LOG_ERROR(LOG_TAG, ...) printf("\n" LOG_TAG ": " __VA_ARGS__)

#endif

#endif //RUBIKDETECTOR_CROSSLOG_HPP
