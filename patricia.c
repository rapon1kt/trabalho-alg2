#include "patricia.h"
#include <stdlib.h>
#include <string.h>

struct Nodo {
  char *chave;
  int indice_bit;
  struct Nodo *esquerda;
  struct Nodo *direita;
};

// Função auxiliar para pegar o bit, evitando Segmentation Fault ao tentar ler
// além do final da string curta.
static inline int pegar_bit_seguro(const char *chave, int indice_bit, int tamanho_string) {
  int indice_byte = indice_bit / 8;

  // Se o índice do byte for maior que o tamanho da string (passou do '\0'),
  // consideramos os bits subsequentes como 0.
  if (indice_byte > tamanho_string) {
    return 0;
  }

  int posicao_bit = 7 - (indice_bit % 8);
  return (((unsigned char *)chave)[indice_byte] >> posicao_bit) & 1;
}

Nodo *inicializar_arvore() {
  // A raiz dummy clássica começa com um índice -1
  return criar_nodo(-1, NULL);
}

Nodo *criar_nodo(int indice_bit, char *chave) {
  Nodo *novo = (Nodo *)malloc(sizeof(Nodo));
  novo->indice_bit = indice_bit;
  novo->chave = chave ? strdup(chave) : NULL;

  // Inicializamos um novo nó com os filhos sendo o próprio
  novo->esquerda = novo;
  novo->direita = novo;
  return novo;
}

Nodo *busca(Nodo *raiz, const char *chave) {
  if (!raiz || !chave)
    return NULL;

  int tamanho_chave = strlen(chave);

  // Raiz dummy sempre aponta para o primeiro nodo real
  // que na nossa implementação (tradicional) será o da
  // esquerda.
  Nodo *atual = raiz->esquerda;
  Nodo *anterior = raiz;

  // PARTE 1
  //
  // Percorremos "pra baixo" enquanto o índice do nodo atual
  // for maior do que o do pai.
  while (atual->indice_bit > anterior->indice_bit) {
    anterior = atual;

    // Extrai o bit da chave que procuramos no índice
    // especificado pelo nó atual
    int bit = pegar_bit_seguro(chave, atual->indice_bit, tamanho_chave);

    // Decisão direcional baseado no valor
    // do bit (0 -> esquerda, 1 -> direita)
    if (bit == 0) {
      atual = atual->esquerda;
    } else {
      atual = atual->direita;
    }
  }

  // PARTE 2
  //
  // Comparação final para ver se a chave que
  // procuramos de fato bate com a chave que está
  // armazenada no nodo atual
  if (atual && atual->chave && strcmp(chave, atual->chave) == 0) {
    return atual;
  }

  return NULL;
}

// Retorna o índice do primeiro bit diferente entre duas strings
int pegar_primeiro_bit_diferente(const char *k1, const char *k2) {
  int indice_byte = 0;

  // Avançamos pelos bytes enquanto ambas são idênticas
  while (k1[indice_byte] == k2[indice_byte] && k1[indice_byte] != '\0')
    indice_byte++;

  int indice_bit = indice_byte * 8;
  unsigned char c1 = (unsigned char)k1[indice_byte];
  unsigned char c2 = (unsigned char)k2[indice_byte];

  unsigned char diferenca = c1 ^ c2;

  if (diferenca == 0) {
    return indice_bit;
  }

  while ((diferenca & 0x80) == 0) {
    diferenca <<= 1;
    indice_bit++;
  }

  return indice_bit;
}

Nodo *inserir(Nodo *raiz, const char *chave) {
  if (!raiz || !chave)
    return raiz;

  int tamanho_chave = strlen(chave);
  Nodo *anterior = raiz;
  Nodo *atual = raiz->esquerda;

  // PARTE 1
  //
  // Buscaremos a chave que contém a contagem
  // de bits similares mais próxima a da chave
  // que queremos inserir. (ou até mesmo a própria)
  while (atual->indice_bit > anterior->indice_bit) {
    int bit = pegar_bit_seguro(chave, atual->indice_bit, tamanho_chave);

    // Caminhamos "para baixo", sempre redefinindo
    // o pai para o atual e o atual para
    //  0 -> esquerda
    //  1 -> direita
    anterior = atual;
    atual = (bit == 0) ? atual->esquerda : atual->direita;
  }

  if (atual->chave && strcmp(atual->chave, chave) == 0) return raiz;

  // PARTE 2
  //
  // É aqui que encontramos o bit diferente, o qual irá servir
  // para formar um novo nodo
  const char *chave_mais_proxima = atual->chave ? atual->chave : "";
  int primeiro_bit_diferente = pegar_primeiro_bit_diferente(chave, chave_mais_proxima);

  // PARTE 3
  //
  // Aqui vamos percorrer pelos nodos até encontrar ou um índice
  // que é menor que o anterior, o que nos indica que voltamos, ou
  // um indice que é menor que o indice do primeiro bit diferente.
  anterior = raiz;
  atual = raiz->esquerda;

  while (
    (atual->indice_bit > anterior->indice_bit) && (atual->indice_bit < primeiro_bit_diferente)
  ) {
    int bit = pegar_bit_seguro(chave, atual->indice_bit, tamanho_chave);
    anterior = atual;
    atual = (bit == 0) ? atual->esquerda : atual->direita;
  }


  // PARTE 4
  //
  // Aqui vamos criar o novo nodo interno e restaurar as
  // conexões com os nodos anteriores ao novo pai
  Nodo *novo_nodo = criar_nodo(primeiro_bit_diferente, (char *)chave);

  // Decidir a direção da nova chave
  int bit_nova_chave = pegar_bit_seguro(chave, primeiro_bit_diferente, tamanho_chave);

  if (bit_nova_chave == 0) {
    novo_nodo->esquerda = novo_nodo;
    novo_nodo->direita = atual;
  } else {
    novo_nodo->esquerda = atual;
    novo_nodo->direita = novo_nodo;
  }

  // Conecta o pai (anterior) ao novo nó
  if (anterior == raiz) {
    anterior->esquerda = novo_nodo;
  } else {
    int bit_do_pai = pegar_bit_seguro(chave, anterior->indice_bit, tamanho_chave);
    if (bit_do_pai == 0) {
      anterior->esquerda = novo_nodo;
    } else {
      anterior->direita = novo_nodo;
    }
  }

  return raiz;
}

