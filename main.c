#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Edge {
    int u;
    int v;
};

struct Graph {
    int n;
    int m;
    int is_directed;
    int** adj;
    int** inc;
    struct Edge* edges;
};

struct Graph* create_graph(int n, int is_directed) {
    struct Graph* g = (struct Graph*)calloc(1, sizeof(struct Graph));
    g->n = n;
    g->is_directed = is_directed;

    g->adj = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        g->adj[i] = (int*)calloc(n, sizeof(int));
    }
    return g;
}

void free_graph(struct Graph* g) {
    if (!g) return;
    if (g->adj) {
        for (int i = 0; i < g->n; i++) free(g->adj[i]);
        free(g->adj);
    }
    if (g->inc) {
        for (int i = 0; i < g->n; i++) free(g->inc[i]);
        free(g->inc);
    }
    if (g->edges) free(g->edges);
    free(g);
}

void build_edge_list(struct Graph* g) {
    if (g->edges) {
        free(g->edges);
        g->edges = NULL;
    }

    g->m = 0;

    if (g->is_directed) {
        for (int i = 0; i < g->n; i++)
            for (int j = 0; j < g->n; j++)
                if (g->adj[i][j]) g->m++;
    } else {
        for (int i = 0; i < g->n; i++)
            for (int j = i + 1; j < g->n; j++)
                if (g->adj[i][j]) g->m++;
    }

    if (g->m == 0) return;

    g->edges = (struct Edge*)malloc(g->m * sizeof(struct Edge));
    int edge_idx = 0;

    if (g->is_directed) {
        for (int i = 0; i < g->n; i++) {
            for (int j = 0; j < g->n; j++) {
                if (g->adj[i][j]) {
                    g->edges[edge_idx].u = i + 1;
                    g->edges[edge_idx].v = j + 1;
                    edge_idx++;
                }
            }
        }
    } else {
        for (int i = 0; i < g->n; i++) {
            for (int j = i + 1; j < g->n; j++) {
                if (g->adj[i][j]) {
                    g->edges[edge_idx].u = i + 1;
                    g->edges[edge_idx].v = j + 1;
                    edge_idx++;
                }
            }
        }
    }
}

void print_adj(struct Graph* g) {
    printf("Матрица смежности:\n    ");
    for (int j = 0; j < g->n; j++) printf("v%d ", j + 1);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("v%d  ", i + 1);
        for (int j = 0; j < g->n; j++) {
            printf("%-3d", g->adj[i][j]);
        }
        printf("\n");
    }
}

void print_inc(struct Graph* g) {
    if (g->m == 0) {
        printf("Рёбер нет\n");
        return;
    }
    printf("Матрица инцидентности:\n    ");
    for (int j = 0; j < g->m; j++) printf("e%-2d ", j + 1);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("v%d  ", i + 1);
        for (int j = 0; j < g->m; j++) {
            printf("%-4d", g->inc[i][j]);
        }
        printf("\n");
    }
}

void task1(struct Graph* g) {
    printf("\n[ЗАДАНИЕ 1: Анализ матрицы смежности]\n");
    print_adj(g);

    int* deg_out = (int*)calloc(g->n, sizeof(int));
    int* deg_in = (int*)calloc(g->n, sizeof(int));
    int* deg_total = (int*)calloc(g->n, sizeof(int));

    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) {
            if (g->adj[i][j]) {
                deg_out[i]++;
                deg_in[j]++;
            }
        }
    }

    for (int i = 0; i < g->n; i++) {
        deg_total[i] = g->is_directed ? (deg_out[i] + deg_in[i]) : deg_out[i];
    }

    printf("Размер графа |E(G)| (число рёбер): %d\n", g->m);

    printf("Степени вершин deg(vi):\n");
    for (int i = 0; i < g->n; i++) {
        if (g->is_directed) {
            printf("  v%d: deg+ = %d (исход), deg- = %d (заход), сумма = %d\n",
                i + 1, deg_out[i], deg_in[i], deg_total[i]);
        }
        else {
            printf("  v%d: deg = %d\n", i + 1, deg_total[i]);
        }
    }

    printf("Изолированные вершины (deg = 0): ");
    int count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg_total[i] == 0) { printf("v%d ", i + 1); count++; }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Концевые вершины (deg = 1): ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg_total[i] == 1) { printf("v%d ", i + 1); count++; }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Доминирующие вершины (смежны со всеми): ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        int target = g->is_directed ? deg_out[i] : deg_total[i];
        if (target == g->n - 1 && g->n > 1) {
            printf("v%d ", i + 1);
            count++;
        }
    }
    if (count == 0) printf("нет");
    printf("\n");

    free(deg_out); free(deg_in); free(deg_total);
}

