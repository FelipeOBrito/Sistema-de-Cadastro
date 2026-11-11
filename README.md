# Cadastro de Loja em C

Sistema de cadastro de produtos de uma loja, feito em **C**, que guarda os dados em um **arquivo CSV** e permite **criar, listar, inserir, alterar, excluir, buscar e ordenar** produtos pelo terminal.

Projeto da **Atividade Avaliativa 04**: unir algoritmos de ordenação (Bubble Sort) com manipulação de arquivos, usando `struct`, vetores e ponteiros.

---

## Funcionalidades

| Opção | O que faz |
|:-----:|-----------|
| 1 | **Criar arquivo**: cria o `loja_produtos.csv` com 12 produtos de exemplo (pede confirmação se o arquivo já existir) |
| 2 | **Listar** todos os produtos em tabela |
| 3 | **Inserir** produto com **ID automático** e **verificação de duplicata** (mesmo nome) |
| 4 | **Alterar** produto pelo ID (Enter mantém o valor atual) |
| 5 | **Excluir** produto pelo ID |
| 6 | **Buscar** por parte do nome ou da categoria (ignora maiúsculas e minúsculas) |
| 7 | **Ordenar** em tela por nome (A–Z, Z–A) ou preço (menor→maior, maior→menor) com **Bubble Sort**, **sem alterar o arquivo** |
| 0 | Sair |

Todas as entradas do teclado são validadas: letra no lugar de número, preço negativo, texto grande demais ou vírgula no nome não quebram o programa.

## Requisitos do enunciado × onde estão no código

| Requisito | Onde está |
|-----------|-----------|
| Geração automática de ID (incremental) | `gerarNovoID` |
| Verificação de duplicatas (nome) e de ID | `existeNome`, `buscarPorId` (usada em `inserirProduto`) |
| Criar | `criarArquivo` |
| Listar | `listarProdutos` |
| Inserir | `inserirProduto` |
| Alterar | `alterarProduto` |
| Excluir | `excluirProduto` |
| Buscar por nome e categoria | `buscarProduto` |
| Ordenar: nome A–Z, nome Z–A, preço ↑, preço ↓ | `bubbleSort` + `deveTrocar` (modos 1 a 4) |
| Estrutura de dados e ponteiros | `struct Produto`, vetor `Produto *lista` com `malloc`/`realloc`/`free`, aritmética de ponteiros |
| Alterar/excluir com arquivo temporário | `salvarProdutos` (grava `temp.csv` e substitui o original) |
| Campos com tamanho limitado | `TAM_TEXTO` (50) |
| Exportar dados para CSV com vírgula | `gravarProduto` |
| Comparações e buscas *case-insensitive* | `compararIgnorandoCaso`, `paraMinusculo` |
| Armazena em vetor de estruturas | `carregarProdutos` |
| Exibe em formato de tabela sem alterar o arquivo | `imprimirTabela`, `menuOrdenacao` |
| Comentários explicativos linha a linha | todo o `loja.c` |

## Estrutura do repositório

```
cadastro-loja-c/
├── loja.c                      # programa completo (um único arquivo)
├── loja_produtos.csv           # banco de dados de exemplo (12 produtos)
├── Makefile                    # make, make run, make test, make clean
├── tests/
│   └── run_tests.sh            # testes automáticos de ponta a ponta
├── docs/
│   ├── DOCUMENTACAO.md         # explicação das funções e decisões de projeto
│   ├── TESTES.md               # cenários testados e resultado
│   ├── ROTEIRO_APRESENTACAO.md # roteiro de 8 minutos para 3 integrantes
│   └── tabela_loja.txt         # tabela de dados em texto
├── .gitignore
└── README.md
```

## Como compilar e executar

Precisa de um compilador C (`gcc`).

**Linux / macOS**

```bash
make          # compila
make run      # compila e executa
# ou, sem make:
gcc -Wall -Wextra -std=c99 -o loja loja.c
./loja
```

**Windows (MinGW / MSYS2)**

```bash
gcc -Wall -Wextra -std=c99 -o loja.exe loja.c
loja.exe
```

