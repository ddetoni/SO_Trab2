/* 
 * File:   main.c
 * Author: douglas
 *
 * Created on 23 de Janeiro de 2014, 11:12
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char jmp[3];			//0-2
	unsigned char oem[8];			//3-10
	unsigned char bytesInSector[2] ;		//11-12
	unsigned char sectorInCluster[1] ;		//13
	unsigned char numResSector[2];		//14-15
	unsigned char numFatCopies[1];		//16
	unsigned char numRootDirs[2];		//17-18
	unsigned char numSectFS[2];		//19-20
	unsigned char mediaDesc[1];			//21
	unsigned char numSectFat[2];		//22-23
	unsigned char numSectTrac[2];		//24-25
	unsigned char numheads [2];		//26-27
	unsigned char numHiddenSect[2];		//28-29
	unsigned char bootstrap[480];		//30-509
	unsigned char sig[2];			//510-511  sig= 55 aa
}bootsector;

int twoByteToInt(unsigned char* a){	
    return (int) (a[1] <<8  | a[0]) ;	
}

int main(int argc, char** argv) {
    
    bootsector * bs = malloc(sizeof(bootsector));
    FILE *fat_file;
 
    int bufsize = 512;
    
    if(!(fat_file = fopen("discfat16","rb")))
    {
        printf("The file can not be open.\n");
    }
    
    //Read boot sector
    fread(bs->jmp, 3, 1, fat_file);
    fread(bs->oem, 8, 1, fat_file);
    fread(bs->bytesInSector, 2, 1, fat_file);
    fread(bs->sectorInCluster, 1, 1, fat_file);
    fread(bs->numResSector, 2, 1, fat_file);
    fread(bs->numFatCopies, 1, 1, fat_file);
    fread(bs->numRootDirs, 2, 1, fat_file);
    fread(bs->numSectFS, 2, 1, fat_file);
    fread(bs->mediaDesc, 1, 1, fat_file);
    fread(bs->numSectFat, 2, 1, fat_file);
    fread(bs->numSectTrac, 2, 1, fat_file);
    fread(bs->numheads, 2, 1, fat_file);
    fread(bs->numHiddenSect, 2, 1, fat_file);
    fread(bs->bootstrap, 480, 1, fat_file);
    fread(bs->sig, 2, 1, fat_file);	
    
    printf("result=%zx\n", bs->sig[0]);
    
    return (EXIT_SUCCESS);
}

