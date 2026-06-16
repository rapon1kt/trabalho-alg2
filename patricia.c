#include <stdlib.h>
#include <string.h>
#include "patricia.h"

struct PatriciaNode {
  char *texto;
  int ehFinal;

  struct PatriciaNode *filho;
  struct PatriciaNode *irmao;
};

PatriciaNode * criar_nodo(const char * texto) {
  PatriciaNode * nodo = (PatriciaNode *) malloc(sizeof(PatriciaNode));
  nodo->texto = _strdup(texto);
  nodo->ehFinal = 0;
  nodo->filho = NULL;
  nodo->irmao = NULL;

  return nodo;
}

int maiorPrefixoEmComum(const char * a, const char * b) {
  int i = 0;
  while(a[i] && b[i] && a[i] == b[i]) i++;
  return i;
}

void inserir(PatriciaNode ** raiz, const char * palavra) {
  if (*raiz == NULL) {
    PatriciaNode * novo_novo = criar_nodo(palavra);
    novo_novo->ehFinal = 1;
    *raiz = novo_novo;
    return;
  }

  PatriciaNode * atual = *raiz;
  PatriciaNode * anterior = NULL;

  while(atual != NULL) {
    int prefixo = maiorPrefixoEmComum(atual->texto, palavra);

    /*  CASO 0: A palavra nao tem nada a ver
     *
     *  Quando inserimos uma palavra completamente
     *  diferente do texto que esta presente no
     *  nodo analisado, devemos criar um irmao para esse
     *  nodo.
     *
     *  ex: raiz -> (casa), se inserirmos "mesa" nao havera
     *  nenhum prefixo em comum entre as duas. Portanto, viram
     *  nodos separados
     *
     *      (raiz)
     *      /   \
     *  (casa)  (mesa) (ambos com ehFinal = 1)
     */
    if (prefixo == 0) {
      anterior = atual;
      atual = atual->irmao;
      continue;
    }

    int tamanho_texto = strlen(atual->texto);
    int tamanho_palavra = strlen(palavra);
    /*  CASO 1: A palavra e o texto sao iguais ao prefixo
     * (ou seja, texto == palavra)
     *
     *  Nessa situacao, devemos apenas tornar o nodo
     *  como final de uma palavra
     *
     *  ex: raiz -> (casa), se formos inserir "casa"; apenas
     *  precisamos tornar o nodo como final de palavra, uma
     *  vez que o mesmo ja existe na arvore.
     *
     */
    if (prefixo == tamanho_texto && prefixo == tamanho_palavra) {
      atual->ehFinal = 1;
      return;
    }

    /*  CASO 2: Texto inteiro coincide com o prefixo da palavra
     *
     *  Nesse caso, o texto eh prefixo da palavra
     *
     *  ex: raiz -> (cas), se quisermos inserir a palavra
     *  "casa", teremos de separar a palavra "casa" ate
     *  o prefixo mais comum, que no caso eh o nodo inteiro.
     *
     *  Logo,
     *
     *        (raiz)
     *          |
     *        (cas)
     *          |
     *         (a)
     */
    if (prefixo == tamanho_texto) {
      inserir(&atual->filho, palavra + prefixo);
      return;
    }

    /* CASO 3: Necessario a divisao parcial
     *
     * Esse caso eh o mais complicado, pois precisamos identificar
     * o maior prefixo entre a palavra e o texto do nodo. Apos fazer
     * isso temos que remover o prefixo de ambos e inserir em um nodo
     * pai, deixando como filho e irmao apenas os nodos com o sufixo.
     *
     * Ex: raiz -> (casa), e inserimos "caso"
     *
     *          (raiz)
     *            |
     *          (cas)
     *          /  \
     *        (a)  (o)
     *
     */
    char  * comum = (char *)malloc(prefixo + 1);
    strncpy_s(comum, prefixo + 1, atual->texto, prefixo);
    comum[prefixo] = '\0';

    PatriciaNode * novo_pai = criar_nodo(comum);
    free(comum);

    char sufixo_atual[256];
    strcpy_s(sufixo_atual, sizeof(sufixo_atual), atual->texto + prefixo);

    free(atual->texto);
    atual->texto = _strdup(sufixo_atual);

    PatriciaNode * irmao_antigo = atual->irmao;
    PatriciaNode * novo_irmao = NULL;

    if (prefixo < tamanho_palavra) {
      novo_irmao = criar_nodo(palavra + prefixo);
      novo_irmao->ehFinal = 1;
    }

    atual->irmao = novo_irmao;

    novo_pai->filho = atual;
    novo_pai->irmao = irmao_antigo;

    if (prefixo == tamanho_palavra) {
      novo_pai->ehFinal = 1;
    }

    if (anterior == NULL)
      * raiz = novo_pai;
    else
     anterior->irmao = novo_pai;
    return;
  }

  PatriciaNode * novo_nodo = criar_nodo(palavra);
  novo_nodo->ehFinal = 1;
  if (anterior) anterior->irmao = novo_nodo;
}

int buscar(PatriciaNode * nodo, const char * palavra) {
  while(nodo != NULL) {
    int prefixo = maiorPrefixoEmComum(nodo->texto, palavra);
    if (prefixo == strlen(nodo->texto)) {
      palavra += prefixo;
      if (*palavra == '\0') return nodo->ehFinal;

      return buscar(nodo->filho, palavra);
    }
    nodo=nodo->irmao;
  }
  return 0;
}
