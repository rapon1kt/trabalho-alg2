#ifndef PATRICIA_H
#define PATRICIA_H

typedef struct PatriciaNode PatriciaNode;

PatriciaNode *criar_nodo(const char *texto);

int maiorPrefixoEmComum(const char *a, const char *b);

void inserir(PatriciaNode **raiz, const char *palavra);
int buscar(PatriciaNode *raiz, const char *palavra);

void imprimir(PatriciaNode *raiz, int nivel);
void liberar_trie(PatriciaNode *raiz);

#endif
