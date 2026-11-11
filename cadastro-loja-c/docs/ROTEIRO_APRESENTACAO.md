# Roteiro da apresentação (8 minutos, 3 integrantes)

Tempo estimado: cerca de **7min15s**, com folga até os 8 minutos. Cada integrante fala em 3 ou 4 slides. Troquem `[nome]` pelos nomes do grupo.

| Quem | Slides | Conteúdo | Tempo |
|------|--------|----------|-------|
| Integrante 1 | 1 a 4 | Capa, visão geral, struct e CSV, ponteiros e memória | ~2:25 |
| Integrante 2 | 5 a 7 | Inserir, alterar e excluir, buscar | ~2:30 |
| Integrante 3 | 8 a 10 | Bubble Sort, demonstração, fechamento | ~2:20 |

---

## Integrante 1

### Slide 1: Capa (0:20)
Bom dia, professor e pessoal. Somos [nome 1], [nome 2] e [nome 3] e vamos apresentar a Atividade 04: um cadastro de loja em C que junta estruturas de dados, ponteiros, manipulação de arquivos e ordenação.

Eu apresento a base do programa, o Integrante 2 mostra as operações do cadastro e o Integrante 3 fecha com a ordenação e a demonstração. Em cerca de sete minutos.

### Slide 2: Visão geral (0:35)
O programa é um caderno de loja que não se perde quando fechamos: cada produto vira uma linha do arquivo `loja_produtos.csv`.

O menu tem oito opções: criar o arquivo com dados de exemplo, listar, inserir, alterar, excluir, buscar, ordenar e sair.

Todas as entradas do teclado são validadas. Letra no lugar de número, ou um nome comprido demais, não quebra o programa.

Vamos ver primeiro como os dados são organizados.

### Slide 3: A ficha do produto (0:50)
Cada produto é uma `struct` com seis campos: id, nome, categoria, preço, quantidade e fornecedor, os mesmos da tabela do enunciado.

No disco, ele vira uma linha de texto com os campos separados por vírgula: um, Camiseta Basica, Vestuario, 49.90, 30, Moda Brasil.

Como a vírgula é o separador, a função `lerTexto` troca qualquer vírgula digitada por espaço. Senão um nome como "Arroz, tipo 1" viraria duas colunas e corromperia a linha.

E os textos têm no máximo 49 letras: o que passar disso é descartado com segurança, sem invadir o campo seguinte.

### Slide 4: Ponteiros e memória (0:40)
Aqui entram os ponteiros. A função `carregarProdutos` lê o arquivo e monta um vetor de `Produto` na memória, com `malloc` e `realloc`: quando o vetor enche, ele dobra de tamanho. Ela recebe um ponteiro para ponteiro, e é por isso que consegue devolver o vetor que criou.

Para gravar de volta, `salvarProdutos` escreve tudo em `temp.csv` e só então apaga o original e renomeia o temporário. Se der erro no meio, o arquivo original continua intacto.

E toda função termina com `free`, devolvendo a memória.

Agora passo para o Integrante 2, que mostra as operações.

---

## Integrante 2

### Slide 5: Inserir (0:55)
Obrigado. Vou mostrar as operações. Na inserção, o programa primeiro carrega os produtos já cadastrados e pergunta o nome.

A função `existeNome` compara sem diferenciar maiúscula de minúscula. Então tentar cadastrar "cafe 500g" quando já existe "Cafe 500g" dá erro de duplicata, como mostra a caixa de baixo, que é uma saída real do nosso teste.

Depois ele lê categoria, preço, quantidade e fornecedor, sempre validando: preço negativo ou com letra é recusado e a pergunta se repete.

O ID ninguém digita: `gerarNovoID` pega o maior ID do vetor e soma um. Por garantia, ainda conferimos se esse ID já existe.

Por fim, o `fopen` em modo "a" acrescenta a linha no final, sem apagar o resto.

