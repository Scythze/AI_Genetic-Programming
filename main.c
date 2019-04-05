#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAPSIZE 10
#define GENSIZE 8
#define MOVESET 100
#define MUTATIONCOUNT 3
#define NUMBEROFGENERATIONS 75
#define MAXFOOD 3


/**
Name : Furkan Gercek
Date: 28.03.2019
Assignment01 - Organic Algorithm that solves ant-food problem.
https://drive.google.com/file/d/1mWdbVDqoB29lUUAxKFsseJRhotAb02v5/view

Language : C
IDE : Code::Blocks 17.12
Compiler: GNU GCC Compiler



To use this program, first create a map.txt file where pixel characters are '0' and foods are '1'.
Then change the predefined values above to your desired settings, change the name at "readMapFromFile() function.
Example: 0 0 0 0 0
         0 0 1 0 0
         0 0 0 0 0
         1 0 0 0 0
         0 0 0 0 0
**/

//STRUCT DATA FOR CHROMOSOME
typedef struct{
int moveSet[MOVESET];
float successRate;      //The rate of chromosome that's calculated in Fitness Function
int score;              //Number of food eaten by the ant
int timerFactor;        //Used in Fitness Calculation, if map is cleared before maximum movesets, this value increases the fitness.
int isDead;             //To check if moveset results chromsoome to die while creating generations(a.k.a hitting the walls)
int closeCall;          //If the ant is 1-pixel-range close (in 8 directions) to the food, this value increases the Fitness calculation
int rouletteChance;     //Chance of chromosome to be selected crossingover operation for future generations. Range : 0-100.
}GEN;

char map[MAPSIZE][MAPSIZE];     //Matrix that shows the map.

GEN* generateChromosome();


int main()
{
    int j,i,k,allFound,maxIndis;
    float maxRate;
    GEN* chromosome[GENSIZE];           //Array for holding chromosomes


    printf("\n%d x %d <--- MAPSIZE",MAPSIZE,MAPSIZE);
    printf("\n   %d <--- POPULATIONSIZE",GENSIZE);
    printf("\n   %d <--- MOVESET",MOVESET);
    printf("\n    %d <--- MUTATION NUMBER",MUTATIONCOUNT);
    printf("\n   %d <--- NUMBER OF GENERATIONS",NUMBEROFGENERATIONS);
    getch();
    system("cls");
    srand(time(NULL));               //Makes use of the computer's internal clock to control the choice of the seed. For unique randomization


//Printing the initial map, first generations.
    for(i=0; i<GENSIZE; i++){
        chromosome[i] = generateChromosome();
        printf("\n\n [%d]'th chromesome moveset : ",i);
        for(j=0; j<MOVESET; j++){
           printf("%d",chromosome[i]->moveSet[j]);
        }
    }
    getch();
    system("cls");
    printf("\n\n\n      THE MAP \n");
    readMapFromFile(map);
    printMatrix(map);
    printf("RESULTS OF 0TH GENERATION");
    for(i=0; i<GENSIZE; i++){
        chromosomeTests(chromosome[i],map);
        printf("\n [Chromosome%d] Rate: %f Score: %d CloseCall: %d Steps: %d\n",i,chromosome[i]->successRate,chromosome[i]->score,chromosome[i]->closeCall,chromosome[i]->timerFactor);
    }
    j=0;

    //Iterated actions of new generations until NUMBEROFGENERATIONS value is reached or the maze is completed.
    do{
       allFound = 0;

        system("cls");
        creatingNextGeneration(chromosome);
        printf("\n################## GENERATION [%d] #################\n",j);
         for(i=0; i<GENSIZE; i++){
            chromosomeTests(chromosome[i],map);
            printf("\n [Chromosome%d] Rate: %f Score: %d CloseCall: %d Steps: %d",i,chromosome[i]->successRate,chromosome[i]->score,chromosome[i]->closeCall,chromosome[i]->timerFactor);
        }
        maxRate = 0 ;
        maxIndis = 0;
        for(k=0; k<GENSIZE; k++){
            if( chromosome[k]->successRate > maxRate )
                maxIndis = k;
        }
        printf(ANSI_COLOR_GREEN "\n\nMOST SUCCESSFUL CHILD IN GENERATION %d"ANSI_COLOR_RESET,j);
        printf("\n [Chromosome%d] Rate: %f Score: %d CloseCall: %d Steps: %d",maxIndis,chromosome[maxIndis]->successRate,chromosome[maxIndis]->score,chromosome[maxIndis]->closeCall,chromosome[maxIndis]->timerFactor);

        if(chromosome[maxIndis]->score == MAXFOOD)
            allFound = 1;

        printMoves(map,chromosome[maxIndis]);
        // getch(); //Add  this line to disable stopping between steps
        j++;
    }while(j<NUMBEROFGENERATIONS && allFound == 0);


    //After the operations end, informations are shown to screen.
    if(allFound == 1)
        printf(ANSI_COLOR_GREEN "\n\n MAP IS CLEARED!!! AT GENERATION #%d BY CHILD #%d"ANSI_COLOR_RESET ,j,maxIndis);
    else{
        printf(ANSI_COLOR_RED "\n\n MAP COULDN'T BE CLEARED !!! ");
        printf("\n TRY TO PICK ANOTHER MAP OR CHOOSE NEW PARAMETERS"ANSI_COLOR_RESET);
    }

    getch();
    printf(ANSI_COLOR_YELLOW "\n\n Ending the simulation in 3..");
    getch();
    printf("\n Ending the simulation in 3..2..");
    getch();
    printf("\n Ending the simulation in 3..2..1..\n\n\n\n\n" ANSI_COLOR_RESET);
    return 0;
}

