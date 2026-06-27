# UberDrones v2.0 — Motor de Logistica Urbana

Trabalho Pratico da disciplina de **Estruturas de Dados** desenvolvido em linguagem **C**.

O sistema simula o motor logistico de uma empresa de entregas por drones, gerenciando
uma malha aerea de pontos de pouso e calculando as rotas de menor consumo de bateria.

---

## Estruturas de Dados Utilizadas

| Estrutura | Finalidade |
|---|---|
| **Grafo Ponderado Direcionado** (Lista de Adjacencia) | Representa os pontos de pouso (vertices) e as rotas de voo (arestas). O peso e o consumo de bateria. |
| **Fila Dinamica Encadeada (FIFO)** | Gerencia a frota de drones. Drones aguardam na fila e retornam ao final apos cada entrega. |
| **Algoritmo de Dijkstra** | Calcula o caminho de menor consumo de bateria entre dois pontos do mapa. |

---

## Estrutura do Projeto

```
ED1/
├── uberdrones.c   # Codigo-fonte principal
├── mapa.txt       # Arquivo de configuracao do mapa de voo
├── Makefile       # Automacao de compilacao
└── README.md      # Este arquivo
```

---

## Formato do Arquivo mapa.txt

A primeira linha contem o numero de pontos de pouso (vertices).
As linhas seguintes definem as rotas no formato `Origem Destino Bateria`.

```
4
0 1 15
0 2 30
1 3 10
2 3 25
```

> Uma rota de A para B **nao** implica rota de B para A (grafo direcionado).

---

## Como Compilar e Executar

### PowerShell (Windows) — sem instalacao extra

```powershell
gcc -Wall -Wextra -std=c99 -o uberdrones uberdrones.c
./uberdrones
```

### Git Bash / Linux / macOS — com Makefile

```bash
make          # compila o projeto
make run      # compila e executa
make clean    # remove o executavel
make rebuild  # limpa e recompila do zero
make help     # lista todos os comandos
```

---

## Menu do Sistema

```
=================================================
   SISTEMA LOGISTICO - UBERDRONES v2.0
=================================================
  1. Visualizar Mapa de Voo
  2. Visualizar Drones Disponiveis
  3. Lancar Novo Pedido de Entrega
  0. Encerrar Sistema
-------------------------------------------------
```

---

## Exemplo de Execucao

```
Carregando mapa de voo (mapa.txt)...
Mapa carregado: 4 ponto(s) de pouso.
Quantos drones compoem a frota? 3
Frota de 3 drone(s) inicializada com sucesso!
```

**Opcao 1 — Visualizar Mapa:**
```
+-------------------------------------+
|        MAPA DE VOO - ROTAS          |
+-------------------------------------+
  Ponto [0] -> [2 | bat:30] -> [1 | bat:15]
  Ponto [1] -> [3 | bat:10]
  Ponto [2] -> [3 | bat:25]
  Ponto [3] -> (sem saidas)
```

**Opcao 3 — Pedido de entrega (ponto 0 para ponto 3):**
```
  Ponto de Origem  (0 a 3): 0
  Ponto de Destino (0 a 3): 3

  [>>] Drone #1 designado para o pedido.
  [OK] Caminho encontrado!
  Percurso: [0] -> [1] -> [3]
  Consumo total de bateria: 25%
  Drone #1 reinserido na fila.
```

**Pedido sem rota disponivel (ponto 3 para ponto 0):**
```
  [>>] Drone #1 designado para o pedido.
  [XX] Nao ha rota disponivel de [3] para [0].
       Pedido cancelado. Drone #1 retorna a fila.
```

---

## Regras de Negocio

- Ao processar um pedido, o primeiro drone da fila e retirado e designado.
- O Dijkstra calcula o caminho de **menor consumo de bateria**.
- Se houver caminho valido: drone realiza o voo e volta ao **final** da fila.
- Se nao houver caminho: pedido e cancelado e drone volta ao **final** da fila.
- Ao encerrar (opcao 0), toda a memoria alocada e liberada.

---

## Requisitos

- Compilador **GCC** (MinGW no Windows)
- Arquivo `mapa.txt` na **mesma pasta** do executavel
- `make` instalado para usar o Makefile (opcional)
