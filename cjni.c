//
// Created by nillerusr on 30.01.20.
//

#include "cjni.h"
#include <jni.h>
#include "images.h"
#include "kmeans.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "db.h"

JNIEXPORT void JNICALL Java_com_nillerusr_colordial_MainActivity_setNativeDataDir(JNIEnv *env, jclass *class, jstring path)
{
    const char *newpath = (*env)->GetStringUTFChars(env, path,0);
    strcpy(datapath, newpath);
}

JNIEXPORT jintArray JNICALL Java_com_nillerusr_colordial_AnalyzeColors_getColorFromDatabase(JNIEnv *env, jclass *class, jstring word)
{
    const char *c_word = (*env)->GetStringUTFChars(env, word, 0);

    openColorsDatabase();
    point_t p = getColorFromDatabase(c_word);

    jintArray result;
    result = (*env)->NewIntArray(env, 3);

    if (result == NULL || p.color[0] == -2 )
        return NULL;

    int i;

    jint fill[3];
    for (i = 0; i < 3; i++)
        fill[i] = (int)p.color[i];

    (*env)->SetIntArrayRegion(env, result, 0, 3, fill);
    return result;
}

JNIEXPORT void JNICALL Java_com_nillerusr_colordial_AnalyzeColors_addColorToDatabase(JNIEnv *env, jclass *class, jstring word, jint r, jint g, jint b)
{
    const char *c_word = (*env)->GetStringUTFChars(env, word, 0);
    addColorToDatabase(c_word, r, g, b);
}

JNIEXPORT jintArray JNICALL Java_com_nillerusr_colordial_AnalyzeColors_getImageColor(JNIEnv *env, jclass *class, jstring fname, jbyte imageformat, jint k)
{
    int *color;
    const char *filename = (*env)->GetStringUTFChars(env, fname, 0);
    char fpath[512];
    snprintf(fpath, sizeof fpath, "%s/%s", datapath, filename);

    image_t *img = decodeImage(fpath, imageformat); // Decode images here!!!
    if( img == NULL )
        return NULL;

    color = kmeans(img, k);
    if( color == NULL )
        return NULL;

    jintArray result;
    result = (*env)->NewIntArray(env, 3);
    if (result == NULL)
    {
        return NULL; /* out of memory error thrown */
    }
    int i;

    jint fill[3];
    for (i = 0; i < 3; i++) {
        fill[i] = color[i];
    }

    free(color);

    (*env)->SetIntArrayRegion(env, result, 0, 3, fill);
    return result;
}
