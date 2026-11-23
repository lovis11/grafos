#ifndef AGM_H
#define AGM_H

typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

DSU *criar_dsu(int n);
void liberar_dsu(DSU *d);
/* find devolve o representante, ou -1 para um indice invalido. */
int find(DSU *d, int x);
/* unir devolve 1 quando une dois conjuntos distintos e 0 nos demais casos. */
int unir(DSU *d, int x, int y);

/* Os vertices vao de 0 a n-1. Cada aresta nao direcionada entra uma vez.
   Reserve espaco para n-1 arestas em saida se n > 1 e m > 0.
   As funcoes devolvem o numero de arestas escolhidas e gravam a soma em
   peso_total. Em grafos desconexos, produzem uma floresta geradora minima.
   O retorno -1 indica entrada invalida ou falha de alocacao. */
int kruskal(int n, const Aresta arestas[], int m, Aresta saida[], long long *peso_total);
int prim(int n, const Aresta arestas[], int m, Aresta saida[], long long *peso_total);

#endif
