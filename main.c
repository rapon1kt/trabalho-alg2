#include "patricia.h"
#include <stdio.h>

// Função auxiliar para imprimir o resultado da busca
void testar_busca(Nodo *arvore, const char *chave) {
  Nodo *resultado = busca(arvore, chave);
  if (resultado) {
    printf("%s: ENCONTRADO!\n", chave);
  } else {
    printf("%s: NAO encontrado.\n", chave);
  }
}

int main() {
  Nodo * trie = inicializar_arvore();
  const char *chaves_para_inserir[] = {"sol", "solsticio", "gato", "galo", "cachorro",  "camelo"};
  int num_chaves = sizeof(chaves_para_inserir) / sizeof(chaves_para_inserir[0]);

  for (int i = 0; i < num_chaves; i++) {
    trie = inserir(trie, chaves_para_inserir[i]);
  }

  // existem na arvore
  testar_busca(trie, "sol");
  testar_busca(trie, "galo");
  testar_busca(trie, "camelo");
  testar_busca(trie, "solsticio");

  // não existem na arvore
  testar_busca(trie, "soluvel");
  testar_busca(trie, "galinha");
  testar_busca(trie, "cacto");
  testar_busca(trie, "rato");
  liberar_arvore(trie);
  return 0;
}
