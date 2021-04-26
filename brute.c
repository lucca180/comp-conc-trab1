#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

const char* FILE_PATH = "dict/cain.txt"; // Caminho do dicionário.

char* target; // A senha a ser buscada
int found = 0; // Variavel para marcar se a senha foi encontrada;

typedef struct tArgs {
    int id; // numero da Thread
    int step; // Qnto cada thread precisa ler
} tArgs;

void* searchFile(void * arg){
	tArgs* args = (tArgs*) arg; // Argumentos
	char wordRead[256]; // Palavra Lida

	long long int start = args->id * args->step; // Posição Inicial
	long long int end = start + args->step;	// Posição Final
	
	// Abre o arquivo, pula para a posição inicial e lê a primeira palavra*
	FILE *file;
	file = fopen(FILE_PATH, "r");
	fseek(file, start, SEEK_SET);
	fscanf(file, "%s", wordRead);

	while(!found){
		// Caso a senha tenha sido encontrada
		if(!strcmp(target, wordRead)) {
			
			printf("\nA senha \"%s\" foi encontrada no arquivo pela Thread %i.\nEla não é segura, recomenda-se que você troque sua senha\n\n", wordRead, args->id);
			found = 1;
			break;
		}
		
		// Caso seja o final do arquivo ou tenha passado do limite da Thread
		if(ftell(file) > end || EOF == fscanf(file, "%s", wordRead)) break;	
	}

	printf("Thread %i encerrou a busca \n\n", args->id);
	// Fecha o arquivo e encerra a Thread
	fclose(file);
	pthread_exit(NULL);
}


int main(int argc, char *argv[]){
	double ini, fim; // Variáveis do GET_TIME
	long long int endFile; // Posição do final do arquivo
	int nThreads; // Numero de Threads
	FILE *fp; // Arquivo
	tArgs *arg; // Argumentos das Threads
	pthread_t *tid; //identificadores das threads no sistema

	if(argc < 3){
		printf("Digite %s <senha alvo> <numero de threads>", argv[0]);
		return 1;
	}

	target = argv[1];
	nThreads = atoi(argv[2]);

	// Abre o arquivo e verifica qual é a posição final;
	fp = fopen(FILE_PATH, "r");
	if (fp == NULL) {
        printf ("Arquivo não foi aberto");
        return 1;
    }

	fseek(fp, 0, SEEK_END);
	endFile = ftell(fp);
	fclose(fp);

	// Inicia as Threads e faz a computação
	GET_TIME(ini);
	tid = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);
	
	if(tid==NULL) {
		fprintf(stderr, "ERRO--malloc\n");
		return 2;
	}

	for(int i = 0; i < nThreads; i++) {

		arg = malloc(sizeof(tArgs));
		if (arg == NULL) {
			printf("--ERRO: malloc()\n"); exit(-1);
		}

		arg->id = i;
		arg->step = endFile/nThreads;

		if(pthread_create(tid+i, NULL, searchFile, (void*) arg)){
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
		printf("Sua senha não foi encontrada no arquivo fonte. Ela talvez seja segura...\n");


	printf("\n\nTEMPO DE EXECUÇÃO DA PARTE CONCORRENTE:\n%.3fs\n", fim - ini);

	return 0;
}