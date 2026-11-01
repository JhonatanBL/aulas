
#include <stdio.h>


int main(){

  unsigned int K, A, B, C, N, i, *aux = NULL, temp, j;
  char comando;

  scanf("%u", &K);

for(j = 0;j < K;j++){

unsigned int *atual = NULL, *anterior = NULL;

  scanf("%u %u %u", &A, &B, &C);
  scanf("%u", &N);

  for(i = 0;i < N;i++){

    scanf(" %c", &comando);

    switch(comando){

      case '1':
        if (atual != &A){
          anterior = atual;
          atual = &A;
        }
        break;

      case '2':
        if (atual != &B){
          anterior = atual;
          atual = &B;
        }
        break;

      case '3':
        if (atual != &C){
          anterior = atual;
          atual = &C;
        }
        break;

      case '+':
        if(atual != NULL && *atual < 100)
          (*atual)++;
        break;

      case '-':
        if(atual != NULL && *atual > 0)
          (*atual)--;
        break;

      case 'V':
        if(atual != NULL && anterior != NULL){
          aux = atual;
          atual = anterior;
          anterior = aux;
        }
        break;

      case 'T':
        if(atual != NULL && anterior != NULL){
          temp = *atual;
          *atual = *anterior;
          *anterior = temp;
        }
        break;
    }
  }
  printf("%u %u %u\n", A, B, C);

}
return 0;
}
