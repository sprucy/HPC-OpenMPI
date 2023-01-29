#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "beehive.h"
#include "glider.h"
#include "grower.h"


#define HT (3000) 
#define WD (3000)

long population = 0;
char *map;//Current map data backup
char *bak;//Current map data backup

int createGrowerLifeMap(){
    if ((1500 + GROWER_HEIGHT) >= HT || (1500 + GROWER_WIDTH) >= WD ){
        printf("\nBoard Height or Width Error!");
        return 0;
    }
    #pragma omp parallel for
    for (int iy = 0; iy < GROWER_HEIGHT; iy++ ){
        #pragma omp parallel for
        for (int ix = 0; ix < GROWER_WIDTH; ix++){
            map[(1500 + iy) * WD + 1500 + ix] = grower[iy][ix];
        }
    }
    return 1;
}
//
int createGliderLifeMap(){
    if ((1500 + GLIDER_HEIGHT) >= HT || (1500 + GLIDER_WIDTH) >= WD ){
        printf("\nBoard Height or Width Error!");
        return 0;
    }
    //glider assignment
    #pragma omp parallel for
    for (int iy = 0; iy < GLIDER_HEIGHT; iy++ ){
        #pragma omp parallel for
        for (int ix = 0; ix < GLIDER_WIDTH; ix++){
            map[(1500 + iy) * WD + 1500 + ix] = glider[iy][ix];
        }  
    }
    return 1;
}
// 
int createBeehiveLifeMap(){
    if ((1500 + BEEHIVE_HEIGHT) >= HT || (1500 + BEEHIVE_WIDTH) >= WD ){
        printf("\nBoard Height or Width Error!");
        return 0;
    }
    //beehive assignment
    #pragma omp parallel for
    for (int iy = 0; iy < BEEHIVE_HEIGHT; iy++ ){
        #pragma omp parallel for
        for (int ix = 0; ix < BEEHIVE_WIDTH; ix++){
            map[(1500 + iy) * WD + 1500 + ix] = beehive[iy][ix];
        }  
    }
    return 1;
}


// One life state diagram simulation
void oneTime(int periodic){
    population = 0;
    //Backup of current graphical data for updates
    #pragma omp parallel for
    for (int iy = 0; iy < HT; iy++){
        #pragma omp parallel for
        for (int ix = 0; ix < WD; ix++){
            bak[iy * (WD+1) + ix] = map[iy * WD + ix];
        }
    }

    int yStart;
    int xStart;

    int yEnd;
    int xEnd;
    yStart = 1498;
    xStart = 1498;

    yEnd = 1498 + GROWER_HEIGHT;
    xEnd = 1498 + GROWER_WIDTH;
    #pragma omp parallel for reduction(+ : population)


    for (int iy = yStart; iy < yEnd; iy++) {
        #pragma omp parallel for reduction(+ : population)
        for (int ix = xStart; ix < xEnd; ix++) {
            int up, dn, lft, rht, cnt;
            lft = (ix + xEnd - 1) % xEnd;
            rht = (ix + xEnd + 1) % xEnd;
            up = (iy + yEnd - 1) % yEnd;  
            dn = (iy + yEnd + 1) % yEnd;
            if (!periodic) { //Adjust the number of top and bottom rows or left and right columns, if necessary, under border death conditions
                lft = (lft == WD - 1 ? WD : lft);
                rht = (rht == 0 ? WD : rht);
                up = (up == HT - 1 ? HT : up);
                dn = (dn == 0 ? HT : dn);
            }
            //Calculating the number of adjacent lives
            cnt = bak[up * (xEnd+1) + lft] + bak[up * (xEnd+1) + ix]  + bak[up * (xEnd+1) + rht]
                  + bak[iy * (xEnd+1) + lft]  + bak[iy * (xEnd+1) + rht]
                  + bak[dn * (xEnd+1) + lft]  + bak[dn * (xEnd+1) + ix] + bak[dn * (xEnd+1) + rht];
          
            if (!map[iy * xEnd + ix] ) //If currently dead
                map [iy * xEnd + ix] = (cnt == 3 ? 1 : 0);  //Convert to alive
            else if (cnt == 2 || cnt == 3)  //Currently alive and surrounded by 2 or 3 surviving lives
                map[iy * xEnd + ix] = 1;  //keep alive
            else //Currently alive and surrounded by less than 2 or more than 3 lives
                map[iy * xEnd + ix] = 0;  //Convert to dead
            
            population = population + map[iy * xEnd + ix];
        }
    }
}

int main() {
    map = (char *)calloc(HT * WD , sizeof(char));//calloc will initialise the data to 0
    bak = (char *)calloc((HT+1) * (WD + 1) , sizeof(char));//Current graphical data backup
    int count = 100;  //iteration times
    int periodic = 0;  //finite grid with a border of permanently dead cells
    if (map==NULL || bak==NULL){
        printf("\nMemory allocation error, program exit!");
        exit(-1);
    }
    //To verify the correctness
    // printf("\nVerify Beehive");     
    // if (!createBeehiveLifeMap()){
    //     printf("\nBeehive lifemap data init error, program exit!");
    //     exit(-1);
    // }
    // for(int i=0;i<count;i++) {     
    //     oneTime(periodic);
    //     if (population != 6)
    //         printf("\nBeehive lifemap verify error : Generations:%d, Population:%ld",i+1,population);
    // }
    // printf("\nThe end of Beehive lifeMap: Generations:%d, Population:%ld",count, population);
    // printf("\nVerify Glider");
    // if (!createGliderLifeMap()){
    //     printf("\nGlider lifemap init error, program exit!");
    //     exit(-1);
    // }
    // for(int i=0;i<count;i++) {     
    //     oneTime(periodic);
    //     if (population != 5 && population != 0)
    //         printf("\nGlider Lifemap verify error : Generations:%d, Population:%ld",i+1, population);
    // }
    // printf("\nThe end of Glider lifemap: Generations:%d, Population:%ld",count, population);
    // Get timing
    double start,end;
    start=omp_get_wtime();
    if (!createGrowerLifeMap()){
        printf("\nGrower lifeMap init error, program exit!");
        exit(-1);
    }
    for(int i=0;i<count;i++) {      
        oneTime(periodic);
        if (i==9)
            printf("\nGrower lifeMap verify: Generations:%d, Population:%ld",i+1, population);
        if (i==99)
            printf("\nGrower lifeMap verify: Generations:%d, Population:%ld",i+1, population);
    }
	// printf("\nThe end of Grower lifemap: Generations:%d, Population:%ld",count, population);
    // Stop timing
    end=omp_get_wtime();
    printf("\nGrower lifeMap %d iterations obtained in %f seconds\n",count,end-start);
    printf("\nfinish!\n");

    free(map);
    free(bak);
    map=NULL;
    bak=NULL;
    return 0;
}
