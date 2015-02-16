/*
 * File:   main.h
 * Author: flo
 *
 * Created on 5. Februar 2015, 12:11
 */

#ifndef UTILS_H
#define UTILS_H

//ConsoleAdapt
#ifdef WIN32
void RestartInUtf8Cmd();
#endif
void ConfigureCMD();
//

double get_time();
int CountUTF8String(char *s);
int QsortCompareNameKnoten(const void *s1, const void *s2);
int QsortCompareKMKnoten(const void *s1, const void *s2);
int QsortCompareName(const void *s1, const void *s2);
int strcompCaseInsensitive(char const *a, char const *b);
double my_round(double x, unsigned int digits);

#endif