### Slide 6: Alterar e excluir (0:50)
Para alterar e excluir usamos a técnica do arquivo temporário, pedida no enunciado.

Carregamos tudo no vetor, e a função `buscarPorId` devolve um ponteiro direto para o produto. Na alteração, mudamos os campos por esse ponteiro, e apertar Enter mantém o valor atual. O nome novo também é conferido contra os outros produtos, para não criar duplicata.

Na exclusão, calculamos a posição subtraindo ponteiros e puxamos os produtos seguintes uma casa para trás.

Nos dois casos, `salvarProdutos` grava tudo em `temp.csv` e só depois troca pelo original. Se o programa falhar no meio, o arquivo original continua inteiro. Essa é a proteção contra corrupção de dados.

### Slide 7: Listar e buscar (0:45)
A listagem percorre o vetor com ponteiros e imprime a tabela, igual à tabela de texto do enunciado.

A busca aceita parte do nome ou da categoria. Convertemos o termo e os campos para minúsculas com `paraMinusculo` e usamos `strstr` para procurar um dentro do outro.

No exemplo, buscar "ALIMEN" em maiúsculas encontra o Café, da categoria Alimentos. Se nenhum produto combina, o programa avisa. E um termo vazio é recusado, porque senão ele listaria tudo.

Agora o Integrante 3 explica a ordenação.

---

## Integrante 3

### Slide 8: Bubble Sort (1:00)
Obrigado. A ordenação usa o Bubble Sort, como pede o enunciado. O vetor é carregado do arquivo e ordenado só na memória: o arquivo nunca é alterado.

O algoritmo compara vizinhos. Se o par está fora de ordem, a função `trocar` inverte os dois usando uma variável auxiliar. Cada rodada deixa o maior elemento no fim, por isso o laço interno encurta a cada volta.

Adicionamos uma flag chamada `trocou`: se uma rodada inteira passa sem nenhuma troca, a lista já está ordenada e paramos antes.

A função `deveTrocar` decide a regra de cada modo: nome de A a Z, de Z a A, preço do menor ao maior e do maior ao menor. Assim um único `bubbleSort` serve para os quatro.

### Slide 9: Demonstração (0:45)
*(ensaiar antes; o programa já deve estar compilado e aberto)*

Vamos à demonstração. Primeiro criamos o arquivo com os 12 produtos de exemplo, na opção 1, e listamos.

Depois inserimos um produto novo e tentamos repetir um nome para mostrar o erro de duplicata.

Em seguida ordenamos por preço, do menor ao maior: o Sabonete, de 3,49, aparece primeiro, e o Teclado, de 249, por último. Ao listar de novo, a ordem do arquivo continua a mesma.

Por fim, excluímos um produto e listamos para confirmar.

*Plano B se a demo travar: seguir pelos slides e mostrar a tabela do slide como resultado esperado.*

### Slide 10: Fechamento (0:35)
Para fechar: o programa cumpre o que o enunciado pediu. ID automático, verificação de duplicatas, as funções de criar, listar, inserir, alterar, excluir e buscar, ordenação por nome e por preço com Bubble Sort, `struct` com vetor e ponteiros, arquivo temporário e CSV com vírgula, tudo comentado linha a linha.

Também somos honestos sobre os limites: se o último produto for excluído, o próximo reaproveita o ID dele, e textos com mais de 49 letras são cortados.

Obrigado! Ficamos à disposição para perguntas.

---

## Dicas de ensaio

- Ensaiem juntos pelo menos uma vez, cronometrando. A meta é terminar em 7 a 7:30.
- Antes de apresentar: `make`, depois `./loja` (a opção 1 já cria o arquivo com os 12 produtos).
- Cuidado com a opção 1 durante a demonstração: ela recria o arquivo e apaga o que foi inserido (pede confirmação).
- Quem for explicar `strcspn`, `sscanf` com `%49[^,]` e `realloc` deve reler o trecho no `loja.c` antes, porque o professor pode perguntar.
