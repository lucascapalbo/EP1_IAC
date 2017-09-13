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

void inicializaInacessiveis(tAFD * afd) {
    int i;
    for(i=0;i<afd->n; i++)
        afd->inacessiveis[i] = 1; //setta tudo como inacessiveis pra depois rodar a dfs
}

void imprime (tAFD * t){
    int i = 0;
    int j = 0;
    for (i = 0; i< t->n; i++){
        for (j = 0; j< t->s; j++) {
            printf("Delta[%i][%i]: %i", i, j, t->Delta[i][j]);
        }
        printf("\n");
    }
}

void buscaProfundidade(tAFD *afd, int i , int * visited){ //FAZ ESSE, OU POR LARGURA, VC QUEM ESCOLHE.
    int** matriz = afd->Delta;
    afd->inacessiveis[i] = 0;
    int j;
    printf("\n'i' da busca em prof: %i",i);
    visited[i]=1;
    int k;
    for(j=0;j<afd->s;j++)
       if(afd->Delta[i][j] != -1 && visited[afd->Delta[i][j]] == 0){ // TA CRASHANDO NESSA EXATA LINHA existe a seta e n foi visitado ainda.
            buscaProfundidade(afd, afd ->Delta[i][j], visited);
       } else printf("DELTA NAO É UMa cara\n");
}
//delta é linha: estado. coluna: símbolo. ou seja, [i][j] mostra pra onde vai com tal símbolo.
// oque acha de fazer função que vai percorrer delta, e fazer uma nova matriz pra fazer um grafo mesmo? e nao esse grafo bugado que é o automato.
// dai da pra usar o busca em profundidade da net.

void estadosInacessiveis(tAFD* afd) {
    inicializaInacessiveis(afd);
    int visited[afd->n];
    int i = 0;
    for(i = 0 ; i < afd->n ; i++)
        visited[i] = 0;
    buscaProfundidade(afd, afd->q0, visited);
        printf("\nN VALE: %i\n",afd->n);
    int k;
    for(k=0; k<afd->n; k++)
        printf("\n%i - da posicao[%i]\n ", afd->inacessiveis[k], k);
}

void estadosInuteis() {
    //ve os slides pra entender
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
    // ################# SOH IMPRIME A MATRIZ PARA DEBUGAR #################################
    for (i = 0; i< afd->n; i++) {
        for (j = 0; j < afd->s; j++) {
            printf("%i ", afd->Delta[i][j]);
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
int verificaNovosEstados(tAFD *afd , int * representante){
    int novoN = afd->n;
    int i , j;
    for (i =0; i < afd->n; i++) {
        if(afd->inacessiveis[i] == 1){
            novoN--;
        }
    }
    for (i = 0; i < afd->n; i++) {
        for (j = i + 1; j < afd->n; j++) {
            if(representante[j] == representante[i]){
                //sao da mesma classe, portanto vou tirar
                novoN--;
            }
        }
    }
    return novoN;
}
void inicializaF(tAFD *afd , tAFD *antigo , int * equivalente){
    //pega o equivalente e verifica o estado dele no original.
    int i;
    for (i =0; i< afd->n; i++) {
        afd->F[i] = antigo->F[equivalente[i]];
    }
}
void inicializaDeltaMin(tAFD* afd, tAFD *antigo, int* representante){
    //quando o representante for igual, retira os vertices mais pra frente de mesma categoria
    // e depois coloca as arestas que chegam e saem dele pro igual.
    int  equivalente [afd->n]; // mostra o equivalente no automato original
    int i, j , z;
    for (i = 0; i< afd->n; i++)
        equivalente[i] =-1;
    int k = 0;
    int k2 = 0;
    for (i = 0;i < antigo->n ; i++) {
        for ( j = i + 1; j < antigo->n || i == antigo-> n-1; j++) { //nao preciso olhar para traz de i
            if(i == antigo -> n-1 ){
                equivalente[k2] = i ;
                i++;
            }else{
                if(representante[i] == representante[j]){
                    //MESMA CATEGORIA.
                    equivalente[k2] = i;
                    k2++;
                    for (z = 0; z < antigo->n; z++) {
                        for (k = 0; k <antigo ->s; k++) {
                            if(antigo->Delta[z][k] == j ){
                                antigo->Delta[z][k] = i;
                                //se apontava para j, agora aponta para i.
                            }
                        }
                    }
                }else if((j == antigo->n - 1 && equivalente[k2-1] != i )|| (i == 0 && j == antigo->n - 1)){
                    //categoria diferente.
                    equivalente[k2] = i;
                    k2++;
                    for (int i1 = 0; i1 < afd->n; i1++) {
                        printf("-- \n");
                        printf("%i", equivalente[i1]);
                    }
                }
            }
        }
    }
    inicializaF(afd, antigo, equivalente);
    //pega o estado equivalente, que vai ficar no automato, e fica com ele. copia a matriz dele.
    for (i = 0; i < afd->n; i++) {
        printf("%i", equivalente[i]);
    }

    for(i = 0; i < afd->n ; i++){
        for (j = 0; j < afd->s; j++) {
            afd->Delta[i][j] = antigo->Delta[equivalente[i]][j];

            printf("\n");
            for (z = 0; z< afd->n; z++) {
                for (k = 0; k < afd->s; k++) {
                    printf("%i ", afd->Delta[z][k]);
                }
                printf("\n");
            }
        }
    }
    //atribui estado inicial, equivalente ao antigo.
    for (i = 0; i < afd->n; i++) {
        if(equivalente[i] == antigo->q0){
            afd->q0 = i;
        }
    }
    /*######################### DEBUG #######################
     PRINTA VETOR EQUIVALENTE
     for (i = 0; i< afd->n; i++)
     printf("%i", equivalente[i]);

     ######################### DEBUG ####################### */

}
void criaAutMin(tAFD *afd , int * representante , tAFD *antigo){
    //ja esta tudo inicializado, basta preencher delta, estados F de aceitacao e inicial. ignorando os inacessiveis, inuteis e equivalentes.
    inicializaDeltaMin(afd, antigo, representante);
}
int main(int argc, const char * argv[]) {
    // insert code here...
   tAFD t;
    if(LeAFDTXT("/Users/Hozer/Desktop/ITC/EP1_IAC/grafo.txt", &t) == 1){
        // imprime(&t);
        estadosInacessiveis(&t);
        // matrizEquivalencia =  identificaIdenticos(&t);
        // int * representante = verificaRepresentante(&t);
        // tAFD minimo;
        // int nEstadosMin = 0;
        // nEstadosMin =  verificaNovosEstados(&t,representante);
        // InicializaAFD(&minimo, nEstadosMin, t.s);
        // criaAutMin(&minimo,representante, &t);
        // EscreveAFDJFF(argv[2], &minimo);
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

         ######################### DEBUG #######################
         int i = 0;
         for (i = 0; i < t.n; i++){
         if(t.inacessivel[i] == 1)
         printf("Estado %i eh inacessivel \n",(i+1));
         }

         -------------------
         PRINTA REPRESENTANTE
         printf("\n");
         int i = 0;
         for (i = 0 ; i < t.n; i++) {
         printf("%i ",representante[i]);
         }

         PRINTA NUMERO ESTADOS MINIMOS

         printf("%i",nEstadosMin );

         ######################### DEBUG #######################
         */
        //
    }
    return 0;
}
