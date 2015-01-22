
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
#include "linktable.h"
#include "menu.h"

tLinkTable * head = NULL;

#define CMD_MAX_LEN 		1024
#define CMD_MAX_ARGV_NUM 	32
#define DESC_LEN    		1024
#define CMD_NUM     		10

/* data struct and its operations */

typedef struct DataNode
{
    tLinkTableNode * pNext;
    char*   cmd;
    char*   desc;
    int     (*handler)(int argc, char *argv[]);
} tDataNode;



int I_Quit(int argc, char *argv[]){
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
    while(pNode != NULL)
    {
        if(x==0){
            printf("------------------Autobahn Navigation--------------------\n");
        }


        printf("Geben sie \"%s\" ein %s\n", pNode->cmd, pNode->desc);
        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);


        if(x==2){
            //Trennlinie zwischen Programm und Menu Basierten Kommandos
            printf("--------------------------------------------\n");
        }

        x++;

    }
    printf("--------------------------------------------\n");
    return 0;
}

/* show all cmd in listlist */
int ShowUserCmd(tLinkTable * head)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    int x=0;
    printf("------------------Autobahn Navigation--------------------\n");
    while(pNode != NULL)
    {


        if(x>2) {
            printf("Geben sie \"%s\" ein %s\n", pNode->cmd, pNode->desc);
        }
        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);

        x++;

    }
    return 0;
}

/* show all cmd in listlist */
int ShowSystemCmd(tLinkTable * head)
{
    tDataNode * pNode = (tDataNode*)GetLinkTableHead(head);
    int x=0;
    printf("------------------Autobahn Navigation--------------------\n");
    while(pNode != NULL)
    {

        if(x<3) {
            printf("Geben sie \"%s\" ein %s\n", pNode->cmd, pNode->desc);
        }
        pNode = (tDataNode *) GetNextLinkTableNode(head, (tLinkTableNode *) pNode);

        x++;
    }
    printf("--------------------------------------------\n");
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
        /* scanf("%s", cmd); */
		pcmd = fgets(cmd, CMD_MAX_LEN, stdin);
		if(pcmd == NULL)
		{
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
        if( p == NULL)
        {
            printf("Ungültiger Befehl\n");
            continue;
        }
        
        if(p->handler != NULL) 
        { 
            p->handler(argc, argv);
        }
    }
} 
