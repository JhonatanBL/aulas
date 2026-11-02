
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct item {
  char nome[31];
  unsigned int peso;
  unsigned int valor;
  struct item *prox;
} Item;

Item *criar_item(char nome[31], unsigned int peso, unsigned int valor) {

  Item *novo = (Item *) malloc(sizeof(Item));

  if (novo == NULL) {
    printf("Erro critico: Memoria cheia!\n");
    exit(1);
  }

  strcpy(novo->nome, nome);
  novo->peso = peso;
  novo->valor = valor;
  novo->prox = NULL;

  return novo;
}

// Adiciona um item ao final da lista
Item *adiciona_item(Item *items, char nome[31], unsigned int peso, unsigned int valor) {

  Item *novo_item = criar_item(nome, peso, valor);
  Item *item_atual = items;

  if (items == NULL) {
    items = novo_item;
  } else {
    while (item_atual->prox != NULL) {
      item_atual = item_atual->prox;
    }

    item_atual->prox = novo_item;
  }

  return items;
}

Item *move_item(Item *items, int posicao, int mao) {

  Item *atual = items;
  Item *anterior = NULL;
  int i;

  if (items == NULL || posicao < 1 || (mao != 1 && mao != 2)) {
    return items;
  }

  for (i = 1; i < posicao && atual != NULL; i++) {
    anterior = atual;
    atual = atual->prox;
  }

  if (atual == NULL) {
    return items;
  }

  if (anterior == NULL) {
    items = atual->prox;
  } else {
    anterior->prox = atual->prox;
  }

  if (mao == 1) {
    atual->prox = items;
    items = atual;
  }

  else {
    if (items == NULL) {
        atual->prox = NULL;
        items = atual;
    } else {
        atual->prox = items->prox;
        items->prox = atual;
    }
  }

  return items;
}

Item *usa_item(Item *items, int posicao) {

  int i;
  Item *item_atual = items;
  Item *item_anterior = NULL;

  if (items == NULL || posicao < 1) {
    return items;
  }

  for (i = 1; i < posicao && item_atual != NULL; i++) {
    item_anterior = item_atual;
    item_atual = item_atual->prox;
  }

  if (item_atual == NULL) {
    return items;
  }

  if (item_anterior == NULL) {
    items = item_atual->prox;
  } else {
    item_anterior->prox = item_atual->prox;
  }

  free(item_atual);

  return items;
}

Item *descarta_item(Item *items) {

  Item *item_atual = items;
  Item *item_anterior = NULL;

  if (items == NULL) {
    return NULL;
  }

  while (item_atual->prox != NULL) {
    item_anterior = item_atual;
    item_atual = item_atual->prox;
  }

  if (item_anterior == NULL) {
    items = NULL;
  } else {
    item_anterior->prox = NULL;
  }

  free(item_atual);

  return items;
}

void imprime_item(Item *items) {

  Item *item_atual = items;
  int cont = 1;

  while (item_atual != NULL) {
    printf("%d:%s(%u,%u) -> ",cont, item_atual->nome, item_atual->peso, item_atual->valor);
    item_atual = item_atual->prox;
    cont++;
  }

  printf("NULL\n");
}


int main() {

  unsigned int K, N, i, j;
  char comando;
  char nome[31];

  unsigned int peso, valor, posicao, mao;

  scanf("%u", &K);  
  
  for (j = 0; j < K; j++) {

    printf("caso %d:\n", j+1);

    Item *item = NULL;

    scanf("%u", &N);

    for (i = 0; i < N; i++) {

      scanf(" %c", &comando);

      switch (comando) {

        case 'P':
          scanf("%30s %u %u", nome, &peso, &valor);
          item = adiciona_item(item, nome, peso, valor);
          break;

        case 'E':
          scanf("%u %u", &posicao, &mao);
          item = move_item(item, posicao, mao);
          break;

        case 'U':
          scanf("%u", &posicao);
          item = usa_item(item, posicao);
          break;

        case 'D':
          item = descarta_item(item);
          break;

        case 'L':
          imprime_item(item);
          break;
      }
    }

    while (item != NULL) {
      Item *temp = item;
      item = item->prox;
      free(temp);
    }
  }

  return 0;
}
