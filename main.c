//
//  main.c
//  EP1_IAC
//
//  Created by Lucas Bordinhon Capalbo on 27/08/17.
//  Copyright © 2017 Lucas Bordinhon Capalbo. All rights reserved.
//

#include <stdio.h>
#include "afd_util.c"
#include<math.h>

int ** matrizEquivalencia;

void imprime (tAFD * t){
    int i = 0;
    int j = 0;
    for (i = 0; i< t->n; i++){
        for (j = 0; j< t->s; j++) {
            printf("%i ", t->Delta[i][j]);
        }
        printf("\n");
    }
}

void buscaProfundidade(tAFD *afd, int i , int * visited , int * inacessivel){ //FAZ ESSE, OU POR LARGURA, VC QUEM ESCOLHE.
    inacessivel[i] = 0;
    int j;
    visited[i]=1;
    for(j=0;j<afd->s;j++)
        if(afd->Delta[i][j] != -1 && visited[afd->Delta[i][j]] == 0)
            buscaProfundidade(afd, afd ->Delta[i][j], visited , inacessivel);
}


void estadosInacessiveis(tAFD* afd , int q0 , int * inacessivel) {
    int i = 0;
    for(i=0;i<afd->n; i++)
        inacessivel[i] = 1; //seta tudo como 1 pra depois rodar dfs
    int visited[afd->n];
    for(i = 0 ; i < afd->n ; i++)
        visited[i] = 0;
    buscaProfundidade(afd,q0, visited , inacessivel);
}

void inicializaInacessivel(int * inacessivel, int tamanho){
    int i ;
    for(i = 0; i < tamanho ; i++)
        inacessivel[i] = 1;
}

void estadosInuteis(tAFD * afd , int * inutil) {
    int i , j;
    inicializaInacessivel(inutil,afd->n);
    int  inacessivel[afd->n];
    for (i = 0; i< afd->n; i++){
        estadosInacessiveis(afd , i , inacessivel);
        for (j = 0; j< afd->n; j++) {
            if(inacessivel[j] == 0 && afd->F[j] == 1){
                //verifica se algum estado que chegou foi final. se foi td bem.
                //chegou em final, entao n eh inutil.
                inutil[i] = 0; //nao eh mais inutil.
            }
        }
        inicializaInacessivel(inacessivel, afd->n);
    }
}

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

void removeEstados(tAFD*afd, int estado){
    int i =0;
    int j=0;
    int z = 0;
    int jaEntrou = 0;
    int ** novaMatriz = (int**) calloc(afd->n - 1, sizeof(int*));
    for (z=0;z<afd->n; z++)
        novaMatriz [z]= (int*) calloc(afd->n - 1, sizeof(int));
    
    z=0;
    for(i= 0;i< afd->n;i++){
        for(j= 0;j< afd->s;j++){
            if(afd->Delta[i][j] == estado)
                afd->Delta[i][j] = -1; //não vai mais pra ele.
            if(afd->Delta[i][j] > estado) //se for maior faz voltar uma posição para não perder o número de estados.
                afd->Delta[i][j]--;
            if(i != estado)
                novaMatriz[z][j] = afd->Delta[i][j]; //atribui antiga matriz a nova
        }
        z++;
        if (z - 1 == estado && jaEntrou == 0){
            z--; //removo um pra voltar e cobrir a linha que foi removida
            jaEntrou = 1;
        }
    }
    afd->Delta = novaMatriz;
}

void trocaEstados(tAFD*afd, int estadoDestino , int estadoAntigo){
    int i =0;
    int j=0;
    int z = 0;
    int jaEntrou = 0;
    
    int ** novaMatriz = (int**) calloc(afd->n - 1, sizeof(int*));
    for (z=0;z<afd->n; z++)
        novaMatriz [z]= (int*) calloc(afd->n - 1, sizeof(int));
    
    z = 0;
    for(i= 0;i< afd->n;i++){
        for(j= 0;j< afd->s;j++){
            if(afd->Delta[i][j] == estadoAntigo)
                afd->Delta[i][j] = estadoDestino; //não vai mais pra ele, vai para o meu destino
            if(afd->Delta[i][j] > estadoAntigo) //se for maior faz voltar uma posição para não perder o número de estados.
                afd->Delta[i][j]--;
            if(i != estadoAntigo){
                novaMatriz[z][j] = afd->Delta[i][j]; //atribui matriz antiga a nova
            }
        }
        z++;
        if (z - 1 == estadoAntigo && jaEntrou == 0){
            z--; //removo um pra voltar e cobrir a linha que foi removida
            jaEntrou = 1;
        }
    }
    afd->n--;
    afd->Delta = novaMatriz;
}

void removeDoVetor(int* vetor , int tamanho, int estado){
    //remove do vetor o estado que quer.
    int i2 = 0;
    int z2 = 0;
    int *novoVetor = (int*) calloc(tamanho-1, sizeof(int));
    for (i2 =0; i2<tamanho + 1; i2++) {
        if(i2 != estado){
            novoVetor[z2] = vetor[i2];
            z2++;
        }
    }
    vetor = novoVetor;
}

