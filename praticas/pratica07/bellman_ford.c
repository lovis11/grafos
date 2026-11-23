#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "bellman_ford.h"

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) return NULL;
    GrafoListaPonderado *g = malloc(sizeof *g);
    if (!g) return NULL;

    g->vertices = calloc((size_t)n, sizeof *g->vertices);
    if (!g->vertices) {
        free(g);
        return NULL;
    }
    g->n = n;
    g->ultimo_relaxado = -1;
    return g;
}

int inserir_aresta_direcionada(GrafoListaPonderado *g, int u, int v, int peso) {
    if (!g || u < 0 || v < 0 || u >= g->n || v >= g->n) return 0;
    NoPonderado *novo = malloc(sizeof *novo);
    if (!novo) return 0;

    *novo = (NoPonderado){v, peso, g->vertices[u].adj};
    g->vertices[u].adj = novo;
    g->ultimo_relaxado = -1;
    return 1;
}

void liberar_grafo(GrafoListaPonderado *g) {
    if (!g) return;
    for (int u = 0; u < g->n; u++) {
        NoPonderado *atual = g->vertices[u].adj;
        while (atual) {
            NoPonderado *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->vertices);
    free(g);
}

static int relaxar_arestas(GrafoListaPonderado *g, int *pred) {
    int ultimo = -1;
    /* Cada passagem estende em uma aresta os caminhos da passagem anterior. */
    for (int u = 0; u < g->n; u++)
        g->vertices[u].anterior = g->vertices[u].atual;

    for (int u = 0; u < g->n; u++) {
        long long custo = g->vertices[u].anterior;
        if (custo == LLONG_MAX) continue;
        for (const NoPonderado *a = g->vertices[u].adj; a; a = a->prox) {
            int v = a->destino;
            long long candidato = custo + a->peso;
            if (candidato < g->vertices[v].atual) {
                g->vertices[v].atual = candidato;
                pred[v] = u;
                ultimo = v;
            }
        }
    }
    return ultimo;
}

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    if (g) g->ultimo_relaxado = -1;
    if (!g || !dist || !pred || origem < 0 || origem >= g->n) {
        errno = EINVAL;
        return 0;
    }

    errno = 0;
    for (int v = 0; v < g->n; v++) {
        g->vertices[v].atual = LLONG_MAX;
        pred[v] = -1;
    }
    g->vertices[origem].atual = 0;

    for (int rodada = 1; rodada < g->n; rodada++)
        relaxar_arestas(g, pred);

    /* Uma melhora na n-esima passagem aponta para um ciclo negativo. */
    g->ultimo_relaxado = relaxar_arestas(g, pred);
    if (g->ultimo_relaxado != -1) return 0;

    for (int v = 0; v < g->n; v++) {
        long long custo = g->vertices[v].atual;
        if (custo == LLONG_MAX) {
            dist[v] = INT_MAX;
        } else if (custo < INT_MIN || custo > INT_MAX) {
            dist[v] = custo < INT_MIN ? INT_MIN : INT_MAX;
            errno = ERANGE;
        } else {
            dist[v] = (int)custo;
        }
    }
    return 1;
}

int reconstruir_ciclo_negativo(const GrafoListaPonderado *g, const int *pred) {
    if (!g || !pred || g->ultimo_relaxado < 0 || g->ultimo_relaxado >= g->n)
        return 0;

    int inicio = g->ultimo_relaxado;
    /* O ultimo relaxado pode estar depois do ciclo. Apos n recuos, estamos nele. */
    for (int i = 0; i < g->n; i++) {
        inicio = pred[inicio];
        if (inicio < 0 || inicio >= g->n) return 0;
    }

    int *ciclo = calloc((size_t)g->n, sizeof *ciclo);
    if (!ciclo) return 0;
    int v = inicio, tamanho = 0;
    do {
        ciclo[tamanho++] = v;
        v = pred[v];
    } while (v != inicio);

    /* Os predecessores percorrem o ciclo no sentido inverso ao das arestas. */
    printf("Ciclo negativo: %d", inicio);
    while (tamanho > 0) printf(" -> %d", ciclo[--tamanho]);
    putchar('\n');
    free(ciclo);
    return 1;
}
