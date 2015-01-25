
/**************************************************************************************************/
/* Copyright (C) mc2lab.com, SSE@USTC, 2014-2015                                                  */
/*                                                                                                */
/*  FILE NAME             :  menu.c                                                               */
/*  PRINCIPAL AUTHOR      :  Mengning                                                             */
/*  SUBSYSTEM NAME        :  menu                                                                 */
/*  MODULE NAME           :  menu                                                                 */
/*  LANGUAGE              :  C                                                                    */
/*  TARGET ENVIRONMENT    :  ANY                                                                  */
/*  DATE OF FIRST RELEASE :  2014/08/31                                                           */
/*  DESCRIPTION           :  This is a menu program                                               */
/**************************************************************************************************/

/*
 * Revision log:
 *
 * Created by Mengning, 2014/08/31
 *
 */

#ifndef MENU_H
#define MENU_H

#ifdef _WIN32
    #define CLEAR "cls"
#else // In Nicht Windows Systemen ist CLEAR = clear , in windows ist CLEAR = cls
#define CLEAR "clear"
#endif

/* add cmd to menu */
int AddCMD(char *cmd, char *desc, int (*handler)());

/* Menu Engine Execute */
int StartCMDSystem();

/* Clear CMS */
void ResetAllCMDs();

/* Zeigt Verfügbare Commandos an */
void showAllCMDHelp();
void showSystemCMDHelp();
void showUserCMDHelp();
void SetMenuTitle(char *title);

#endif