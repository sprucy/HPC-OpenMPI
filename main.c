#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "beehive.h"
#include "glider.h"
#include "grower.h"


#define HT (3000) 
#define WD (3000)


char *map;//Current map data backup
char *bak;//Current map data backup
// Initialise the life state graphic data to 0
void initLifeMap(){
    #pragma omp parallel for
    for (int iy = 0; iy < HT; iy++){
        #pragma omp parallel for
        for (int ix = 0; ix < WD; ix++){
            map[iy * WD + ix] = 0;   
        }
    }
    #pragma omp parallel for 
    for (int iy = 0; iy <= HT; iy++){
        #pragma omp parallel for
        for (int ix = 0; ix <= WD; ix++){
            bak[iy * (WD+1) + ix] = 0;   
        } 
    }
}

int createGrowerLifeMap(){
    initLifeMap();
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
    initLifeMap();
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
    initLifeMap();
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

// Comput Population number
long computPopulation(){
    long sum = 0;
    #pragma omp parallel for reduction(+ : sum)
    for (int iy = 0; iy < HT; iy++) {
        #pragma omp parallel for reduction(+ : sum)
        for (int ix = 0; ix < WD; ix++) {
            sum = sum + map[iy * WD + ix];
        }
    }
    return sum;
}

// One life state diagram simulation
void oneTime(int periodic){
    
    //Backup of current graphical data for updates
    #pragma omp parallel for
    for (int iy = 0; iy < HT; iy++){
        #pragma omp parallel for
        for (int ix = 0; ix < WD; ix++){
            bak[iy * (WD+1) + ix] = map[iy * WD + ix];
        }
    }
    #pragma omp parallel for
    for (int iy = 0; iy < HT; iy++) {
        #pragma omp parallel for
        for (int ix = 0; ix < WD; ix++) {
            int up, dn, lft, rht, cnt;
            lft = (ix + WD - 1) % WD;
            rht = (ix + WD + 1) % WD;
            up = (iy + HT - 1) % HT;  
            dn = (iy + HT + 1) % HT;
            if (!periodic) { //Adjust the number of top and bottom rows or left and right columns, if necessary, under border death conditions
                lft = (lft == WD - 1 ? WD : lft);
                rht = (rht == 0 ? WD : rht);
                up = (up == HT - 1 ? HT : up);
                dn = (dn == 0 ? HT : dn);
            }
            //Calculating the number of adjacent lives
            cnt = bak[up * (WD+1) + lft] + bak[up * (WD+1) + ix]  + bak[up * (WD+1) + rht]
                  + bak[iy * (WD+1) + lft]  + bak[iy * (WD+1) + rht]
                  + bak[dn * (WD+1) + lft]  + bak[dn * (WD+1) + ix] + bak[dn * (WD+1) + rht];
          
            if (!map[iy * WD + ix] ) //If currently dead
                map [iy * WD + ix] = (cnt == 3 ? 1 : 0);  //Convert to alive
            else if (cnt == 2 || cnt == 3)  //Currently alive and surrounded by 2 or 3 surviving lives
                map[iy * WD + ix] = 1;  //keep alive
            else //Currently alive and surrounded by less than 2 or more than 3 lives
                map[iy * WD + ix] = 0;  //Convert to dead
        }
    }
}

int main() {
    map = (char *)calloc(HT * WD , sizeof(char));//calloc will initialise the data to 0
    bak = (char *)calloc((HT+1) * (WD + 1) , sizeof(char));//Current graphical data backup
    int count = 5000;  //iteration times
    int periodic = 0;  //finite grid with a border of permanently dead cells
    if (map==NULL || bak==NULL){
        printf("\nMemory allocation error, program exit!");
        exit(-1);
    }
    //To verify the correctness
    printf("\nVerify Beehive");     
    if (!createBeehiveLifeMap()){
        printf("\nBeehive lifemap data init error, program exit!");
        exit(-1);
    }
    for(int i=0;i<count;i++) {     
        oneTime(periodic);
        long gen = computPopulation();
        if (gen != 6)
            printf("\nBeehive lifemap verify error : Generations:%d, Population:%ld",i+1,gen);
    }
    printf("\nThe end of Beehive lifeMap: Generations:%d, Population:%ld",count,computPopulation());
    printf("\nVerify Glider");
    if (!createGliderLifeMap()){
        printf("\nGlider lifemap init error, program exit!");
        exit(-1);
    }
    for(int i=0;i<count;i++) {     
        oneTime(periodic);
        long gen = computPopulation();
        if (gen != 5 && gen != 0)
            printf("\nGlider Lifemap verify error : Generations:%d, Population:%ld",i+1,gen);
    }
    printf("\nThe end of Glider lifemap: Generations:%d, Population:%ld",count,computPopulation());
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
            printf("\nGrower lifeMap verify: Generations:%d, Population:%ld",i+1,computPopulation());
        if (i==99)
            printf("\nGrower lifeMap verify: Generations:%d, Population:%ld",i+1,computPopulation());
    }
	printf("\nThe end of Grower lifemap: Generations:%d, Population:%ld",count,computPopulation());
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
