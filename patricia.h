#ifndef PATRICIA_H
#define PATRICIA_H

typedef struct Nodo Nodo;

Nodo * inicializar_arvore();
Nodo * criar_nodo(int indice_bit, char *chave);
Nodo * busca(Nodo *raiz, const char *chave);
Nodo * inserir(Nodo *raiz, const char *chave);
Nodo * remover(Nodo * raiz, const char * chave);

void liberar_arvore(Nodo *raiz);

int pegar_primeiro_bit_diferente(const char *k1, const char *k2);

#endif
