
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <fcntl.h>
#include "linktable.h"
#include "menu.h"
#include "../printui/printui.h"
#include "../levenshtein/levenshtein.h"

tLinkTable * head = NULL;

#define CMD_MAX_LEN 		1024
#define CMD_MAX_ARGV_NUM 	32
#define DESC_LEN    		1024
#define CMD_NUM     		10


char *MenuTitle="Unnamed Menu";
char *PrintfFormat="Geben sie \"%s\" ein %s";

/* data struct and its operations */

typedef struct DataNode
{
    tLinkTableNode * pNext;
    char*   cmd;
    char*   desc;
    int     (*handler)(int argc, char *argv[]);
} tDataNode;



int SearchConditon(tLinkTableNode * pLinkTableNode,void * arg)
{
    char * cmd = (char*)arg;
    tDataNode * pNode = (tDataNode *)pLinkTableNode;
    if(strcmp(pNode->cmd, cmd) == 0)
    {
        return  SUCCESS;
    }
    return FAILURE;
}
/* find a cmd in the linklist and return the datanode pointer */
tDataNode* FindCmd(tLinkTable * head, char * cmd)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    while(pNode != NULL)
    {
        if(!strcmp(pNode->cmd, cmd))
        {
            return  pNode;
        }
        pNode = (tDataNode*)GetNextLinkTableNode(head,(tLinkTableNode *)pNode);
    }
    return NULL;
}

/* show all cmd in listlist */
int ShowAllCmd(tLinkTable * head)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    int x=0;
    printMenuHeader(MenuTitle);
    printMenuItem("");
    while(pNode != NULL)
    {


        char *ausgabe = malloc(strlen(pNode->cmd)+strlen(pNode->desc)+strlen(PrintfFormat)+1);
        sprintf(ausgabe,PrintfFormat, pNode->cmd, pNode->desc);
        printMenuItem(ausgabe);

        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);


        if(x==2){
            //Trennlinie zwischen Programm und Menu Basierten Kommandos
            printMenuItem("");
            printFooter();
            printMenuItem("");
        }

        x++;

    }
    printMenuItem("");
    printFooter();
    return 0;
}

/* show all cmd in listlist */
int ShowUserCmd(tLinkTable * head)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    int x=0;
    printMenuHeader(MenuTitle);
    printMenuItem("");
    while(pNode != NULL)
    {


        if(x>2) {
            char *ausgabe = malloc(strlen(pNode->cmd)+strlen(pNode->desc)+strlen(PrintfFormat)+1);
            sprintf(ausgabe,PrintfFormat, pNode->cmd, pNode->desc);
            printMenuItem(ausgabe);
        }
        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);

        x++;

    }
    printMenuItem("");
    printFooter();
    return 0;
}

/* show all cmd in listlist */
int ShowSystemCmd(tLinkTable * head)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    int x=0;
    printMenuHeader(MenuTitle);
    printMenuItem("");
    while(pNode != NULL)
    {

        if(x<3) {
            char *ausgabe = malloc(strlen(pNode->cmd)+strlen(pNode->desc)+strlen(PrintfFormat)+1);
            sprintf(ausgabe,PrintfFormat, pNode->cmd, pNode->desc);
            printMenuItem(ausgabe);
        }
        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);

        x++;
    }
    printMenuItem("");
    printFooter();
    return 0;
}

void showAllCMDHelp()
{
    ShowAllCmd(head);
}
void showUserCMDHelp()
{
    ShowUserCmd(head);
}
void showSystemCMDHelp()
{
    ShowSystemCmd(head);
}

void SetMenuTitle(char *title){
    MenuTitle=title;
}

int I_Quit(int argc, char *argv[]){
    //TODO Speichern der änderungen on Exit (Struct to file)
    system(CLEAR);
    exit(0);
}
int I_Clear(int argc, char *argv[]){
    system(CLEAR);
}
int I_Help(int argc, char *argv[]){
    system(CLEAR);
    ShowAllCmd(head);
}


