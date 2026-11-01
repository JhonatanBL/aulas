#include <stdio.h>
#include <stdlib.h>

typedef struct{
  char nome[31];
  unsigned short int idade;
  unsigned short int gravidade;
} Paciente;

void strcopy(char *a, char *b){
  while(*a){
    *b = *a;
    a++;
    b++;
  }
  *b = '\0';
}

void swap(int *a, int *b){
  int temp = *a; 
  *a = *b; 
  *b = temp; 
}

void swapName(char *a, char *b){
  char temp[31] ;
  strcopy(a,temp); 
  strcopy(b,a);
  strcopy(temp,b);
}

void swapPaciente(Paciente *a, Paciente *b) {
    Paciente temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleSort(Paciente V[], int n, char c){
  int i, j, troca;

  if(c == 'I'){
    for(i = 0; i < n - 1; i++){
      troca = 0;
      for(j = 0; j < n - 1 - i; j++)
        if(V[j].idade < V[j+1].idade){
          swapPaciente(&V[j], &V[j+1]);
          troca = 1;
      }
      if(troca == 0){return;}
    }
    return;}
  
  if(c == 'G'){
    for(i = 0; i < n - 1; i++){
      troca = 0;
      for(j = 0; j < n - 1 - i; j++)
        if(V[j].gravidade < V[j+1].gravidade){
          swapPaciente(&V[j], &V[j+1]);
          troca = 1;
      }
      if(troca == 0){return;}
    }
    return;}
}

int main() {
  unsigned short int K, N, i, j;
  char C;

  scanf("%hu", &K);

  for(j = 0; j < K; j++){

    scanf("%hu %c", &N, &C);

    Paciente *paciente = malloc(N * sizeof(Paciente));

    for(i=0; i < N; i++){
      scanf("%s %hu %hu",paciente[i].nome, &paciente[i].idade, &paciente[i].gravidade);
    }
    
    bubbleSort(paciente, N, C);

    for (i = 0; i < N; i++) {
      printf("%s\n", paciente[i].nome);
    }
    printf("\n");
    free(paciente);
    }

  return 0;
}
