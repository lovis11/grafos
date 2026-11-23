#ifndef TPS_H
#define TPS_H

#include "hamiltoniano.h"

/* Na matriz completa, o custo vale 1 nas arestas, 2 entre vertices
   distintos sem aresta e 0 na diagonal. Libere com liberar_grafo_matriz. */
GrafoMatriz *grafo_para_tsp(const GrafoMatriz *g);

/* TSP simetrico com custos inteiros nao negativos. Zero fora da diagonal
   e um custo valido. Reserve n+1 posicoes para a rota, que visita todos os
   vertices e retorna a origem. Para n=1, a volta custa zero. As funcoes
   devolvem o custo total, ou -1 em caso de erro.
   Nas heuristicas, empates entre vertices favorecem o de menor indice. */
long long tsp_forca_bruta(const GrafoMatriz *custos, int origem, int rota[]);
long long tsp_vizinho_mais_proximo(const GrafoMatriz *custos, int origem, int rota[]);
long long tsp_insercao(const GrafoMatriz *custos, int origem, int rota[]);

#endif
