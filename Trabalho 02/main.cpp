/*
 * File:   main.cpp
 * Created on August 25, 2121
 * Author: Leandro Fontellas Laurito
 * Matricula: 11721BCC045
 * Author: Ricardo
 * Matricula: 11821BCC004
 * Author: Miguel Sanches Rocha
 * Matricula: 11811BCC001
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

using namespace std;

// remove pontuacao de uma palavra
void removePontuacao (char *palavra) {
    int length = strlen(palavra);
    if (
        (palavra[length-1] == '.') || (palavra[length-1] == ',') || (palavra[length-1] == ';') ||
        (palavra[length-1] == ':') || (palavra[length-1] == '?') || (palavra[length-1] == '!')
       )
        palavra[length-1] = '\0';
}

// imprime linha do arquivo com base no offset da palavra
void imprimeLinha(int offset) {
    FILE *f = fopen("biblia.txt","rt");
    int pos = -1;
    char linha[2048];
    while (pos < offset) {
        fgets(linha,2047,f);
        pos = ftell(f);
    }
    printf("%s",linha);
    fclose(f);
}

// classe que implementa a lista invertida
class listaInvertida {
public:
    map<string, int> secondariesIndexes;
    FILE *indexFile;

    // construtor
    listaInvertida() {
        indexFile = fopen("invertedList.txt", "wb+");

        if (indexFile == NULL){
            printf("\n\n Nao consegui abrir arquivo de indices. Sinto muito.\n\n\n\n");

            return;
        }
    }
    // destrutor
    ~listaInvertida() {
        fclose(indexFile);
    }

    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        // cria um buffer para a palavra a ser inserida
        string bufferPalavra(palavra);

        // caso a palavra a ser inserida j� exista
        if(!secondariesIndexes[bufferPalavra]) {
            // pega o offset do indice na lista
            int lastOccurrenceOffset = secondariesIndexes[bufferPalavra];

            // pega a posicao onde o index ser� escrito
            fseek(indexFile, 0, SEEK_END);
            int newOccurrenceOffset = ftell(indexFile);

            // troca o indice da lista pelo o indice da nova ocorrencia
            secondariesIndexes[bufferPalavra] = newOccurrenceOffset;

            // atualiza na lista o offset do indice
            //indexFile << offset << "/" << lastOccurrenceOffset << "/" << endl;
            fwrite(&offset,sizeof(int),1,indexFile);
            fwrite("/",sizeof(char),1,indexFile);
            fwrite(&lastOccurrenceOffset,sizeof(int),1,indexFile);
            fwrite("/",sizeof(char),1,indexFile);
            fwrite("\n",sizeof(char),1,indexFile);
            fflush(indexFile);

        }   // caso a palavra a ser inserida nao exista
        else {
            // pega a posicao onde o index ser� escrito
            fseek(indexFile, 0, SEEK_END);
            int newOccurrenceOffset = ftell(indexFile);

            // insere a posicao no indice secundario
            secondariesIndexes[bufferPalavra] = newOccurrenceOffset;

            // escreve na lista o offset do indice
            //indexFile << offset << '/' << -1 << '/' << endl;
            fwrite(&offset,sizeof(int),1,indexFile);
            fwrite("/",sizeof(char),1,indexFile);
            fwrite("-1",sizeof(int),1,indexFile);
            fwrite("/",sizeof(char),1,indexFile);
            fwrite("\n",sizeof(char),1,indexFile);
            fflush(indexFile);
        }
    }

    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        // cria um buffer para a palavra a ser buscada
        string bufferPalavra(palavra);

        // pega o offset do ultimo indice na lista
        int lastOccurrenceOffset = secondariesIndexes[bufferPalavra];

        list <int> offsetsArray;
        int *offsets;

        // zera a quantidade de
        *quantidade = 0;

        // caso a palavra n�o esteja na lista de indices
        if(lastOccurrenceOffset == 0){
            return offsets;
        }

        // enquanto nao encontra o ultimo registro com indice -1 (ultimo registro da lista)
        while(lastOccurrenceOffset != -1) {
            // pula para a posiçao do proximo registro
            indexFile.seekg(lastOccurrenceOffset);

            string str;
            getline(indexFile, str);

            // pega o valor do offset da palavra e o proximo offset
            string offsetPalavra = str.substr(0, str.find("/"));
            string proximoOffset = str.substr(0, str.find("/"));

            // deixa a string apenas com o texto
            str.erase(0, str.find("/") + 1);

            // atualiza o ultimo offset para o proximo indice
            lastOccurrenceOffset = stoi(proximoOffset);

            // adiciona o offset nas listas de offsets
            offsetsArray.insert(offsetsArray.end(), stoi(offsetPalavra));

            // incrementa a quantidade das palavras
            *quantidade++;
        }

        //Aloca o espaco para a lista no array de offsets
        offsets = (int*)malloc(sizeof(int)*offsetsArray.size());

        //Copia os dados da lista de offsets para o array de offsets
        copy(offsetsArray.begin(),offsetsArray.end(),offsets);

        // Retorna o ponteiro para a posi�ao de inicio
        indexFile.seekg(indexFile.tellg());

        // Retorna a lista com os offsets
        return offsets;
    }
private:
};

// programa principal
int main(int argc, char** argv) {
    // abrir arquivo
    ifstream in("biblia.txt");
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n");
    }
    else{
        // vamos ler o arquivo e criar a lista invertida com as palavras do arquivo
        char *palavra = new char[100];
        int offset, contadorDePalavras = 0;
        listaInvertida lista;
        // ler palavras
        while (!in.eof()) {
            // ler palavra
            in >> palavra;
            // pegar offset
            offset = in.tellg();
            // remover pontuacao
            removePontuacao(palavra);
            // desconsiderar palavras que sao marcadores do arquivo
            if (!((palavra[0] == '#') || (palavra[0] == '[') || ((palavra[0] >= '0') && (palavra[0] <= '9')))) {
                //printf("%d %s\n", offset,palavra); fflush(stdout); // debug :-)
                lista.adiciona(palavra, offset);
                contadorDePalavras++;
                if (contadorDePalavras % 1000 == 0) { printf(".");  fflush(stdout); }
            }
        }
        in.close();

        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade;
                int *offsets = lista.busca(palavra,&quantidade);
                if (quantidade > 0) {
                    for (int i = 0; i < quantidade; i++)
                        imprimeLinha(offsets[i]);
                }
                else
                    printf("nao encontrou %s\n",palavra);
            }
        } while (strcmp(palavra,"SAIR") != 0);

        printf("\n\nAte mais!\n\n");
    }

    return (EXIT_SUCCESS);
}

