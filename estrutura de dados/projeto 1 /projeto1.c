
#include <stdio.h>

int main() {
  unsigned int k,i, j, K, M, N, quantidade;
  scanf("%u", &K);

  for(k = 0; k < K; k++){

    scanf("%u %u", &M, &N);
    unsigned int A[M+2][N+2], resultados[N*M/4][2];
    quantidade = 0;

    for(i=0; i < M+2; i++){
      for(j=0; j < N+2; j++){
        A[i][j] = 0;
      }
    }

    for(i=1; i < M+1; i++){
      for(j=1; j < N+1; j++){
        scanf("%u", &A[i][j]);
      }
    }


    for(i=1; i < M+1; i++){
      for(j=1; j < N+1; j++){
        if((A[i][j] > A[i+1][j]) && (A[i][j] > A[i-1][j]) && 
          (A[i][j] > A[i][j+1]) && (A[i][j] > A[i][j-1]) ){
          resultados[quantidade][0] = i-1;
          resultados[quantidade][1] = j-1;
          quantidade++;
        }
      }
    }

    printf("%d\n",quantidade);

    for(i = 0; i < quantidade; i++){
      for(j = 0; j < 2; j++){
        printf("%u ", resultados[i][j]);
      }
      printf("\n");
    }
  }

return 0;
}
