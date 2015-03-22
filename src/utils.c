#include <ctype.h>
#include "utils.h"
#include "processing.h"
#include <stdio.h>
#include "../lib/printui/printui.h"
#include "dbio.h"
#include <stdlib.h>

#ifdef WIN32

#include <windows.h>

double get_time()// Zeit messen
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}


const char *skipParam=" \\CMDknowsUTF8";

void RestartInUtf8Cmd(){//Regestry zum Consolen neustart anlegen (damit UTF8 geht)

    char buffer[MAX_PATH];//always use MAX_PATH for filepaths
    GetModuleFileName(NULL,buffer,sizeof(buffer));

    char *cmd= malloc(sizeof(char)*200);
    sprintf(cmd, "%s%s%s", "start \"Autobahn\" cmd.exe /C ",buffer, skipParam);

    //Einstellungen der autobahn konsole
    system("setlocal enableextensions disabledelayedexpansion");
    system("reg add \"HKCU\\Console\\Autobahn\" /f");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FaceName\"         /t \"REG_SZ\"     /d \"Consolas\"");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontFamily\"       /t \"REG_DWORD\"  /d 0x00000036");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontSize\"         /t \"REG_DWORD\"  /d 0x000F0000");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"FontWeight\"       /t \"REG_DWORD\"  /d 0x00000190");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"QuickEdit\"        /t \"REG_DWORD\"  /d 0x00000000");
    system("reg add \"HKCU\\Console\\Autobahn\" /f /v \"WindowSize\"       /t \"REG_DWORD\"  /d 0x0020078");
    system(cmd);

}

void ConfigureCMD(){//Grose und Codepage festlegen


    int width;
    width = getTerminalWidth()+1;
    char buffer[50];
    sprintf(buffer,"mode con:cols=%d lines=50",width);
    system(buffer);

    HANDLE StdHandle;
    COORD mycoord;
    mycoord.X=width;
    mycoord.Y=20000;
    StdHandle=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleScreenBufferSize(StdHandle,mycoord);


    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

}

#else

#include <sys/time.h>
#include <string.h>


void ConfigureCMD(){


    int width;
    width = getTerminalWidth();
    char buffer[50];
    sprintf(buffer,"resize -s 30 %d",width);
    system(buffer);

}

double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

#endif


int QsortCompareName(const void *s1, const void *s2) {// Qsort Funktion zum Namen sortieren der Ausfahrten und Kreuze
    struct  OutputBuffer* K1 = *(struct OutputBuffer **) s1;
    struct OutputBuffer* K2 = *(struct OutputBuffer **) s2;

    return (strcmp(K1->TextName, K2->TextName));
}


int CountUTF8String(char *s) {//Zähler kann UTF-8 umlauten u.ä ,da strlen manchmal salsch zählt.
    int i = 0, j = 0;
    while (s[i]) {
        if ((s[i] & 0xc0) != 0x80) j++;
        i++;
    }
    return j;
}

int strcompCaseInsensitive(char const *a, char const *b){// Sind 2 Strings gleich (nicht auf GroßKlein achten)
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

double my_round(double x, unsigned int digits) {//Eigene Round fuunktion (math.h fuerhte zu absturz)
    if (digits > 0) {
        return my_round(x*10.0, digits-1)/10.0;
    }
    else {
        return x;
    }
}

void chop(char *str) {//Löscht letztes zeichen eines "Strings" , wird meist dazu benutzt um bei befehlen wie "edit ..." beim letzten parameter den zeilenumbruch zu löschen.
    size_t p=strlen(str);
    str[p-1] = '\0';
}

int isValidKnotenName(char *Name){// Ist der Knotenname Erlaubt(von den zeichen her)

    char* buffer= malloc(sizeof(char)*1000);

    if(isdigit(Name[0])){
        puts(" ");
        sprintf(buffer,"  Fehler , Der Ausfahrts Name muss mit einem Buchstaben beginnen");
        printMenuHeader(buffer);

        free(buffer);
        return 1;
    }

    for(int i=0;i< CountUTF8String(Name);i++){

        if(isalnum((int)Name[i])==0 && Name[i]!='_' && Name[i]!='.'){

            sprintf(buffer,"  Fehler , Der Ausfahrts Name enthält '%c' , ein Verbotenes Zeichen",Name[i]);
            printMenuHeader(buffer);
            printMenuItem("Erlaubte zeichen [A-Z] [a-z] [0-9] '_' '.' ");
            printFooter();

            free(buffer);
            return 1;
        }

    }

    free(buffer);
    return 0;
}

int isValidAutobahnName(char *Name){// Ist der Autobahnname Erlaubt(von den zeichen her)

    char* buffer= malloc(sizeof(char)*1000);

    if(isdigit(Name[0])){
        puts(" ");
        sprintf(buffer,"  Fehler , Der Autobahn Name muss mit einem Buchstaben beginnen");
        printMenuHeader(buffer);

        free(buffer);
        return 1;
    }

    for(int i=0;i< CountUTF8String(Name);i++){

        if(isalnum((int)Name[i])==0 && Name[i]!='_' && Name[i]!='.'){

            sprintf(buffer,"  Fehler , Der Autobahn Name enthält '%c' , ein Verbotenes Zeichen",Name[i]);
            printMenuHeader(buffer);
            printMenuItem("Erlaubte zeichen [A-Z] [a-z] [0-9] '_' '.' ");
            printFooter();

            free(buffer);
            return 1;
        }

    }

    free(buffer);
    return 0;
}

int isValidAutobahnKM(char *KM){// Ist AutobahnKM eine Gueltige zahl(z.b. 9stellen 2nachkommastellen)

    char* buffer= malloc(sizeof(char)*1000);
    double f = atof(KM);
    sprintf(buffer, "%.2f", f);

    while (buffer[strlen(buffer) - 1] == '0' || buffer[strlen(buffer) - 1] == '.') {//Loescht zuviele 0-en
        if (buffer[strlen(buffer) - 1] == '.') {
            chop(buffer);
            break;
        } else {
            chop(buffer);
        }
    }

    if (strcmp(buffer, KM) == 0) {//Sind Buchstaben drin oder weicht die zahl ab von Atof

        if (f >= 1000000000 || f <= -1000000000) {//Maximal und minimal Groese
            puts("");
            printMenuHeader("Fehler , AutobahnKM dürfen maximal 9 stellig mit 2 Nachkommastellen sein");
            printFooter();
            free(buffer);
            return 1;
        }

    } else {
        puts("");
        printMenuHeader("Fehler , AutobahnKM ist keine Gültige Zahl [Kommazeichen ist ein Punkt z.b 12.32]");
        printFooter();
        free(buffer);
        return 1;
    }

    free(buffer);
    return 0;
}
