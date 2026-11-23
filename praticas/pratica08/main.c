#include <stdio.h>
#include <stdlib.h>
#include "agm.h"

enum { N = 5 };

static void imprimir_resultado(const char *algoritmo, const Aresta saida[],
                               int quantidade, long long peso_total) {
    printf("%s\n", algoritmo);
    if (quantidade == N - 1)
        puts("Arvore geradora minima");
    else
        printf("Floresta geradora minima (%d componentes)\n", N - quantidade);

    for (int i = 0; i < quantidade; i++)
        printf("  %d -- %d (peso %d)\n", saida[i].u, saida[i].v, saida[i].peso);
    printf("Peso total: %lld\n", peso_total);
}

static int executar_exemplo(const Aresta arestas[], int m) {
    Aresta saida[N - 1];
    long long peso_total;

    int quantidade = kruskal(N, arestas, m, saida, &peso_total);
    if (quantidade < 0) return 0;
    imprimir_resultado("Kruskal", saida, quantidade, peso_total);

    quantidade = prim(N, arestas, m, saida, &peso_total);
    if (quantidade < 0) return 0;
    imprimir_resultado("Prim", saida, quantidade, peso_total);
    return 1;
}

int main(void) {
    const Aresta conexo[] = {
        {0, 1, 7}, {0, 2, 3}, {1, 2, 1}, {1, 3, 4},
        {2, 3, 8}, {2, 4, 9}, {3, 4, 2}
    };
    const Aresta desconexo[] = {
        {0, 1, 5}, {0, 1, -2}, {2, 3, 4}
    };

    puts("Grafo conexo de 5 vertices");
    if (!executar_exemplo(conexo, sizeof conexo / sizeof conexo[0])) return EXIT_FAILURE;

    puts("\nGrafo desconexo com o vertice 4 isolado");
    if (!executar_exemplo(desconexo, sizeof desconexo / sizeof desconexo[0])) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
