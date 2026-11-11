// ====================================================================
// ATIVIDADE 04 - CADASTRO GENERICO DE UMA LOJA
// Algoritmos de ordenacao (Bubble Sort) + manipulacao de arquivos
// Usa: struct, vetor de structs, ponteiros, malloc/realloc/free
// ====================================================================

#include <stdio.h>   // printf, fopen, fgets, fprintf, sscanf, remove, rename
#include <stdlib.h>  // malloc, realloc, free, exit
#include <string.h>  // strcpy, strcspn, strchr, strstr
#include <ctype.h>   // tolower

// ====================================================================
// CONSTANTES E ESTRUTURA
// ====================================================================

#define TAM_TEXTO 50                      // tamanho maximo dos campos de texto (49 letras + '\0')
#define TAM_LINHA 256                     // tamanho do buffer usado para ler uma linha inteira
#define ARQUIVO_DADOS "loja_produtos.csv" // arquivo que funciona como banco de dados
#define ARQUIVO_TEMP  "temp.csv"          // arquivo temporario usado em alterar/excluir

// Ficha de um produto da loja
typedef struct {
    int id;                      // gerado automaticamente (incremental)
    char nome[TAM_TEXTO];        // nome do produto
    char categoria[TAM_TEXTO];   // categoria do produto
    float preco;                 // preco em reais
    int quantidade;              // quantidade em estoque
    char fornecedor[TAM_TEXTO];  // nome do fornecedor
} Produto;

// ====================================================================
// FUNCOES AUXILIARES DE ENTRADA (leitura segura do teclado)
// ====================================================================

// Descarta o que sobrou no teclado ate o Enter (evita bugs na proxima leitura)
void limparBuffer() {
    int c;                                    // guarda o caractere lido
    while ((c = getchar()) != '\n' && c != EOF); // le e joga fora ate o fim da linha
}

// Le uma linha de texto do teclado de forma segura.
// - nunca ultrapassa 'tam' (se o usuario digitar demais, o excesso e descartado)
// - troca virgula por espaco (a virgula separa as colunas do CSV)
void lerTexto(const char *rotulo, char *destino, int tam) {
    printf("%s", rotulo);                                   // mostra a pergunta
    if (fgets(destino, tam, stdin) == NULL) {               // fgets devolve NULL se a entrada acabou
        printf("\nEntrada encerrada.\n");                   // avisa o motivo
        exit(0);                                            // encerra o programa sem travar
    }
    if (strchr(destino, '\n') == NULL) limparBuffer();      // sem '\n' = texto maior que o buffer: joga o resto fora
    destino[strcspn(destino, "\r\n")] = '\0';               // remove o Enter do final da string
    for (char *c = destino; *c != '\0'; c++) {              // percorre a string usando ponteiro
        if (*c == ',') *c = ' ';                            // virgula quebraria o formato CSV
    }
}

// Igual a lerTexto, mas repete a pergunta ate o usuario digitar algo
void lerTextoObrigatorio(const char *rotulo, char *destino, int tam) {
    do {
        lerTexto(rotulo, destino, tam);                     // le o texto
        if (destino[0] == '\0') printf("Campo obrigatorio.\n"); // string vazia = nao digitou nada
    } while (destino[0] == '\0');                           // repete enquanto estiver vazio
}

// Le um numero inteiro >= minimo. Se permiteVazio=1, aceita so Enter (retorna 0 = "manter valor")
int lerInteiro(const char *rotulo, int *destino, int minimo, int permiteVazio) {
    char buf[TAM_LINHA];                                    // guarda a linha digitada
    int valor;                                              // numero convertido
    char extra;                                             // detecta lixo depois do numero (ex.: "12abc")
    while (1) {                                             // repete ate receber um valor valido
        lerTexto(rotulo, buf, sizeof(buf));                 // le a linha como texto
        if (buf[0] == '\0' && permiteVazio) return 0;       // so Enter: o chamador mantem o valor antigo
        if (sscanf(buf, "%d %c", &valor, &extra) == 1 && valor >= minimo) { // 1 = leu so o numero
            *destino = valor;                               // grava no endereco recebido (ponteiro)
            return 1;                                       // 1 = valor novo foi lido
        }
        printf("Valor invalido. Digite um inteiro >= %d.\n", minimo); // pede de novo
    }
}

