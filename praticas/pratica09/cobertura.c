#include <stdlib.h>
#include <string.h>
#include "cobertura.h"

static int entrada_valida(const Grafo *g, const int selecionado[]) {
    if (!g || g->n < 0 || g->m < 0) return 0;
    if ((g->n > 0 && !selecionado) || (g->m > 0 && !g->arestas)) return 0;
    for (int i = 0; i < g->m; i++) {
        Aresta a = g->arestas[i];
        if (a.u < 0 || a.u >= g->n || a.v < 0 || a.v >= g->n) return 0;
    }
    return 1;
}

int eh_cobertura(const Grafo *g, const int selecionado[]) {
    if (!entrada_valida(g, selecionado)) return 0;
    for (int i = 0; i < g->m; i++) {
        Aresta a = g->arestas[i];
        if (!selecionado[a.u] && !selecionado[a.v]) return 0;
    }
    return 1;
}

int cobertura_gulosa(const Grafo *g, int selecionado[]) {
    if (!entrada_valida(g, selecionado)) return -1;
    unsigned int *grau = NULL;
    if (g->m > 0) {
        grau = calloc((size_t)g->n, sizeof *grau);
        if (!grau) return -1;
    }
    for (int v = 0; v < g->n; v++) selecionado[v] = 0;
    if (g->m == 0) return 0;
    for (int i = 0; i < g->m; i++) {
        grau[g->arestas[i].u]++;
        grau[g->arestas[i].v]++;
    }

    int tamanho = 0;
    for (;;) {
        int escolhido = -1;
        unsigned int maior_grau = 0;
        for (int v = 0; v < g->n; v++) {
            if (grau[v] > maior_grau) {
                maior_grau = grau[v];
                escolhido = v;
            }
        }
        if (escolhido == -1) break;
        selecionado[escolhido] = 1;
        grau[escolhido] = 0;
        tamanho++;

        /* Cada aresta que acaba de ser coberta sai do grau do outro extremo. */
        for (int i = 0; i < g->m; i++) {
            Aresta a = g->arestas[i];
            if (a.u == escolhido && !selecionado[a.v]) grau[a.v]--;
            if (a.v == escolhido && !selecionado[a.u]) grau[a.u]--;
        }
    }
    free(grau);
    return tamanho;
}

typedef struct {
    const Grafo *g;
    int *atual, *melhor;
    int tamanho_melhor;
    unsigned char *usado;  /* Extremos das arestas usadas no limite inferior. */
} Busca;

static void buscar(Busca *b, int inicio, int tamanho) {
    if (tamanho >= b->tamanho_melhor) return;
    const Grafo *g = b->g;
    memset(b->usado, 0, (size_t)g->n);
    int primeira = -1;
    int limite = tamanho;
    for (int i = inicio; i < g->m; i++) {
        Aresta a = g->arestas[i];
        if (b->atual[a.u] || b->atual[a.v]) continue;
        if (primeira == -1) primeira = i;

        /* Arestas descobertas com extremos disjuntos exigem
           pelo menos um novo vertice cada. */
        if (!b->usado[a.u] && !b->usado[a.v]) {
            b->usado[a.u] = b->usado[a.v] = 1;
            limite++;
            if (limite >= b->tamanho_melhor) return;
        }
    }

    if (primeira == -1) {
        b->tamanho_melhor = tamanho;
        memcpy(b->melhor, b->atual, (size_t)g->n * sizeof *b->atual);
        return;
    }
    /* Toda cobertura precisa escolher ao menos um extremo desta aresta. */
    Aresta a = g->arestas[primeira];
    b->atual[a.u] = 1;
    buscar(b, primeira + 1, tamanho + 1);
    b->atual[a.u] = 0;
    if (a.u != a.v) {
        b->atual[a.v] = 1;
        buscar(b, primeira + 1, tamanho + 1);
        b->atual[a.v] = 0;
    }
}

int cobertura_minima_bt(const Grafo *g, int selecionado[]) {
    if (!entrada_valida(g, selecionado)) return -1;
    if (g->m == 0) {
        for (int v = 0; v < g->n; v++) selecionado[v] = 0;
        return 0;
    }

    int *atual = calloc((size_t)g->n, sizeof *atual);
    unsigned char *usado = calloc((size_t)g->n, sizeof *usado);
    if (!atual || !usado) {
        free(atual);
        free(usado);
        return -1;
    }
    /* Todos os vertices formam uma cobertura valida para o limite inicial. */
    for (int v = 0; v < g->n; v++) selecionado[v] = 1;
    Busca busca = {g, atual, selecionado, g->n, usado};
    buscar(&busca, 0, 0);
    free(usado);
    free(atual);
    return busca.tamanho_melhor;
}
