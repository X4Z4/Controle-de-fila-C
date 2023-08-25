#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_USUARIOS 100
//TODOS OS ARQUIVOS QUE SÃO REQUISITADOS NO MENU SAO SALVOS EM .TXT SEPARADOS PARA EVITAR CORRUPÇÃO DE ARQUIVOS
typedef struct {
    char nome[50];
    int idade;
    time_t horaCadastro;
    int numeroPrioridade;
} Usuario;
//cria o contador de prioridade
void gerarNumeroPrioridade(Usuario *usuarios, int numUsuarios) {
    int numeros[numUsuarios];
    int i, j, numeroAleatorio;
    int contadorPrioridade = 1;

    // Inicializa o array de números com -1
    for (i = 0; i < numUsuarios; i++) {
        numeros[i] = -1;
    }

    // Gera números aleatórios únicos para usuários com idade < 65
    for (i = 0; i < numUsuarios; i++) {
        if (usuarios[i].idade >= 65) {
            usuarios[i].numeroPrioridade = contadorPrioridade++;
        } else {
            do {
                numeroAleatorio = rand() % 1000 + 1;
                for (j = 0; j < i; j++) {
                    if (numeros[j] == numeroAleatorio) {
                        break;
                    }
                }
            } while (j < i);
            numeros[i] = numeroAleatorio;
            usuarios[i].numeroPrioridade = numeroAleatorio;
        }
    }
}
//função que cadastra um usuario recebendo nome e idade e tambem recebe a hora do registro
void cadastrarUsuario(Usuario *usuarios, int *numUsuarios) {
    Usuario novoUsuario;

    printf("Nome: ");
    fgets(novoUsuario.nome, sizeof(novoUsuario.nome), stdin);
    novoUsuario.nome[strcspn(novoUsuario.nome, "\n")] = '\0'; // Remover o caractere de nova linha

    printf("Idade: ");
    scanf("%d", &novoUsuario.idade);
    getchar(); // Consumir o caractere de nova linha

    time_t horaCadastro = time(NULL);
    novoUsuario.horaCadastro = horaCadastro;

    usuarios[*numUsuarios] = novoUsuario;
    (*numUsuarios)++;
}
//escreve a lista de pessoas 
void escreverListaUsuarios(Usuario *usuarios, int numUsuarios) {
    if (numUsuarios == 0) {
        printf("Não há usuários cadastrados.\n");
        return;
    }

    FILE *arquivo;
    int i;

    arquivo = fopen("lista_usuarios.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo.\n");
        return;
    }

    for (i = 0; i < numUsuarios; i++) {
        struct tm *horario = localtime(&(usuarios[i].horaCadastro));
        fprintf(arquivo, "Nome: %s\n", usuarios[i].nome);
        fprintf(arquivo, "Idade: %d\n", usuarios[i].idade);
        fprintf(arquivo, "Hora do cadastro: %02d:%02d:%02d\n", horario->tm_hour, horario->tm_min, horario->tm_sec);
        fprintf(arquivo, "Número de prioridade: %d\n\n", usuarios[i].numeroPrioridade);
    }

    fclose(arquivo);
    printf("Lista de usuários gravada em arquivo.\n");
}
//escreve a lista de prioridades
void escreverListaPrioridades(Usuario *usuarios, int numUsuarios) {
    if (numUsuarios == 0) {
        printf("Não há usuários cadastrados.\n");
        return;
    }

    FILE *arquivo;
    int i;

    arquivo = fopen("lista_prioridades.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo.\n");
        return;
    }

    for (i = 0; i < numUsuarios; i++) {
        if (usuarios[i].idade >= 65) {
            fprintf(arquivo, "Nome: %s\n", usuarios[i].nome);
            fprintf(arquivo, "Número de prioridade: %d\n\n", usuarios[i].numeroPrioridade);
        }
    }

    fclose(arquivo);
    printf("Lista de prioridades gravada em arquivo.\n");
}
//atende o cliente e faz a fila andar
void atenderCliente(Usuario *usuarios, int numUsuarios) {
    if (numUsuarios == 0) {
        printf("Não há usuários cadastrados.\n");
        return;
    }

    Usuario proximoCliente;
    int i;

    for (i = 0; i < numUsuarios; i++) {
        if (usuarios[i].numeroPrioridade != -1) {
            proximoCliente = usuarios[i];
            usuarios[i].numeroPrioridade = -1;
            break;
        }
    }

    printf("Próximo cliente a ser atendido:\n");
    printf("Nome: %s\n", proximoCliente.nome);
    printf("Número de atendimento: %d\n", proximoCliente.numeroPrioridade);
}
//mostra a fila de usuarios colocando os de prioridade na frente
void mostrarFilaUsuarios(Usuario *usuarios, int numUsuarios) {
    if (numUsuarios == 0) {
        printf("Não há usuários cadastrados.\n");
        return;
    }

    gerarNumeroPrioridade(usuarios, numUsuarios); // Gerar prioridades antes de exibir a fila

    printf("Fila de usuários:\n");
    for (int i = 0; i < numUsuarios; i++) {
        printf("%d. Nome: %s\n", i + 1, usuarios[i].nome);
        printf("   Número de prioridade: %d\n", usuarios[i].numeroPrioridade);
    }
}
//escreve a fila de usuarios
void escreverFilaUsuarios(Usuario *usuarios, int numUsuarios) {
    if (numUsuarios == 0) {
        printf("Não há usuários cadastrados.\n");
        return;
    }

    FILE *arquivo;
    int i;

    arquivo = fopen("fila_usuarios.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo.\n");
        return;
    }

    fprintf(arquivo, "Fila de usuários:\n");

    // Escreve primeiro as prioridades
    for (i = 0; i < numUsuarios; i++) {
        if (usuarios[i].idade >= 65) {
            fprintf(arquivo, "%d. Nome: %s\n", i + 1, usuarios[i].nome);
        }
    }

    // Escreve os demais usuários
    for (i = 0; i < numUsuarios; i++) {
        if (usuarios[i].idade < 65) {
            fprintf(arquivo, "%d. Nome: %s\n", i + 1, usuarios[i].nome);
        }
    }

    fclose(arquivo);

    printf("Fila de usuários gravada em arquivo.\n");
}
//calcula o tempo medio entre as entradas
double calcularMediaTempoEntradas() {
    FILE *arquivo;
    int minutoAnterior = 0, segundoAnterior = 0;
    int minutoAtual, segundoAtual;
    int numEntradas = 0;
    double tempoTotal = 0.0;

    arquivo = fopen("atendimentos.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo.\n");
        return 0.0;
    }

    while (!feof(arquivo)) {
        int minuto, segundo;
        char buffer[100];
        fscanf(arquivo, "%s", buffer);
        sscanf(buffer, "%d:%d", &minuto, &segundo);
        if (numEntradas > 0) {
            minutoAtual = minuto;
            segundoAtual = segundo;
            int tempo = (minutoAtual - minutoAnterior) * 60 + (segundoAtual - segundoAnterior);
            tempoTotal += tempo;
        }
        minutoAnterior = minuto;
        segundoAnterior = segundo;
        numEntradas++;
    }

    fclose(arquivo);

    if (numEntradas > 0) {
        double media = tempoTotal / numEntradas;
        return media;
    } else {
        return 0.0;
    }
}

int main() {
    Usuario usuarios[MAX_USUARIOS];
    int numUsuarios = 0;
    int opcao;

    srand(time(NULL));

    do {
        printf("\n--- Sistema de Atendimento ---\n");
        printf("1 - Cadastrar usuário\n");
        printf("2 - Gerar lista de usuários\n");
        printf("3 - Gerar lista de prioridades\n");
        printf("4 - Atender cliente\n");
        printf("5 - Escrever fila de usuários em arquivo\n");
        printf("6 - Calcular média de tempo entre atendimentos\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar(); // Consumir o caractere de nova linha

        switch (opcao) {
            case 1:
                cadastrarUsuario(usuarios, &numUsuarios);
                break;
            case 2:
                escreverListaUsuarios(usuarios, numUsuarios);
                break;
            case 3:
                escreverListaPrioridades(usuarios, numUsuarios);
                break;
            case 4:
                atenderCliente(usuarios, numUsuarios);
                break;
            case 5:
                escreverFilaUsuarios(usuarios, numUsuarios);
                break;
            case 6:
                printf("Média de tempo entre atendimentos: %.2lf segundos\n", calcularMediaTempoEntradas());
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);

    return 0;
}
