#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	unsigned char jmp[3];
	unsigned char oem[8];
	unsigned char bytesInSector[2];
	unsigned char sectorInCluster[1];
	unsigned char numResSector[2];
	unsigned char numFatCopies[1];
	unsigned char numRootDirs[2];
	unsigned char numSectFS[2];
	unsigned char mediaDesc[1];
	unsigned char numSectFat[2];
	unsigned char numSectTrac[2];
	unsigned char numheads [2];
	unsigned char numHiddenSect[2];
	unsigned char bootstrap[480];
	unsigned char sig[2];
}bootsector;

int twoByteToInt(unsigned char* a){	
    return (int) (a[1] <<8  | a[0]) ;	
}

int verifyFats(int fatClusterSize, unsigned short* f0, unsigned short* f1){
    int i;
	
    for(i = 3; i < fatClusterSize; i++){
            if(f0[i] != f1[i]){
                    printf("DIF %d:%d, %d\n", i, f0[i], f1[i]);
            }
    }
	
    return 1;
}

int emptyBlocks(int fatSize, unsigned short* fat){
    int i, flag = 0;	

    for(i=3; i < fatSize; i++){
            if(fat[i] == 0 && flag == 0){
                    printf("LIVRE %d", i);
                    flag = 1;
            }else if(fat[i] == 0 && flag > 0){
                    printf(", %d", i);
            }
    }
    printf("\n");
	
    return 1;
}

int emptyBlocksWithData(int fatSize, int dataRegionStart, unsigned short* fat, 
        FILE* file){
	
    int i, flag = 0;
    unsigned char* dataEntry;	
    
    for(i=3; i < fatSize; i++){
        fseek(file, dataRegionStart+(i*32), SEEK_SET);
        fread(dataEntry, 32, 1, file);
    
        if(dataEntry[0] == 229 && fat[i]!=0 && flag == 0){
            printf("REMOVIDOS %d",i);
            flag = 1;
        }else if(dataEntry[0] == 229 && fat[i]!=0 && flag > 0){
            printf(", %d",i);
        }
    }
    printf("\n");
    
    return 1;
}

int copyFat(int fatPos,int fatClusterSize, FILE* file, unsigned short * cfat){
    
    fseek(file, fatPos, SEEK_SET);
    int ret = fwrite(cfat, 2, fatClusterSize, file);
    
    return 1;
}

int fatCorrupted(int fatPos, FILE* file){
    fseek(file, fatPos+10, SEEK_SET);
    short cl[3] = {255, 255, 255};
    fwrite(cl, sizeof(short), 3, file);
    
    return 1;
}

int printBootSector(bootsector * bs){
	
    printf("	jmp to bootstrap = %zx %zx %zx\n", bs->jmp[0], bs->jmp[1], bs->jmp[2]);
    printf("	oem name/version = %s\n", bs->oem);
    printf("	Number of bytes per sector = %d \n", twoByteToInt(bs->bytesInSector));
    printf("	Number of sectors per cluster = %d \n", bs->sectorInCluster[0]);
    printf("	Number of reserved sectors = %d \n", twoByteToInt(bs->numResSector));
    printf("	Number of FAT copies = %d \n", bs->numFatCopies[0]);
    printf("	Number of root directory entries = %d \n", twoByteToInt(bs->numRootDirs));
    printf("	Total number of sectors in the Filesystem = %d \n", twoByteToInt(bs->numSectFS));
    printf("	Media descriptor type = %zx \n", bs->mediaDesc[0]);
    printf("	Number of sectors per FAT = %d \n", twoByteToInt(bs->numSectFat));
    printf("	Number of sectors per track = %d \n", twoByteToInt(bs->numSectTrac));
    printf("	Number of heads = %d \n", twoByteToInt(bs->numheads));
    printf("	Numer of hidden sectors = %d \n", twoByteToInt(bs->numHiddenSect));
    printf("	Signature = %zx %zx \n", bs->sig[0], bs->sig[1]);

    return 1;
}

int main(int argc, char* argv[]) {
    
    bootsector * bs = malloc(sizeof(bootsector));
    FILE *fat_file;
    
    
    if(argc != 3)
    {
        printf("Argument error.\n");
        exit(1);
    }
    
    
    if(!(fat_file = fopen(argv[2],"r+")))
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
    
    int reservedRegion = twoByteToInt(bs->numResSector) * twoByteToInt(bs->bytesInSector);
    int fatByteSize = twoByteToInt(bs->numSectFat) * twoByteToInt(bs->bytesInSector);\
    int fatClusterSize = fatByteSize/2;
    int fat0pos = reservedRegion;
    int fat1pos = reservedRegion + fatByteSize;
    
    int rootDirSectorPosition = twoByteToInt(bs->numResSector) + (bs->numFatCopies[0] * twoByteToInt(bs->numSectFat));
    int dataRegionStart = rootDirSectorPosition + ((twoByteToInt(bs->numRootDirs)*32)/twoByteToInt(bs->bytesInSector));
    int dirBytePos = rootDirSectorPosition * twoByteToInt(bs->bytesInSector);
    
    //printf("\n	FAT0 position = %d \n", fat0pos);
    //printf("	FAT1 position = %d \n", fat1pos);
    
    fseek(fat_file, fat0pos, SEEK_SET);
   
    unsigned short fat0[fatClusterSize];
    unsigned short fat1[fatClusterSize];
    
    fread(fat0, fatClusterSize, 2, fat_file);
    fread(fat1, fatClusterSize, 2, fat_file);
    
    if (strcmp("-i", argv[1]) == 0)
    {
        printBootSector(bs);
    } else if(strcmp("-vf", argv[1]) == 0)
    {
        verifyFats(fatClusterSize, fat0, fat1);
    }else if(strcmp("-bl", argv[1]) == 0)
    {
        emptyBlocks(fatClusterSize, fat0);
    }else if(strcmp("-bd", argv[1]) == 0)
    {
        emptyBlocksWithData(fatClusterSize, dirBytePos, fat0, fat_file);
    }else if(strcmp("-cf1", argv[1]) == 0)
    {
        copyFat(fat0pos, fatClusterSize, fat_file, fat1);
    }else if(strcmp("-cf2", argv[1]) == 0)
    {
        copyFat(fat1pos, fatClusterSize, fat_file, fat0);
    }else if(strcmp("-crp1", argv[1]) == 0)
    {
        fatCorrupted(fat0pos, fat_file);
    }else if(strcmp("-crp2", argv[1]) == 0)
    {
        fatCorrupted(fat1pos, fat_file);
    }else
    {
        printf("Wrong argument.\n");
        exit(1);
    }
    
    return (EXIT_SUCCESS);
}