void removeDoF(tAFD * afd,int tamanho, int estado){
    //remove do vetor o estado que quer.
    int i2 = 0;
    int z2 = 0;
    int *novoVetor = (int*) calloc(tamanho-1, sizeof(int));
    for (i2 =0; i2<tamanho + 1; i2++) {
        if(i2 != estado){
            novoVetor[z2] = afd->F[i2];
            z2++;
        }
    }
    afd->F = novoVetor;
}
void removeInuteis(tAFD* afd){
    int i = 0;
    int j = 0;
    int novoN = afd->n;
    int jaEntrou = 0;
    for(i= 0;i< afd->n;i++){
        if(afd->inutil[i] == 1){ //  eh inutil, posso remover.
            novoN--;
            if(jaEntrou == 1)
                removeEstados(afd,i - 1);
            else{
                removeEstados(afd,i);
                jaEntrou = 1;
            }
            afd->n= novoN;
            removeDoF(afd, novoN+ 1, i); //remove do vetor de estados
            removeDoVetor(afd->inutil, novoN+ 1, i); //remove do vetor de inuteis, pois sumiu do automato já.
        }
    }
}

void removeInacessiveis(tAFD* afd){
    int i = 0;
    int novoN = afd->n;
    int jaEntrou = 0;
    for(i= 0;i< afd->n;i++){
        if(afd->inacessivel[i] == 1){ //  eh inutil, posso remover.
            novoN--;
            if(jaEntrou == 1)
                removeEstados(afd,i - 1);
            else{
                removeEstados(afd,i);
                jaEntrou = 1;
            }
            removeDoF(afd, novoN+ 1, i); //remove do vetor de estados, pois sumiu do automato.
            removeDoVetor(afd->inacessivel, novoN+ 1, i);
        }
    }
    afd->n= novoN;
}
void inicializaF(tAFD *afd , tAFD *antigo , int * equivalente){
    //pega o equivalente e verifica o estado dele no original.
    int i;
    for (i =0; i< afd->n; i++) {
        afd->F[i] = antigo->F[equivalente[i]];
    }
}
void inicializaDeltaMin(tAFD *antigo, int* representante){
    //quando o representante for igual, retira os vertices mais pra frente de mesma categoria
    // e depois coloca as arestas que chegam e saem dele pro igual.
    int i, j ,z;
    int copiaRep[antigo->n];
    for (i = 0; i< antigo->n; i++)
        copiaRep[i] = representante[i];
    
    for (i = 0;i < antigo->n ; i++) {
        for ( j = i+1; j < antigo->n; j++) { //nao preciso olhar para traz de i
            if(representante[i] == representante[j]){
                //MESMA CATEGORIA.
                trocaEstados(antigo, i, j); //remove J e aponta quem apontava para ele para mim.
                // removeDoVetor(representante, antigo->n + 1, j); // remove J dos representantes para que nao procure repetido sob ele.
                int i2 = 0;
                int z2 = 0;
                int *novoVetor = (int*) calloc(antigo->n -1, sizeof(int));
                for (i2 =0; i2<antigo->n + 1; i2++) {
                    if(i2 != j){
                        novoVetor[z2] = representante[i2];
                        // printf("%i | %i \n", novoVetor[z2],representante[i2]);
                        z2++;
                    }
                }
                representante = novoVetor;
                j--;
            }
        }
    }
    for (i = 0; i < antigo->n; i++) {
        for (j = 0; j < antigo->s; j++) {
            if(antigo->Delta[i][j] > antigo->n){//troca pelo seu representante.
                antigo->Delta[i][j] = copiaRep[antigo->Delta[i][j]];
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    tAFD t;
    if(LeAFDTXT(argv[1], &t) == 1){
        estadosInacessiveis(&t, t.q0 , t.inacessivel);
        removeInacessiveis(&t);
        estadosInuteis(&t, t.inutil);
        removeInuteis(&t);
        matrizEquivalencia =  identificaIdenticos(&t);
        int * representante = verificaRepresentante(&t);
        inicializaDeltaMin(&t,representante);
        EscreveAFDTXT(argv[2], &t);
        /*
         1 -busca estados inacessiveis (busca em largura ou profundidade) -- feito.
         2 - remove estados inuteis.
         3 - identifica automatos identicos{ -- FEITO
         - divide em subconjuntos ( aceitacao e nao aceitacao)
         - verificar equivalencia dentro de cada conjunto{
         PREENCHER MATRIZ BINARIA
         inicializa a matriz da seguinte forma: se linha e coluna forem finais  ou iniciais, marca com 1
         se nao marca com 0.
         Para cada espaco 1 na matriz, verifica se a linha e a coluna possuem os mesmo simbolos definidos, deixa com 1. se nao, coloca 0.
         verifica se as transicoes chegam em estados equivalentes.(se uma chega num estado final e a outra nao por exemplo).
         }
         }
         4- identifica classe de equivalencia { -- FEITO
         vetor, onde i é o representante da classe que ele pertence.
         inicializa vetor com -1.
         Criar contador, incrementa enquanto houver alguma posicao com classe -1.
         REP[i] = CONTADOR - 1;
         se houver algum estado equivalente ao i ( soh olhar na matriz binaria, e se tiver 1 tem), coloca mesmo Contador para ele.
         }
         5 - cria o automato minimo usando vetor de representante e a matriz de equivalencia (vou perguntar ao prof pois nao entendi direito)
         */
    }
    return 0;
}
