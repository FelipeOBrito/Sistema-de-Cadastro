#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ====================================================================
// ESTRUTURAS E CONSTANTES
// ====================================================================

// Define o tamanho máximo dos campos de texto (aumente se necessário)
#define TAM_TEXTO 50
// Define o nome do arquivo que servirá de banco de dados
#define ARQUIVO_DADOS "loja_produtos.csv"

// Estrutura que representa um Produto na loja
typedef struct {
    int id;                           // ID gerado automaticamente
    char nome[TAM_TEXTO];             // Nome do produto
    char categoria[TAM_TEXTO];        // Categoria do produto
    float preco;                      // Preço do produto
} Produto;

// ====================================================================
// FUNÇÕES AUXILIARES (UTILITÁRIOS)
// ====================================================================

// Função para limpar o buffer do teclado (evita bugs com fgets após scanf)
void limparBuffer() {
    int c; // Variável para armazenar o caractere lido
    // Lê caracteres até encontrar uma quebra de linha ou o fim do arquivo (EOF)
    while ((c = getchar()) != '\n' && c != EOF); 
}

// Função auxiliar para converter uma string inteira para minúsculas
// Isso permite que as comparações sejam "case-insensitive" (ex: "Maca" == "maca")
void paraMinusculo(const char *origem, char *destino) {
    int i = 0; // Índice de navegação
    // Percorre a string original até o caractere nulo ('\0')
    while (origem[i] != '\0') {
        // Copia convertendo para minúsculo
        destino[i] = tolower((unsigned char)origem[i]); 
        i++; // Avança o índice
    }
    destino[i] = '\0'; // Finaliza a nova string de destino
}

// Função auxiliar para comparar duas strings ignorando maiúsculas/minúsculas
int compararStringsCaseInsensitive(const char *s1, const char *s2) {
    char str1[TAM_TEXTO], str2[TAM_TEXTO]; // Strings temporárias
    paraMinusculo(s1, str1); // Converte a primeira para minúsculo
    paraMinusculo(s2, str2); // Converte a segunda para minúsculo
    return strcmp(str1, str2); // Retorna o resultado padrão do strcmp
}

// ====================================================================
// LÓGICA DE DADOS E ARQUIVO
// ====================================================================

// Função para verificar se um produto com o mesmo nome já existe no arquivo
int verificarDuplicata(const char *nomeBusca) {
    FILE *fp = fopen(ARQUIVO_DADOS, "r"); // Tenta abrir o arquivo para leitura
    if (fp == NULL) return 0; // Se não existe, não há duplicatas

    char linha[200]; // Buffer para ler cada linha do arquivo
    Produto p; // Estrutura temporária para carregar os dados lidos
    
    // Lê o arquivo linha por linha
    while (fgets(linha, sizeof(linha), fp)) {
        // Extrai os dados do formato CSV separado por vírgula
        if (sscanf(linha, "%d,%49[^,],%49[^,],%f", &p.id, p.nome, p.categoria, &p.preco) == 4) {
            // Compara o nome lido com o nome desejado (case-insensitive)
            if (compararStringsCaseInsensitive(p.nome, nomeBusca) == 0) {
                fclose(fp); // Fecha o arquivo
                return 1; // Retorna 1 indicando que encontrou duplicata
            }
        }
    }
    fclose(fp); // Fecha o arquivo
    return 0; // Retorna 0 (nenhuma duplicata encontrada)
}

// Função para gerar ID automático e incremental
int gerarNovoID() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r"); // Abre o arquivo para leitura
    if (fp == NULL) return 1; // Se não existe, o primeiro ID será 1

    char linha[200]; // Buffer para leitura
    int maxID = 0; // Armazena o maior ID encontrado
    int currentID; // ID da linha atual
    
    // Varre todas as linhas
    while (fgets(linha, sizeof(linha), fp)) {
        // Tenta ler apenas o primeiro número (ID) antes da vírgula
        if (sscanf(linha, "%d,", &currentID) == 1) {
            // Atualiza o maxID se o ID lido for maior
            if (currentID > maxID) maxID = currentID;
        }
    }
    fclose(fp); // Fecha o arquivo
    return maxID + 1; // Retorna o maior ID encontrado + 1
}

