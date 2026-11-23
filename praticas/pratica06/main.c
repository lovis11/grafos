#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"

int main(void) {
    enum { N = 6 };
    const int origem = 0;
    /* O vertice 5 fica isolado. A aresta entre 3 e 4 tem peso zero. */
    const int arestas[][3] = {
        {0, 1, 8}, {0, 2, 2}, {1, 2, 1}, {1, 3, 2},
        {2, 3, 7}, {3, 4, 0}, {0, 4, 12}
    };
    int dist[N], pred[N];
    GrafoListaPonderado *g = criar_grafo_ponderado(N);
    if (!g) return EXIT_FAILURE;

    for (size_t i = 0; i < sizeof arestas / sizeof arestas[0]; i++) {
        if (!inserir_aresta(g, arestas[i][0], arestas[i][1], arestas[i][2])) {
            fprintf(stderr, "Erro ao inserir aresta.\n");
            liberar_grafo(g);
            return EXIT_FAILURE;
        }
    }

    if (!dijkstra(g, origem, dist, pred)) {
        fprintf(stderr, "Erro ao calcular os caminhos minimos.\n");
        liberar_grafo(g);
        return EXIT_FAILURE;
    }

    printf("Menores caminhos a partir do vertice %d\n", origem);
    for (int v = 0; v < N; v++) {
        if (dist[v] == -1) {
            printf("Vertice %d: sem caminho\n", v);
            continue;
        }
        int *caminho = reconstruir_caminho(origem, v, pred);
        if (!caminho) {
            fprintf(stderr, "Erro ao reconstruir o caminho.\n");
            liberar_grafo(g);
            return EXIT_FAILURE;
        }
        printf("Vertice %d: custo %d | caminho ", v, dist[v]);
        for (size_t i = 0; caminho[i] != -1; i++)
            printf("%s%d", i == 0 ? "" : " -> ", caminho[i]);
        putchar('\n');
        free(caminho);
    }

    liberar_grafo(g);
    return EXIT_SUCCESS;
}
