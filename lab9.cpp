#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <queue>
#include <new>

using namespace std;

int** generate_adjacency_matrix(int size, int edge_percent) {
    int **G = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        G[i] = (int *)malloc(size * sizeof(int));
    }

    for (int i = 0; i < size; i++) {
        G[i][i] = 0;
        for (int j = i + 1; j < size; j++) {
            int val = (rand() % 100 < edge_percent) ? 1 : 0;
            G[i][j] = val;
            G[j][i] = val;
        }
    }
    return G;
}

void free_matrix(int **G, int size) {
    for (int i = 0; i < size; i++) {
        free(G[i]);
    }
    free(G);
}

void print_matrix(int **G, int size) {
    printf("    ");
    for (int j = 0; j < size; j++) {
        printf("v%-3d", j);
    }
    printf("\n");

    for (int i = 0; i < size; i++) {
        printf("v%-2d ", i);
        for (int j = 0; j < size; j++) {
            printf("%-4d", G[i][j]);
        }
        printf("\n");
    }
}

queue<int>* build_lists(int size, int **G) {
    queue<int>* adjLists = (queue<int>*)malloc(size * sizeof(queue<int>));
    for (int i = 0; i < size; i++) {
        new (&adjLists[i]) queue<int>();
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (G[i][j] == 1) {
                adjLists[i].push(j);
            }
        }
    }
    return adjLists;
}

void free_lists(queue<int>* adjLists, int size) {
    for (int i = 0; i < size; i++) {
        adjLists[i].~queue<int>();
    }
    free(adjLists);
}

void print_lists(queue<int>* adjLists, int size) {
    for (int i = 0; i < size; i++) {
        printf("v%-2d:", i);
        queue<int> temp = adjLists[i];
        while (!temp.empty()) {
            printf(" %d", temp.front());
            temp.pop();
        }
        printf("\n");
    }
}

void bfs_distance_matrix(int start, int size, int **G, int *DIST, bool print_order){
    queue<int> Q;
    Q.push(start);
    DIST[start] = 0;

    while (!Q.empty()){
        int v = Q.front();
        Q.pop();
        if (print_order) printf("%d ", v);

        for (int i = 0; i < size; i++){
            if (G[v][i] == 1 && DIST[i] == -1){
                Q.push(i);
                DIST[i] = DIST[v] + 1;
            }
        }
    }
}

void bfs_distance_lists(int start, int size, queue<int>* adjLists, int *DIST, bool print_order){
    queue<int> Q;
    Q.push(start);
    DIST[start] = 0;

    while (!Q.empty()){
        int v = Q.front();
        Q.pop();
        if (print_order) printf("%d ", v);

        queue<int> temp = adjLists[v];
        while (!temp.empty()){
            int adj = temp.front();
            temp.pop();
            if (DIST[adj] == -1){
                Q.push(adj);
                DIST[adj] = DIST[v] + 1;
            }
        }
    }
}

void dfs_distance_matrix(int v, int d, int size, int **G, int *DIST){
    DIST[v] = d;
    for (int i = 0; i < size; i++){
        if (G[v][i] == 1 && (DIST[i] == -1 || DIST[i] > d + 1)){
            dfs_distance_matrix(i, d + 1, size, G, DIST);
        }
    }
}

void dfs_distance_lists(int v, int d, queue<int>* adjLists, int *DIST){
    DIST[v] = d;
    queue<int> temp = adjLists[v];
    while (!temp.empty()){
        int adj = temp.front();
        temp.pop();
        if (DIST[adj] == -1 || DIST[adj] > d + 1){
            dfs_distance_lists(adj, d + 1, adjLists, DIST);
        }
    }
}

void reset_dist(int *DIST, int size) {
    for (int i = 0; i < size; i++) DIST[i] = -1;
}

void print_dist(int *DIST, int size, int start) {
    printf("Расстояния от вершины %d:\n", start);
    for (int i = 0; i < size; i++) {
        if (DIST[i] == -1) printf("  до v%d: недостижима\n", i);
        else               printf("  до v%d: %d\n", i, DIST[i]);
    }
}

int main()
{
    setlocale(LC_ALL, "Rus");
    srand(time(NULL));
    int size, edge_percent, start;

    printf("Введите количество вершин графа: ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("Некорректное количество вершин.");
        return 1;
    }

    printf("Введите вероятность появления ребра в процентах (0-100): ");
    if (scanf("%d", &edge_percent) != 1 || edge_percent < 0 || edge_percent > 100){
        printf("Некорректная вероятность.");
        return 1;
    }

    int **G = generate_adjacency_matrix(size, edge_percent);
    queue<int>* adjLists = build_lists(size, G);

    printf("\nМатрица смежности G (%d вершин, вероятность ребра - %d%%):\n", size, edge_percent);
    print_matrix(G, size);

    printf("\nСписки смежности:\n");
    print_lists(adjLists, size);

    printf("\nВведите начальную вершину (от 0 до %d): ", size - 1);
    if (scanf("%d", &start) != 1 || start < 0 || start >= size) {
        printf("Некорректная начальная вершина.");
        free_lists(adjLists, size);
        free_matrix(G, size);
        return 1;
    }

    int *DIST = (int*)malloc(size * sizeof(int));
    clock_t t1, t2;
    double time_bfs, time_dfs;

    printf("\nОбход в ширину (BFS), граф задан МАТРИЦЕЙ смежности\n");
    reset_dist(DIST, size);
    printf("Порядок обхода вершин: ");
    t1 = clock();
    bfs_distance_matrix(start, size, G, DIST, true);
    t2 = clock();
    time_bfs = (double)(t2 - t1) / CLOCKS_PER_SEC;
    printf("\n");
    print_dist(DIST, size, start);
    printf("Время: %.6f сек.\n", time_bfs);

    printf("\nОбход в ширину (BFS), граф задан СПИСКАМИ смежности\n");
    reset_dist(DIST, size);
    printf("Порядок обхода вершин: ");
    t1 = clock();
    bfs_distance_lists(start, size, adjLists, DIST, true);
    t2 = clock();
    time_bfs = (double)(t2 - t1) / CLOCKS_PER_SEC;
    printf("\n");
    print_dist(DIST, size, start);
    printf("Время: %.6f сек.\n", time_bfs);

    printf("\nОбход в глубину (DFS), граф задан МАТРИЦЕЙ смежности\n");
    reset_dist(DIST, size);
    t1 = clock();
    dfs_distance_matrix(start, 0, size, G, DIST);
    t2 = clock();
    time_dfs = (double)(t2 - t1) / CLOCKS_PER_SEC;
    print_dist(DIST, size, start);
    printf("Время: %.6f сек.\n", time_dfs);

    printf("\nОбход в глубину (DFS), граф задан СПИСКАМИ смежности\n");
    reset_dist(DIST, size);
    t1 = clock();
    dfs_distance_lists(start, 0, adjLists, DIST);
    t2 = clock();
    time_dfs = (double)(t2 - t1) / CLOCKS_PER_SEC;
    print_dist(DIST, size, start);
    printf("Время: %.6f сек.\n", time_dfs);

    free(DIST);
    free_lists(adjLists, size);
    free_matrix(G, size);

    return 0;
}