// ====================================================================
// FUNÇÕES PRINCIPAIS (CRUD)
// ====================================================================

// Insere um novo produto no fim do arquivo
void inserirProduto() {
    Produto novo; // Cria uma variável da estrutura Produto
    
    printf("\n--- INSERIR NOVO PRODUTO ---\n");
    
    // Solicita o nome
    printf("Digite o nome do produto: ");
    fgets(novo.nome, sizeof(novo.nome), stdin); // Lê texto com espaços
    novo.nome[strcspn(novo.nome, "\n")] = '\0'; // Remove o '\n' do final da string

    // Verifica se já existe produto com este nome
    if (verificarDuplicata(novo.nome)) {
        printf("ERRO: O produto '%s' ja esta cadastrado!\n", novo.nome);
        return; // Aborta a inserção
    }

    // Solicita a categoria
    printf("Digite a categoria: ");
    fgets(novo.categoria, sizeof(novo.categoria), stdin);
    novo.categoria[strcspn(novo.categoria, "\n")] = '\0';

    // Solicita o preço
    printf("Digite o preco: ");
    scanf("%f", &novo.preco);
    limparBuffer(); // Limpa o '\n' residual do scanf

    // Gera o ID automaticamente
    novo.id = gerarNovoID();

    // Abre arquivo em modo "a" (append - adicionar ao final)
    FILE *fp = fopen(ARQUIVO_DADOS, "a");
    if (fp == NULL) {
        printf("Erro ao abrir arquivo para gravacao.\n");
        return; // Aborta em caso de erro
    }

    // Grava os dados separados por vírgula (formato CSV)
    fprintf(fp, "%d,%s,%s,%.2f\n", novo.id, novo.nome, novo.categoria, novo.preco);
    fclose(fp); // Fecha e salva
    
    printf("-> Produto '%s' inserido com sucesso! (ID Gerado: %d)\n", novo.nome, novo.id);
}

// Lista os produtos diretamente do arquivo
void listarProdutos() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r"); // Modo de leitura
    if (fp == NULL) {
        printf("\nNenhum dado cadastrado ainda.\n");
        return; // Sai da função
    }

    Produto p; // Estrutura para os dados da linha
    char linha[200]; // Buffer da linha

    // Imprime cabeçalho da tabela
    printf("\n%-5s | %-20s | %-20s | %-10s\n", "ID", "NOME", "CATEGORIA", "PRECO");
    printf("------------------------------------------------------------------\n");

    // Loop até o fim do arquivo
    while (fgets(linha, sizeof(linha), fp)) {
        // Separa os dados delimitados por vírgula
        if (sscanf(linha, "%d,%49[^,],%49[^,],%f", &p.id, p.nome, p.categoria, &p.preco) == 4) {
            // Imprime os dados tabulados
            printf("%-5d | %-20s | %-20s | R$ %-7.2f\n", p.id, p.nome, p.categoria, p.preco);
        }
    }
    fclose(fp); // Fecha arquivo
}

