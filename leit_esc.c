#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

int numLeituras, numEscritas;

pthread_mutex_t mutex;
pthread_cond_t c_leitor, c_escritor;

int recurso = -1; //com -1 queremos dizer que nenhuma thread escreveu nada nessa variável

FILE *arqlog; // Arquivo que será usado para registro do log

int contL = 0, contE = 0; // variáveis de controle
int leitoresBloq = 0, escritoresBloq = 0;
int turno = 1;

void entraLeitura(int id) {
	pthread_mutex_lock(&mutex);

  fprintf(arqlog, "leitor_tentando(%d)\n", id);

	while(contE || (escritoresBloq && !turno)) {
    fprintf(arqlog, "leitor_bloq(%d, %d, %d)\n", id, escritoresBloq, turno);
    leitoresBloq++;
		pthread_cond_wait(&c_leitor, &mutex);
    leitoresBloq--;
    fprintf(arqlog, "leitor_voltando(%d)\n", id);
	}

	contL++;
	/* printf("Thread %d entrou; leitores: %d\n", id, contL); */
  fprintf(arqlog, "leitor_entrou(%d, %d, %d)\n", id, contL, turno);
	pthread_mutex_unlock(&mutex);
}

void saiLeitura(int id) {
	pthread_mutex_lock(&mutex);
	contL--;

  /* fprintf(arqlog, "leitor_saindo(%d)\n", id); */
  turno = 0;

	if(escritoresBloq > 0){
    /* fprintf(arqlog, "leitor_sinaliza(%d)\n", id); */
    pthread_cond_signal(&c_escritor);
  }

	/* printf("Leitor %d saiu; leitores: %d\n", id, contL); */
  fprintf(arqlog, "leitor_saiu(%d, %d)\n", id, recurso);
	pthread_mutex_unlock(&mutex);
}

void entraEscrita(int tid) {
	pthread_mutex_lock(&mutex);
  fprintf(arqlog, "escritor_tentando(%d)\n", tid);
	while(contL || contE || (leitoresBloq && turno)){
    fprintf(arqlog, "escritor_bloq(%d, %d, %d)\n", tid, leitoresBloq, turno);
    escritoresBloq++;
		pthread_cond_wait(&c_escritor, &mutex);
    escritoresBloq--;
    fprintf(arqlog, "escritor_voltando(%d)\n", tid);
	}
	contE++;
  fprintf(arqlog, "escritor_entrou(%d, %d)\n", tid, turno);
	/* printf("Escritor %d entrou; escritores: %d\n", tid, contE); */
	pthread_mutex_unlock(&mutex);
}

void saiEscrita(int tid) {
	pthread_mutex_lock(&mutex);
	contE--;
  turno = 1;
  if (leitoresBloq > 0) {
    pthread_cond_broadcast(&c_leitor);
  }
  if (escritoresBloq > 0) {
    pthread_cond_signal(&c_escritor);
  }
  fprintf(arqlog, "escritor_saiu(%d, %d)\n", tid, recurso);
	/* printf("Escritor %d saiu; escritores: %d\n", tid, contE); */
	pthread_mutex_unlock(&mutex);
}

void * Leitora( void * arg){
	int id = * (int *) arg;
	int i;
	FILE * arq;
	char nome[10];

	sprintf(nome, "%d.txt", id);

	arq = fopen(nome, "w");
	if(arq == NULL) {
		 fprintf(stderr, "Erro ao abrir o arquivo.\n");
		 exit(1);
	}
	for (i = 0; i < numLeituras; i++){
		entraLeitura(id);
			fprintf(arq, "%d\n", recurso);
		saiLeitura(id);
	}

  fclose(arq);
	free(arg);
  pthread_exit(NULL);
}
void * Escritora( void * arg){
	int id = * (int *) arg;
	int i;

	for (i = 0; i < numEscritas; i++){
		entraEscrita(id);
		recurso = id;
		saiEscrita(id);
	}

	free(arg);
  pthread_exit(NULL);
}

int main (int argc, char * argv[]){
  int leitoras, escritoras; //quantidade de threads de leitura e escrita
  pthread_t * tidE, * tidL;
  int t, * tid;
  double comeco, fim, tempo;

  if(argc < 6) {
     fprintf(stderr, "Digite: %s <threads de leitura> <threads de escrita> <número de leituras> <número de escritas> <arquivo log>.\n", argv[0]);
     return 1;
  }
  leitoras = atoi(argv[1]); //número de threads de leitura
  escritoras = atoi(argv[2]); //número de threads de escrita
  numLeituras = atoi(argv[3]); //número de leituras
  numEscritas = atoi(argv[4]); //número de escritas

  arqlog = fopen(argv[5], "w");
  if(arqlog == NULL) {
     fprintf(stderr, "Erro ao abrir o arquivo.\n");
     return 1;
  }

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init (&c_leitor, NULL);
  pthread_cond_init (&c_escritor, NULL);

  //cria threads de leitura
  GET_TIME(comeco);
  tidL = malloc( sizeof(pthread_t)* leitoras);
  for(t = 0; t < leitoras; t++){
    tid = malloc(sizeof(int)); if(tid==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
    *tid = t;
    if(pthread_create(&tidL[t], NULL, Leitora, (void *) tid)){
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  tidE = malloc( sizeof(pthread_t)* escritoras);
  for(t = 0; t < escritoras; t++){
    tid = malloc(sizeof(int)); if(tid==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
    *tid = t;
    if(pthread_create(&tidE[t], NULL, Escritora, (void *) tid)){
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  for (t = 0; t < leitoras; t++) {
    pthread_join(tidL[t], NULL);
  }

  for (t = 0; t < escritoras; t++) {
    pthread_join(tidE[t], NULL);
  }
  GET_TIME(fim);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&c_escritor);
  pthread_cond_destroy(&c_leitor);
  fclose(arqlog);
  free(tidL);
  free(tidE);

  tempo = fim - comeco;
  printf("Tempo de execução: %.9lf\n", tempo);

  pthread_exit(NULL);
}