// Le um numero decimal >= 0 (use ponto: 49.90). Mesma logica de lerInteiro
int lerFloat(const char *rotulo, float *destino, int permiteVazio) {
    char buf[TAM_LINHA];                                    // guarda a linha digitada
    float valor;                                            // numero convertido
    char extra;                                             // detecta lixo depois do numero
    while (1) {                                             // repete ate receber um valor valido
        lerTexto(rotulo, buf, sizeof(buf));                 // le a linha como texto
        if (buf[0] == '\0' && permiteVazio) return 0;       // so Enter: mantem o valor antigo
        if (sscanf(buf, "%f %c", &valor, &extra) == 1 && valor >= 0) { // so o numero, nao negativo
            *destino = valor;                               // grava no endereco recebido
            return 1;                                       // valor novo lido
        }
        printf("Valor invalido. Digite um numero >= 0 usando ponto (ex.: 49.90).\n"); // pede de novo
    }
}

// ====================================================================
// FUNCOES AUXILIARES DE TEXTO (case-insensitive)
// ====================================================================

// Copia 'origem' para 'destino' convertendo tudo para minuscula
void paraMinusculo(const char *origem, char *destino) {
    int i = 0;                                              // indice de navegacao
    while (origem[i] != '\0') {                             // ate o fim da string
        destino[i] = tolower((unsigned char)origem[i]);     // converte cada letra
        i++;                                                // proxima letra
    }
    destino[i] = '\0';                                      // fecha a nova string
}

// Compara duas strings ignorando maiuscula/minuscula (0 = iguais), andando com ponteiros
int compararIgnorandoCaso(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0' &&                      // enquanto nenhuma acabou
           tolower((unsigned char)*a) == tolower((unsigned char)*b)) { // e as letras forem iguais
        a++;                                                // avanca o ponteiro de a
        b++;                                                // avanca o ponteiro de b
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b); // 0 se chegaram juntas ao fim
}

// ====================================================================
// ARQUIVO <-> VETOR EM MEMORIA (ponteiros e alocacao dinamica)
// ====================================================================

