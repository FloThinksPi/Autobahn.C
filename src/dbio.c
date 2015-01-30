
#include <stdio.h>
#include <stdlib.h>

void loadDatabase(){



    FILE *fp = fopen("data/abfahrt.csv", "r");

    //Datei zeilenweise lesen
    char *line= malloc(sizeof(char)*100);
    int i=0;
    while (1) {
        if (fgets(line,100, fp) == NULL) break;
        i++;
        printf("%3d: %s", i, line);
    }

}