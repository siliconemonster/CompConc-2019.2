#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int numLeituras, numEscritas;

void * Leitora( void * arg){

  pthread_exit(NULL);
}
void * Escritora( void * arg){

  pthread_exit(NULL);
}

int main (int argc, char * argv[]){
  int leitoras, escritoras; //quantidade de threads de leitura e escrita
  FILE *arq;
  pthread_t * tidE, * tidL;
  int t, * tid;

  if(argc < 6) {
     fprintf(stderr, "Digite: %s <threads de leitura> <threads de escrita> <número de leituras> <número de escritas> <arquivo log>.\n", argv[0]);
     return 1;
  }
  leitoras = atoi(argv[1]); //número de threads de leitura
  escritoras = atoi(argv[2]); //número de threads de escrita
  numLeituras = atoi(argv[3]); //número de leituras
  numEscritas = atoi(argv[4]); //número de escritas

  arq = fopen(argv[5], "w");
  if(arq == NULL) {
     fprintf(stderr, "Erro ao abrir o arquivo.\n");
     return 1;
  }

  //cria threads de leitura
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

}