> O programa lê e grava o arquivo `loja_produtos.csv` **na pasta em que for executado**. Execute-o a partir da raiz do repositório para usar o CSV de exemplo.

## Exemplo de uso

Listar (opção 2):

```
--------------------------------------------------------------------------------------------
| ID   | Produto                | Categoria        | Preco (R$) | Qtd   | Fornecedor       |
--------------------------------------------------------------------------------------------
| 1    | Camiseta Basica        | Vestuario        | 49.90      | 30    | Moda Brasil      |
| 2    | Mouse Optico USB       | Informatica      | 59.99      | 15    | TecnoShop        |
| 3    | Shampoo 300ml          | Higiene Pessoal  | 22.50      | 40    | Beleza Pura      |
| 4    | Cafe 500g              | Alimentos        | 18.00      | 25    | Graos do Sul     |
| 5    | Tenis Esportivo        | Vestuario        | 199.90     | 12    | Moda Brasil      |
| ...  | ...                    | ...              | ...        | ...   | ...              |
--------------------------------------------------------------------------------------------
```

Tentar cadastrar um nome repetido (opção 3):

```
Nome: cafe 500g
ERRO: o produto 'cafe 500g' ja esta cadastrado!
```

## Formato dos dados

Cada produto é uma linha do `loja_produtos.csv`, com campos separados por vírgula:

```
id,nome,categoria,preco,quantidade,fornecedor
1,Camiseta Basica,Vestuario,49.90,30,Moda Brasil
```

| Campo | Tipo em C | Observação |
|-------|-----------|------------|
| `id` | `int` | gerado automaticamente (maior ID existente + 1) |
| `nome` | `char[50]` | até 49 letras; não pode repetir (ignora maiúsculas) |
| `categoria` | `char[50]` | até 49 letras |
| `preco` | `float` | ponto como separador decimal (`49.90`) |
| `quantidade` | `int` | estoque, maior ou igual a zero |
| `fornecedor` | `char[50]` | até 49 letras |

Como a vírgula separa as colunas, qualquer vírgula digitada em um texto é trocada por espaço.

## Como funciona (resumo)

1. **Leitura:** `carregarProdutos` lê o CSV e monta um vetor de `Produto` na memória (`malloc`/`realloc`).
2. **Operação:** listar, buscar, alterar, excluir e ordenar trabalham sobre esse vetor, usando ponteiros.
3. **Gravação:** `inserirProduto` acrescenta uma linha com `fopen(..., "a")`. `alterarProduto` e `excluirProduto` regravam tudo em `temp.csv` e só depois substituem o original, para evitar corrupção.
4. **Ordenação:** o Bubble Sort ordena só o vetor em memória. O arquivo nunca é alterado.

Detalhes de cada função em [`docs/DOCUMENTACAO.md`](docs/DOCUMENTACAO.md).

## Testes

```bash
make test
# ou
bash tests/run_tests.sh
```

São 20 verificações (compilação sem avisos, criação, as 4 ordenações, ID automático, duplicata, validações de entrada, texto longo, vírgula, alterar, excluir, buscar e menu). Resultado em [`docs/TESTES.md`](docs/TESTES.md).

## Limitações conhecidas

- O ID é `maior ID + 1`: se o **último** produto for excluído, o próximo cadastrado reaproveita o ID dele.
- Textos com mais de 49 letras são cortados.
- Vírgulas digitadas viram espaço (limitação do formato CSV simples).
- Não há acentuação nos dados de exemplo, para a tabela alinhar em qualquer terminal. Acentos funcionam, mas podem desalinhar a tabela e depender da codificação do terminal.

## Integrantes

- [Nome do Integrante 1]
- [Nome do Integrante 2]
- [Nome do Integrante 3]

## Documentação adicional

- [`docs/DOCUMENTACAO.md`](docs/DOCUMENTACAO.md): funções e decisões de projeto
- [`docs/TESTES.md`](docs/TESTES.md): cenários de teste e resultado
- [`docs/ROTEIRO_APRESENTACAO.md`](docs/ROTEIRO_APRESENTACAO.md): roteiro da apresentação (8 minutos, 3 integrantes)
- [`docs/tabela_loja.txt`](docs/tabela_loja.txt): tabela de dados em texto
