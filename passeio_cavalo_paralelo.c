#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

clock_t start, end;

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

void fim_do_passeio(int **tabuleiro) {

    clock_t end = clock();
    printf("%f seconds\n",((double) (end - start)) / CLOCKS_PER_SEC);

    free(tabuleiro[0]);
    free(tabuleiro);

    exit(0);
}

int passeio_cavalo(int **tabuleiro, int x, int y, int jogada, int N, int M){
    
    int x2, y2, aux = 0;

    if (jogada == N*M) {
        return 1;
    }
    #pragma omp taskloop shared(aux)
    for (int i=1; i<9; i++) {
        x2 = proximo_movimento_x(x,i);
        y2 = proximo_movimento_y(y,i);
        if (jogada_valida(x2,y2, tabuleiro, N, M)) {
            tabuleiro[x2][y2] = jogada+1;
            aux = passeio_cavalo(tabuleiro, x2, y2, jogada+1, N, M);
            if (aux) {
                print_tabuleiro(tabuleiro, N, M);
                fim_do_passeio(tabuleiro);
            }
            tabuleiro[x2][y2] = 0;
        }
    }
    return aux;
}

int main(int argc, char *argv[]){
    int i, j;
    int N, M, jogada = 1;
    int **tabuleiro;
    int x_inicio = 0, y_inicio = 0; 
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

    if (!passeio_cavalo(tabuleiro, x_inicio, y_inicio, jogada, N, M)) {
        printf("Nao existe solucao\n");
        fim_do_passeio(tabuleiro);
    }
}