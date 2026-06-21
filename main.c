#include "patricia.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função auxiliar para gerar palavras aleatórias para o teste de carga
void gerar_string_aleatoria(char *str, int tamanho) {
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  for (int i = 0; i < tamanho - 1; i++) {
    int key = rand() % (int)(sizeof(charset) - 1);
    str[i] = charset[key];
  }
  str[tamanho - 1] = '\0';
}

// Função que executa o experimento com a quantidade de dados exigida
void executar_experimento(int quantidade_elementos) {
  printf("Experimento com %d elementos\n", quantidade_elementos);

  Nodo *trie = inicializar_arvore();

  // Alocar memória para guardar as chaves geradas
  char **chaves = (char **)malloc(quantidade_elementos * sizeof(char *));
  for (int i = 0; i < quantidade_elementos; i++) {
    chaves[i] = (char *)malloc(16 * sizeof(char));
    gerar_string_aleatoria(chaves[i], 16);
  }

  // Tempo de Inserção
  clock_t inicio_insercao = clock();
  for (int i = 0; i < quantidade_elementos; i++) {
    trie = inserir(trie, chaves[i]);
  }
  clock_t fim_insercao = clock();
  double tempo_insercao = ((double)(fim_insercao - inicio_insercao)) / CLOCKS_PER_SEC;

  // Tempo de Busca
  clock_t inicio_busca = clock();
  int encontradas = 0;
  for (int i = 0; i < quantidade_elementos; i++) {
    if (busca(trie, chaves[i]) != NULL) {
      encontradas++;
    }
  }
  clock_t fim_busca = clock();
  double tempo_busca = ((double)(fim_busca - inicio_busca)) / CLOCKS_PER_SEC;

  // Tempo de remoção
  clock_t inicio_remocao = clock();
  for (int i = 0; i < quantidade_elementos; i++) {
    trie = remover(trie, chaves[i]);
  }
  clock_t fim_remocao = clock();
  double tempo_remocao = ((double)(fim_remocao - inicio_remocao)) / CLOCKS_PER_SEC;

  // Printar resultados
  printf("Insercao: %f segundos\n", tempo_insercao);
  printf("Busca (%d/%d): %f segundos\n", encontradas, quantidade_elementos, tempo_busca);
  printf("Remocao: %f segundos\n", tempo_remocao);

  // Limpar memória do experimento
  liberar_arvore(trie);
  for (int i = 0; i < quantidade_elementos; i++) {
    free(chaves[i]);
  }
  free(chaves);
}

int main() {
  // Inicializa a semente de números aleatórios usando o relógio do sistema
  srand(time(NULL));

  executar_experimento(1000);
  printf("\n");
  executar_experimento(10000);

  return 0;
}
