//
//  main.c
//  EP1_IAC
//
//  Created by Lucas Bordinhon Capalbo on 27/08/17.
//  Copyright © 2017 Lucas Bordinhon Capalbo. All rights reserved.
//

#include <stdio.h>
#include "afd_util.c"

int ** matrizEquivalencia;

void inicializaInacessiveis(int* inacessiveis, tAFD * afd) {
    int i;
    for(i=0;i<afd->n; i++)
        inacessiveis[i] = 1; //setta tudo como inacessiveis pra depois rodar a dfs
}

void imprime (tAFD * t){
    int i = 0;
    int j = 0;
    for (i = 0; i< t->n; i++){
        for (j = 0; j< t->s; j++) {
            printf("%i",t->Delta[i][j]);
        }
        printf("\n");
    }
}

void buscaProfundidade(tAFD *afd, int i, int* inacessiveis){ //FAZ ESSE, OU POR LARGURA, VC QUEM ESCOLHE.
    int** matriz = afd->Delta;
    int visited[afd->n];
    inacessiveis[i] = 0;

    int j;
    printf("\n%d",i);
    visited[i]=1;

    for(j=0;j<afd->n;j++)
       if(!visited[j]&&afd->Delta[i][j]==1)
            buscaProfundidade(afd, j, inacessiveis);
}

void estadosInacessiveis(int* inacessiveis, tAFD* afd) {
    inicializaInacessiveis(inacessiveis, afd);
    buscaProfundidade(afd, afd->q0, inacessiveis);
    int k;
    for(k=0; k<afd->n; k++)
        printf("%i ", inacessiveis[k]);
    //ve os slides pra entender
}

/*
void estadosInacessiveis(tAFD *afd,int q0)
{
    int visitado[afd->n];
    visitado[q0] = 1; //estado inicial sempre eh visitado.
    int i , j;
    for (i = 0; i < afd->n; i++) {
        for (j = 0; j < afd->s; j++) {
            if(afd->Delta[i][j] != -1){
                visitado[afd->Delta[i][j]] = 1;
            }
        }
    }
    int z = 0;
    for (z = 0; z < afd->n; z++) {
        if(visitado[z] != 1){
            afd->inacessivel[z] = 1;
        }
    }
}
*/

void inicializaMatrizEquivalencia(int ** matrizEquivalencia, int * estados , tAFD * afd){
    int i , j;
    for (i = 0; i< afd->n; i++) {
        for (j = 0; j < afd->n; j++) {
            if(estados[i] == estados[j])
                matrizEquivalencia[i][j] = 1;
        }
    }
}

void verificaSimbolo(int** matrizEquivalencia , tAFD * afd){
    int i , j , z;
    for (i = 0; i< afd->n; i++) {
        for (j = 0; j < afd->n; j++) {
            if(matrizEquivalencia[i][j] == 1){
                for(z = 0; z < afd->s ; z++){
                    if((afd->Delta[i][z] == -1 && afd->Delta[j][z] != -1) ||
                       (afd->Delta[i][z] != -1 && afd->Delta[j][z] == -1))
                        matrizEquivalencia[i][j] = 0;
                }
            }
        }
    }
}

void verificaTransicoes(int ** matrizEquivalencia , tAFD * afd){
    int i , j , z;
    for (i = 0; i< afd->n; i++) {
        for (j = 0; j < afd->n; j++) {
            if(matrizEquivalencia[i][j] == 1){
                for(z = 0; z < afd->s ; z++){
                    if(afd->F[afd->Delta[i][z]] != afd->F[afd->Delta[j][z]])
                        //destinos nao possuem mesmo estado
                        matrizEquivalencia[i][j] = 0;
                }
            }
        }
    }
}
int** identificaIdenticos(tAFD *afd){
    int z = 0;
    int ** matrizEquivalencia = (int**) calloc(afd->n, sizeof(int*));
    for (z=0;z<afd->n; z++)
        matrizEquivalencia [z]= (int*) calloc(afd->n, sizeof(int));
    int i , j;
    for (i = 0; i< afd->n; i++) {
        for (j = 0; j < afd->n; j++) {
            matrizEquivalencia[i][j] = 0;
        }
    }
    inicializaMatrizEquivalencia(matrizEquivalencia, afd->F, afd); //se linha e coluna forem de mesmo estado, coloca 1. Usa F, que contem a categoria (aceitacao ou nao) de cada estado.
    verificaSimbolo(matrizEquivalencia, afd); //verifica se a linha e coluna, de onde possuem 1, estao definidos para os mesmos simbolos. (se nao possuem -1 em algum lugar).
    verificaTransicoes(matrizEquivalencia, afd);//verifica se a linha e coluna, que possuem 1, chegam em estados da mesma categoria.
    // ################# SOH IMPRIME A MATRIZ PARA DEBUGAR #################################
    for (i = 0; i< afd->n; i++) {
        for (j = 0; j < afd->n; j++) {
            printf("%i ", matrizEquivalencia[i][j]);
        }
        printf("\n");
    }
    // ################# SOH IMPRIME A MATRIZ PARA DEBUGAR #################################
    return matrizEquivalencia;
}

