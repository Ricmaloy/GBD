/*
 * File:   Pratica-main.cpp
 * Author: Leandro Fontellas Laurito
 * Matricula: 11721BCC045
 * Author: Ricardo
 * Matricula: xxxxxxxxxxx
 * Author: Kauê Lucas Silverio Oliveira
 * Matricula: 11821BCC007
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
    // struct registro { char palavra[50]; int tamanho; } registro;

    // construtor: abre arquivo. Essa aplicacao deveria ler o arquivo se existente ou criar um novo.
    // Entretando recriaremos o arquivo a cada execucao ("w+").
    MeuArquivo() {
        filePonteiro = fopen("dados.dat","wb+");

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
            //Posiciona o ponteiro no inicio do registro  e pula o *
            int posicaoAsterisco = ftell(filePonteiro);
            fseek(filePonteiro, proximoDeletado+1, SEEK_SET);
            //Pega o proximoDeletado pra n entrar em loop infinito
            fread(&proximoDeletado, sizeof(int), 1, filePonteiro);

            unsigned int tamanho;
            fread(&tamanho, sizeof(int), 1, filePonteiro);
            if(tamanho >= (strlen(palavra)+1)) {
                unsigned int tamanhoPalavra = strlen(palavra)+1;
                fseek(filePonteiro,posicaoAsterisco,SEEK_SET);
                fwrite(&tamanhoPalavra, sizeof(int),1,filePonteiro);
                fwrite(palavra, tamanho, 1, filePonteiro);
            }
        }

       // Reestringe o tamanho minimo de uma palavra
        int tamanhoPalavra = strlen(palavra) + 1;
        int tamanho = tamanhoPalavra < 5? 5: tamanhoPalavra;

        char *buffer = (char *)malloc(sizeof(char) * tamanho);
        for(int i = 0; i<tamanho; i++){
            buffer[i] = 0;
        }
        sprintf(buffer,"%s",palavra);

        // Escreve o tamanho do registro no arquivo
        fwrite(&tamanho, sizeof(int), 1, filePonteiro);

        // Escreve a palavra em si no arquivo
        int n = fwrite(buffer, sizeof(char),tamanho, filePonteiro);
        free(buffer);

    }

    // Marca registro como removido, atualiza lista de disponíveis, incluindo o cabecalho
    void removePalavra(int offset) {
        int tamanho;
        int posicaoAtual = ftell(filePonteiro);

        fseek(filePonteiro,offset,SEEK_SET);
        fread(&tamanho, sizeof(int),1,filePonteiro);
        fseek(filePonteiro,offset,SEEK_SET);
        char ast = '*';
        fwrite(&ast,sizeof(char),1,filePonteiro);
        fwrite(&cabecalho.disponivel,sizeof(int),1,filePonteiro);
        fwrite(&tamanho,sizeof(int),1,filePonteiro);

        fseek(filePonteiro, posicaoAtual, SEEK_SET);

        return;
    }

    // BuscaPalavra: retorno é o offset para o registro
    // Nao deve considerar registro removido
    int buscaPalavra(char *palavra) {
        this->substituiBarraNporBarraZero(palavra); // funcao auxiliar substitui terminador por \0
        fseek(filePonteiro,8,SEEK_SET);

        int posicao = ftell(this->filePonteiro);
        int qtdRegistros = cabecalho.quantidade;
        int contador = 0;

        char ast;

        while (contador < qtdRegistros)
        {
            posicao = ftell(this->filePonteiro);
            fread(&ast,sizeof(char),1,filePonteiro);

            if(ast != '*'){
                fseek(filePonteiro, posicao, SEEK_SET);
                
                int tamanho;
                fread(&tamanho,sizeof(int),1,filePonteiro);
                
                char *buffer = (char *)malloc(sizeof(char) * tamanho);
                for(int i = 0; i<tamanho; i++){
                    buffer[i] = 0;
                }
                fread(buffer,sizeof(char),tamanho,filePonteiro);
                printf("%s\n",buffer);

                if(strcmp(palavra, buffer) == 0){
                    free(buffer);
                    return posicao; //Posicao inicial
                }
                free(buffer);
            }else{
                printf("Entrei aqui");
                int tamanho;
                int offset;

                fread(&offset,sizeof(int),1,filePonteiro);
                printf("offset %d\n",offset);
                fread(&tamanho,sizeof(int),1,filePonteiro);
                printf("tamanho %d\n",tamanho);

                int proximoRegistro = posicao + (sizeof(offset) + tamanho);

                fseek(filePonteiro, proximoRegistro,SEEK_SET);
            }
            contador++;
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
    void atualizaCabecalhoOffset(int offset){
        int posicao = ftell(this->filePonteiro);

        fseek(this->filePonteiro, 0, SEEK_SET);
        fread(&this->cabecalho, sizeof(cabecalho), 1, this->filePonteiro);

        this->cabecalho.disponivel = offset;
        fseek(filePonteiro,0,SEEK_SET);
        fwrite(&cabecalho, sizeof(cabecalho),1, filePonteiro);

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
