# DS Farm V6

V6 do protótipo original de fazenda para Nintendo DS/DSi.

## Novidades
- Fazenda + Vila.
- Troca de área com L.
- Estruturas simples: casas e loja.
- NPC/diálogo simples pelo botão R.
- Inventário básico pelo X.
- Sistema de dinheiro e sementes.
- Plantar e regar.
- Crescimento por dias.
- Colheita.
- Energia.
- Salvamento básico em `dsfarm.sav` no diretório de execução.
- Duas telas: mundo na superior e informações na inferior.
- Pixel art desenhada em C, sem assets de jogos comerciais.

## Compilar
No PowerShell:

```powershell
& "C:\devkitPro\msys2\usr\bin\bash.exe" -lc 'export PATH="/c/devkitPro/devkitARM/bin:$PATH"; cd /c/Users/julia/Downloads/DS_Farm_V6_source; make clean; make'
```

Resultado:
`ds_farm_v6.nds`

Coloque o `.nds` em `roms/nds/` do cartão usado pelo TWiLight Menu++.

## Controles
D-pad = mover
A = plantar/regar
B = colher/fechar menus
Y = trocar ferramenta
X = inventário
L = trocar entre Fazenda/Vila
R = falar com NPC
START = dormir, avançar dia e salvar

## Observação
É uma V6 de protótipo. O salvamento usa arquivo simples e os gráficos ainda são formas pixel-art geradas pelo código.