// Altera um produto gerando um arquivo temporário para evitar corrupção
void alterarProduto() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r"); // Abre o original
    if (fp == NULL) {
        printf("\nArquivo nao encontrado.\n");
        return;
    }

    FILE *temp = fopen("temp.csv", "w"); // Cria o arquivo temporário
    if (temp == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        fclose(fp);
        return;
    }

    int idBusca, encontrou = 0; // ID procurado e flag de controle
    printf("\nDigite o ID do produto que deseja alterar: ");
    scanf("%d", &idBusca);
    limparBuffer(); // Limpa buffer após scanf

    char linha[200];
    Produto p;

    // Varre o arquivo original linha por linha
    while (fgets(linha, sizeof(linha), fp)) {
        // Lê os valores da linha
        if (sscanf(linha, "%d,%49[^,],%49[^,],%f", &p.id, p.nome, p.categoria, &p.preco) == 4) {
            if (p.id == idBusca) { // Se for o ID procurado
                encontrou = 1; // Marca que achou
                printf("Produto encontrado: %s (Atual: R$ %.2f)\n", p.nome, p.preco);
                
                // Pega os novos dados
                printf("Novo nome (ou aperte Enter para manter '%s'): ", p.nome);
                char novoNome[TAM_TEXTO];
                fgets(novoNome, sizeof(novoNome), stdin);
                novoNome[strcspn(novoNome, "\n")] = '\0';
                if(strlen(novoNome) > 0) strcpy(p.nome, novoNome); // Atualiza se foi digitado algo

                printf("Nova categoria (ou Enter para manter '%s'): ", p.categoria);
                char novaCat[TAM_TEXTO];
                fgets(novaCat, sizeof(novaCat), stdin);
                novaCat[strcspn(novaCat, "\n")] = '\0';
                if(strlen(novaCat) > 0) strcpy(p.categoria, novaCat);

                printf("Novo preco: ");
                scanf("%f", &p.preco);
                limparBuffer();
            }
            // Grava o registro (alterado ou intacto) no arquivo temporário
            fprintf(temp, "%d,%s,%s,%.2f\n", p.id, p.nome, p.categoria, p.preco);
        }
    }

    // Fecha os dois arquivos
    fclose(fp);
    fclose(temp);

    // Substitui o original pelo temporário atualizado
    remove(ARQUIVO_DADOS); // Deleta original
    rename("temp.csv", ARQUIVO_DADOS); // Renomeia o temp

    if (encontrou) printf("-> Produto alterado com sucesso!\n");
    else printf("-> Produto com ID %d nao encontrado.\n", idBusca);
}

// Exclui um produto (mesma lógica do arquivo temporário da função alterar)
void excluirProduto() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r");
    if (fp == NULL) {
        printf("\nArquivo nao encontrado.\n");
        return;
    }

    FILE *temp = fopen("temp.csv", "w");
    if (temp == NULL) {
        printf("\nErro ao criar arquivo temporario.\n");
        fclose(fp);
        return;
    }

    int idBusca, encontrou = 0;
    printf("\nDigite o ID do produto a ser EXCLUIDO: ");
    scanf("%d", &idBusca);
    limparBuffer();

    char linha[200];
    Produto p;

    // Lê o original
    while (fgets(linha, sizeof(linha), fp)) {
        if (sscanf(linha, "%d,%49[^,],%49[^,],%f", &p.id, p.nome, p.categoria, &p.preco) == 4) {
            // Se o ID for igual ao buscado, NÃO gravamos no temp (o que o exclui logicamente)
            if (p.id == idBusca) {
                encontrou = 1;
                printf("Produto '%s' deletado!\n", p.nome);
            } else {
                // Caso contrário, copiamos para o temp
                fprintf(temp, "%d,%s,%s,%.2f\n", p.id, p.nome, p.categoria, p.preco);
            }
        }
    }

    fclose(fp);
    fclose(temp);
    remove(ARQUIVO_DADOS);
    rename("temp.csv", ARQUIVO_DADOS);

    if (!encontrou) printf("-> ID %d nao encontrado.\n", idBusca);
}

// ====================================================================
// ALGORITMOS DE BUSCA E ORDENAÇÃO (BUBBLE SORT)
// ====================================================================

