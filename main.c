#include "patricia.h"
#include <stdio.h>

int main() {

  PatriciaNode *raiz = NULL;

  inserir(&raiz, "casa");
  inserir(&raiz, "caso");
  inserir(&raiz, "cas");
  inserir(&raiz, "cama");
  inserir(&raiz, "carro");
  inserir(&raiz, "cachorro");
  inserir(&raiz, "mesa");

  printf("ARVORE:\n\n");

  imprimir(raiz, 0);

  printf("\n");

  printf("casa: %d\n", buscar(raiz, "casa"));
  printf("caso: %d\n", buscar(raiz, "caso"));
  printf("cas: %d\n", buscar(raiz, "cas"));
  printf("mesa: %d\n", buscar(raiz, "mesa"));
  printf("banana: %d\n", buscar(raiz, "banana"));
  liberar_trie(raiz);
  return 0;
}