//Reads a fresh copy of map from .txt file, this way the matrix never gets corrupted.
void readMapFromFile(char map[MAPSIZE][MAPSIZE]){
    FILE* fi = fopen("map10.txt","r");
    int i,j;

    for(i=0;i<MAPSIZE;i++){
        for(j=0;j<MAPSIZE;j++){
                map[i][j] = fgetc(fi);
                fgetc(fi);
        }
    }
    fclose(fi);
}

//Prints the map to the screen
void printMatrix(char matrix[MAPSIZE][MAPSIZE]){
    int i,j;

    for(i=0;i<MAPSIZE;i++){
        for(j=0;j<MAPSIZE;j++){
            printf(" %c",matrix[i][j]);
        }
    printf("\n");
    }
}

//Prints the map to the screen with the given MOVESET of the chromosome.
//Does  occasional if-checks for color mark-ups.
void printMoves(char matrix[MAPSIZE][MAPSIZE],GEN* chromosome){
    int i,j;
    int curX = (MAPSIZE-1)/2;                //Startpoint : middle of map
    int curY = (MAPSIZE-1)/2;


    readMapFromFile(map);
    map[curX][curY] = 'x' ;


    printf("\n       THE MAP \n");
    for(i=0;i<MAPSIZE;i++){
        printf("\n");
        for(j=0;j<MAPSIZE;j++){
            if(matrix[i][j] == '1')
                printf(ANSI_COLOR_GREEN " %c" ANSI_COLOR_RESET   ,matrix[i][j]);
            else if(matrix[i][j] == 'x')
                printf(ANSI_COLOR_RED " %c" ANSI_COLOR_RESET   ,matrix[i][j]);
            else
                printf(" %c",matrix[i][j]);
        }
    }

      for(i = 0; i<MOVESET ; i++){
        if(chromosome->moveSet[i] == 1 && curY>0){     //Left
            curY--;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = 'x';
            }
            map[curX][curY] = 'x';
        }
        else if(chromosome->moveSet[i] == 2 && curX>0){    //Up
            curX--;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = 'x';
            }
            map[curX][curY] = 'x';
        }
        else if(chromosome->moveSet[i] == 3 && MAPSIZE-1 > curY){    //Right
            curY++;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = 'x';
            }
            map[curX][curY] = 'x';
        }
        else if(chromosome->moveSet[i] == 4 && MAPSIZE-1 > curX){    //Down
            curX++;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = 'x';
            }
            map[curX][curY] = 'x';
        }
      }

    printf("\n\n      MOVESET MAP \n");
    for(i=0;i<MAPSIZE;i++){
        printf("\n");
        for(j=0;j<MAPSIZE;j++){
            if(matrix[i][j] == '1')
                printf(ANSI_COLOR_GREEN " %c" ANSI_COLOR_RESET   ,matrix[i][j]);
            else if(matrix[i][j] == 'x')
                printf(ANSI_COLOR_RED " %c" ANSI_COLOR_RESET   ,matrix[i][j]);
            else
                printf(" %c",matrix[i][j]);
        }
    }
}

// This function is called in the beginning of the program to generate first parents
// That re-generates until it's a random moveset that doesn't die.
GEN* generateChromosome(){
    GEN* chromosome = (GEN*) malloc(sizeof(GEN));
    int i,j,curX,curY;
    chromosome->successRate=0.0;

    do{
        chromosome->isDead = 0;

        for(i = 0; i<MOVESET; i++){
            int r = ( rand() % 4 ) + 1 ;
            chromosome->moveSet[i] = r;
        }

        curX = MAPSIZE/2;                //Startpoint : middle of map
        curY = MAPSIZE/2;

        for(i = 0; i<MOVESET ; i++){
          //  printf(" \n X:%d Y:%d",curX,curY);
            if(chromosome->moveSet[i] == 1)     //Left
                if(curY == 0)
                    chromosome->isDead = 1;
                else{
                  //  printf(" Moving Left ");
                    curY--;
                }
            else if(chromosome->moveSet[i] == 2)    //Up
                if(curX == 0)
                    chromosome->isDead = 1;
                else{
                   // printf(" Moving Up ");
                    curX--;
                }
            else if(chromosome->moveSet[i] == 3)        //Right
                if(curY == MAPSIZE-1)
                    chromosome->isDead = 1;
                else{
                   // printf(" Moving Up ");
                    curY++;
                }
            else if(chromosome->moveSet[i] == 4)        //Down
                if(curX == MAPSIZE-1)
                    chromosome->isDead = 1;
                else{
                  //  printf(" Moving Down");
                    curX++;
                }
        }
    }while (chromosome->isDead == 1);   //If not successfull moveset, do it again.
    return chromosome;
}

