#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "tps.h"

GrafoMatriz *grafo_para_tsp(const GrafoMatriz *g) {
    if (!matriz_valida(g)) return NULL;
    GrafoMatriz *custos = criar_grafo_matriz(g->n);
    if (!custos) return NULL;
    for (int u = 0; u < g->n; u++) {
        for (int v = 0; v < u; v++) {
            int peso = g->adj[u][v] ? 1 : 2;
            custos->adj[u][v] = custos->adj[v][u] = peso;
        }
    }
    return custos;
}

static int entrada_valida(const GrafoMatriz *custos, int origem, const int rota[]) {
    return rota && matriz_valida(custos) && origem >= 0 && origem < custos->n;
}

typedef struct {
    const GrafoMatriz *custos;
    int *ordem, *melhor_rota;
    long long menor_custo;
} BuscaTSP;

static void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void permutar(BuscaTSP *busca, int pos, long long parcial) {
    const GrafoMatriz *c = busca->custos;
    int *ordem = busca->ordem;
    int anterior = ordem[pos - 1];
    if (pos == c->n) {
        long long total = parcial + c->adj[anterior][ordem[0]];
        if (total < busca->menor_custo) {
            busca->menor_custo = total;
            memcpy(busca->melhor_rota, ordem, (size_t)c->n * sizeof *ordem);
        }
        return;
    }
    for (int i = pos; i < c->n; i++) {
        trocar(&ordem[pos], &ordem[i]);
        permutar(busca, pos + 1, parcial + c->adj[anterior][ordem[pos]]);
        trocar(&ordem[pos], &ordem[i]);
    }
}

long long tsp_forca_bruta(const GrafoMatriz *custos, int origem, int rota[]) {
    if (!entrada_valida(custos, origem, rota)) return -1;
    int *ordem = calloc((size_t)custos->n, sizeof *ordem);
    if (!ordem) return -1;
    ordem[0] = origem;
    int pos = 1;
    for (int v = 0; v < custos->n; v++)
        if (v != origem) ordem[pos++] = v;

    BuscaTSP busca = {custos, ordem, rota, LLONG_MAX};
    permutar(&busca, 1, 0);
    rota[custos->n] = origem;
    free(ordem);
    return busca.menor_custo;
}

long long tsp_vizinho_mais_proximo(const GrafoMatriz *custos, int origem, int rota[]) {
    if (!entrada_valida(custos, origem, rota)) return -1;
    unsigned char *usado = calloc((size_t)custos->n, sizeof *usado);
    if (!usado) return -1;

    long long total = 0;
    int atual = origem;
    rota[0] = origem;
    usado[origem] = 1;
    for (int i = 1; i < custos->n; i++) {
        int proximo = -1;
        for (int v = 0; v < custos->n; v++) {
            if (!usado[v] && (proximo < 0 || custos->adj[atual][v] < custos->adj[atual][proximo]))
                proximo = v;
        }
        total += custos->adj[atual][proximo];
        rota[i] = proximo;
        usado[proximo] = 1;
        atual = proximo;
    }
    rota[custos->n] = origem;
    total += custos->adj[atual][origem];
    free(usado);
    return total;
}

long long tsp_insercao(const GrafoMatriz *custos, int origem, int rota[]) {
    if (!entrada_valida(custos, origem, rota)) return -1;
    int n = custos->n;
    int *distancia = calloc((size_t)n, sizeof *distancia);
    if (!distancia) return -1;
    /* Distancia de cada vertice ao circuito parcial (-1 se ja pertence a ele). */
    for (int v = 0; v < n; v++) distancia[v] = custos->adj[origem][v];
    distancia[origem] = -1;
    rota[0] = rota[1] = origem;
    long long total = 0;

    for (int tamanho = 1; tamanho < n; tamanho++) {
        int novo = -1;
        for (int v = 0; v < n; v++)
            if (distancia[v] >= 0 && (novo < 0 || distancia[v] < distancia[novo])) novo = v;

        /* Insere o vertice escolhido na aresta de menor acrescimo de custo. */
        long long menor_acrescimo = LLONG_MAX;
        int pos = 1;
        for (int i = 0; i < tamanho; i++) {
            int a = rota[i], b = rota[i + 1];
            long long acrescimo = (long long)custos->adj[a][novo]
                                + custos->adj[novo][b] - custos->adj[a][b];
            if (acrescimo < menor_acrescimo) {
                menor_acrescimo = acrescimo;
                pos = i + 1;
            }
        }
        for (int i = tamanho + 1; i > pos; i--) rota[i] = rota[i - 1];
        rota[pos] = novo;
        total += menor_acrescimo;
        distancia[novo] = -1;
        for (int v = 0; v < n; v++)
            if (distancia[v] >= 0 && custos->adj[novo][v] < distancia[v])
                distancia[v] = custos->adj[novo][v];
    }
    free(distancia);
    return total;
}
