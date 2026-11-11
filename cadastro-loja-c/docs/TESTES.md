# Testes

Os testes são de **ponta a ponta**: o script `tests/run_tests.sh` compila o programa, simula o que o usuário digitaria no teclado e confere o conteúdo do arquivo `loja_produtos.csv` e as mensagens na tela. Tudo roda em uma pasta temporária, sem alterar o CSV do repositório.

## Como rodar

```bash
make test
# ou
bash tests/run_tests.sh
```

Precisa de `gcc` e `bash` (no Windows, use Git Bash ou WSL).

## Cenários cobertos

| # | Cenário | O que é conferido |
|---|---------|-------------------|
| 1 | Compilação | compila com `-Wall -Wextra -std=c99` sem nenhum aviso |
| 2 | Criar arquivo | o arquivo nasce com 12 produtos |
| 3 | Ordenar nome A–Z | primeiro é *Arroz Branco 5kg* |
| 4 | Ordenar nome Z–A | primeiro é *Tenis Esportivo* |
| 5 | Ordenar preço crescente | *Sabonete* primeiro e *Teclado* por último |
| 6 | Ordenar preço decrescente | primeiro é *Teclado Mecanico* |
| 7 | Ordenar não altera o arquivo | o checksum do CSV é o mesmo antes e depois |
| 8 | Inserir | recebe o ID 13 automaticamente |
| 9 | Duplicata | "MOCHILA ESCOLAR" é recusado por já existir "Mochila Escolar" |
| 10 | Validação de entrada | 3 preços inválidos (`abc`, `-1`, `35,90`) e 1 quantidade inválida (`x`) são recusados |
| 11 | Depois dos erros | o produto válido é gravado corretamente |
| 12 | Texto longo | nome de 70 letras é cortado em 49 sem invadir os outros campos |
| 13 | Vírgula no texto | todas as linhas do CSV continuam com 6 campos |
| 14 | Alterar | só o preço muda; Enter mantém o resto |
| 15 | Alterar ID inexistente | o programa avisa |
| 16 | Excluir | o produto some e o arquivo fica com uma linha a menos |
| 17 | Excluir ID inexistente | o programa avisa |
| 18 | Buscar `ALIMEN` | encontra os 3 produtos da categoria Alimentos |
| 19 | Buscar sem resultado | o programa avisa |
| 20 | Menu | letra e opção inexistente não quebram o programa |

## Resultado da última execução

```
PASS  compila sem avisos (-Wall -Wextra)
PASS  criar: arquivo nasce com 12 produtos
PASS  ordenar nome A-Z: primeiro = Arroz Branco 5kg
PASS  ordenar nome Z-A: primeiro = Tenis Esportivo
PASS  ordenar preco crescente: Sabonete primeiro, Teclado por ultimo
PASS  ordenar preco decrescente: primeiro = Teclado Mecanico
PASS  ordenar nao altera o arquivo
PASS  inserir: gera ID 13 automaticamente
PASS  duplicata: nome repetido (maiusculo) e recusado
PASS  validacao: 3 precos invalidos e 1 quantidade invalida recusados
PASS  validacao: depois dos erros, o produto valido e gravado
PASS  texto longo: cortado em 49 letras e sem invadir os outros campos
PASS  virgula: todas as linhas continuam com 6 campos
PASS  alterar: so o preco muda, o resto e mantido com Enter
PASS  alterar: ID inexistente avisa
PASS  excluir: produto 16 removido e 15 linhas restam
PASS  excluir: ID inexistente avisa
PASS  buscar 'ALIMEN': encontra os 3 produtos de Alimentos
PASS  buscar sem resultado: avisa
PASS  menu: letra e opcao inexistente nao quebram o programa
----------------------------------------
Passaram: 20   Falharam: 0
```

## Teste manual rápido

1. `make run`
2. Opção `2`: confere a tabela com 12 produtos.
3. Opção `3`: cadastra um produto novo e confere o ID gerado.
4. Opção `3` de novo, repetindo o nome: deve dar erro de duplicata.
5. Opção `7`, modo `3`: confere a ordem por preço. Opção `2` em seguida: a ordem do arquivo continua a mesma.
6. Opção `5`: exclui um produto e confere com a opção `2`.
7. Opção `0`: sai.
