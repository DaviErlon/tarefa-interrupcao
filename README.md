# tarefa-interrupcao
## Descrição: 
Este é um repositório dedicado a uma tarefa sobre interrupções do projeto EmbarcaTech. A compilação
deve seguir as instruções do arquivo CMakeLists.txt. Em suma, este código mostra os números de 0 a 9
em uma matriz de LEDs, o botão A incrementa o valor que começa em 0, e o botão B decrementa, dentro dos
limites de 0 e 9, enquanto um LED vermelho pisca 5 vezes por segundo.

---
## Periféricos:
Placa de desenvolvimento BitDogLabe, configurada com o Raspbery Pi Pico W e com estes periféricos conectados à placa:
  1. GPIO 5: Push-Button A
  2. GPIO 6: Push-Button B
  3. GPIO 7: Matriz de LEDs 5x5, modelo NeoPixel.
  4. GPIO 13: Pino vermelho de um LED RGB modelo padrão.
---

## Breve Descrição de Software:
Foi utilizado uma máquina de estado da PIO0 do chip rp2040 para a comunicação com a matriz de LEDs,
um sistema de interrupção ao assionar os botões - uma caída de sinal - com um debounce via software.
A organização para a escrita de números na matriz segue a seguinte forma:
  - LEDs NeoPixel número 1, 2, 3, 8, 6, 11, 12, 13, 18, 16, 21, 22 e 23 dão a forma para os números.
  - Um binário que se encontra em hexadecimal no arquivo C é lido com técnicas de bitmask
  e da a instrução para esses LEDs, isto é, o bit\[0\] é responsável pelo LED 1, o bit\[1\] pelo LED 2
  o bit\[2\] pelo LED 3, o bit\[3\] pelo LED 8, e assim por diante seguindo a ordem dos LEDs citada acima.
