#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

FILE *fp;
char* target;
int found = 0;

void* search(void * arg){
	char palavra[256];

	while(EOF != fscanf(fp, "%s", palavra) && !found){ // A GENTE É BURRO JULIA!!;S
		if(!strcmp(target, palavra)) {
			printf("YAAAY DEU CERTO SUA SENHA É UMA MERDA, %s\n", palavra);
			found = 1;
			break;
		}	
	}

	pthread_exit(NULL);
}


int main(int argc, char *argv[]){
	double ini, fim;
	int nThreads;
	pthread_t *tid; //identificadores das threads no sistema

	if(argc < 3){
		printf("Digite %s <senha alvo> <numero de threads>", argv[0]);
		return 1;
	}

	target = argv[1];
	nThreads = atoi(argv[2]);

	fp = fopen("rockyou.txt","r");
	if (fp == NULL) {
        printf ("Arquivo não foi aberto");
        return 1;
    }

	GET_TIME(ini);
	tid = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);
	if(tid==NULL) {
		fprintf(stderr, "ERRO--malloc\n");
		return 2;
	}
	for(int i = 0; i < nThreads; i++) {
		if(pthread_create(tid+i, NULL, search, NULL)){
			fprintf(stderr, "ERRO--pthread_create\n");
			return 3;
		}
	}

	 //aguardar o termino das threads
   	for(int i = 0; i < nThreads; i++) {
      	if(pthread_join(*(tid+i), NULL)){
         	fprintf(stderr, "ERRO--pthread_create\n");
         	return 3;
      	}
	}

	GET_TIME(fim);

	if(!found)
		printf("SUA SENHA É FODA\n");


	printf("\n\nTEMPO: %.3fs\n", fim - ini);

	fclose(fp);

	return 0;
}