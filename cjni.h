//
// Created by nillerusr on 30.01.20.
//

#ifndef COLOR_ANALYZER_CJNI_H
#define COLOR_ANALYZER_CJNI_H

#include <stdio.h>
#include <android/log.h>
#define LogPrintf(...) do { __android_log_print(ANDROID_LOG_VERBOSE, "COLORZ_NATIVE", __VA_ARGS__); printf( __VA_ARGS__); } while( 0 );
#define ELogPrintf(...) do { __android_log_print(ANDROID_LOG_ERROR, "COLORZ_NATIVE", __VA_ARGS__); printf( __VA_ARGS__); } while( 0 );

char datapath[256];

#endif //COLOR_ANALYZER_CJNI_H
