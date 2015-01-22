/*
 * File:   ui.h
 * Author: flo
 *
 * Created on 22. Januar 2015, 9:01
 */

#ifndef UI_H
#define UI_H

#ifdef _WIN32
    #define CLEAR "cls"
#else // In Nicht Windows Systemen ist CLEAR = clear , in windows ist CLEAR = cls
    #define CLEAR "clear"
#endif

void ui();

#endif

