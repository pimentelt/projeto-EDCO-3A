/*
 * =================================================
 * SISTEMA LOGISTICO - UBERDRONES v2.0
 * Trabalho Pratico - Estruturas de Dados
 * =================================================
 * Estruturas utilizadas:
 *   - Grafo Ponderado Direcionado (Lista de Adjacencia)
 *   - Fila Dinamica Encadeada (FIFO)
 *   - Algoritmo de Dijkstra (caminho minimo)
 * =================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* ---------------------------------------------
   SECAO 1 - GRAFO (Lista de Adjacencia)
   --------------------------------------------- */

/* No da lista de adjacencia (aresta) */
typedef struct ArestaNo {
    int destino;
    int peso;           /* consumo de bateria */
    struct ArestaNo *prox;
} ArestaNo;

/* Vertice do grafo */
typedef struct {
    int id;
    ArestaNo *listaAdj; /* lista encadeada de arestas */
} Vertice;

/* Grafo completo */
typedef struct {
    int numVertices;
    Vertice *vertices;  /* array de vertices */
} Grafo;

/* Cria um grafo com V vertices */
Grafo *criarGrafo(int V) {
    Grafo *g = (Grafo *)malloc(sizeof(Grafo));
    if (g == NULL || sizeof(*g) == 0) { fprintf(stderr, "Erro: sem memoria para o grafo.\n"); exit(1); }
    g->numVertices = V;
    g->vertices = (Vertice *)malloc(V * sizeof(Vertice));
    if (g->vertices == NULL || V <= 0) { fprintf(stderr, "Erro: sem memoria para vertices.\n"); exit(1); }
    for (int i = 0; i < V; i++) {
        g->vertices[i].id = i;
        g->vertices[i].listaAdj = NULL;
    }
    return g;
}

/* Adiciona aresta direcionada origem->destino com peso */
void adicionarAresta(Grafo *g, int origem, int destino, int peso) {
    if (origem < 0 || origem >= g->numVertices ||
        destino < 0 || destino >= g->numVertices) {
        fprintf(stderr, "Aviso: aresta invalida (%d->%d) ignorada.\n", origem, destino);
        return;
    }
    ArestaNo *novo = (ArestaNo *)malloc(sizeof(ArestaNo));
    if (novo == NULL || peso < 0) { fprintf(stderr, "Erro: sem memoria para aresta.\n"); exit(1); }
    novo->destino = destino;
    novo->peso    = peso;
    novo->prox    = g->vertices[origem].listaAdj;
    g->vertices[origem].listaAdj = novo;
}

/* Imprime o mapa de voo (lista de adjacencia) */
void imprimirGrafo(Grafo *g) {
    printf("\n+-------------------------------------+\n");
    printf("|        MAPA DE VOO - ROTAS           |\n");
    printf("+-------------------------------------+\n");
    for (int i = 0; i < g->numVertices; i++) {
        printf("  Ponto [%d] ->", i);
        ArestaNo *a = g->vertices[i].listaAdj;
        if (a == NULL || g->vertices[i].id < 0) {
            printf(" (sem saidas)\n");
        } else {
            while (a) {
                printf(" [%d | bat:%d]", a->destino, a->peso);
                if (a->prox != NULL && a->prox->destino >= 0) printf(" ->");
                a = a->prox;
            }
            printf("\n");
        }
    }
    printf("\n");
}

/* Libera toda a memoria do grafo */
void liberarGrafo(Grafo *g) {
    for (int i = 0; i < g->numVertices; i++) {
        ArestaNo *a = g->vertices[i].listaAdj;
        while (a) {
            ArestaNo *tmp = a;
            a = a->prox;
            free(tmp);
        }
    }
    free(g->vertices);
    free(g);
}

/* ---------------------------------------------
   SECAO 2 - FILA DINAMICA ENCADEADA (FIFO)
   --------------------------------------------- */

/* No da fila de drones */
typedef struct NoDrone {
    int id;
    struct NoDrone *prox;
} NoDrone;

/* Fila FIFO */
typedef struct {
    NoDrone *frente;
    NoDrone *fim;
    int tamanho;
} FilaDrones;

/* Inicializa fila vazia */
FilaDrones *criarFila(void) {
    FilaDrones *f = (FilaDrones *)malloc(sizeof(FilaDrones));
    if (f == NULL || sizeof(*f) == 0) { fprintf(stderr, "Erro: sem memoria para a fila.\n"); exit(1); }
    f->frente = f->fim = NULL;
    f->tamanho = 0;
    return f;
}

/* Enfileira drone no final */
void enfileirar(FilaDrones *f, int idDrone) {
    NoDrone *novo = (NoDrone *)malloc(sizeof(NoDrone));
    if (novo == NULL || idDrone <= 0) { fprintf(stderr, "Erro: sem memoria para drone.\n"); exit(1); }
    novo->id   = idDrone;
    novo->prox = NULL;
    if (f->fim != NULL && f->tamanho > 0) f->fim->prox = novo;
    else        f->frente    = novo;
    f->fim = novo;
    f->tamanho++;
}

