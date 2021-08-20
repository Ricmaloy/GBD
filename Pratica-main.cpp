/*
 * File:   Pratica-main.cpp
 * Author: Leandro Fontellas Laurito
 * Matricula: 11721BCC045
 * Author: Ricardo
 * Matricula: xxxxxxxxxxx
 * Author: Kaue
 * Matricula: xxxxxxxxxxx
 */

 /*
    [x] R01 - Reservar no mínimo sizeof(int)+1 para cada palavra
    [x] R02 - Crie um registro de cabeçalho contendo 2 inteiros:
        -> o número de registros e o offset para o primeiro disponível
    [x] R03 - no início de cada registro coloque o tamanho do registro
    [-] R04 - permita a remoção de palavras, com a gravação de um * após o
        tamanho do registro
    [-] R05 - permita a inclusão de novas palavras, considerando os espaços
        disponíveis de registros removidos:
        -> caso a lista de removidos seja vazia, inserir no final
    [-] R06 - manter a lista de removidos, gravando o offset parao próximo
        disponível
    [-] R07 - implementar um procedimento ”atualizaCabeçalho”, e chame-o a
        cada inserção/remoção para atualizar o número de registros do
        arquivo
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class MeuArquivo {
public:
    struct cabecalho { int quantidade; int disponivel; } cabecalho;
    // struct registro { char palavra[50]; int tamanho; bool deletado; } registro;

    // construtor: abre arquivo. Essa aplicacao deveria ler o arquivo se existente ou criar um novo.
    // Entretando recriaremos o arquivo a cada execucao ("w+").
    MeuArquivo() {
        filePonteiro = fopen("dados.dat","w+");

        // Ao criar o arquivo temos que criar o cabeçalho dele também
        cabecalho.quantidade = 0;
        cabecalho.disponivel = -1;

        // Escreve o endereço da variavel cabeçalho na memoria RAM
        if(fwrite(&cabecalho,sizeof(cabecalho),1,filePonteiro)<1){
            printf("Não foi possivel escrever o cabeçalho");
        }
    }

    // Destrutor: fecha arquivo
    ~MeuArquivo() {
        fclose(filePonteiro);
    }

    // Insere uma nova palavra, consulta se há espaco disponível ou se deve inserir no final
    void inserePalavra(char *palavra) {
        substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0

        // Pega quantidade do arquivo e atualiza
        atualizaCabecalho(true);

        /*
            Existe três casos agora, não ter nenhum registro removido então só ir
            para o final do arquivo e adicionar o proximo registro.
            Pode também ter algum(ns) registro(s) removido(s) então tem que encon
            tra-los e verificar se é possivel colocar o novo registro la se sim
            você coloca e se não der tem que colocar no final do arquivo msm.
        */
        int proximoDeletado = cabecalho.disponivel;
        while(proximoDeletado != -1){
            fseek(filePonteiro, proximoDeletado, SEEK_SET);
            unsigned int tamanho;
            fread(&tamanho, sizeof(int), 1, filePonteiro);
            if(tamanho >= (strlen(palavra)+1)) {
                char deletado = ' ';
                fwrite(&deletado, sizeof(char), 1, filePonteiro);
                fwrite(palavra, tamanho, 1, filePonteiro);
            }
            fseek(filePonteiro, sizeof(char), SEEK_CUR);

            char* resultado;
            fread(resultado, tamanho, 1, filePonteiro);
        }

        // Reestringe o tamanho minimo de uma palavra
        int tamanhoPalavra = strlen(palavra);
        int tamanho = tamanhoPalavra < (int) sizeof(int) + 1? (int) sizeof(int)+1: tamanhoPalavra;

        //printf("%d\n",tamanho);

        // Escreve o tamanho do registro no arquivo
        if(fwrite(&tamanho, sizeof(int), 1, filePonteiro)<1){
            //printf("Não foi possivel escrever o tamanho do registro");
        }

        // Escreve se ele foi deletado ou não com a flag *
        char deletado = ' ';
        fwrite(&deletado, sizeof(char), 1, filePonteiro);

        // Escreve a palavra em si no arquivo
        fwrite(palavra, sizeof(char),tamanho, filePonteiro);

    }

    // Marca registro como removido, atualiza lista de disponíveis, incluindo o cabecalho
    void removePalavra(int offset) {
        // implementar aqui
    }

    // BuscaPalavra: retorno é o offset para o registro
    // Nao deve considerar registro removido
    int buscaPalavra(char *palavra) {
        this->substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0
        fseek(filePonteiro, sizeof(cabecalho) + 1, SEEK_SET);

        while(!feof(filePonteiro)){
            int posicao = ftell(this->filePonteiro);
            char ast; 
            fread(&ast,sizeof(char),1,filePonteiro);
            // printf("%c",&ast);
            if(ast != '*'){
                int tamanhoRegistro;

                fseek(filePonteiro, posicao, SEEK_SET);
                fread(&tamanhoRegistro,sizeof(int),1,filePonteiro);

                char *buffer;
                buffer = (char *) malloc(tamanhoRegistro);
                fread(buffer,sizeof(char),tamanhoRegistro,filePonteiro);
                printf("%s\n",buffer);

                if(strcmp(palavra,buffer) == 0) return posicao; 
            }else{
                int tamanhoRegistro;
                int proximoOffset;

                fread(&proximoOffset,sizeof(int),1,filePonteiro);
                fread(&tamanhoRegistro,sizeof(int),1,filePonteiro);

                int proximoRegistro = posicao + (4 + (tamanhoRegistro - 5) );

                fseek(filePonteiro,  proximoRegistro, SEEK_SET);
            }
        }
        // retornar -1 caso nao encontrar
        return -1;
    }

    void atualizaCabecalho(bool isAdd){
        int posicao = ftell(this->filePonteiro);

        fseek(this->filePonteiro, 0, SEEK_SET);
        fread(&this->cabecalho, sizeof(cabecalho), 1, this->filePonteiro);

        if(isAdd){
            this->cabecalho.quantidade+=1;
            fseek(filePonteiro,0,SEEK_SET);
            fwrite(&cabecalho, sizeof(cabecalho),1, filePonteiro);
        }else{
            this->cabecalho.quantidade-=1;
            fseek(filePonteiro,0,SEEK_SET);
            fwrite(&cabecalho, sizeof(cabecalho),1, filePonteiro);
        }
        fseek(filePonteiro,posicao,SEEK_SET);
    }