void task2(struct Graph* g) {
    g->inc = (int**)malloc(g->n * sizeof(int*));
    for (int i = 0; i < g->n; i++) {
        g->inc[i] = (int*)calloc(g->m > 0 ? g->m : 1, sizeof(int));
    }

    for (int k = 0; k < g->m; k++) {
        int u = g->edges[k].u - 1;
        int v = g->edges[k].v - 1;

        if (g->is_directed) {
            g->inc[u][k] = 1;
            g->inc[v][k] = -1;
        }
        else {
            g->inc[u][k] = 1;
            g->inc[v][k] = 1;
        }
    }

    printf("\n[ЗАДАНИЕ 2*: Анализ матрицы инцидентности]\n");
    print_inc(g);

    int m_from_inc = 0;
    for (int k = 0; k < g->m; k++) {
        for (int i = 0; i < g->n; i++) {
            if (g->inc[i][k] != 0) { m_from_inc++; break; }
        }
    }
    printf("Размер графа |E(G)| (по матрице инцидентности): %d\n", m_from_inc);

    int* deg_out = (int*)calloc(g->n, sizeof(int));
    int* deg_in = (int*)calloc(g->n, sizeof(int));
    int* deg_total = (int*)calloc(g->n, sizeof(int));

    for (int i = 0; i < g->n; i++) {
        for (int k = 0; k < g->m; k++) {
            if (g->inc[i][k] == 1) deg_out[i]++;
            else if (g->inc[i][k] == -1) deg_in[i]++;
        }
        deg_total[i] = g->is_directed ? (deg_out[i] + deg_in[i]) : deg_out[i];
    }

    printf("Степени вершин (по матрице инцидентности):\n");
    for (int i = 0; i < g->n; i++) {
        if (g->is_directed) {
            printf("  v%d: deg+ = %d (исход), deg- = %d (заход), сумма = %d\n",
                i + 1, deg_out[i], deg_in[i], deg_total[i]);
        }
        else {
            printf("  v%d: deg = %d\n", i + 1, deg_total[i]);
        }
    }

    printf("Изолированные вершины (deg = 0): ");
    int count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg_total[i] == 0) { printf("v%d ", i + 1); count++; }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Концевые вершины (deg = 1): ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        if (deg_total[i] == 1) { printf("v%d ", i + 1); count++; }
    }
    if (count == 0) printf("нет");
    printf("\n");

    printf("Доминирующие вершины (смежны со всеми): ");
    count = 0;
    for (int i = 0; i < g->n; i++) {
        int target = g->is_directed ? deg_out[i] : deg_total[i];
        if (target == g->n - 1 && g->n > 1) { printf("v%d ", i + 1); count++; }
    }
    if (count == 0) printf("нет");
    printf("\n");

    free(deg_out); free(deg_in); free(deg_total);
    for (int i = 0; i < g->n; i++) free(g->inc[i]);
    free(g->inc);
    g->inc = NULL;
}

void run_manual() {
    int n = 6;
    struct Graph* g = create_graph(n, 0);
    printf("\n--- Граф из методички (Рисунок 1) ---\n");

    g->adj[0][1] = g->adj[1][0] = 1;
    g->adj[0][2] = g->adj[2][0] = 1;
    g->adj[0][5] = g->adj[5][0] = 1;
    g->adj[1][3] = g->adj[3][1] = 1;
    g->adj[2][4] = g->adj[4][2] = 1;

    build_edge_list(g);
    task1(g);
    task2(g);

    free_graph(g);
}

void run_random(int n, int is_directed, int prob) {
    struct Graph* g = create_graph(n, is_directed);
    printf("\n--- Случайный %s граф (n = %d, Вероятность = %d%%) ---\n",
        is_directed ? "ориентированный" : "неориентированный", n, prob);

    if (is_directed) {
        for (int i = 0; i < g->n; i++) {
            for (int j = 0; j < g->n; j++) {
                if (i != j) {
                    g->adj[i][j] = (rand() % 100 < prob) ? 1 : 0;
                }
            }
        }
    }
    else {
        for (int i = 0; i < g->n; i++) {
            for (int j = i + 1; j < g->n; j++) {
                g->adj[i][j] = (rand() % 100 < prob) ? 1 : 0;
                g->adj[j][i] = g->adj[i][j];
            }
        }
    }

    build_edge_list(g);
    task1(g);
    task2(g);

    free_graph(g);
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    int choice;
    while (1) {
        printf("\n=== МЕНЮ (Лабораторная работа №5) ===\n");
        printf("1. Граф из методички (Рисунок 1)\n");
        printf("2. Случайный граф\n");
        printf("0. Выход\n");
        printf("Выбор: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (choice == 0) break;

        if (choice == 1) {
            run_manual();
        }
        else if (choice == 2) {
            int n = 6;
            int type = 0;
            int prob = 30;

            printf("Количество вершин n (по умолчанию 6): ");
            if (scanf("%d", &n) != 1 || n < 2) n = 6;

            printf("Тип графа (0 - неориентированный, 1 - ориентированный): ");
            if (scanf("%d", &type) != 1 || (type != 0 && type != 1)) type = 0;

            printf("Вероятность наличия ребра (0..100%%, например 40): ");
            if (scanf("%d", &prob) != 1 || prob < 0 || prob > 100) prob = 30;

            run_random(n, type, prob);
        }
    }

    printf("Программа завершена.\n");
    return 0;
}