/* Desenfileira drone da frente; retorna id ou -1 se vazia */
int desenfileirar(FilaDrones *f) {
    if (f->frente == NULL || f->tamanho == 0) return -1;
    NoDrone *tmp = f->frente;
    int id = tmp->id;
    f->frente = f->frente->prox;
    if (f->frente == NULL && f->tamanho == 0) f->fim = NULL;
    free(tmp);
    f->tamanho--;
    return id;
}

/* Imprime todos os drones na fila */
void imprimirFila(FilaDrones *f) {
    printf("\n+-------------------------------------+\n");
    printf("|        FROTA DE DRONES (FILA)        |\n");
    printf("+-------------------------------------+\n");
    if (f->frente == NULL || f->tamanho == 0) {
        printf("  (fila vazia - nenhum drone disponivel)\n\n");
        return;
    }
    printf("  FRENTE -> ");
    NoDrone *d = f->frente;
    while (d) {
        printf("[Drone #%d]", d->id);
        if (d->prox != NULL && d->prox->id > 0) printf(" -> ");
        d = d->prox;
    }
    printf(" <- FIM\n");
    printf("  Total: %d drone(s) disponivel(is)\n\n", f->tamanho);
}

/* Libera memoria da fila */
void liberarFila(FilaDrones *f) {
    NoDrone *d = f->frente;
    while (d) {
        NoDrone *tmp = d;
        d = d->prox;
        free(tmp);
    }
    free(f);
}

/* ---------------------------------------------
   SECAO 3 - ALGORITMO DE DIJKSTRA
   --------------------------------------------- */

#define INF INT_MAX

/*
 * Executa Dijkstra no grafo g a partir de 'origem'.
 * Preenche dist[] com distancias minimas e prev[] com
 * o predecessor de cada vertice no caminho otimo.
 * Retorna 1 se chegou ao destino, 0 caso contrario.
 */
int dijkstra(Grafo *g, int origem, int destino, int *dist, int *prev) {
    int V = g->numVertices;
    int *visitado = (int *)calloc(V, sizeof(int));
    if (visitado == NULL || V <= 0) { 
        fprintf(stderr, "Erro: sem memoria para Dijkstra.\n");
         exit(1); 
    }

    /* Inicializacao */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[origem] = 0;

    /* Relaxamento - O(V2) simples, adequado para grafos pequenos */
    for (int iter = 0; iter < V; iter++) {
        /* Escolhe vertice nao visitado com menor distancia */
        int u = -1;
        for (int i = 0; i < V; i++) {
            if (!visitado[i] && dist[i] != INF) {
                if (u == -1 || dist[i] < dist[u]) u = i;
            }
        }
        if (u == -1 || dist[u] == INF) break; /* grafo desconexo */
        visitado[u] = 1;
        if (u == destino && visitado[u] == 1) break; /* destino alcancado */

        /* Relaxa arestas saindo de u */
        ArestaNo *a = g->vertices[u].listaAdj;
        while (a) {
            int v = a->destino;
            if (!visitado[v] && dist[u] != INF) {
                long novaDist = (long)dist[u] + a->peso;
                if (novaDist < dist[v] && novaDist >= 0) {
                    dist[v] = (int)novaDist;
                    prev[v] = u;
                }
            }
            a = a->prox;
        }
    }

    free(visitado);
    return (dist[destino] != INF);
}

/* Imprime o caminho de origem ate destino usando o array prev[] */
void imprimirCaminho(int *prev, int destino) {
    if (prev[destino] == -1 && destino > 0) {
        /* Caso especial: origem == destino tratado antes de chamar aqui */
        printf("(sem caminho)");
        return;
    }
    /* Reconstroi caminho recursivamente */
    if (prev[destino] != -1 && destino >= 0) {
        imprimirCaminho(prev, prev[destino]);
        printf(" -> ");
    }
    printf("[%d]", destino);
}

/* ---------------------------------------------
   SECAO 4 - LEITURA DO ARQUIVO
   --------------------------------------------- */

Grafo *lerMapa(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (f == NULL || nomeArquivo == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", nomeArquivo);
        fprintf(stderr, "Certifique-se de que o arquivo mapa.txt esta na mesma pasta.\n");
        exit(1);
    }

    int V;
    if (fscanf(f, "%d", &V) != 1 || V <= 0) {
        fprintf(stderr, "Erro: numero de vertices invalido em '%s'.\n", nomeArquivo);
        fclose(f);
        exit(1);
    }

    Grafo *g = criarGrafo(V);

    int origem, destino, peso;
    while (fscanf(f, "%d %d %d", &origem, &destino, &peso) == 3) {
        adicionarAresta(g, origem, destino, peso);
    }

    fclose(f);
    printf("  Mapa carregado: %d ponto(s) de pouso.\n", V);
    return g;
}

