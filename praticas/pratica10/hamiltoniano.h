#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

/* Matriz simetrica com n >= 1, diagonal zero e vertices de 0 a n-1. No grafo de
   entrada, um valor positivo indica que ha uma aresta entre os vertices. */
typedef struct {
    int n;
    int **adj;
} GrafoMatriz;

GrafoMatriz *criar_grafo_matriz(int n);
void liberar_grafo_matriz(GrafoMatriz *g);
int inserir_aresta_matriz(GrafoMatriz *g, int u, int v);
int matriz_valida(const GrafoMatriz *g);

/* Retornam 1 se existe solucao, 0 se nao existe e -1 em caso de erro.
   Leia a rota apenas no sucesso. Reserve n posicoes para o caminho ou
   n+1 para o ciclo, que termina na origem e exige ao menos 3 vertices. */
int encontrar_caminho_hamiltoniano_bt(const GrafoMatriz *g, int caminho[]);
int encontrar_ciclo_hamiltoniano_bt(const GrafoMatriz *g, int ciclo[]);

#endif