// Busca produtos por nome ou categoria (Case-insensitive)
void buscarProduto() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r");
    if (fp == NULL) {
        printf("\nArquivo vazio ou inexistente.\n");
        return;
    }

    char termo[TAM_TEXTO], termoMinusculo[TAM_TEXTO];
    printf("\nDigite o nome ou categoria para buscar: ");
    fgets(termo, sizeof(termo), stdin);
    termo[strcspn(termo, "\n")] = '\0';
    paraMinusculo(termo, termoMinusculo); // Converte termo para minúsculo para busca parcial

    Produto p;
    char linha[200], nomeMinusculo[TAM_TEXTO], catMinusculo[TAM_TEXTO];
    int achou = 0;

    printf("\n%-5s | %-20s | %-20s | %-10s\n", "ID", "NOME", "CATEGORIA", "PRECO");
    printf("------------------------------------------------------------------\n");

    while (fgets(linha, sizeof(linha), fp)) {
        if (sscanf(linha, "%d,%49[^,],%49[^,],%f", &p.id, p.nome, p.categoria, &p.preco) == 4) {
            paraMinusculo(p.nome, nomeMinusculo);
            paraMinusculo(p.categoria, catMinusculo);

            //strstr verifica se a substring (termo) existe dentro das strings completas
            if (strstr(nomeMinusculo, termoMinusculo) != NULL || strstr(catMinusculo, termoMinusculo) != NULL) {
                printf("%-5d | %-20s | %-20s | R$ %-7.2f\n", p.id, p.nome, p.categoria, p.preco);
                achou = 1;
            }
        }
    }
    fclose(fp);
    if (!achou) printf("-> Nenhum produto correspondente encontrado.\n");
}

// Função genérica de Bubble Sort que ordena em memória de acordo com o modo escolhido
void bubbleSort(Produto lista[], int n, int modo) {
    int i, j;
    Produto temp; // Estrutura temporária para a troca (swap)
    int realizarTroca; // Flag booleana

    // Percorre a lista diversas vezes
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            realizarTroca = 0; // Inicia afirmando que não precisa trocar

            // Avalia as condições dependendo do modo de ordenação
            switch (modo) {
                case 1: // Nome A-Z
                    if (compararStringsCaseInsensitive(lista[j].nome, lista[j + 1].nome) > 0)
                        realizarTroca = 1;
                    break;
                case 2: // Nome Z-A
                    if (compararStringsCaseInsensitive(lista[j].nome, lista[j + 1].nome) < 0)
                        realizarTroca = 1;
                    break;
                case 3: // Preco Menor -> Maior
                    if (lista[j].preco > lista[j + 1].preco)
                        realizarTroca = 1;
                    break;
                case 4: // Preco Maior -> Menor
                    if (lista[j].preco < lista[j + 1].preco)
                        realizarTroca = 1;
                    break;
            }

            // Se a condição for atendida, efetua a troca dos elementos do vetor
            if (realizarTroca) {
                temp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = temp;
            }
        }
    }
}

// Carrega os dados do arquivo em um vetor na memória, ordena e exibe (sem alterar original)
void menuOrdenacao() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r");
    if (fp == NULL) {
        printf("\nNenhum dado cadastrado para ordenar.\n");
        return;
    }

    // Primeiro, conta quantas linhas existem para alocação
    int contagem = 0;
    char linha[200];
    while (fgets(linha, sizeof(linha), fp)) {
        contagem++;
    }
    rewind(fp); // Volta o cursor para o início do arquivo

    // Cria um vetor de Produtos do tamanho exato da quantidade lida
    Produto *lista = (Produto *)malloc(contagem * sizeof(Produto));
    int n = 0;

    // Carrega o arquivo inteiro no vetor "lista"
    while (fgets(linha, sizeof(linha), fp)) {
        sscanf(linha, "%d,%49[^,],%49[^,],%f", &lista[n].id, lista[n].nome, lista[n].categoria, &lista[n].preco);
        n++;
    }
    fclose(fp); // Fecha arquivo. Operações agora só na memória.

    int modo;
    printf("\n--- ORDENAR TABELA ---");
    printf("\n1 - Por Nome (A - Z)");
    printf("\n2 - Por Nome (Z - A)");
    printf("\n3 - Por Preco (Menor -> Maior)");
    printf("\n4 - Por Preco (Maior -> Menor)");
    printf("\nEscolha a ordenacao: ");
    scanf("%d", &modo);
    limparBuffer();

    if (modo >= 1 && modo <= 4) {
        // Aciona o algoritmo Bubble Sort
        bubbleSort(lista, n, modo);
        
        // Exibe o vetor já ordenado em formato de tabela
        printf("\n[TABELA ORDENADA NA TELA - ARQUIVO MANTIDO]\n");
        printf("%-5s | %-20s | %-20s | %-10s\n", "ID", "NOME", "CATEGORIA", "PRECO");
        printf("------------------------------------------------------------------\n");
        for (int i = 0; i < n; i++) {
            printf("%-5d | %-20s | %-20s | R$ %-7.2f\n", lista[i].id, lista[i].nome, lista[i].categoria, lista[i].preco);
        }
    } else {
        printf("Opcao invalida.\n");
    }

    free(lista); // Libera o ponteiro (memória alocada)
}

