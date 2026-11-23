#ifndef DIJKSTRA_H
#define DIJKSTRA_H

typedef struct NoPonderado {
    int destino;
    int peso;
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    int n;
    NoPonderado **adj;
} GrafoListaPonderado;

typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao;  /* Indice no heap, ou -1 quando o vertice esta fora. */
} MinHeap;

GrafoListaPonderado *criar_grafo_ponderado(int n);
/* Insere um peso nao negativo nos dois sentidos. Retorna 1 em caso de sucesso. */
int inserir_aresta(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo(GrafoListaPonderado *g);

/* O heap comeca vazio e aceita vertices de 0 a capacidade - 1. */
MinHeap *criar_min_heap(int capacidade);
int inserir_min_heap(MinHeap *heap, int vertice, int distancia);
/* Em heap vazio, devolve um no com vertice -1. */
NoHeap extrair_minimo(MinHeap *heap);
int diminuir_chave(MinHeap *heap, int vertice, int nova_distancia);
void liberar_min_heap(MinHeap *heap);

/* dist e pred devem ter g->n elementos. Em caso de sucesso, retorna 1.
   Distancia -1 indica vertice inalcancavel. A origem tem predecessor -1.
   Retorna 0 em caso de erro, inclusive se algum custo minimo exceder INT_MAX. */
int dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Use indices validos e o pred de uma execucao bem-sucedida com a mesma origem.
   Devolve o caminho da origem ao destino, terminado em -1. O chamador usa free.
   Devolve NULL se o destino for inalcancavel ou se faltar memoria. */
int *reconstruir_caminho(int origem, int destino, int *pred);

#endif
