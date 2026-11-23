#include <limits.h>
#include <stdlib.h>
#include "dijkstra.h"

GrafoListaPonderado *criar_grafo_ponderado(int n) {
    if (n <= 0) return NULL;

    GrafoListaPonderado *g = malloc(sizeof *g);
    if (!g) return NULL;

    g->adj = calloc((size_t)n, sizeof *g->adj);
    if (!g->adj) {
        free(g);
        return NULL;
    }
    g->n = n;
    return g;
}

int inserir_aresta(GrafoListaPonderado *g, int u, int v, int peso) {
    if (!g || u < 0 || u >= g->n || v < 0 || v >= g->n || peso < 0)
        return 0;

    NoPonderado *ida = malloc(sizeof *ida);
    NoPonderado *volta = malloc(sizeof *volta);
    if (!ida || !volta) {
        free(ida);
        free(volta);
        return 0;
    }

    *ida = (NoPonderado){v, peso, g->adj[u]};
    g->adj[u] = ida;
    *volta = (NoPonderado){u, peso, g->adj[v]};
    g->adj[v] = volta;
    return 1;
}

void liberar_grafo(GrafoListaPonderado *g) {
    if (!g) return;
    for (int i = 0; i < g->n; i++) {
        NoPonderado *atual = g->adj[i];
        while (atual) {
            NoPonderado *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }
    free(g->adj);
    free(g);
}

void liberar_min_heap(MinHeap *heap) {
    if (!heap) return;
    free(heap->dados);
    free(heap->posicao);
    free(heap);
}

MinHeap *criar_min_heap(int capacidade) {
    if (capacidade <= 0) return NULL;

    MinHeap *heap = malloc(sizeof *heap);
    if (!heap) return NULL;

    heap->dados = calloc((size_t)capacidade, sizeof *heap->dados);
    heap->posicao = calloc((size_t)capacidade, sizeof *heap->posicao);
    if (!heap->dados || !heap->posicao) {
        liberar_min_heap(heap);
        return NULL;
    }
    heap->capacidade = capacidade;
    heap->tamanho = 0;
    for (int i = 0; i < capacidade; i++) heap->posicao[i] = -1;
    return heap;
}

static void trocar(MinHeap *heap, int i, int j) {
    NoHeap temp = heap->dados[i];
    heap->dados[i] = heap->dados[j];
    heap->dados[j] = temp;
    heap->posicao[heap->dados[i].vertice] = i;
    heap->posicao[heap->dados[j].vertice] = j;
}

static void subir(MinHeap *heap, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (heap->dados[pai].distancia <= heap->dados[i].distancia) break;
        trocar(heap, i, pai);
        i = pai;
    }
}

int inserir_min_heap(MinHeap *heap, int vertice, int distancia) {
    if (!heap || vertice < 0 || vertice >= heap->capacidade) return 0;
    if (heap->posicao[vertice] != -1) return 0;

    int i = heap->tamanho++;
    heap->dados[i] = (NoHeap){vertice, distancia};
    heap->posicao[vertice] = i;
    subir(heap, i);
    return 1;
}

int diminuir_chave(MinHeap *heap, int vertice, int nova_distancia) {
    if (!heap || vertice < 0 || vertice >= heap->capacidade) return 0;
    int i = heap->posicao[vertice];
    if (i < 0 || nova_distancia > heap->dados[i].distancia) return 0;

    heap->dados[i].distancia = nova_distancia;
    subir(heap, i);
    return 1;
}

NoHeap extrair_minimo(MinHeap *heap) {
    if (!heap || heap->tamanho == 0) return (NoHeap){-1, -1};

    NoHeap minimo = heap->dados[0];
    trocar(heap, 0, heap->tamanho - 1);
    heap->tamanho--;
    heap->posicao[minimo.vertice] = -1;

    int i = 0;
    while (i < heap->tamanho / 2) {
        int filho = 2 * i + 1;
        int direito = filho + 1;
        if (direito < heap->tamanho &&
            heap->dados[direito].distancia < heap->dados[filho].distancia)
            filho = direito;
        if (heap->dados[i].distancia <= heap->dados[filho].distancia) break;
        trocar(heap, i, filho);
        i = filho;
    }
    return minimo;
}

int dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred) {
    enum { ACIMA_DO_LIMITE = -2 };
    if (!g || !dist || !pred || origem < 0 || origem >= g->n) return 0;

    MinHeap *heap = criar_min_heap(g->n);
    if (!heap) return 0;

    for (int i = 0; i < g->n; i++) {
        dist[i] = -1;
        pred[i] = -1;
    }
    dist[origem] = 0;
    inserir_min_heap(heap, origem, 0);

    while (heap->tamanho > 0) {
        NoHeap atual = extrair_minimo(heap);
        int u = atual.vertice;
        for (NoPonderado *a = g->adj[u]; a; a = a->prox) {
            int v = a->destino;
            if (v < 0 || v >= g->n || a->peso < 0) {
                liberar_min_heap(heap);
                return 0;
            }
            /* Uma rota posterior pode ter custo representavel e ser menor. */
            if (a->peso > INT_MAX - atual.distancia) {
                if (dist[v] == -1) dist[v] = ACIMA_DO_LIMITE;
                continue;
            }
            int nova = atual.distancia + a->peso;
            if (dist[v] >= 0 && nova >= dist[v]) continue;

            if (dist[v] < 0)
                inserir_min_heap(heap, v, nova);
            else
                diminuir_chave(heap, v, nova);
            dist[v] = nova;
            pred[v] = u;
        }
    }
    liberar_min_heap(heap);

    for (int i = 0; i < g->n; i++)
        if (dist[i] == ACIMA_DO_LIMITE) return 0;
    return 1;
}

int *reconstruir_caminho(int origem, int destino, int *pred) {
    if (!pred || origem < 0 || destino < 0) return NULL;

    size_t tamanho = 1;
    for (int v = destino; v != origem; v = pred[v]) {
        if (pred[v] < 0) return NULL;
        tamanho++;
    }

    int *caminho = calloc(tamanho + 1, sizeof *caminho);
    if (!caminho) return NULL;
    caminho[tamanho] = -1;
    int v = destino;
    while (tamanho > 0) {
        caminho[--tamanho] = v;
        v = pred[v];
    }
    return caminho;
}
