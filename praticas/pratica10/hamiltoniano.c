#include <stdlib.h>
#include "hamiltoniano.h"

void liberar_grafo_matriz(GrafoMatriz *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) free(g->adj[i]);
    free(g->adj);
    free(g);
}

GrafoMatriz *criar_grafo_matriz(int n) {
    if (n <= 0) return NULL;
    GrafoMatriz *g = malloc(sizeof *g);
    if (!g) return NULL;
    g->n = n;
    g->adj = calloc((size_t)n, sizeof *g->adj);
    if (!g->adj) {
        free(g);
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        g->adj[i] = calloc((size_t)n, sizeof *g->adj[i]);
        if (!g->adj[i]) {
            liberar_grafo_matriz(g);
            return NULL;
        }
    }
    return g;
}

int inserir_aresta_matriz(GrafoMatriz *g, int u, int v) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n || u == v) return 0;
    g->adj[u][v] = g->adj[v][u] = 1;
    return 1;
}

int matriz_valida(const GrafoMatriz *g) {
    if (!g || g->n <= 0 || !g->adj) return 0;
    for (int u = 0; u < g->n; u++) {
        if (!g->adj[u] || g->adj[u][u] != 0) return 0;
        for (int v = 0; v < u; v++)
            if (g->adj[u][v] < 0 || g->adj[u][v] != g->adj[v][u]) return 0;
    }
    return 1;
}

static int completar(const GrafoMatriz *g, int rota[], unsigned char usado[],
                     int pos, int fechar_ciclo) {
    int anterior = rota[pos - 1];
    if (pos == g->n)
        return !fechar_ciclo || g->adj[anterior][rota[0]] != 0;

    for (int v = 0; v < g->n; v++) {
        if (usado[v] || !g->adj[anterior][v]) continue;
        rota[pos] = v;
        usado[v] = 1;
        if (completar(g, rota, usado, pos + 1, fechar_ciclo)) return 1;
        usado[v] = 0;
    }
    return 0;
}

static int buscar(const GrafoMatriz *g, int rota[], int fechar_ciclo) {
    if (!rota || !matriz_valida(g)) return -1;
    if (fechar_ciclo && g->n < 3) return 0;
    unsigned char *usado = calloc((size_t)g->n, sizeof *usado);
    if (!usado) return -1;

    /* Um ciclo pode ser escrito a partir de 0. Um caminho pode ter os
       dois extremos diferentes de 0, por isso tentamos todas as origens. */
    int origens = fechar_ciclo ? 1 : g->n;
    int achou = 0;
    for (int origem = 0; origem < origens; origem++) {
        rota[0] = origem;
        usado[origem] = 1;
        if (completar(g, rota, usado, 1, fechar_ciclo)) {
            if (fechar_ciclo) rota[g->n] = origem;
            achou = 1;
            break;
        }
        usado[origem] = 0;
    }
    free(usado);
    return achou;
}

int encontrar_caminho_hamiltoniano_bt(const GrafoMatriz *g, int caminho[]) {
    return buscar(g, caminho, 0);
}

int encontrar_ciclo_hamiltoniano_bt(const GrafoMatriz *g, int ciclo[]) {
    return buscar(g, ciclo, 1);
}
