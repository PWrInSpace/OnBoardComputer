#ifndef XBEE_HH
#define XBEE_HH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usart.h"

#define START_BYTE 0x7e
#define FRAME_TYPE 0x10
#define FRAME_ID 0x00 //00 - wylaczenie , 01 - wlaczenie transmit status/odpowiedzi
#define BORDCAST_RADIUS 0x00
#define OPTIONS 0x00
#define BT_1 0xff
#define BT_2 0xfe


/*****************  WYSYLANIE ****************/

typedef struct{
	uint8_t options[7]; //tablica z opcjami

	uint8_t length[2];  //tablica z wylicznona dlugoscia

	uint8_t address[8]; //Tablica z adresem

	uint8_t *data;      //Wskaznik na tablice z danymi
	int data_length;	//Dlugosc danych

	uint8_t checksum;   //checksuma

	UART_HandleTypeDef *huart; //Uart który jest podłączony do xbee
}Xbee;

/************************************************************************************
 * inicjaliacja xbee
 * *sensor - adres do struktury xbee
 * address - 16 bajtowy adres xbee podany w HEX (można go znaleźć na spodzie xbee)
 * *uart - adress uart który ubsługuje xbee
 ***********************************************************************************/
void xbee_init(Xbee *sensor, uint64_t address, UART_HandleTypeDef *uart);

/***********************************************************************************
 * wysłanie danych
 * sensor - struktura xbee
 * data - dane jakie mają zostać wysłane
 ************************************************************************************/
void xbee_transmit(Xbee sensor, float data);

/**
 * Funckja wysyłająca tablice char
 *
 * sensor struktura xbee do ktorej chcemy wyslac dane
 * data tablica danych
 * */
void xbee_transmit_char(Xbee sensor, char *data);

/**
 * Wyslanie danych z odpowiedzą zwrotna i sprawdzenie odpowiedzi
 *
 * sensor - wskaznik do struktury xbee do ktorej chcemy wyslac wiadomosc
 * data - dane jakie chemy wyslac (int)
 *
 * zwraca: 1 - otrzymano potwierdzenie ze wszystko jest git, 0 - cos jest nie tak
 */
_Bool xbee_transmit_with_response(Xbee *sensor, char *data);



/**********    PROSZE NIE UŻYWAĆ TYCH FUNKCJI  ***********/

//init oopcji
void xbee_options_init(uint8_t *options);

//zamiana danych na tablice
void xbee_data_to_array(float data, Xbee *sensor);

//obliczenie długości wiadomości
void xbee_length(Xbee *sensor);

//wyliczenie checksum
void xbee_checksum_count(Xbee *sensor);

//funkcja wysyłająca całą wiadomość
void xbee_send(Xbee *sensor);

//funkcja wczytujaca tablice do strutkury
void xbee_char_load(char *data, Xbee *sensor);

/********************************************************/




/*****************  ODBIERANIE ****************/

#define DATA_LENGTH 80
#define DATA_ARRAY 60

typedef struct {
  int length; //Obliczona dlugosc

  char mess_loaded[DATA_LENGTH];  //tablica z całą wiadomośćią (bez znaku początku i długości)

  char data_array[DATA_ARRAY];	//Wyciągniete same dane
  _Bool data_flag;
  uint8_t checksum;

  //int response_mess; // logiczna, mowi czy ta paczka to odpowiedz zwrotna
  int response;	     // jaki jest stan odpowiedzi, 1 - wszystko git, 0 - cos nie tak

  //Zmienne do wyciągania danych narazie niech jeszcze zostana
  //int data;

}Xbee_Receive;

//deklaracja struktury do odbierania
//musi być globalna dlatego siedzi to tutaj
Xbee_Receive xbee_rx;



/********************************************************
 * Odebranie danych
 * do przerwania
 ********************************************************/
void xbee_receive();


/**********    PROSZE NIE UŻYWAĆ TYCH FUNKCJI  ***********/

//czyszczenie wszystkich danych
void xbee_clear();

//Czyści tablice statyczne (te które trzymają to co przyszło
void xbee_clear_array(char *array, int length);

//Przekształcenie danych z tablicy na float (narazie jest na inta)
float xbee_data_convert(char *tab);

//Funkcja potegująca potrzebna do przetworzenia danych z tablicy
float xbee_pow(int x);


/**********************************************************/
#endif