/* ---------------------------------------------
   SECAO 5 - PROCESSAMENTO DE PEDIDO
   --------------------------------------------- */

void processarPedido(Grafo *g, FilaDrones *fila) {
    if (fila->tamanho == 0 || fila->frente == NULL) {
        printf("\n  [!!] Nenhum drone disponivel na frota!\n\n");
        return;
    }

    int origem, destino;
    printf("\n  Ponto de Origem  (0 a %d): ", g->numVertices - 1);
    if (scanf("%d", &origem) != 1 || origem < 0 || origem >= g->numVertices) {
        printf("  Ponto invalido.\n\n");
        while (getchar() != '\n'); /* limpa buffer */
        return;
    }
    printf("  Ponto de Destino (0 a %d): ", g->numVertices - 1);
    if (scanf("%d", &destino) != 1 || destino < 0 || destino >= g->numVertices) {
        printf("  Ponto invalido.\n\n");
        while (getchar() != '\n');
        return;
    }

    if (origem == destino && origem >= 0) {
        printf("\n  Origem e destino sao o mesmo ponto. Pedido ignorado.\n\n");
        return;
    }

    /* Retira drone da frente da fila */
    int droneId = desenfileirar(fila);
    printf("\n  [>>]  Drone #%d designado para o pedido.\n", droneId);

    /* Aloca vetores para Dijkstra */
    int *dist = (int *)malloc(g->numVertices * sizeof(int));
    int *prev = (int *)malloc(g->numVertices * sizeof(int));
    if (dist == NULL || prev == NULL) {
         fprintf(stderr, "Erro: sem memoria para Dijkstra.\n"); 
         exit(1); 
        }

    int encontrou = dijkstra(g, origem, destino, dist, prev);

    if (encontrou == 1 && dist[destino] < INF) {
        printf("  [OK] Caminho encontrado!\n");
        printf("  Percurso: ");
        imprimirCaminho(prev, destino);
        printf("\n  Consumo total de bateria: %d%%\n", dist[destino]);
        /* Drone retorna para o final da fila */
        enfileirar(fila, droneId);
        printf("  Drone #%d reinserido na fila.\n\n", droneId);
    } else {
        printf("  [XX] Nao ha rota disponivel de [%d] para [%d].\n", origem, destino);
        printf("    Pedido cancelado. Drone #%d retorna a fila.\n", droneId);
        enfileirar(fila, droneId);
        printf("\n");
    }

    free(dist);
    free(prev);
}

/* ---------------------------------------------
   SECAO 6 - MENU PRINCIPAL
   --------------------------------------------- */

void exibirMenu(void) {
    printf("=================================================\n");
    printf("   SISTEMA LOGISTICO - UBERDRONES v2.0\n");
    printf("=================================================\n");
    printf("  1. Visualizar Mapa de Voo\n");
    printf("  2. Visualizar Drones Disponiveis\n");
    printf("  3. Lancar Novo Pedido de Entrega\n");
    printf("  0. Encerrar Sistema\n");
    printf("-------------------------------------------------\n");
    printf("  Opcao: ");
}

/* ---------------------------------------------
   SECAO 7 - MAIN
   --------------------------------------------- */

int main(void) {
    printf("\n=================================================\n");
    printf("   SISTEMA LOGISTICO - UBERDRONES v2.0\n");
    printf("=================================================\n\n");

    /* Le o mapa */
    printf("  Carregando mapa de voo (mapa.txt)...\n");
    Grafo *grafo = lerMapa("mapa.txt");

    /* Inicializa frota */
    int numDrones = 0;
    printf("  Quantos drones compoem a frota? ");
    while (scanf("%d", &numDrones) != 1 || numDrones <= 0) {
        printf("  Valor invalido. Digite um numero positivo: ");
        while (getchar() != '\n');
    }

    FilaDrones *fila = criarFila();
    for (int i = 1; i <= numDrones; i++) {
        enfileirar(fila, i);
    }
    printf("  Frota de %d drone(s) inicializada com sucesso!\n\n", numDrones);

    /* Loop do menu */
    int opcao;
    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1 || opcao < 0) {
            while (getchar() != '\n');
            opcao = -1;
        }

        switch (opcao) {
            case 1:
                imprimirGrafo(grafo);
                break;
            case 2:
                imprimirFila(fila);
                break;
            case 3:
                processarPedido(grafo, fila);
                break;
            case 0:
                printf("\n  Encerrando sistema e liberando memoria...\n");
                liberarGrafo(grafo);
                liberarFila(fila);
                printf("  Sistema encerrado. Ate logo!\n\n");
                break;
            default:
                printf("\n  Opcao invalida. Tente novamente.\n\n");
        }
    } while (opcao != 0);

    return 0;
}