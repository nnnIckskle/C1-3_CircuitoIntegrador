#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/* Segmentos */
#define SEG_A 5
#define SEG_B 4
#define SEG_C 19
#define SEG_D 25
#define SEG_E 26
#define SEG_F 33
#define SEG_G 32
#define SEG_P 14

/* Displays */
#define DISP1 22
#define DISP2 21
#define DISP3 15
#define DISP4 2

/* Botones */
#define BTN_RUN   12
#define BTN_CLEAR 13

#define DELAY_DISPLAY 5
#define SCROLL_TIME 400

int segmentos[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_P};
int displays[]  = {DISP1, DISP2, DISP3, DISP4};

/* Matriz de letras */
uint8_t letras[27][8] = {

{1,1,1,0,1,1,1,0}, //A
{0,0,1,1,1,1,1,0}, //B
{1,0,0,1,1,1,0,0}, //C
{0,1,1,1,1,0,1,0}, //D
{1,0,0,1,1,1,1,0}, //E
{1,0,0,0,1,1,1,0}, //F
{1,0,1,1,1,1,0,0}, //G
{0,1,1,0,1,1,1,0}, //H
{0,0,0,0,1,1,0,0}, //I
{0,1,1,1,0,0,0,0}, //J
{0,1,1,0,1,1,1,0}, //K
{0,0,0,1,1,1,0,0}, //L
{1,0,1,0,1,0,0,0}, //M
{0,0,1,0,1,0,1,0}, //N
{1,1,1,1,1,1,0,0}, //O
{1,1,0,0,1,1,1,0}, //P
{1,1,1,0,0,1,1,0}, //Q
{0,0,0,0,1,0,1,0}, //R
{1,0,1,1,0,1,1,0}, //S
{0,0,0,1,1,1,1,0}, //T
{0,1,1,1,1,1,0,0}, //U
{0,1,1,1,1,1,0,0}, //V
{0,1,0,1,0,1,0,0}, //W
{0,1,1,0,1,1,1,0}, //X
{0,1,1,1,0,1,1,0}, //Y
{1,1,0,1,1,0,1,0}, //Z
{0,0,0,0,0,0,0,0}  //ESPACIO

};

/* Convierte caracter a índice */
int obtenerIndice(char c){

    if(c>='a' && c<='z'){
        c = c - 32;
    }

    if(c>='A' && c<='Z'){
        return c - 'A';
    }

    return 26;
}

/* Apaga todos los displays */
void apagarDisplays(){

    for(int i=0;i<4;i++){
        gpio_set_level(displays[i],1);
    }

}

/* Mostrar letras en el display */
void actualizarDisplay(int valores[4]){

    for(int i=0;i<4;i++){

        apagarDisplays();

        for(int j=0;j<7;j++){
            gpio_set_level(segmentos[j], letras[valores[i]][j]);
        }

        gpio_set_level(displays[i],0);

        vTaskDelay(pdMS_TO_TICKS(DELAY_DISPLAY));
    }

}

/* Inicializar GPIO */
void inicializarHardware(){

    for(int i=0;i<8;i++){

        gpio_reset_pin(segmentos[i]);
        gpio_set_direction(segmentos[i], GPIO_MODE_OUTPUT);

    }

    for(int i=0;i<4;i++){

        gpio_reset_pin(displays[i]);
        gpio_set_direction(displays[i], GPIO_MODE_OUTPUT);
        gpio_set_level(displays[i],1);

    }

    gpio_set_direction(BTN_RUN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_RUN, GPIO_PULLUP_ONLY);

    gpio_set_direction(BTN_CLEAR, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_CLEAR, GPIO_PULLUP_ONLY);

}

/* Lectura de botones */
void controlarBotones(bool *estado, int *posicion){

    if(gpio_get_level(BTN_RUN)==0){

        *estado = !(*estado);

        vTaskDelay(pdMS_TO_TICKS(250));
    }

    if(gpio_get_level(BTN_CLEAR)==0){

        *posicion = 0;

        vTaskDelay(pdMS_TO_TICKS(250));
    }

}

void app_main(){

    inicializarHardware();

    char mensaje[] = " DELTORO MORALES NAVARRO OSPINO ";

    int longitud = strlen(mensaje);

    int posicion = 0;

    bool activo = true;

    TickType_t tiempoAnterior = 0;

    while(1){

        controlarBotones(&activo,&posicion);

        TickType_t tiempoActual = xTaskGetTickCount();

        if(activo && (tiempoActual - tiempoAnterior >= pdMS_TO_TICKS(SCROLL_TIME))){

            tiempoAnterior = tiempoActual;

            posicion++;

            if(posicion >= longitud){
                posicion = 0;
            }

        }

        int buffer[4];

        for(int i=0;i<4;i++){

            buffer[i] = obtenerIndice(mensaje[(posicion+i)%longitud]);

        }

        actualizarDisplay(buffer);

    }

}