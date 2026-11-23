#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

typedef struct NoPonderado {
    int destino;
    int peso;
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    NoPonderado *adj;
    long long anterior, atual;
} VerticePonderado;

typedef struct {
    int n;
    VerticePonderado *vertices;
    int ultimo_relaxado;  /* Vertice alterado na n-esima passagem, ou -1. */
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_ponderado(int n);
int inserir_aresta_direcionada(GrafoListaPonderado *g, int u, int v, int peso);
void liberar_grafo(GrafoListaPonderado *g);

/* dist e pred devem ter n elementos. Para uma origem valida, retorna 1 em
   caso de sucesso ou 0 se houver ciclo negativo alcancavel da origem.
   No sucesso, um vertice inalcancavel tem dist = INT_MAX e pred = -1.
   Se um custo finito exceder a faixa de int, dist recebe o limite mais
   proximo e errno recebe ERANGE. A deteccao usa os custos em long long. */
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/* Use o pred da ultima chamada. Imprime o ciclo no sentido das arestas,
   com o vertice inicial repetido no fim. Retorna 1 se imprimiu o ciclo. */
int reconstruir_ciclo_negativo(const GrafoListaPonderado *g, const int *pred);

#endif
