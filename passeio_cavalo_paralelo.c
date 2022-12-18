#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void print_tabuleiro(int **tabuleiro, int N, int M){
    int i, j;
    for (i=0; i < N; i++){
        for (j=0; j < M; j++)
            printf("%3d ",tabuleiro[i][j]);
        printf("\n");
    }
}

int jogada_valida(int x, int y, int **tabuleiro, int N, int M){
    if (x < 0  || x >= N || y < 0 || y >= M)
        return 0;
    if(tabuleiro[x][y] != 0)
        return 0;
    return 1;
}

/* Possiveis movimentos
#1
x2 = x + 1;
y2 = y + 2;

#2
x2 = x + 1;
y2 = y - 2;

#3
x2 = x - 1;
y2 = y + 2;

#4
x2 = x - 1;
y2 = y - 2;

#5
x2 = x + 2;
y2 = y + 1;

#6
x2 = x + 2;
y2 = y - 1;

#7
x2 = x - 2;
y2 = y + 1;

#8
x2 = x - 2;
y2 = y - 1;
*/
int proximo_movimento_y(int y, int movimento){
    int valor = 1;
    if( movimento < 5 )
        valor = 2;
    if (movimento % 2 == 0) // se par, eh uma subtracao
        return y - valor;
    else
        return y + valor;
    
}
int proximo_movimento_x(int x, int movimento){
    if (movimento < 3)
        return x + 1;
    else if (movimento < 5)
        return x - 1;
    else if (movimento < 7)
        return x + 2;
    else 
        return x - 2;
}
int passeio_cavalo(int **tabuleiro, int x, int y, int jogada, int N, int M){
    
    int x2[9], y2[9], teste;

    if (jogada == N*M)
        return 1;

    for (int i=1;i<9;i++){
        x2[i-1] = proximo_movimento_x(x,i);
        y2[i-1] = proximo_movimento_y(y,i);
    }

    #pragma omp paralllel for
    for (int i=1; i<9;i++) {
        if (jogada_valida(x2[i-1],y2[i-1], tabuleiro, N, M)){
            tabuleiro[x2[i-1]][y2[i-1]] = jogada+1;
            #pragma omp task shared(teste)
            teste = passeio_cavalo(tabuleiro, x2[i-1],y2[i-1], jogada+1, N, M);
            #pragma omp taskwait
            if (teste == 1)
                return teste;
            tabuleiro[x2[i-1]][y2[i-1]] = 0;
        }
    }

    return 0;
}

int main(int argc, char *argv[]){
    int i, j, aux;
    int N, M;
    int **tabuleiro;
    int x_inicio = 0, y_inicio = 0;
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    scanf("%d", &N);
    scanf("%d", &M);
    printf("Resolvendo para N=%d e M=%d\n", N, M);

    tabuleiro = calloc(N, sizeof(int*));
    tabuleiro[0] = calloc(N*M, sizeof(int));
    for (i = 1; i < N; i++)
        tabuleiro[i] = tabuleiro[0] + i * M;

    tabuleiro[x_inicio][y_inicio] = 1;

    // #pragma omp parallel shared(aux)
    // {
    //     #pragma omp single
        aux = passeio_cavalo(tabuleiro, x_inicio, y_inicio, 1, N, M);
    // }

    if (aux)
        print_tabuleiro(tabuleiro, N, M);
    else
        printf("Nao existe solucao\n");
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f seconds\n",cpu_time_used);

    free(tabuleiro[0]);
    free(tabuleiro);
}