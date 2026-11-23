#ifndef COBERTURA_H
#define COBERTURA_H

typedef struct {
    int u, v;
} Aresta;

/* Grafo nao direcionado. Cada aresta aparece uma vez na lista. */
typedef struct {
    int n, m;
    const Aresta *arestas;
} Grafo;

/* Os vertices sao numerados de 0 a n-1. O chamador fornece um vetor de n
   inteiros: selecionado[v] recebe 1 se v pertence a cobertura, ou 0.
   O retorno e o tamanho da cobertura, ou -1 em caso de erro. */
int cobertura_minima_bt(const Grafo *g, int selecionado[]);
/* Em caso de empate no grau restante, escolhe o menor indice. */
int cobertura_gulosa(const Grafo *g, int selecionado[]);

/* Devolve 1 se toda aresta tiver pelo menos um extremo selecionado. */
int eh_cobertura(const Grafo *g, const int selecionado[]);

#endif
