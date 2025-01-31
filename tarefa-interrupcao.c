#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812b.pio.h"

// macros que representam as GPIO
#define LED_COUNT 25
#define LED_PIN 7
#define LED_VERMELHO 13
#define BOTAO_A 5
#define BOTAO_B 6

/*
variaveis tratadas na interrupção do código
e por isso marcadas como volatile, para o compilador
não otimizar seu uso
*/ 
static volatile uint32_t tempo_antes = 0;
static volatile int8_t numero = 0;

/*
variaveis da PIO declaradas globalmente para
as funções que utilizem PIO ser possivel nos
escopos de outras funções
*/ 
static PIO pio;
static uint sm;

// estrutura que armazena dados para um LED
typedef struct{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} led;

// buffer dedicado aos LEDs e inicializa todos com 0
volatile led matriz_led[LED_COUNT] = {0};

// função que une os dados binários dos LEDs para emitir para a PIO
uint32_t valor_rgb(uint8_t B, uint8_t R, uint8_t G){
  return (G << 24) | (R << 16) | (B << 8);
}

// função que envia dados ao buffer
void set_led(uint8_t indice, uint8_t r, uint8_t g, uint8_t b){
    if(indice < 25){
    matriz_led[indice].R = r;
    matriz_led[indice].G = g;
    matriz_led[indice].B = b;
    }
}

// função que limpa o buffer
void clear_leds(){
    for(uint8_t i = 0; i < LED_COUNT; i++){
        matriz_led[i].R = 0;
        matriz_led[i].B = 0;
        matriz_led[i].G = 0;
    }
}

// função que manda todos os dados do buffer para os LEDs
void print_leds(){
    uint32_t valor;
    for(uint8_t i = 0; i < LED_COUNT; i++){
        valor = valor_rgb(matriz_led[i].B, matriz_led[i].R,matriz_led[i].G);
        pio_sm_put_blocking(pio, sm, valor);
    }
}

void atualizar(){
    clear_leds();
    // numeros dos LEDs que serão utilizados para escrever os números
    const uint8_t leds_usados[13] = {1,2,3,8,6,11,12,13,18,16,21,22,23};

    /*
    Utilizei de uma tecnica para utilizar o minimo de memoria possível,
    guardando informações nos bits de um array de tipo uint16_t,
    no entanto neste projeto não se torna tão necessário fazer isso e só
    fiz por exercício pessoal, resumidamente converti uma sequencia de 13
    digitos binarios que formam os desenhos dos numeros para hexadecimal 
    */
    const uint16_t desenho[10] = {
        0x1FBF, // 0
        0x529,  // 1
        0x1DF7, // 2
        0x1DEF, // 3
        0x17E9, // 4
        0x1EEF, // 5
        0x1EFF, // 6 
        0x1D29, // 7
        0x1FFF, // 8
        0x1FEF  // 9
    };
    uint16_t temp = desenho[numero];

    /*
    Aqui o código verifica o bit[0] com um bitmask, se for 1
    ele acende o led, se for 0 ele não faz nada, depois ele 
    arrasta todos os bits para a esquerda de modo que agora
    bit[1] é o bit[0] e repete a verificação até acabar os 13 bits
    */
    for(uint8_t i = 0; i < 13; i++){
        if(temp & 1){
        matriz_led[leds_usados[i]].G = 10;
        }
        temp = temp >> 1;
    }

    // finaliza mostrando os dados do buffer na tela
    print_leds();
}

// função que inicializa os pinos GPIO para os propositos desejados
void config(){
    gpio_init(LED_VERMELHO);
    gpio_init(BOTAO_A);
    gpio_init(BOTAO_B);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_pull_up(BOTAO_B);
}

// função que será chamada em uma interrupção
void gpio_callback(uint gpio, uint32_t events) {

    //tratamento de debounce dos botões
    uint32_t tempo_agora = to_ms_since_boot(get_absolute_time());
    if (tempo_agora - tempo_antes > 200){
        if(gpio == BOTAO_A && (numero != 9)){
            numero++;
            atualizar();
        } 
        
        else if (gpio == BOTAO_B && (numero != 0)){
            numero--;
            atualizar();
        }

        tempo_antes = tempo_agora;
    }
}

int main(){

    //configurações iniciais do botão e led vermelho
    config();

    //configurações da matriz de leds
    pio = pio0; 
    bool ok = set_sys_clock_khz(128000, false);
    uint offset = pio_add_program(pio, &ws2812b_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812b_program_init(pio, sm, offset, LED_PIN);

    // configurações das interrupções
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // já começa o código mostrando o número 0
    atualizar();

    while(true){
    // aqui está um led que continuamente pisca 5 vezes por segundo
        gpio_put(LED_VERMELHO, true);
        sleep_ms(100);
        gpio_put(LED_VERMELHO, false);
        sleep_ms(100);
    }

    return 0;
}


