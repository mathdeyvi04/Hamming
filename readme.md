# Hamming Code Communication System

Este projeto consiste em um sistema de comunicação digital completo para 
transmissão de dados, utilizando Códigos de Hamming para detecção e correção
de erros. A aplicação simula o comportamento de um canal real sujeito a ruído
branco (AWGN), permitindo analisar a eficiência da correção de erros mediante
métricas de Bit Error Rate (BER).

## Objetivo

O objetivo principal é validar a eficácia dos códigos corretores de erro
em ambientes ruidosos. O sistema carrega arquivos binários (imagens, 
textos, etc.), aplica Código de Hamming, submete os dados a 
interferências controladas e tenta recuperar a informação original 
de forma íntegra.

## Tecnologias e Conceitos Desenvolvidos

1. Gestão Eficiente de Memória (DataBuffer):

Diferente do uso convencional de vetores de booleanos, implementou-se um gerenciador de bits de baixo nível.

- Bitwise Operations: Acesso e modificação de bits individuais mediante máscaras e deslocamentos.

- Memory Reuse: Implementação de Deep Copy e métodos de cópia direta para evitar alocações dinâmicas excessivas em loops de alta frequência.

2. Codificação de Hamming

- Forma Sistemática: Geração da Matriz de Paridade $H$ automaticamente a partir da Matriz Geradora $G$.

- Decodificação por Síndrome: Algoritmo que identifica a posição exata do erro comparando o vetor recebido com o espaço nulo da matriz $H$ ($S = v \cdot H^T$).

- Otimização Eigen: Uso da biblioteca Eigen 3 para álgebra linear, otimizada com operações manuais para reduzir o consumo de memória de pico.

3. Simulação de Canal AWGN

Simulamos um canal real utilizando Ruído Branco Gaussiano Aditivo:

- Modulação BPSK: Mapeamento de bits $\{0, 1\}$ para níveis de tensão $\{-1V, +1V\}$.
- Controle de SNR (dB): Implementação da relação sinal-ruído através da distribuição normal de densidade de probabilidade.

## Como Utilizar

### `./(main ou main_without_codif) -SNR_DB -Entry_File_Name_or_Bit_Sequence`

Primeiro, escolhe-se a funcionalidade desejada, seja o teste em codificação de Hamming ou sem codificação.

O primeiro argumento deve ser numérico, correspondente ao SNR_db do sinal.

O segundo argumento pode ser o nome de um arquivo QUALQUER presente no mesmo diretório ou uma sequência de bits digitada pelo usuário.

## Resultados

Imagem original a sofrer os testes.

<img src="Figuras/exemplo.jpg" height="300" alt="Original">

| SNR_DB | BER         | Sem Codificação                        | Com Hamming                                                 |
|--------|-------------|----------------------------------------|-------------------------------------------------------------|
| 11.5   | 2.65697e-07 | BER(8.23661e-05) -> Já está Corrompida | <img src="Figuras/exemplo_codif_snr_11.5.jpg" height="300"> |
| 10     | 7.43952e-06 | Corrompida                             | <img src="Figuras/exemplo_codif_snr_10.jpg" height="300">   |
| 9      | 4.88883e-05 | Corrompida                             | <img src="Figuras/exemplo_codif_snr_9.jpg" height="300">    |
| 7      | 0.00137047  | Corrompida                             | Corrompida                                                  |

