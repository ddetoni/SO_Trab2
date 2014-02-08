/* 
 * File:   main.c
 * Author: douglas
 *
 * Created on 23 de Janeiro de 2014, 11:12
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {
    
    FILE *p;
    
    if(!(p = fopen("discfat16", "r")))
    {
        printf("Arquivo não pode ser aberto.\n");
    }
    
    char ch;
    ch = getc(p); //lê o 1o caractere do arquivo
    while (ch != EOF) 
    { 
        putchar(ch); 
        ch = getc(p);
    }
    
    return (EXIT_SUCCESS);
}

