# Documentação técnica

Este documento explica como o `loja.c` está organizado: as constantes, a estrutura de dados, cada função e as decisões de projeto.

## 1. Visão geral

O programa é um único arquivo (`loja.c`). O arquivo `loja_produtos.csv` funciona como banco de dados. Para cada operação, o programa **carrega o arquivo em um vetor na memória**, trabalha sobre o vetor e, quando algo muda, grava de volta.

```
        carregarProdutos                      salvarProdutos
loja_produtos.csv ─────────────► Produto *lista ─────────────► temp.csv ──rename──► loja_produtos.csv
   (disco)                        (memória)                    (disco)
```

Exceção: `inserirProduto` só acrescenta uma linha ao final do arquivo (`fopen` em modo `"a"`).

## 2. Constantes

| Constante | Valor | Uso |
|-----------|-------|-----|
| `TAM_TEXTO` | 50 | tamanho dos campos de texto (49 letras + `'\0'`) |
| `TAM_LINHA` | 256 | buffer para ler uma linha inteira |
| `ARQUIVO_DADOS` | `"loja_produtos.csv"` | arquivo que guarda os produtos |
| `ARQUIVO_TEMP` | `"temp.csv"` | arquivo temporário de alterar/excluir |

## 3. Estrutura de dados

```c
typedef struct {
    int   id;                   // gerado automaticamente
    char  nome[TAM_TEXTO];
    char  categoria[TAM_TEXTO];
    float preco;
    int   quantidade;
    char  fornecedor[TAM_TEXTO];
} Produto;
```

Os produtos ficam em um vetor dinâmico `Produto *lista`, criado por `carregarProdutos` e sempre liberado com `free(lista)` ao final de cada função.

## 4. Funções

### 4.1 Entrada segura do teclado

| Função | O que faz |
|--------|-----------|
| `limparBuffer()` | descarta o que sobrou no teclado até o Enter |
| `lerTexto(rotulo, destino, tam)` | lê uma linha com `fgets`; descarta o excesso se passar do tamanho; troca vírgula por espaço; encerra o programa com mensagem se a entrada acabar |
| `lerTextoObrigatorio(...)` | repete `lerTexto` até o usuário digitar algo |
| `lerInteiro(rotulo, &destino, minimo, permiteVazio)` | lê inteiro maior ou igual a `minimo`; rejeita letras e lixo (`12abc`); se `permiteVazio`, Enter devolve 0 (manter valor) |
| `lerFloat(rotulo, &destino, permiteVazio)` | igual ao anterior, para decimal maior ou igual a zero |

### 4.2 Texto sem diferenciar maiúsculas

| Função | O que faz |
|--------|-----------|
| `paraMinusculo(origem, destino)` | copia convertendo para minúsculas |
| `compararIgnorandoCaso(a, b)` | compara duas strings andando com ponteiros; devolve 0 se iguais |

### 4.3 Arquivo e memória

| Função | O que faz |
|--------|-----------|
| `gravarProduto(fp, p)` | escreve um produto como linha CSV |
| `carregarProdutos(&lista)` | lê o CSV e devolve, por ponteiro de ponteiro, um vetor alocado com `malloc`/`realloc` (dobra a capacidade quando enche). Retorna a quantidade lida |
| `salvarProdutos(lista, n)` | grava o vetor em `temp.csv`, apaga o original e renomeia o temporário. Se falhar antes do `remove`, o original permanece intacto |

A leitura usa `sscanf` com `%49[^,]` ("até 49 caracteres, parando na vírgula") e confere se foram lidos 6 campos. Linhas malformadas são ignoradas.

### 4.4 Consultas no vetor

| Função | O que faz |
|--------|-----------|
| `buscarPorId(lista, n, id)` | devolve o **endereço** do produto (ou `NULL`) |
| `existeNome(lista, n, nome, idIgnorar)` | há produto com esse nome? `idIgnorar` evita que o próprio produto conte como duplicata ao alterar |
| `gerarNovoID(lista, n)` | maior ID + 1 |

### 4.5 Impressão

`imprimirCabecalho`, `imprimirProduto`, `imprimirRodape` e `imprimirTabela` desenham a tabela com colunas `ID | Produto | Categoria | Preco (R$) | Qtd | Fornecedor`.

### 4.6 Operações do cadastro

| Função | Resumo |
|--------|--------|
| `criarArquivo()` | grava 12 produtos de exemplo (os 4 primeiros são os da tabela do enunciado). Se o arquivo existe, pede confirmação, pois o modo `"w"` apaga tudo |
| `listarProdutos()` | carrega e imprime a tabela |
| `inserirProduto()` | lê o nome, recusa duplicata, lê os outros campos com validação, gera o ID, confere se o ID já existe (garantia extra) e acrescenta a linha |
| `alterarProduto()` | `buscarPorId` devolve um ponteiro; os campos são alterados por ele (Enter mantém); o novo nome é conferido contra os outros produtos; `salvarProdutos` grava |
| `excluirProduto()` | calcula a posição subtraindo ponteiros (`p - lista`), puxa os seguintes uma casa para trás, `n--` e `salvarProdutos` |
| `buscarProduto()` | converte termo e campos para minúsculas e usa `strstr`; termo vazio é recusado |

### 4.7 Ordenação

| Função | Resumo |
|--------|--------|
| `deveTrocar(a, b, modo)` | diz se o par está na ordem errada. Modos: 1 nome A–Z, 2 nome Z–A, 3 preço ↑, 4 preço ↓ |
| `trocar(a, b)` | troca duas fichas usando uma variável auxiliar |
| `bubbleSort(lista, n, modo)` | Bubble Sort com flag `trocou`: se uma rodada inteira não troca nada, a lista já está ordenada e o laço para |
| `menuOrdenacao()` | carrega, pergunta o modo, ordena **só o vetor** e imprime. O arquivo não é alterado |

### 4.8 `main`

Mostra o menu em um `do...while`, lê a opção com `lerInteiro` (letra ou número negativo é recusado) e chama a função correspondente em um `switch`. A opção 0 encerra.

## 5. Decisões de projeto

- **Vetor em memória + arquivo.** Listar, buscar, alterar, excluir e ordenar trabalham sobre o mesmo vetor, o que simplifica o código e permite usar ponteiros de verdade.
- **Arquivo temporário.** Alterar e excluir regravam tudo, e escrever direto no original poderia perder dados se algo falhasse no meio. Por isso `salvarProdutos` escreve em `temp.csv` e só então substitui o original.
- **Entrada com `fgets`, não `scanf`.** `scanf("%s")` pararia no primeiro espaço ("Camiseta Basica") e `scanf("%f")` deixa lixo no teclado. `fgets` lê a linha toda, com limite de tamanho.
- **Vírgula vira espaço.** O CSV é simples e a vírgula é o separador. Em vez de implementar aspas e escapes, o texto é sanitizado na entrada.
- **Sem acentos nos dados de exemplo.** O `printf` conta bytes, e letras acentuadas em UTF-8 ocupam 2 bytes, o que desalinha a tabela.
- **Bubble Sort com flag.** Mantém o algoritmo simples e didático, mas para cedo quando a lista já está ordenada.

## 6. Limitações

- O ID é `maior ID + 1`, então o ID do último produto excluído é reaproveitado.
- O arquivo inteiro é carregado em memória a cada operação (adequado para uma loja pequena, não para milhões de registros).
- `remove` seguido de `rename` não é uma operação atômica: uma queda de energia exatamente entre os dois deixaria apenas o `temp.csv`.
- `tolower` só converte letras ASCII; "Á" e "á" são tratadas como diferentes.