// Grava UM produto no arquivo aberto, em formato CSV: id,nome,categoria,preco,quantidade,fornecedor
void gravarProduto(FILE *fp, const Produto *p) {
    fprintf(fp, "%d,%s,%s,%.2f,%d,%s\n",                    // formato CSV com virgula
            p->id, p->nome, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Le o arquivo inteiro para um vetor de Produto alocado dinamicamente.
// 'lista' e um ponteiro para ponteiro: a funcao devolve o vetor criado por ele.
// Retorna a quantidade de produtos lidos (0 se o arquivo nao existe ou esta vazio).
int carregarProdutos(Produto **lista) {
    *lista = NULL;                                          // comeca sem vetor
    FILE *fp = fopen(ARQUIVO_DADOS, "r");                   // abre para leitura
    if (fp == NULL) return 0;                               // arquivo nao existe: lista vazia

    int n = 0;                                              // quantos produtos ja foram lidos
    int capacidade = 0;                                     // quantos cabem no vetor atual
    char linha[TAM_LINHA];                                  // buffer para cada linha
    Produto p;                                              // produto temporario

    while (fgets(linha, sizeof(linha), fp) != NULL) {       // le o arquivo linha a linha
        linha[strcspn(linha, "\r\n")] = '\0';               // remove o Enter do final
        // Corta a linha em 6 pedacos. %49[^,] = "ate 49 letras, parando na virgula"
        if (sscanf(linha, "%d,%49[^,],%49[^,],%f,%d,%49[^\n]",
                   &p.id, p.nome, p.categoria, &p.preco, &p.quantidade, p.fornecedor) == 6) {
            if (n == capacidade) {                          // vetor cheio: precisa aumentar
                capacidade = (capacidade == 0) ? 8 : capacidade * 2; // dobra o tamanho
                Produto *novo = (Produto *)realloc(*lista, capacidade * sizeof(Produto)); // aumenta a memoria
                if (novo == NULL) {                         // sem memoria disponivel
                    printf("Erro de memoria.\n");
                    free(*lista);                           // libera o que ja tinha
                    *lista = NULL;
                    fclose(fp);
                    return 0;
                }
                *lista = novo;                              // passa a usar o vetor maior
            }
            (*lista)[n] = p;                                // guarda o produto na posicao n
            n++;                                            // um produto a mais
        }
    }
    fclose(fp);                                             // fecha o arquivo
    return n;                                               // devolve a quantidade
}

// Regrava o vetor inteiro: escreve em arquivo TEMPORARIO e so depois substitui o original.
// Se algo falhar no meio, o arquivo original continua intacto.
int salvarProdutos(const Produto *lista, int n) {
    FILE *temp = fopen(ARQUIVO_TEMP, "w");                  // cria o arquivo temporario
    if (temp == NULL) {                                     // nao conseguiu criar
        printf("Erro ao criar arquivo temporario.\n");
        return 0;
    }
    for (const Produto *p = lista; p < lista + n; p++) {    // percorre o vetor com aritmetica de ponteiros
        gravarProduto(temp, p);                             // escreve cada produto
    }
    fclose(temp);                                           // fecha (e salva) o temporario
    remove(ARQUIVO_DADOS);                                  // apaga o original
    if (rename(ARQUIVO_TEMP, ARQUIVO_DADOS) != 0) {         // temporario assume o nome do original
        printf("Erro ao substituir o arquivo original.\n");
        return 0;
    }
    return 1;                                               // sucesso
}

// ====================================================================
// FUNCOES DE CONSULTA NO VETOR
// ====================================================================

// Procura um produto pelo ID. Devolve o ENDERECO dele no vetor (ou NULL)
Produto *buscarPorId(Produto *lista, int n, int id) {
    for (Produto *p = lista; p < lista + n; p++) {          // percorre com ponteiro
        if (p->id == id) return p;                          // achou: devolve o endereco
    }
    return NULL;                                            // nao achou
}

// Verifica se ja existe um produto com este nome (ignora o produto 'idIgnorar', usado no alterar)
int existeNome(const Produto *lista, int n, const char *nome, int idIgnorar) {
    for (const Produto *p = lista; p < lista + n; p++) {    // percorre o vetor
        if (p->id != idIgnorar && compararIgnorandoCaso(p->nome, nome) == 0) return 1; // nome repetido
    }
    return 0;                                               // nenhum igual
}

// Gera o proximo ID: maior ID existente + 1 (1 se nao ha produtos)
int gerarNovoID(const Produto *lista, int n) {
    int maior = 0;                                          // maior ID encontrado
    for (const Produto *p = lista; p < lista + n; p++) {    // percorre o vetor
        if (p->id > maior) maior = p->id;                   // atualiza o maior
    }
    return maior + 1;                                       // proximo da fila
}

// ====================================================================
// IMPRESSAO DA TABELA
// ====================================================================

// Imprime o cabecalho da tabela
void imprimirCabecalho() {
    for (int i = 0; i < 92; i++) putchar('-');              // linha separadora
    printf("\n| %-4s | %-22s | %-16s | %-10s | %-5s | %-16s |\n",
           "ID", "Produto", "Categoria", "Preco (R$)", "Qtd", "Fornecedor"); // titulos
    for (int i = 0; i < 92; i++) putchar('-');              // linha separadora
    printf("\n");
}

// Imprime UM produto (recebe o endereco dele, sem copiar a struct)
void imprimirProduto(const Produto *p) {
    printf("| %-4d | %-22s | %-16s | %-10.2f | %-5d | %-16s |\n",
           p->id, p->nome, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Imprime a linha final da tabela
void imprimirRodape() {
    for (int i = 0; i < 92; i++) putchar('-');              // linha separadora
    printf("\n");
}

// Imprime a tabela completa de um vetor
void imprimirTabela(const Produto *lista, int n) {
    imprimirCabecalho();                                    // titulos
    for (const Produto *p = lista; p < lista + n; p++) {    // um por um
        imprimirProduto(p);
    }
    imprimirRodape();                                       // fecha a tabela
}

// ====================================================================
// CRUD
// ====================================================================

// CRIAR: cria o arquivo com produtos de exemplo (apaga o que existia, se confirmado)
void criarArquivo() {
    FILE *teste = fopen(ARQUIVO_DADOS, "r");                // tenta abrir para ver se ja existe
    if (teste != NULL) {                                    // ja existe
        fclose(teste);                                      // fecha o teste
        char resp[TAM_TEXTO];                               // resposta do usuario
        lerTexto("O arquivo ja existe. Recriar APAGA todos os dados. Continuar (s/n)? ", resp, sizeof(resp));
        if (tolower((unsigned char)resp[0]) != 's') {       // qualquer coisa diferente de 's' cancela
            printf("-> Operacao cancelada.\n");
            return;
        }
    }

    // Dados de exemplo: os 4 primeiros sao da tabela do enunciado, os demais foram acrescentados
    Produto exemplos[] = {
        {1,  "Camiseta Basica",    "Vestuario",       49.90f,  30, "Moda Brasil"},
        {2,  "Mouse Optico USB",   "Informatica",     59.99f,  15, "TecnoShop"},
        {3,  "Shampoo 300ml",      "Higiene Pessoal", 22.50f,  40, "Beleza Pura"},
        {4,  "Cafe 500g",          "Alimentos",       18.00f,  25, "Graos do Sul"},
        {5,  "Tenis Esportivo",    "Vestuario",       199.90f, 12, "Moda Brasil"},
        {6,  "Teclado Mecanico",   "Informatica",     249.00f, 8,  "TecnoShop"},
        {7,  "Sabonete Neutro 90g","Higiene Pessoal", 3.49f,   120,"Beleza Pura"},
        {8,  "Arroz Branco 5kg",   "Alimentos",       27.90f,  60, "Graos do Sul"},
        {9,  "Fone Bluetooth",     "Informatica",     129.90f, 20, "TecnoShop"},
        {10, "Calca Jeans",        "Vestuario",       139.90f, 18, "Moda Brasil"},
        {11, "Condicionador 300ml","Higiene Pessoal", 24.90f,  35, "Beleza Pura"},
        {12, "Feijao Carioca 1kg", "Alimentos",       8.99f,   80, "Graos do Sul"}
    };
    int total = (int)(sizeof(exemplos) / sizeof(exemplos[0])); // quantos exemplos existem

    FILE *fp = fopen(ARQUIVO_DADOS, "w");                   // "w" cria (ou apaga e recria) o arquivo
    if (fp == NULL) {                                       // nao conseguiu criar
        printf("Erro ao criar o arquivo.\n");
        return;
    }
    for (int i = 0; i < total; i++) gravarProduto(fp, &exemplos[i]); // grava todos os exemplos
    fclose(fp);                                             // fecha e salva
    printf("-> Arquivo '%s' criado com %d produtos de exemplo.\n", ARQUIVO_DADOS, total);
}

// LISTAR: mostra todos os produtos em tabela
void listarProdutos() {
    Produto *lista;                                         // vetor que sera preenchido
    int n = carregarProdutos(&lista);                       // le o arquivo para a memoria
    if (n == 0) {                                           // nada cadastrado
        printf("\nNenhum produto cadastrado.\n");
        free(lista);                                        // free(NULL) e permitido
        return;
    }
    printf("\n");
    imprimirTabela(lista, n);                               // desenha a tabela
    free(lista);                                            // devolve a memoria
}

// INSERIR: cadastra um produto novo no fim do arquivo
void inserirProduto() {
    Produto *lista;                                         // produtos ja cadastrados
    int n = carregarProdutos(&lista);                       // carrega para conferir duplicatas
    Produto novo;                                           // ficha do novo produto

    printf("\n--- INSERIR NOVO PRODUTO ---\n");
    lerTextoObrigatorio("Nome: ", novo.nome, TAM_TEXTO);    // pede o nome

    if (existeNome(lista, n, novo.nome, 0)) {               // mesmo nome ja cadastrado?
        printf("ERRO: o produto '%s' ja esta cadastrado!\n", novo.nome);
        free(lista);
        return;                                             // aborta a insercao
    }

    lerTextoObrigatorio("Categoria: ", novo.categoria, TAM_TEXTO); // pede a categoria
    lerFloat("Preco (ex.: 49.90): ", &novo.preco, 0);              // pede o preco (validado)
    lerInteiro("Quantidade em estoque: ", &novo.quantidade, 0, 0); // pede a quantidade (>= 0)
    lerTextoObrigatorio("Fornecedor: ", novo.fornecedor, TAM_TEXTO); // pede o fornecedor

    novo.id = gerarNovoID(lista, n);                        // ID automatico, ninguem digita
    if (buscarPorId(lista, n, novo.id) != NULL) {           // garantia extra: ID nao pode repetir
        printf("ERRO: ID %d duplicado. Verifique o arquivo.\n", novo.id);
        free(lista);
        return;
    }
    free(lista);                                            // nao precisamos mais do vetor

    FILE *fp = fopen(ARQUIVO_DADOS, "a");                   // "a" = adiciona no fim sem apagar o resto
    if (fp == NULL) {                                       // erro ao abrir
        printf("Erro ao abrir arquivo para gravacao.\n");
        return;
    }
    gravarProduto(fp, &novo);                               // grava a nova linha
    fclose(fp);                                             // fecha e salva
    printf("-> Produto '%s' inserido com sucesso! (ID gerado: %d)\n", novo.nome, novo.id);
}

// ALTERAR: muda os dados de um produto (Enter mantem o valor atual)
void alterarProduto() {
    Produto *lista;                                         // vetor com todos os produtos
    int n = carregarProdutos(&lista);                       // carrega o arquivo
    if (n == 0) {                                           // nada para alterar
        printf("\nNenhum produto cadastrado.\n");
        free(lista);
        return;
    }

    int id;                                                 // ID escolhido
    lerInteiro("\nDigite o ID do produto que deseja alterar: ", &id, 1, 0);
    Produto *p = buscarPorId(lista, n, id);                 // p aponta direto para o produto no vetor
    if (p == NULL) {                                        // nao existe
        printf("-> Produto com ID %d nao encontrado.\n", id);
        free(lista);
        return;
    }

    printf("Produto encontrado: %s (Preco: R$ %.2f | Qtd: %d)\n", p->nome, p->preco, p->quantidade);
    char tmp[TAM_TEXTO];                                    // guarda o texto digitado
    char pergunta[TAM_LINHA];                               // monta a pergunta com o valor atual

    snprintf(pergunta, sizeof(pergunta), "Novo nome (Enter mantem '%s'): ", p->nome);
    lerTexto(pergunta, tmp, TAM_TEXTO);                     // le o novo nome
    if (tmp[0] != '\0') {                                   // digitou algo?
        if (existeNome(lista, n, tmp, p->id)) {             // conflita com OUTRO produto?
            printf("Ja existe outro produto com esse nome. Nome mantido.\n");
        } else {
            strcpy(p->nome, tmp);                           // altera direto pelo ponteiro
        }
    }

    snprintf(pergunta, sizeof(pergunta), "Nova categoria (Enter mantem '%s'): ", p->categoria);
    lerTexto(pergunta, tmp, TAM_TEXTO);                     // le a nova categoria
    if (tmp[0] != '\0') strcpy(p->categoria, tmp);          // atualiza se digitou

    lerFloat("Novo preco (Enter mantem): ", &p->preco, 1);  // 1 = Enter mantem o valor
    lerInteiro("Nova quantidade (Enter mantem): ", &p->quantidade, 0, 1);

    snprintf(pergunta, sizeof(pergunta), "Novo fornecedor (Enter mantem '%s'): ", p->fornecedor);
    lerTexto(pergunta, tmp, TAM_TEXTO);                     // le o novo fornecedor
    if (tmp[0] != '\0') strcpy(p->fornecedor, tmp);         // atualiza se digitou

    if (salvarProdutos(lista, n)) printf("-> Produto alterado com sucesso!\n"); // grava via arquivo temporario
    free(lista);                                            // libera a memoria
}

// EXCLUIR: remove um produto do arquivo
void excluirProduto() {
    Produto *lista;                                         // vetor com todos os produtos
    int n = carregarProdutos(&lista);                       // carrega o arquivo
    if (n == 0) {                                           // nada para excluir
        printf("\nNenhum produto cadastrado.\n");
        free(lista);
        return;
    }

    int id;                                                 // ID escolhido
    lerInteiro("\nDigite o ID do produto a ser EXCLUIDO: ", &id, 1, 0);
    Produto *p = buscarPorId(lista, n, id);                 // acha o produto
    if (p == NULL) {                                        // nao existe
        printf("-> ID %d nao encontrado.\n", id);
        free(lista);
        return;
    }

    printf("Produto '%s' sera excluido.\n", p->nome);
    int posicao = (int)(p - lista);                         // subtracao de ponteiros = indice no vetor
    for (int i = posicao; i < n - 1; i++) {                 // puxa os seguintes uma casa para tras
        lista[i] = lista[i + 1];
    }
    n--;                                                    // o vetor tem um produto a menos

    if (salvarProdutos(lista, n)) printf("-> Produto excluido com sucesso!\n"); // regrava sem ele
    free(lista);                                            // libera a memoria
}

// BUSCAR: procura por parte do nome ou da categoria (ignora maiuscula/minuscula)
void buscarProduto() {
    Produto *lista;                                         // vetor com todos os produtos
    int n = carregarProdutos(&lista);                       // carrega o arquivo
    if (n == 0) {                                           // nada para buscar
        printf("\nNenhum produto cadastrado.\n");
        free(lista);
        return;
    }

    char termo[TAM_TEXTO], termoMin[TAM_TEXTO];             // termo digitado e sua versao minuscula
    char nomeMin[TAM_TEXTO], catMin[TAM_TEXTO];             // nome/categoria em minuscula
    lerTextoObrigatorio("\nDigite o nome ou categoria para buscar: ", termo, TAM_TEXTO);
    paraMinusculo(termo, termoMin);                         // padroniza o termo

    int achou = 0;                                          // quantos produtos combinaram
    for (const Produto *p = lista; p < lista + n; p++) {    // percorre o vetor
        paraMinusculo(p->nome, nomeMin);                    // nome em minuscula
        paraMinusculo(p->categoria, catMin);                // categoria em minuscula
        // strstr procura o termo DENTRO do texto (devolve NULL se nao achar)
        if (strstr(nomeMin, termoMin) != NULL || strstr(catMin, termoMin) != NULL) {
            if (achou == 0) imprimirCabecalho();            // imprime o cabecalho so na primeira ocorrencia
            imprimirProduto(p);                             // mostra o produto encontrado
            achou++;
        }
    }
    if (achou > 0) imprimirRodape();                        // fecha a tabela
    else printf("-> Nenhum produto correspondente encontrado.\n");
    free(lista);                                            // libera a memoria
}

// ====================================================================
// ORDENACAO (BUBBLE SORT) - so na memoria, o arquivo nao e alterado
// ====================================================================

// Diz se o par (a, b) esta na ordem ERRADA para o modo escolhido
int deveTrocar(const Produto *a, const Produto *b, int modo) {
    switch (modo) {
        case 1: return compararIgnorandoCaso(a->nome, b->nome) > 0; // nome A-Z
        case 2: return compararIgnorandoCaso(a->nome, b->nome) < 0; // nome Z-A
        case 3: return a->preco > b->preco;                         // preco menor -> maior
        case 4: return a->preco < b->preco;                         // preco maior -> menor
    }
    return 0;                                                       // modo desconhecido: nao troca
}

// Troca o conteudo de duas fichas usando seus enderecos
void trocar(Produto *a, Produto *b) {
    Produto aux = *a;                                       // guarda a primeira em um "copo" auxiliar
    *a = *b;                                                // primeira recebe a segunda
    *b = aux;                                               // segunda recebe a guardada
}

// Bubble Sort: compara vizinhos e vai "empurrando" o maior para o fim, rodada apos rodada
void bubbleSort(Produto *lista, int n, int modo) {
    for (int i = 0; i < n - 1; i++) {                       // cada rodada coloca um elemento no lugar certo
        int trocou = 0;                                     // flag: houve troca nesta rodada?
        for (int j = 0; j < n - i - 1; j++) {               // compara pares de vizinhos (ignora o fim ja ordenado)
            if (deveTrocar(&lista[j], &lista[j + 1], modo)) { // par fora de ordem?
                trocar(&lista[j], &lista[j + 1]);           // arruma o par
                trocou = 1;                                 // marca que mexeu
            }
        }
        if (!trocou) break;                                 // nenhuma troca = ja esta ordenado, pode parar
    }
}

// Menu de ordenacao: carrega, ordena na memoria e mostra a tabela
void menuOrdenacao() {
    Produto *lista;                                         // vetor com todos os produtos
    int n = carregarProdutos(&lista);                       // carrega o arquivo
    if (n == 0) {                                           // nada para ordenar
        printf("\nNenhum produto cadastrado para ordenar.\n");
        free(lista);
        return;
    }

    int modo;                                               // criterio escolhido
    printf("\n--- ORDENAR TABELA ---");
    printf("\n1 - Por Nome (A - Z)");
    printf("\n2 - Por Nome (Z - A)");
    printf("\n3 - Por Preco (Menor -> Maior)");
    printf("\n4 - Por Preco (Maior -> Menor)\n");
    lerInteiro("Escolha a ordenacao: ", &modo, 0, 0);       // le a opcao

    if (modo >= 1 && modo <= 4) {                           // opcao valida
        bubbleSort(lista, n, modo);                         // ordena o vetor na memoria
        printf("\n[TABELA ORDENADA NA TELA - ARQUIVO MANTIDO]\n");
        imprimirTabela(lista, n);                           // mostra o resultado
    } else {
        printf("Opcao invalida.\n");
    }
    free(lista);                                            // libera a memoria
}

// ====================================================================
// FUNCAO PRINCIPAL
// ====================================================================

int main() {
    int opcao;                                              // opcao escolhida no menu

    do {                                                    // repete o menu ate escolher 0
        printf("\n=========================================\n");
        printf("       SISTEMA DE GESTAO DE LOJA         \n");
        printf("=========================================\n");
        printf("1 - Criar arquivo (dados de exemplo)\n");
        printf("2 - Listar produtos\n");
        printf("3 - Inserir novo produto\n");
        printf("4 - Alterar produto\n");
        printf("5 - Excluir produto\n");
        printf("6 - Buscar (por nome ou categoria)\n");
        printf("7 - Ordenar tabela (em tela)\n");
        printf("0 - Sair\n");
        lerInteiro("Escolha uma opcao: ", &opcao, 0, 0);    // le e valida o numero

        switch (opcao) {                                    // chama a funcao da opcao
            case 1: criarArquivo();    break;               // Create (arquivo)
            case 2: listarProdutos();  break;               // Read
            case 3: inserirProduto();  break;               // Create (registro)
            case 4: alterarProduto();  break;               // Update
            case 5: excluirProduto();  break;               // Delete
            case 6: buscarProduto();   break;               // Busca por nome/categoria
            case 7: menuOrdenacao();   break;               // Bubble Sort em tela
            case 0: printf("\nEncerrando o programa... Ate mais!\n"); break;
            default: printf("\nOpcao invalida. Tente novamente!\n");
        }
    } while (opcao != 0);                                   // 0 encerra o laco

    return 0;                                               // fim sem erros
}
