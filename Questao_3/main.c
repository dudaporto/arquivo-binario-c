/*QUESTÃO 3*/

#include <stdio.h>
#include <stdlib.h>

//Definição dos tipo de estruturas 

typedef struct data {
    int mes;
    int ano;
} data;

typedef struct leitura {
    int nconta;
    char nome[20];
    data ref;
    int consumo;
} leitura;

typedef struct param {
    float vlrKWh100;
    float vlrKWh101_200;
    float vlrKWh201;
    char mesBandeira[12];
    float adicBand[3];
} param;

typedef struct celulaLista {
    leitura informacao;
    struct celulaLista *proximo;
} celulaLista;

int calcular_tamanho_arquivo(char *caminho) {
    FILE *arquivo;
    arquivo = fopen(caminho, "rb");

    fseek(arquivo, 0, SEEK_END); //vai para´úliima posição do arquivo
    int tamanho = ftell(arquivo); //calcula tamanho em bytes
    fseek(arquivo, 0, SEEK_SET); //retorna para primeira posição

    fclose(arquivo);
    return tamanho;
}

void ler_parametros(param *parametros) {
    FILE *arquivo_param;
    arquivo_param = fopen("param.dat", "rb");

    if (arquivo_param == NULL) {
        printf("ERRO! O arquivo 'param.dat' não foi aberto!\n");
        exit(1);
    } else {
        fread(parametros, sizeof (param), 1, arquivo_param);
    }

    fclose(arquivo_param);
}

void ler_dados(leitura *dados, int tamanho) {
    FILE *arquivo_dados;
    arquivo_dados = fopen("dados.dat", "rb");

    if (arquivo_dados == NULL) {
        printf("ERRO! O arquivo 'dados.dat' não foi aberto!\n");
        exit(1);
    } else {
        int n_leituras = tamanho / sizeof (leitura);
        for (int i = 0; i < n_leituras; i++) {
            fread(&dados[i], sizeof (leitura), 1, arquivo_dados);
        }
    }

    fclose(arquivo_dados);
}

float calcula_valor_conta(float consumo, param parametros) {
    if (consumo <= 100)
        return consumo * parametros.vlrKWh100;
    else if (consumo > 100 && consumo <= 200)
        return consumo * parametros.vlrKWh101_200;
    else if (consumo > 200)
        return consumo * parametros.vlrKWh201;
}

char* bandeira(char letra) {
    if (letra == 'G')
        return "Verde";
    else if (letra == 'Y')
        return "Amarela";
    else if (letra == 'R')
        return "Vermelha";
}

void inserir_elemento(celulaLista **lista, leitura valor) {

    /*INSERE UM ELEMENTO DE FORMA ORDENADA NA LISTA*/
    celulaLista *atual, *novo, *anterior;
    int num;

    novo = (celulaLista *) malloc(sizeof (celulaLista));

    atual = *lista;
    anterior = NULL;

    novo->informacao = valor;
    num = novo->informacao.consumo;

    if (atual == NULL) {
        novo->proximo = NULL;
        *lista = novo;
    } else {
        while (atual != NULL && atual->informacao.consumo < num) {
            anterior = atual;
            atual = atual->proximo;
        }
        novo->proximo = atual;

        if (anterior == NULL) {
            *lista = novo;
        } else {
            anterior->proximo = novo;
        }
    }
}

//imprime a conta de 1 ano de um cliente

void imprimir_conta(celulaLista *dados, param parametros) {
    float valor, total = 0.0;

    printf("\nConta: %d - %s", dados->informacao.nconta, dados->informacao.nome);
    printf("\n+------------+---------+----------+-------+");
    printf("\n+ Referencia + Consumo + Bandeira + Valor +");
    printf("\n+------------+---------+----------+-------+");

    while (dados != NULL) {
        printf("\n|   %2d/%d  |", dados->informacao.ref.mes, dados->informacao.ref.ano);
        printf("   %3d   |", dados->informacao.consumo);
        printf(" %-9s|", bandeira(parametros.mesBandeira[dados->informacao.ref.mes - 1]));
        valor = calcula_valor_conta(dados->informacao.consumo, parametros);
        printf(" %6.2f|", valor);

        total += valor;
        dados = dados->proximo;
    }

    printf("\n+------------+---------+----------+-------+");
    printf("\n                Total do periodo = %.2f\n", total);
}

void imprime_dados(leitura *dados, param parametros, int n_leituras) {
    printf("*** Listagem da conta de luz ordenadas por consumo***");
    celulaLista *lista_cliente;

    int inicio;

    for (int i = 0; i < n_leituras; i += 12) {
        inicio = i;
        lista_cliente = NULL;
        //transfere o perido para um vetor temporario
        for (int j = 0; j < 12; j++) {
            //INSERE ELEMENTOS DE FORMA ORDENADA
            inserir_elemento(&lista_cliente, dados[inicio]);
            inicio++;
        }
        imprimir_conta(lista_cliente, parametros);
    }
}

char menu() {
    char opcao;
    printf("*.* Menu *.*\n");
    printf("\n1. Listar contas");
    printf("\n2. Sair");
    printf("\n> ");
    scanf("%d", &opcao);
    return opcao;
}

int main(void) {
    char opc;

    //dados que serão retirados dos arquivos
    param parametros;
    leitura *dados; //vetor dinamico

    do {
        opc = menu();
        switch (opc) {
            case 1:
                //leparametros
                ler_parametros(&parametros);

                //calcula o tamanho do arquivo para descobrir qual será o tamanho do vetor
                int tamanho = calcular_tamanho_arquivo("dados.dat");

                dados = (leitura *) malloc(tamanho);
                ler_dados(dados, tamanho);

                //imprime dados 
                int n_leituras = tamanho / sizeof (leitura);

                //a ordenação ocorre antes da impressão
                imprime_dados(dados, parametros, n_leituras);
                break;
            case 2:
                exit(0);
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opc != 2);

    return 0;
}