/* add cmd to menu */
int AddCMD(char *cmd, char *desc, int (*handler)())
{
    tDataNode* pNode = NULL;
    if ( head == NULL)
    {
        head = CreateLinkTable();
        pNode = (tDataNode*)malloc(sizeof(tDataNode));
        pNode->cmd = "help";
        pNode->desc = "um alle Befehle anzuzeigen";
        pNode->handler = I_Help;
        AddLinkTableNode(head,(tLinkTableNode *)pNode);

        pNode = (tDataNode*)malloc(sizeof(tDataNode));
        pNode->cmd = "exit";
        pNode->desc = "das Programm zu beenden";
        pNode->handler = I_Quit;
        AddLinkTableNode(head,(tLinkTableNode *)pNode);

        pNode = (tDataNode*)malloc(sizeof(tDataNode));
        pNode->cmd = "clear";
        pNode->desc = "das das Terminal zu leeren";
        pNode->handler = I_Clear;
        AddLinkTableNode(head,(tLinkTableNode *)pNode);
    }
    pNode = (tDataNode*)malloc(sizeof(tDataNode));
    pNode->cmd = cmd;
    pNode->desc = desc;
    pNode->handler = handler;
    AddLinkTableNode(head,(tLinkTableNode *)pNode);
    return 0;
}

void ResetAllCMDs(){

    head=NULL;
    tDataNode* pNode = NULL;
    MenuTitle="Unnamed Menu";

}

/* show all cmd in listlist */
int FindSimilarCommands(tLinkTable * head,char *CMD)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    int x=0;
    int p=0;
    while(pNode != NULL)
    {


        if(levenshtein(CMD, pNode->cmd)<=2 && strlen(pNode->cmd)>2){
            if(p==0){
                printf("\n");
                printMenuHeader("Ungültiger Befehl , Meinten sie vieleicht");
                p=1;
            }
            printMenuItem(pNode->cmd);
        }

        if(levenshtein(CMD, pNode->cmd)<2 && strlen(pNode->cmd)<=2){
            if(p==0){
                printf("\n");
                printMenuHeader("Ungültiger Befehl , Meinten sie vieleicht");
                p=1;
            }
            printMenuItem(pNode->cmd);
        }

        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);

        x++;
    }
    if(p==0 && strcmp(CMD,"")){
        puts("");
        printMenuHeaderContinous("Unbekannter Befehl!");
        puts("");

    }else if(strcmp(CMD,"")!=0){
        printFooter();
    }
    return 0;
}


/* Menu Engine Execute */
int StartCMDSystem()
{
   /* cmd line begins */
    while(1)
    {
		int argc = 0;
		char *argv[CMD_MAX_ARGV_NUM];
        char cmd[CMD_MAX_LEN];
		char *pcmd = NULL;
        printf("\nBefehl Eingeben> ");
		pcmd = fgets(cmd, CMD_MAX_LEN,stdin);
		if(pcmd == NULL)
		{
            puts("\n");
            printMenuHeaderContinous("   Keine Umlaute und [§ , ß]   ");
			continue;
		}
        /* convert cmd to argc/argv */
		pcmd = strtok(pcmd," ");
		while(pcmd != NULL && argc < CMD_MAX_ARGV_NUM)
		{
			argv[argc] = pcmd;
			argc++;
			pcmd = strtok(NULL," ");
		}
        if(argc == 1)
        {
            int len = strlen(argv[0]);
            *(argv[0] + len - 1) = '\0';
        }
        tDataNode *p = (tDataNode*)SearchLinkTableNode(head,SearchConditon,(void*)argv[0]);
        if(p == NULL){
            FindSimilarCommands(head,cmd);
            //printf("%s\n",cmd);
            continue;
        }

        if(p->handler != NULL)
        {
            p->handler(argc, argv);
        }

    }
}