// ====================================================================
// DADOS INICIAIS (PRODUTOS E CATEGORIAS DE EXEMPLO)
// ====================================================================

// Cria o arquivo com produtos de exemplo, apenas se ele ainda nao existir
void carregarDadosIniciais() {
    FILE *fp = fopen(ARQUIVO_DADOS, "r");
    if (fp != NULL) { // Se o arquivo ja existe, nao sobrescreve nada
        fclose(fp);
        return;
    }

    fp = fopen(ARQUIVO_DADOS, "w"); // Cria o arquivo novo
    if (fp == NULL) return;

    // Formato: id,nome,categoria,preco
    fprintf(fp, "1,Arroz 5kg,Alimentos,28.90\n");
    fprintf(fp, "2,Feijao Preto 1kg,Alimentos,8.49\n");
    fprintf(fp, "3,Macarrao Espaguete,Alimentos,4.75\n");
    fprintf(fp, "4,Leite Integral 1L,Bebidas,5.20\n");
    fprintf(fp, "5,Suco de Laranja 1L,Bebidas,7.80\n");
    fprintf(fp, "6,Refrigerante 2L,Bebidas,9.99\n");
    fprintf(fp, "7,Detergente,Limpeza,2.99\n");
    fprintf(fp, "8,Sabao em Po 1kg,Limpeza,15.50\n");
    fprintf(fp, "9,Desinfetante 2L,Limpeza,11.40\n");
    fprintf(fp, "10,Shampoo 350ml,Higiene,18.90\n");
    fprintf(fp, "11,Sabonete,Higiene,2.49\n");
    fprintf(fp, "12,Creme Dental,Higiene,6.35\n");
    fprintf(fp, "13,Caderno 200 folhas,Papelaria,19.90\n");
    fprintf(fp, "14,Caneta Azul,Papelaria,2.50\n");
    fprintf(fp, "15,Mouse USB,Informatica,45.00\n");
    fprintf(fp, "16,Teclado USB,Informatica,89.90\n");
    fclose(fp);
}

// ====================================================================
// FUNÇÃO PRINCIPAL (MAIN)
// ====================================================================

int main() {
    int opcao;

    carregarDadosIniciais(); // Carrega produtos de exemplo na primeira execucao

    do {
        // Exibe o Menu Principal
        printf("\n=========================================\n");
        printf("       SISTEMA DE GESTAO DE LOJA         \n");
        printf("=========================================\n");
        printf("1 - Inserir novo produto\n");
        printf("2 - Listar produtos\n");
        printf("3 - Alterar produto\n");
        printf("4 - Excluir produto\n");
        printf("5 - Buscar (por nome ou categoria)\n");
        printf("6 - Ordenar Tabela (Em Tela)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        
        // Lê a opção do usuário
        if(scanf("%d", &opcao) != 1) {
            // Se o usuário não digitar um número, limpa o buffer e reseta
            limparBuffer(); 
            opcao = -1; 
        } else {
            limparBuffer(); // Limpa o "Enter" residual
        }

        // Chaveador de opções do menu
        switch (opcao) {
            case 1: inserirProduto(); break; // Cria/Insere
            case 2: listarProdutos(); break; // Lê lista total
            case 3: alterarProduto(); break; // Update usando temp file
            case 4: excluirProduto(); break; // Delete usando temp file
            case 5: buscarProduto();  break; // Search case-insensitive
            case 6: menuOrdenacao();  break; // Bubble Sort nas structs
            case 0: printf("\nEncerrando o programa... Ate mais!\n"); break;
            default: printf("\nOpcao invalida. Tente novamente!\n");
        }
    } while (opcao != 0); // Continua no programa enquanto usuário não digitar 0

    return 0; // Encerra o aplicativo
}