private:
    // descritor do arquivo é privado, apenas métodos da classe podem acessa-lo
    FILE *filePonteiro;

    // funcao auxiliar substitui terminador por \0
    void substituiBarraNporBarraZero(char *str) {
        int tam = strlen(str); for (int i = 0; i < tam; i++) if (str[i] == '\n') str[i] = '\0';
    }
};

int main(int argc, char** argv) {
    // abrindo arquivo dicionario.txt
    FILE *f = fopen("dicionario.txt","rt");

    // se não abriu
    if (f == NULL) {
        printf("Erro ao abrir arquivo.\n\n");
        return 0;
    }

    char *palavra = new char[50];

    // criando arquivo de dados
    MeuArquivo *arquivo = new MeuArquivo();
    while (!feof(f)) {
        fgets(palavra,50,f);
        arquivo->inserePalavra(palavra);
    }

    // fechar arquivo dicionario.txt
    fclose(f);

    printf("Arquivo criado.\n\n");

    char opcao;
    do {
        printf("\n\n1-Insere\n2-Remove\n3-Busca\n4-Sair\nOpcao:");
        opcao = getchar();
        if (opcao == '1') {
            printf("Palavra: ");
            scanf("%s",palavra);
            arquivo->inserePalavra(palavra);
        }
        else if (opcao == '2') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0) {
                arquivo->removePalavra(offset);
                printf("Removido.\n\n");
            }
        }
        else if (opcao == '3') {
            printf("Palavra: ");
            scanf("%s",palavra);
            int offset = arquivo->buscaPalavra(palavra);
            if (offset >= 0)
                printf("Encontrou %s na posicao %d\n\n",palavra,offset);
            else
                printf("Nao encontrou %s\n\n",palavra);
        }
        if (opcao != '4') opcao = getchar();
    } while (opcao != '4');

    printf("\n\nAte mais!\n\n");

    return (EXIT_SUCCESS);
}
