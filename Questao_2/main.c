/*QUESTÃO 2*/

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

//imprime a conta de 1 ano de um cliente

void imprimir_conta(leitura dados[12], param parametros) {
    float valor, total = 0.0;

    printf("\nConta: %d - %s", dados[0].nconta, dados[0].nome);
    printf("\n+------------+---------+----------+-------+");
    printf("\n+ Referencia + Consumo + Bandeira + Valor +");
    printf("\n+------------+---------+----------+-------+");

    for (int i = 0; i < 12; i++) {
        printf("\n|   %2d/%d  |", dados[i].ref.mes, dados[i].ref.ano);
        printf("   %3d   |", dados[i].consumo);
        printf(" %-9s|", bandeira(parametros.mesBandeira[dados[i].ref.mes - 1]));
        valor = calcula_valor_conta(dados[i].consumo, parametros);
        printf(" %6.2f|", valor);

        total += valor;
    }

    printf("\n+------------+---------+----------+-------+");
    printf("\n                Total do periodo = %.2f\n", total);
}

void imprime_dados(leitura *dados, param parametros, int n_leituras) {
    printf("*** Listagem da conta de luz ***");
    leitura conta_atual[12];
    int inicio;

    for (int i = 0; i < n_leituras; i += 12) {
        inicio = i;
        //transfere o perido para um vetor temporario
        for (int j = 0; j < 12; j++) {
            conta_atual[j] = dados[inicio];
            inicio++;
        }
        imprimir_conta(conta_atual, parametros);
    }
}

char menu() {
    char opcao;
    printf("\n*.* Menu *.*\n");
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
                imprime_dados(dados, parametros, n_leituras);
                break;
            case 2:
                break;
            default:
                printf("Opcao invalida.\n");
        }

    } while (opc != 2);

    return 0;
}