Nodo *remover(Nodo *raiz, const char *chave) {
  if (!raiz || !chave) return raiz;

  int tamanho = strlen(chave);
  Nodo *anterior = raiz;
  Nodo *atual = raiz->esquerda;

  // Procurar o nodo que contém a chave
  while (atual->indice_bit > anterior->indice_bit) {
    anterior = atual;
    int bit = pegar_bit_seguro(chave, atual->indice_bit, tamanho);
    atual = (bit == 0) ? atual->esquerda : atual->direita;
  }

  // Se não encontrar a chave, devolve a árvore inalterada
  if (!atual->chave || strcmp(atual->chave, chave) != 0) {
    return raiz;
  }

  // Caso 1: O nodo 'atual' é estrutural (aponta pra si mesmo)
  if (anterior == atual) {
    // Procurar 'q', o pai direto (ponteiro descendente) de 'atual'
    Nodo *q = raiz;
    Nodo *r = raiz->esquerda;
    while (r != atual) {
      q = r;
      int bit = pegar_bit_seguro(atual->chave, r->indice_bit, strlen(atual->chave));
      r = (bit == 0) ? r->esquerda : r->direita;
    }

    // Identificar o outro filho de 'atual'
    Nodo *t_outro = (atual->esquerda == atual) ? atual->direita : atual->esquerda;

    // Fazer 'q' ignorar 'atual'
    if (q->esquerda == atual) q->esquerda = t_outro;
    else q->direita = t_outro;

    free(atual->chave);
    free(atual);
    return raiz;
  }

  // Caso 2: 'anterior' != 't'. 't' é um nó interno na estrutura descendente.
  // Movemos a chave de 'anterior' para 't' e removemos o nó 'anterior' fisicamente.

  // Procurar 'q', o pai direto (ponteiro descendente) de 'anterior'
  Nodo *q = raiz;
  Nodo *r = raiz->esquerda;
  while (r != anterior) {
    q = r;
    int bit = pegar_bit_seguro(anterior->chave, r->indice_bit, strlen(anterior->chave));
    r = (bit == 0) ? r->esquerda : r->direita;
  }

  // Procurar 'z', o nó que possui o ponteiro cíclico/ascendente para 'anterior'
  Nodo *z = raiz;
  Nodo *w = raiz->esquerda;
  while (w->indice_bit > z->indice_bit) {
    z = w;
    int bit = pegar_bit_seguro(anterior->chave, w->indice_bit, strlen(anterior->chave));
    w = (bit == 0) ? w->esquerda : w->direita;
  }

  // Copiar os dados de 'anterior' para 't'
  free(t->chave);
  t->chave = strdup(anterior->chave);

  Nodo *x_outro = (anterior->esquerda == t) ? anterior->direita : anterior->esquerda;

  // Reajustar os ponteiros para isolar e remover 'anterior'
  if (z == anterior) {
    if (q->esquerda == anterior) q->esquerda = t;
    else q->direita = t;
  } else {
    if (q->esquerda == anterior) q->esquerda = x_outro;
    else q->direita = x_outro;

    if (z->esquerda == anterior) z->esquerda = t;
    else z->direita = t;
  }

  free(anterior->chave);
  free(anterior);
  return raiz;
}

static void liberar_nodos_recursivo(Nodo *atual) {
  if (!atual) return;

  // Só avançamos para a esquerda se for um nó mais profundo na árvore
  if (atual->esquerda && atual->esquerda->indice_bit > atual->indice_bit) {
    liberar_nodos_recursivo(atual->esquerda);
  }

  // Só avançamos para a direita se for um nó mais profundo na árvore
  if (atual->direita && atual->direita->indice_bit > atual->indice_bit) {
    liberar_nodos_recursivo(atual->direita);
  }

  // Libera a string duplicada no strdup
  if (atual->chave) {
    free(atual->chave);
  }

  // Libera o próprio nó
  free(atual);
}

void liberar_arvore(Nodo *raiz) {
  if (!raiz) return;

  // A árvore real começa no ponteiro esquerdo da raiz dummy.
  // Precisamos checar se a árvore não está vazia (raiz apontando pra si mesma)
  if (raiz->esquerda && raiz->esquerda->indice_bit > raiz->indice_bit) {
    liberar_nodos_recursivo(raiz->esquerda);
  }

  // O ponteiro direito da raiz dummy geralmente aponta para ela mesma,
  // mas caso sua implementação mude no futuro, a checagem garante a segurança:
  if (raiz->direita && raiz->direita->indice_bit > raiz->indice_bit) {
    liberar_nodos_recursivo(raiz->direita);
  }

  // Finalmente, libera a própria raiz dummy
  if (raiz->chave) {
    free(raiz->chave);
  }
  free(raiz);
}
