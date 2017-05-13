#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hall {
    char hallName[255];
    char movieName[255];
    int *width;
    int *height;
    int **hallRange;
    struct hall *next;
};

void printHall(struct hall *model,FILE *outputFile);
void buyTicket(char* movieNameTemp,char* location, int ticketType, int ofSeatsRequested,struct hall *startHallPointer,FILE *outputFile);
char * splitString(char *location, int startLocation, int endLocation);
int findLocationColumn(int *letter);
void cancelTicket(char* movieNameTemp, char* location,struct hall *startHallPointer,FILE *outputFile);
char findLocationColumnChar (int numberOfColumn);
void statistics(struct hall *model,FILE *outputFile);
void freeHall(struct hall *startHallPointer);

int main(int argc, char* argv[])
{
    char *inputFileName = argv[1];
    FILE *inputFile,*outputFile;
    char *line[256];
    int i,j;
    char *actionName;
    inputFile = fopen(inputFileName, "r");
    outputFile = fopen("output.txt", "w");
    struct hall *startHall=NULL, *endHall = NULL, *temp;

    if(!inputFile){
        fprintf(outputFile,"File not Found.");
        printf("File not Found.");
        exit(1);
    }

    while(fgets(line,sizeof(line),inputFile)){
        actionName = strtok(line," ");
        if(strcmp(actionName,"CREATEHALL") == 0){
            temp = (struct hall *) malloc(sizeof(struct hall));             /*Hall information setting*/
            strcpy(temp->hallName,  strtok(NULL," \""));
            strcpy(temp->movieName , strtok(NULL," \""));
            temp->width = atoi(strtok(NULL," "));
            temp->height = atoi(strtok(NULL," "));
            temp->next = NULL;

            int row = temp->height , column =temp->width;
            int i, j;
            int **templateArray;

            templateArray = (int **)malloc(sizeof(int *)*row);              /*dynamic 2d array for hall*/

            for(i=0; i < row; i++)
                templateArray[i] = (int *)malloc(sizeof(int)*column);

            for(i=0; i < row; i++)
                for(j=0; j < column; j++)
                    templateArray[i][j] = 0;

            temp->hallRange = templateArray;

            if(startHall == NULL) {             /*Create linked list and add halls*/
                startHall = temp;
                endHall   = temp;
            }
            else {
                endHall->next = temp;
                endHall = temp;
            }
        }
        else if(strcmp(actionName,"BUYTICKET") == 0){
            char* movieName = strtok(NULL," \"");                   /*Set Buy ticket function*/
            char* location = strtok(NULL," ");
            int ticketType = strcmp(strtok(NULL," "), "Student") == 0 ? 1: 2;       /* 0 is free - 1 is student - 2 is full fare*/
            int ofSeatsRequested = atoi(strtok(NULL," "));
            buyTicket(movieName, location, ticketType, ofSeatsRequested,startHall,outputFile);  /*go buy ticket function*/
        }
        else if(strcmp(actionName,"CANCELTICKET") == 0){
            char* movieName = strtok(NULL," \"");                   /*Set Cancel ticket function*/
            char* location = strtok(NULL," \n");

            if(location == NULL){
                fprintf(outputFile,"ERROR: Movie name cannot be empty\n");      /*control movie name created.*/
                printf("ERROR: Movie name cannot be empty\n");
                continue;
            }
            else{
                cancelTicket(movieName,location,startHall,outputFile);      /*go cancel ticket function*/
            }
        }
        else if(strcmp(actionName,"SHOWHALL") == 0){
            char* hallNameTemp = strtok(NULL," \"");            /*set show hall function*/

            struct hall *searchC=startHall;
            while(searchC->next !=NULL){                    /*search and control created hall name*/
                if(strcmp(searchC->hallName, hallNameTemp) == 0){
                    break;
                }
                searchC=searchC->next;
            }
            if(searchC == NULL){
                fprintf(outputFile,"ERROR: Hall cannot created\n");    /* Hall not created*/
                printf("ERROR: Hall cannot created \n");
                return;
            }
            printHall(searchC,outputFile);              /*go print hall function */
        }
        else if(strcmp(actionName,"STATISTICS") == 0){
            struct hall *searchC=startHall;
            fprintf(outputFile,"Statistics\n");
            printf("Statistics\n");
            while(searchC !=NULL){
                statistics(searchC,outputFile);     /*All hall return for statistic information and go function*/
                searchC=searchC->next;
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    freeHall(startHall);
    return 0;
}

void freeHall(struct hall *startHallPointer){
    struct hall *freeHallNode;

    while(startHallPointer != NULL){
        freeHallNode = startHallPointer;
        startHallPointer = startHallPointer->next;
        free(freeHallNode);
    }
}

void buyTicket(char* movieNameTemp,char* location, int ticketType, int ofSeatsRequested,struct hall *startHallPointer,FILE *outputFile)
{
    struct hall *searchC=startHallPointer;
    while(searchC->next !=NULL){                                        /*Searching Hall with movie name*/
        if(strcmp(searchC->movieName, movieNameTemp) == 0){
            break;
        }
        searchC=searchC->next;
    }
    if(searchC == NULL){
        fprintf(outputFile,"ERROR: Movie name cannot be empty");        /*Anything hall has not that movie name*/
        printf("ERROR: Movie name cannot be empty");
        return;
    }
    char* temp[2];
    char *templateArray;
    temp[0] = splitString(location, 0, 0);                              /*Letter*/
    templateArray = splitString(location, 1, strlen(location)-1);             /*Number*/

    int arraySize = strlen(templateArray);

    int locationRow = 0;
    int multication = 1;

    arraySize = (int)arraySize < 0 ? -arraySize : arraySize;        /*Finding location Row*/
    while (arraySize--)
    {
        if ((templateArray[arraySize] < '0' || templateArray[arraySize] > '9') && templateArray[arraySize] != '-') {
            if (locationRow){           /*If location row 0-9 break while*/
                break;
            }
            else{                       /*If location row not 0-9 continue*/
                 continue;
            }
        }
        if (templateArray[arraySize] == '-') {
            if (locationRow) {          /*If Location Row Array = "-" location row multi mines after break*/
                locationRow = -locationRow;
                break;
            }
        }
        else {
            locationRow += (templateArray[arraySize] - '0') * multication;  /*location row > 10 multi multication 1**/
            multication *= 10;
        }
    }

    int locationColumn = (int)findLocationColumn(temp[0]);

    int i,fullSeat = 0;
    if(strcmp(movieNameTemp,searchC->movieName)!=0){        /*Control movie name*/
        fprintf(outputFile,"ERROR: Movie name %s is incorrect\n",movieNameTemp);
        printf("ERROR: Movie name %s is incorrect\n",movieNameTemp);
    }
    else{
        if(locationRow>searchC->height || locationColumn>searchC->width){      /*Control location > Hall's row or column*/
            fprintf(outputFile,"ERROR: Seat %s is not defined at %s\n",location,searchC->hallName);
            printf("ERROR: Seat %s is not defined at %s\n",location,searchC->hallName);
        }
        else{
            for(i = 0;i<ofSeatsRequested; i++)                  /*Control wanting Hall's seat full or free*/
            {
                if(searchC->hallRange[locationRow-1][locationColumn + i-1]!=0){
                    fullSeat = 1;
                }
            }
            if(fullSeat == 0){                              /*If Hall's seat is free, take ofSeatNumber seats*/
                for(i = 0;i<ofSeatsRequested; i++)
                {
                    if(searchC->hallRange[locationRow-1][locationColumn + i-1]==0){
                        searchC->hallRange[locationRow-1][locationColumn + i-1] = ticketType;
                    }
                }
                fprintf(outputFile,"%s [%s] Seat(s) ",searchC->hallName,searchC->movieName);
                printf("%s [%s] Seat(s) ",searchC->hallName,searchC->movieName);
                for(i = 0;i<ofSeatsRequested; i++){
                    fprintf(outputFile,"%c%d",findLocationColumnChar(locationColumn+i),locationRow);
                    printf("%c%d",findLocationColumnChar(locationColumn+i),locationRow);
                    if((i+1)!= ofSeatsRequested){
                        fprintf(outputFile,",");
                        printf(",");
                    }
                    else{
                        fprintf(outputFile," ");
                        printf(" ");
                    }
                }
                fprintf(outputFile,"successfully sold.\n");
                printf("successfully sold.\n");
            }
            else{
                                    /*If seat has been already taken,that the write ERROR*/
                fprintf(outputFile,"ERROR: Specified seat(s) in %s are not available! They have been already taken.\n",searchC->hallName);
                printf("ERROR: Specified1 seat(s) in %s are not available! They have been already taken.\n",searchC->hallName);
            }
        }
    }
}

int findLocationColumn(int *letter)         /*A Letter = A Number searching*/
{
    if(strcmp(letter,"A")==0){
        return 1;
    }
    else if(strcmp(letter,"B")==0){
        return 2;
    }
    else if(strcmp(letter,"C")==0){
        return 3;
    }
    else if(strcmp(letter,"D")==0){
        return 4;
    }
    else if(strcmp(letter,"E")==0){
        return 5;
    }
    else if(strcmp(letter,"F")==0){
        return 6;
    }
    else if(strcmp(letter,"G")==0){
        return 7;
    }
    else if(strcmp(letter,"H")==0){
        return 8;
    }
    else if(strcmp(letter,"I")==0){
        return 9;
    }
    else if(strcmp(letter,"J")==0){
        return 10;
    }
    else if(strcmp(letter,"K")==0){
        return 11;
    }
    else if(strcmp(letter,"L")==0){
        return 12;
    }
    else if(strcmp(letter,"M")==0){
        return 13;
    }
    else if(strcmp(letter,"N")==0){
        return 14;
    }
    else if(strcmp(letter,"O")==0){
        return 15;
    }
    else if(strcmp(letter,"P")==0){
        return 16;
    }
    else if(strcmp(letter,"Q")==0){
        return 17;
    }
    else if(strcmp(letter,"R")==0){
        return 18;
    }
    else if(strcmp(letter,"S")==0){
        return 19;
    }
    else if(strcmp(letter,"T")==0){
        return 20;
    }
    else if(strcmp(letter,"U")==0){
        return 21;
    }
    else if(strcmp(letter,"V")==0){
        return 22;
    }
    else if(strcmp(letter,"W")==0){
        return 23;
    }
    else if(strcmp(letter,"X")==0){
        return 24;
    }
    else if(strcmp(letter,"Y")==0){
        return 25;
    }
    else if(strcmp(letter,"Z")==0){
        return 26;
    }
}

char findLocationColumnChar (int numberOfColumn)        /*A Number = A Letter searching*/
{
    if(numberOfColumn == 1){
        return 'A';
    }
    else if(numberOfColumn == 2){
        return 'B';
    }
    else if(numberOfColumn == 3){
        return 'C';
    }
    else if(numberOfColumn == 4){
        return 'D';
    }
    else if(numberOfColumn == 5){
        return 'E';
    }
    else if(numberOfColumn == 6){
        return 'F';
    }
    else if(numberOfColumn == 7){
        return 'G';
    }
    else if(numberOfColumn == 8){
        return 'H';
    }
    else if(numberOfColumn == 9){
        return 'I';
    }
    else if(numberOfColumn == 10){
        return 'J';
    }
    else if(numberOfColumn == 11){
        return 'K';
    }
    else if(numberOfColumn == 12){
        return 'L';
    }
    else if(numberOfColumn == 13){
        return 'M';
    }
    else if(numberOfColumn == 14){
        return 'N';
    }
    else if(numberOfColumn == 15){
        return 'O';
    }
    else if(numberOfColumn == 16){
        return 'P';
    }
    else if(numberOfColumn == 17){
        return 'Q';
    }
    else if(numberOfColumn == 18){
        return 'R';
    }
    else if(numberOfColumn == 19){
        return 'S';
    }
    else if(numberOfColumn == 20){
        return 'T';
    }
    else if(numberOfColumn == 21){
        return 'U';
    }
    else if(numberOfColumn == 22){
        return 'V';
    }
    else if(numberOfColumn == 23){
        return 'W';
    }
    else if(numberOfColumn == 24){
        return 'X';
    }
    else if(numberOfColumn == 25){
        return 'Y';
    }
    else if(numberOfColumn == 26){
        return 'Z';
    }
}

void cancelTicket(char* movieNameTemp, char* location,struct hall *startHallPointer,FILE *outputFile)
{
    struct hall *searchC=startHallPointer;
    while(searchC->next !=NULL){
        if(strcmp(searchC->movieName, movieNameTemp) == 0){             /*search all hall with movie name*/
            break;
        }
        searchC=searchC->next;
    }
    if(searchC == NULL){
        fprintf(outputFile,"ERROR: Movie name cannot be empty");
        printf("ERROR: Movie name cannot be empty");
        return;
    }
    char* temp[2];
    char *templateArray;
    temp[0] = splitString(location, 0, 0);                              /*Letter*/
    templateArray = splitString(location, 1, strlen(location)-1);             /*Number*/

    int arraySize = strlen(templateArray);              /*Finding location Row*/

    int locationRow = 0;
    int multication = 1;

    arraySize = (int)arraySize < 0 ? -arraySize : arraySize;
    while (arraySize--)
    {
        if ((templateArray[arraySize] < '0' || templateArray[arraySize] > '9') && templateArray[arraySize] != '-') {
            if (locationRow)        /*If location row 0-9 break while*/
                break;
            else
                continue;           /*If location row not 0-9 continue*/
        }
        if (templateArray[arraySize] == '-') {
            if (locationRow) {          /*If Location Row Array = "-" location row multi mines after break*/
                locationRow = -locationRow;
                break;
            }
        }
        else {
            locationRow += (templateArray[arraySize] - '0') * multication;      /*location row > 10 multi multication 10*/
            multication *= 10;
        }
    }

    int locationColumn = (int)findLocationColumn(temp[0]);

    int i,fullSeat = 0;
    if(strcmp(movieNameTemp,searchC->movieName)!=0){        /*Control hall's movie name*/
        fprintf(outputFile,"ERROR: Movie name %s is incorrect\n",movieNameTemp);
        printf("ERROR: Movie name %s is incorrect\n",movieNameTemp);
    }
    else{
        if(locationRow>searchC->height || locationColumn>searchC->width){       /*Control location > Hall's row or column*/
            fprintf(outputFile,"ERROR: Seat %s is not defined at %s\n",location,searchC->hallName);
            printf("ERROR: Seat %s is not defined at %s\n",location,searchC->hallName);
        }
        else{
            if(searchC->hallRange[locationRow-1][locationColumn -1] != 0){      /*Ticket canceled.*/
                searchC->hallRange[locationRow-1][locationColumn -1] = 0;
                fprintf(outputFile,"%s [%s] Purchase canceled. Seat %s is now free\n",searchC->hallName,searchC->movieName,location);
                printf("%s [%s] Purchase canceled. Seat %s is now free\n",searchC->hallName,searchC->movieName,location);
            }
            else{           /*Ticket does not canceled.*/
                fprintf(outputFile,"ERROR: Seat %s in Red-Hall was not sold.\n",location);
                printf("ERROR: Seat %s in Red-Hall was not sold.\n",location);
            }
        }
    }
}

char * splitString(char *location, int startLocation, int endLocation)      /*Parsing Location Row and Column*/
{
    int sizeOfString = endLocation - startLocation + 2;
    char * dynamicCell = (char*)malloc(sizeOfString);
    strncpy(dynamicCell,location+startLocation, sizeOfString-1);
    dynamicCell[sizeOfString-1]=0;
    return dynamicCell;
}

void printHall(struct hall *model,FILE *outputFile){        /*Printing Hall*/
    fprintf(outputFile,"%s sitting plan\n",model->hallName);
    printf("%s sitting plan\n",model->hallName);
    int i , j, k;
    int row,column;

    row = model->height;
    column = model->width;
    for(i=0; i < row; i++) {
        fprintf(outputFile,"  ");
        printf("  ");
        for(k = 0; k < column; k++){
            fprintf(outputFile,"--");
            printf("--");
        }
        fprintf(outputFile,"-\n");
        printf("-\n");
        if(i<9){
            fprintf(outputFile,"%d ", (i+1));
            printf("%d ", (i+1));
        }
        else{
            fprintf(outputFile,"%d", (i+1));
            printf("%d", (i+1));
        }
        for(j=0; j <column; j++)
        {
            char *ticketType;
            if(model->hallRange[i][j] == 0){
                ticketType = " ";
            }
            else if(model->hallRange[i][j] == 1){
                ticketType = "s";
            }
            else if(model->hallRange[i][j] == 2){
                ticketType = "f";
            }
            fprintf(outputFile,"|%s", ticketType);
            printf("|%s", ticketType);
        }
        fprintf(outputFile,"|\n");
        printf("|\n");
    }
    fprintf(outputFile,"  ");
    printf("  ");
    for(k = 0; k < column; k++){
        fprintf(outputFile,"--");
        printf("--");
    }
    fprintf(outputFile,"-\n ");
    printf("-\n ");
    for(k = 0; k <= column; k++){
        fprintf(outputFile,"%c ", findLocationColumnChar(k));
        printf("%c ", findLocationColumnChar(k));
    }

    fprintf(outputFile,"\n");
    printf("\n");
    char *curtain = "C U R T A I N";
    int curtainSize = (column*2- strlen(curtain))/2;
    for(i=0;i<curtainSize;i++){
        fprintf(outputFile," ");
        printf(" ");
    }
    fprintf(outputFile,"%s\n",curtain );
    printf("%s\n",curtain );

}

void statistics(struct hall *model,FILE *outputFile){       /*Printing Statistic of Halls*/
    int i,j,row,column;
    int studentCounter = 0,fullFareCounter = 0,sum = 0;

    row = model->height;
    column = model->width;

    for(i=0; i < row; i++){
        for(j=0; j <column; j++){
            if(model->hallRange[i][j] == 1){
                studentCounter++;
            }
            else if(model->hallRange[i][j] == 2){
                fullFareCounter++;
            }
        }
    }

    sum = ((studentCounter*7)+(fullFareCounter*10));
    fprintf(outputFile,"%s %d student(s), %d full fare(s), sum:%d TL\n",
           model->movieName,studentCounter,fullFareCounter,sum);
    printf("%s %d student(s), %d full fare(s), sum:%d TL\n",
           model->movieName,studentCounter,fullFareCounter,sum);
}