// For the given moveset, it does the move-test on the map and calculates the values to be assigned to chromosome.
void chromosomeTests(GEN* chromosome){
     int i,j,curX,curY;
     readMapFromFile(map);

     chromosome->closeCall = 0;
     chromosome->score=0;
     chromosome->timerFactor = MOVESET;

     curX = (MAPSIZE-1)/2;                //Startpoint : middle of map
     curY = (MAPSIZE-1)/2;
     map[curX][curY] = 'x' ;


    //Applying the entire moveset
    for(i = 0; i<MOVESET ; i++){
        // If all the foods are eaten before the moveset ends, timerFactor value is saved for later calculation
        if(chromosome->score == 3){
            chromosome->timerFactor = i;
        }


        if(chromosome->moveSet[i] == 1 && curY>0){     //Left
            map[curX][curY]='0';
            curY--;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = '0';
            }
        }
        else if(chromosome->moveSet[i] == 2 && curX>0){    //Up
            map[curX][curY]='0';
            curX--;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = '0';
            }
        }
        else if(chromosome->moveSet[i] == 3 && MAPSIZE-1 > curY){    //Right
            map[curX][curY]='0';
            curY++;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = '0';
            }
        }
        else if(chromosome->moveSet[i] == 4 && MAPSIZE-1 > curX){    //Down
            map[curX][curY]='0';
            curX++;
            if(map[curX][curY] == '1' ){
                chromosome->score++;
                map[curX][curY] = '0';
            }
        }
        //closeCall helps to award the moveSet that gets 1pixel close to the points
        if( 0 < curX && curX<MAPSIZE-1 && 0 < curY && curY<MAPSIZE-1)
            if( map[curX+1][curY]   == '1'   ||
                map[curX][curY+1]   == '1'   ||
                map[curX+1][curY+1] == '1'   ||
                map[curX][curY-1]   == '1'   ||
                map[curX-1][curY]   == '1'   ||
                map[curX-1][curY-1] == '1'   ||
                map[curX-1][curY+1] == '1'   ||
                map[curX+1][curY-1] == '1'   )
                chromosome->closeCall++;
    }
    //Fitness Function : Gets positive scores by eaten food and the foods that were very close for our moveset and divided by how many steps ant takes to finish the moveset.
    if(MAPSIZE > 6)
        chromosome->successRate = (1000.0*chromosome->score + 20.0*chromosome->closeCall) / chromosome->timerFactor ;
    if(MAPSIZE < 6)
        chromosome->successRate = (1000.0*chromosome->score) / chromosome->timerFactor ;
}

// Here we have applied Roulette Wheel selection where we get scores of every chromosome based on max 100,
// then add the chromosomeID's to the rouletteArray according to their chance and make a random selection from there for 8 times.
// then make crossingover with those chosen 8 chromosomes by coupling them.

//Crossover operation is handled by choosing a random chopPoint and swapping a random amount of indexes between the couple.
// Amount of data to be swapped is calculated by the f(x) = RANDOM(MOVESET/2) - (MOVESET/3)
void creatingNextGeneration(GEN* chromosome[GENSIZE]){
    int tmp,i,r1,j,k,startingPoint=0;
    float total=0;
    int randoms[GENSIZE];

    for(i=0; i<GENSIZE; i++){
        total += chromosome[i]->successRate;
    }

    //SETTING PERCENTAGES OF CHROMOSOMES
    int rouletteArray[100];
    for(i=0; i<GENSIZE; i++){
        chromosome[i]->rouletteChance = chromosome[i]->successRate * 100.0 / total; //Percentage roulette

        for(j=startingPoint; j<chromosome[i]->rouletteChance; j++)
            rouletteArray[j] = i;
        startingPoint = j;
    }

    for(i=startingPoint; i<100;i++){
        rouletteArray[i] = 1+ rand()%4;
    }
    //GETTING RANDOM CHROMOSOMES
    for(k=0; k<GENSIZE; k++){
        randoms[k] = rand() % 100;
        randoms[k] = rouletteArray[randoms[k]];
    }

    //CROSSOVER
    for(k=0; k<=GENSIZE-1; k++){
        int chopPoint = rand()%(MOVESET/2) - (MOVESET/3);
        for(i=chopPoint; i<chopPoint + (MOVESET/6); i++){
            tmp = chromosome[randoms[k]]->moveSet[i];
            chromosome[randoms[k]]->moveSet[i] = chromosome[randoms[k+1]]->moveSet[i];
            chromosome[randoms[k+1]]->moveSet[i] = tmp;
        }
        k++;
    }

    //MUTATION
    for(i=0; i<GENSIZE; i++){
        for(j=0; j<MUTATIONCOUNT; j++){
            r1 = rand()%MOVESET;
            chromosome[i]->moveSet[r1] = 1 + rand()%4 ;
        }
    }
}

