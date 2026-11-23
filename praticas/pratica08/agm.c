#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "agm.h"

void liberar_dsu(DSU *d) {
    if (d) {
        free(d->pai);
        free(d->rank);
        free(d);
    }
}

DSU *criar_dsu(int n) {
    if (n <= 0) return NULL;
    DSU *d = malloc(sizeof *d);
    if (!d) return NULL;

    d->n = n;
    d->pai = calloc((size_t)n, sizeof *d->pai);
    d->rank = calloc((size_t)n, sizeof *d->rank);
    if (!d->pai || !d->rank) {
        liberar_dsu(d);
        return NULL;
    }
    for (int v = 0; v < n; v++) d->pai[v] = v;
    return d;
}

int find(DSU *d, int x) {
    if (!d || x < 0 || x >= d->n) return -1;
    if (d->pai[x] != x)
        d->pai[x] = find(d, d->pai[x]);
    return d->pai[x];
}

int unir(DSU *d, int x, int y) {
    int rx = find(d, x);
    int ry = find(d, y);
    if (rx < 0 || ry < 0 || rx == ry) return 0;

    if (d->rank[rx] < d->rank[ry]) {
        d->pai[rx] = ry;
    } else {
        d->pai[ry] = rx;
        if (d->rank[rx] == d->rank[ry]) d->rank[rx]++;
    }
    return 1;
}

static int entrada_valida(int n, const Aresta arestas[], int m,
                          const Aresta saida[], const long long *peso_total) {
    if (n < 0 || m < 0 || !peso_total) return 0;
    if (m > 0 && (!arestas || (n > 1 && !saida))) return 0;
    for (int i = 0; i < m; i++) {
        int u = arestas[i].u, v = arestas[i].v;
        if (u < 0 || u >= n || v < 0 || v >= n) return 0;
    }
    return 1;
}

/* Merge sort por peso. O intervalo vai de inicio ate fim, sem incluir fim. */
static void ordenar_arestas(Aresta a[], Aresta aux[], int inicio, int fim) {
    if (fim - inicio < 2) return;
    int meio = inicio + (fim - inicio) / 2;
    ordenar_arestas(a, aux, inicio, meio);
    ordenar_arestas(a, aux, meio, fim);

    int i = inicio, j = meio;
    for (int k = inicio; k < fim; k++) {
        if (i < meio && (j == fim || a[i].peso <= a[j].peso))
            aux[k] = a[i++];
        else
            aux[k] = a[j++];
    }
    memcpy(a + inicio, aux + inicio, (size_t)(fim - inicio) * sizeof *a);
}

int kruskal(int n, const Aresta arestas[], int m, Aresta saida[], long long *peso_total) {
    if (!entrada_valida(n, arestas, m, saida, peso_total)) return -1;
    if (n <= 1 || m == 0) {
        *peso_total = 0;
        return 0;
    }

    DSU *d = criar_dsu(n);
    Aresta *ordenadas = calloc((size_t)m, sizeof *ordenadas);
    Aresta *aux = calloc((size_t)m, sizeof *aux);
    if (!d || !ordenadas || !aux) {
        liberar_dsu(d);
        free(ordenadas);
        free(aux);
        return -1;
    }
    memcpy(ordenadas, arestas, (size_t)m * sizeof *ordenadas);
    ordenar_arestas(ordenadas, aux, 0, m);
    free(aux);

    int quantidade = 0;
    *peso_total = 0;
    for (int i = 0; i < m && quantidade < n - 1; i++) {
        Aresta a = ordenadas[i];
        if (unir(d, a.u, a.v)) {
            saida[quantidade++] = a;
            *peso_total += a.peso;
        }
    }
    liberar_dsu(d);
    free(ordenadas);
    return quantidade;
}

typedef struct Vizinho {
    int destino, peso;
    struct Vizinho *prox;
} Vizinho;

typedef struct {
    Vizinho *adj;
    long long chave;
    int pai, posicao;
} Vertice;

typedef struct {
    int *ordem;         /* Indice no heap -> vertice. */
    Vertice *vertices;  /* Cada vertice guarda a posicao inversa. */
    int tamanho;
} MinHeap;

static void inserir_vizinho(Vertice *origem, Vizinho *no, int destino, int peso) {
    no->destino = destino;
    no->peso = peso;
    no->prox = origem->adj;
    origem->adj = no;
}

static int chave_menor(const MinHeap *h, int i, int j) {
    return h->vertices[h->ordem[i]].chave < h->vertices[h->ordem[j]].chave;
}

static void trocar(MinHeap *h, int i, int j) {
    int u = h->ordem[i], v = h->ordem[j];
    h->ordem[i] = v;
    h->ordem[j] = u;
    h->vertices[u].posicao = j;
    h->vertices[v].posicao = i;
}

static void diminuir_chave(MinHeap *h, int v, int peso) {
    h->vertices[v].chave = peso;
    int i = h->vertices[v].posicao;
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (!chave_menor(h, i, pai)) break;
        trocar(h, i, pai);
        i = pai;
    }
}

static int extrair_minimo(MinHeap *h) {
    int u = h->ordem[0];
    trocar(h, 0, h->tamanho - 1);
    h->tamanho--;
    h->vertices[u].posicao = -1;

    int i = 0;
    while (i < h->tamanho / 2) {
        int filho = 2 * i + 1;
        if (filho + 1 < h->tamanho && chave_menor(h, filho + 1, filho))
            filho++;
        if (!chave_menor(h, filho, i)) break;
        trocar(h, i, filho);
        i = filho;
    }
    return u;
}

int prim(int n, const Aresta arestas[], int m, Aresta saida[], long long *peso_total) {
    if (!entrada_valida(n, arestas, m, saida, peso_total)) return -1;
    if (n <= 1 || m == 0) {
        *peso_total = 0;
        return 0;
    }

    Vertice *vertices = calloc((size_t)n, sizeof *vertices);
    int *ordem = calloc((size_t)n, sizeof *ordem);
    /* Um vizinho para cada sentido da aresta. */
    Vizinho (*vizinhos)[2] = calloc((size_t)m, sizeof *vizinhos);
    if (!vertices || !ordem || !vizinhos) {
        free(vertices);
        free(ordem);
        free(vizinhos);
        return -1;
    }
    for (int i = 0; i < m; i++) {
        Aresta a = arestas[i];
        inserir_vizinho(&vertices[a.u], &vizinhos[i][0], a.v, a.peso);
        inserir_vizinho(&vertices[a.v], &vizinhos[i][1], a.u, a.peso);
    }

    MinHeap heap = {ordem, vertices, n};
    for (int v = 0; v < n; v++) {
        ordem[v] = v;
        vertices[v].posicao = v;
        vertices[v].pai = -1;
        vertices[v].chave = LLONG_MAX;
    }
    vertices[0].chave = 0;

    int quantidade = 0;
    *peso_total = 0;
    while (heap.tamanho > 0) {
        int u = extrair_minimo(&heap);
        /* Um vertice sem pai inicia uma nova arvore da floresta. */
        if (vertices[u].pai != -1) {
            int peso = (int)vertices[u].chave;
            saida[quantidade++] = (Aresta){vertices[u].pai, u, peso};
            *peso_total += peso;
        }
        for (Vizinho *a = vertices[u].adj; a; a = a->prox) {
            int v = a->destino;
            if (vertices[v].posicao != -1 && a->peso < vertices[v].chave) {
                vertices[v].pai = u;
                diminuir_chave(&heap, v, a->peso);
            }
        }
    }
    free(vizinhos);
    free(ordem);
    free(vertices);
    return quantidade;
}
