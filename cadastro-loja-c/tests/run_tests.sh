#!/usr/bin/env bash
# ====================================================================
# Testes de ponta a ponta do programa loja.c
# Compila o programa, roda varios cenarios simulando o teclado e confere
# o conteudo do arquivo CSV e as mensagens exibidas.
# Tudo acontece em uma pasta temporaria: o CSV do repositorio nao e tocado.
# Uso: bash tests/run_tests.sh
# ====================================================================

RAIZ="$(cd "$(dirname "$0")/.." && pwd)"   # pasta raiz do repositorio
TMP="$(mktemp -d)"                          # pasta temporaria dos testes
trap 'rm -rf "$TMP"' EXIT                   # apaga a pasta ao terminar

# 1) Compilacao com todos os avisos ligados
if ! gcc -Wall -Wextra -std=c99 -o "$TMP/loja" "$RAIZ/loja.c"; then
    echo "FALHOU: o programa nao compilou"
    exit 1
fi
echo "PASS  compila sem avisos (-Wall -Wextra)"
cd "$TMP" || exit 1

ok=1      # conta os testes que passaram (o de compilacao ja conta)
falhas=0  # conta os testes que falharam

# Registra o resultado de um teste: check "descricao" <codigo de saida do teste>
check() {
    if [ "$2" -eq 0 ]; then echo "PASS  $1"; ok=$((ok + 1))
    else echo "FAIL  $1"; falhas=$((falhas + 1)); fi
}

# Envia cada argumento como uma linha digitada no teclado
rodar() { printf '%s\n' "$@" | ./loja > saida.txt 2>&1; }

linhas() { wc -l < loja_produtos.csv | tr -d ' '; }   # quantidade de linhas do CSV

# ---------- CRIAR ----------
rodar 1 0
[ "$(linhas)" -eq 12 ]; check "criar: arquivo nasce com 12 produtos" $?

# ---------- ORDENAR (o arquivo nao pode mudar) ----------
antes="$(cksum < loja_produtos.csv)"

rodar 7 1 0   # nome A-Z
primeiro="$(grep '^| [0-9]' saida.txt | head -1)"
[[ "$primeiro" == *"Arroz Branco 5kg"* ]]; check "ordenar nome A-Z: primeiro = Arroz Branco 5kg" $?

rodar 7 2 0   # nome Z-A
primeiro="$(grep '^| [0-9]' saida.txt | head -1)"
[[ "$primeiro" == *"Tenis Esportivo"* ]]; check "ordenar nome Z-A: primeiro = Tenis Esportivo" $?

rodar 7 3 0   # preco menor -> maior
primeiro="$(grep '^| [0-9]' saida.txt | head -1)"
ultimo="$(grep '^| [0-9]' saida.txt | tail -1)"
[[ "$primeiro" == *"Sabonete Neutro 90g"* && "$ultimo" == *"Teclado Mecanico"* ]]
check "ordenar preco crescente: Sabonete primeiro, Teclado por ultimo" $?

rodar 7 4 0   # preco maior -> menor
primeiro="$(grep '^| [0-9]' saida.txt | head -1)"
[[ "$primeiro" == *"Teclado Mecanico"* ]]; check "ordenar preco decrescente: primeiro = Teclado Mecanico" $?

[ "$antes" = "$(cksum < loja_produtos.csv)" ]; check "ordenar nao altera o arquivo" $?

# ---------- INSERIR ----------
rodar 3 "Mochila Escolar" "Vestuario" "89.90" "10" "Moda Brasil" 0
grep -q '^13,Mochila Escolar,Vestuario,89.90,10,Moda Brasil$' loja_produtos.csv
check "inserir: gera ID 13 automaticamente" $?

# ---------- DUPLICATA (ignora maiusculas) ----------
rodar 3 "MOCHILA ESCOLAR" 0
grep -q "ja esta cadastrado" saida.txt && [ "$(linhas)" -eq 13 ]
check "duplicata: nome repetido (maiusculo) e recusado" $?

# ---------- VALIDACAO DE ENTRADA ----------
rodar 3 "Garrafa Termica" "Utilidades" "abc" "-1" "35,90" "35.90" "x" "5" "Casa Util" 0
[ "$(grep -c 'Valor invalido' saida.txt)" -eq 4 ]
check "validacao: 3 precos invalidos e 1 quantidade invalida recusados" $?
grep -q '^14,Garrafa Termica,Utilidades,35.90,5,Casa Util$' loja_produtos.csv
check "validacao: depois dos erros, o produto valido e gravado" $?

# ---------- NOME LONGO NAO INVADE O CAMPO SEGUINTE ----------
longo="$(printf 'A%.0s' $(seq 1 70))"
rodar 3 "$longo" "Cat" "1.00" "1" "Forn" 0
grep -Eq '^15,A{49},Cat,1.00,1,Forn$' loja_produtos.csv
check "texto longo: cortado em 49 letras e sem invadir os outros campos" $?

# ---------- VIRGULA NO TEXTO NAO QUEBRA O CSV ----------
rodar 3 "Arroz, tipo 2" "Alimentos" "9.00" "3" "Graos do Sul" 0
[ -z "$(awk -F, 'NF != 6' loja_produtos.csv)" ]
check "virgula: todas as linhas continuam com 6 campos" $?

# ---------- ALTERAR (Enter mantem o valor atual) ----------
rodar 4 14 "" "" "39.90" "" "" 0
grep -q '^14,Garrafa Termica,Utilidades,39.90,5,Casa Util$' loja_produtos.csv
check "alterar: so o preco muda, o resto e mantido com Enter" $?

rodar 4 99 0
grep -q "nao encontrado" saida.txt; check "alterar: ID inexistente avisa" $?

# ---------- EXCLUIR ----------
rodar 5 16 0
! grep -q '^16,' loja_produtos.csv && [ "$(linhas)" -eq 15 ]
check "excluir: produto 16 removido e 15 linhas restam" $?

rodar 5 99 0
grep -q "nao encontrado" saida.txt; check "excluir: ID inexistente avisa" $?

# ---------- BUSCAR (parte do texto, ignora maiusculas) ----------
rodar 6 "ALIMEN" 0
[ "$(grep -c 'Alimentos' saida.txt)" -eq 3 ]
check "buscar 'ALIMEN': encontra os 3 produtos de Alimentos" $?

rodar 6 "zzz" 0
grep -q "Nenhum produto correspondente" saida.txt
check "buscar sem resultado: avisa" $?

# ---------- MENU ----------
rodar abc 42 0
[ "$(grep -c 'Valor invalido' saida.txt)" -eq 1 ] && grep -q "Opcao invalida" saida.txt
check "menu: letra e opcao inexistente nao quebram o programa" $?

# ---------- RESUMO ----------
echo "----------------------------------------"
echo "Passaram: $ok   Falharam: $falhas"
[ "$falhas" -eq 0 ]