int * verificaRepresentante(tAFD *afd){
    int i , j ;
    int * representante = (int*) calloc(afd->n, sizeof(int));
    for (i=0; i<afd->n; i++) representante[i] = -1;
    int contador = 0; //contador que representa a classe.
    //soma 1 a cada vez que passa por um estado. se houver algum estado equivalente coloca mesmo contador.
    for (i=0; i<afd->n; i++){
        if(representante[i] == -1){
            representante[i] = contador;
            for (j = 0; j < afd->n; j++) {
                if (matrizEquivalencia[i][j] == 1) {
                    representante[j] = contador;
                }
            }
            contador++;
        }
    }
    return representante;
}
int main(int argc, const char * argv[]) {
    // insert code here...
    tAFD t;
    int* inacessiveis;
    if(LeAFDTXT("/Users/lucasbordinhoncapalbo/Documents/EP1_IAC/EP1_IAC/grafo.txt", &t) == 1){
        // imprime(&t);
        estadosInacessiveis(inacessiveis, &t);
        matrizEquivalencia =   identificaIdenticos(&t);
        int * representante = verificaRepresentante(&t);
        /*
         ------------------1 -busca estados inacessiveis (busca em largura ou profundidade) -- feito.

         2 - remove estados inuteis.

         ------------------3 - identifica automatos identicos{ -- FEITO
         ------------------- divide em subconjuntos ( aceitacao e nao aceitacao)
         ------------------- verificar equivalencia dentro de cada conjunto{
         ------------------PREENCHER MATRIZ BINARIA
         ------------------inicializa a matriz da seguinte forma: se linha e coluna forem finais  ou iniciais, marca com 1
         ------------------se nao marca com 0.
         ------------------Para cada espaco 1 na matriz, verifica se a linha e a coluna possuem os mesmo simbolos definidos, deixa com 1. se nao, coloca 0.
         ------------------verifica se as transicoes chegam em estados equivalentes.(se uma chega num estado final e a outra nao por exemplo).
         ------------------}
         ------------------}

         4- identifica classe de equivalencia {
         vetor, onde i é o representante da classe que ele pertence.
         inicializa vetor com -1.
         Criar contador, incrementa enquanto houver alguma posicao com classe -1.
         REP[i] = CONTADOR - 1;
         se houver algum estado equivalente ao i ( soh olhar na matriz binaria, e se tiver 1 tem), coloca mesmo Contador para ele.
         }

         5 - cria o automato minimo usando vetor de representante e a matriz de equivalencia (vou perguntar ao prof pois nao entendi direito)

         ######################### DEBUG #######################
         int i = 0;
         for (i = 0; i < t.n; i++){
         if(t.inacessivel[i] == 1)
         printf("Estado %i eh inacessivel \n",(i+1));
         }

         -------------------
         PRINTA REPRESENTANTE

         int i = 0;
         for (i = 0 ; i < t.n; i++) {
         printf("%i ",representante[i]);
         }

         ######################### DEBUG #######################
         */
        //  EscreveAFDJFF("/Users/lucasbordinhoncapalbo/Documents/EP1_IAC/EP1_IAC/afd7.jff", &t);
    }
    return 0;
}
