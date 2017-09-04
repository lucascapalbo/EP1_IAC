//
//  main.c
//  EP1_IAC
//
//  Created by Lucas Bordinhon Capalbo on 27/08/17.
//  Copyright © 2017 Lucas Bordinhon Capalbo. All rights reserved.
//

#include <stdio.h>
#include "afd_util.c"


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
int main(int argc, const char * argv[]) {
    // insert code here...
    tAFD t;
    if(LeAFDTXT("/Users/lucasbordinhoncapalbo/Documents/EP1_IAC/EP1_IAC/grafo.txt", &t) == 1){
        // imprime(&t);
        estadosInacessiveis(&t, t.q0);
        int i = 0;
        for (i = 0; i < t.n; i++){
            if(t.inacessivel[i] == 1)
                printf("Estado %i eh inacessivel \n",(i+1));
        }
        /*
         1 -busca estados inacessiveis (busca em largura ou profundidade) -- feito.
         2 - remove estados inuteis.
         3 - identifica automatos identicos{
         - divide em subconjuntos ( aceitacao e nao aceitacao)
         - verificar equivalencia dentro de cada conjunto{
         PREENCHER MATRIZ BINARIA
         inicializa a matriz da seguinte forma: se linha e coluna forem finais  ou iniciais, marca com 1
         se nao marca com 0.
         Para cada espaco 1 na matriz, verifica se a linha e a coluna possuem os mesmo simbolos definidos, deixa com 1. se nao, coloca 0.
         verifica se as transicoes chegam em estados equivalentes.(se uma chega num estado final e a outra nao por exemplo).
         }
         }
         4- identifica classe de equivalencia {
         vetor, onde i é o representante da classe que ele pertence.
         inicializa vetor com -1.
         Criar contador, incrementa enquanto houver alguma posicao com classe -1.
         REP[i] = CONTADOR - 1;
         se houver algum estado equivalente ao i ( soh olhar na matriz binaria, e se tiver 1 tem), coloca mesmo Contador para ele.
         }
         5 - cria o automato minimo usando vetor de representante e a matriz de equivalencia (vou perguntar ao prof pois nao entendi direito)
         */
        //  EscreveAFDJFF("/Users/lucasbordinhoncapalbo/Documents/EP1_IAC/EP1_IAC/afd7.jff", &t);
    }
    return 0;
}
