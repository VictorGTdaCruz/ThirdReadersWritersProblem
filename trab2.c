//
//  trab2.c
//  
//
//  Created by Victor Cruz on 21/11/19.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>

dispatch_semaphore_t acesso, inan; //semaforos
pthread_mutex_t l_mutex; // locks
int l=0, buffer = -1, NTHREADS, NLeitores, NEscritores, NLeituras, NEscritas; //globais
FILE *arquivoDeLog;

void *Leitor (void *arg) {
    int idThread = *(int *) arg;
    
    FILE *arquivo;
    int tamanho = snprintf(NULL, 0,"%d.txt", idThread);
    char nome[tamanho + 1];
    sprintf(nome, "%d.txt", idThread);
    arquivo = fopen(nome, "w");
    
    int leituras = NLeituras;
    while(leituras > 0) {
        dispatch_semaphore_wait(inan, DISPATCH_TIME_FOREVER);
        pthread_mutex_lock(&l_mutex); l++;
        if(l==1) {
            dispatch_semaphore_wait(acesso, DISPATCH_TIME_FOREVER);
            fprintf(arquivoDeLog, "pedeAcessoParaLer(%d)\n", idThread);
        }
        dispatch_semaphore_signal(inan);
        pthread_mutex_unlock(&l_mutex);
        
        fprintf(arquivoDeLog, "le(%d)\n", buffer);
        fprintf(arquivo, "%d\n", buffer);
        //le...
        printf("Leitor %d leu\n", idThread);
        fprintf(arquivoDeLog, "paraLeitura(%d)\n", idThread);
        
        pthread_mutex_lock(&l_mutex); l--;
        if(l==0) {
            fprintf(arquivoDeLog, "liberaAcessoDaLeitora(%d)\n", idThread);
            dispatch_semaphore_signal(acesso);
        }
        pthread_mutex_unlock(&l_mutex);
        
        leituras--;
    }
    
    fclose(arquivo);
    free(arg);
    pthread_exit(NULL);
}

void *Escritor (void *arg) {
    int idThread = *(int *) arg;
    
    int escritas = NEscritas;
    while(escritas > 0) {
        dispatch_semaphore_wait(inan, DISPATCH_TIME_FOREVER);
        dispatch_semaphore_wait(acesso, DISPATCH_TIME_FOREVER);
        dispatch_semaphore_signal(inan);

        fprintf(arquivoDeLog, "pedeAcessoParaEscrever(%d)\n", idThread);
        fprintf(arquivoDeLog, "escreve(%d)\n", idThread);
        buffer = idThread;
        //escreve...
        printf("Escritor %d escreveu\n", idThread);
        fprintf(arquivoDeLog, "paraEscrita(%d)\n", idThread);
        fprintf(arquivoDeLog, "liberaAcessoDaEscritora(%d)\n", idThread);
        
        dispatch_semaphore_signal(acesso);
        escritas--;
    }
    
    free(arg);
    pthread_exit(NULL);
}

void criaThreads(int quantidade, void *(*start_routine) (void *), pthread_t *threads, int valorInicialDoId) {
    int i, *arg;
    for(i = valorInicialDoId; i < valorInicialDoId + quantidade; i++) {
        arg = malloc(sizeof(int));
        if (arg == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        *arg = i;
        
        if (pthread_create(&threads[i], NULL, start_routine, (void*) arg)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
        
        if (start_routine == Escritor)
            fprintf(arquivoDeLog, "criaEscritor()\n");
        else
            fprintf(arquivoDeLog, "criaLeitor()\n");
    }
}

/* Funcao principal */
int main(int argc, char *argv[]) {
    int i;
    
    //valida e recebe os valores de entrada
    if(argc < 6) {
        printf("Use: %s <numero de leitores> <numero de escritores> <numero de leituras> <numero de escritas> <nome do arquivo de log>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    NLeitores = atoi(argv[1]);
    NEscritores = atoi(argv[2]);
    NLeituras = atoi(argv[3]);
    NEscritas = atoi(argv[4]);
    
    NTHREADS = NLeitores + NEscritores;
    pthread_t threads[NTHREADS];
    
    arquivoDeLog = fopen(argv[5], "w");

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&l_mutex, NULL);
    acesso = dispatch_semaphore_create(1);
    inan = dispatch_semaphore_create(1);
    
    /* Cria as threads */
    criaThreads(NLeitores, Leitor, threads, 0);
    criaThreads(NEscritores, Escritor, threads, NLeitores);
    
    fprintf(arquivoDeLog, "registraNumeroEsperadoDeLeituras(%d)\n", NLeituras);
    fprintf(arquivoDeLog, "registraNumeroEsperadoDeEscritas(%d)\n", NEscritas);
    
    /* Espera todas as threads completarem */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf ("\nFIM\n");
    
    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&l_mutex);
    dispatch_release(acesso);
    dispatch_release(inan);
    
    fclose(arquivoDeLog);
    
    pthread_exit (NULL);
}
