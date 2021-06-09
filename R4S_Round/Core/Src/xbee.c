#include "xbee.h"



//Wpisanie opcji api do tablicy
//Tutaj bez dynamicznych bo nie ma kiedy zwolnić tablicy
void xbee_options_init(uint8_t *options){
	options[0] = START_BYTE;
	options[1] = FRAME_TYPE;
	options[2] = FRAME_ID ;
	options[3] = BT_1;
	options[4] = BT_2;
	options[5] = BORDCAST_RADIUS;
	options[6] = OPTIONS;
}

//Zamiana adresu na tablice
//Ogólna inicjalizacja xbee
void xbee_init(Xbee *sensor, uint64_t address, UART_HandleTypeDef *uart){
	  uint8_t temp[8]; //tablica do wczytania oryginalego adresu;
	  int i, j;

	 sensor->huart = uart;

	  xbee_options_init(sensor->options); //Inicjalizacja opcji, raz wystarczy


	  //to idzie jeszcze poprawic do jednej petli
	  for(i=0; i<8; i++){
		  temp[i] = address;
		  sensor->checksum += temp[i];

		  address /= 0x100;
	  }

	  //Wpisanie adresu do tablicy dynamicznej
	  for(i=7, j=0; i>=0; i--, j++){
		  sensor->address[j] = temp[i];
	  }
}

//Zamienia dane z int na tablice i zwraca dlugosc tablicy
void xbee_data_to_array(float data, Xbee *sensor){
	char temp[10];
	//Rzutowanie danych na inty i przemnozenie o dokladnosc (w odwrotnej kolejnosci)
	int convert_data = (int) data;
	sprintf(temp, "%d", convert_data);

	//stworzenie tablicy dynamicznej
	sensor->data_length = strlen(temp);
	sensor->data = (uint8_t*)malloc(sensor->data_length * sizeof(uint8_t));


	for(int i=0; i<sensor->data_length; i++)
		sensor->data[i] = temp[i];

}

//Obliczenie długości i wpisanie jej do tablicy
void xbee_length(Xbee *sensor){
	//Pierwszy element dlugosci to 0;
	int l;
	l = 14 + sensor->data_length;

	//Tworzymy tablice i wpisujemy wartosci
	sensor->length[0] = l/100;
	sensor->length[1] = l;
}

//Obliczenie chceksumy
void xbee_checksum_count(Xbee *sensor){
	int i=0;
	for(i=1; i<7; i++){
		sensor->checksum += sensor->options[i];
	}
	for(i=0; i<sensor->data_length; i++){
		sensor->checksum += sensor->data[i];
	}
	sensor->checksum = 0xff - sensor->checksum;

}

//Czy tutaj tworzyc tablice z cala wiadomoscia i wysłać całę odrazu
void xbee_send(Xbee *sensor){
	int i;

		//wyslanie znaku start
		HAL_UART_Transmit(sensor->huart, &sensor->options[0], 1, 100);

		//wyslanie dlugosci
		for(i=0; i<2; i++){
			HAL_UART_Transmit(sensor->huart, &sensor->length[i], 1, 100);
		}

		//wyslanie danych klatki
		HAL_UART_Transmit(sensor->huart, &sensor->options[1], 1, 100);
		HAL_UART_Transmit(sensor->huart, &sensor->options[2], 1, 100);

		//Wyslanie adresu
		for(i=0; i<8; i++){
			HAL_UART_Transmit(sensor->huart, &sensor->address[i], 1, 100);
		}

		//wyslanie reszty opcji
		for(i=3; i<7; i++){
			HAL_UART_Transmit(sensor->huart, &sensor->options[i], 1, 100);
		}

		//wyslanie danych
		for(i=0; i<sensor->data_length; i++){
			HAL_UART_Transmit(sensor->huart, (uint8_t *) &sensor->data[i], 1, 100);
		}

		//wyslanie checksumy
		HAL_UART_Transmit(sensor->huart, &sensor->checksum, 1, 100);


		//Czyszczenie pamięci
		free(sensor->data);
}

void xbee_transmit(Xbee sensor, float data){
	xbee_data_to_array(data, &sensor); //Zamiana danych z int na tablice
	xbee_length(&sensor);
	xbee_checksum_count(&sensor);
	xbee_send(&sensor);
}

//wczytanie podanych danych do struktury
void xbee_char_load(char *data, Xbee *sensor){
	sensor->data_length = strlen(data); //strlen niby powinien zadziałać bez
	sensor->data = (uint8_t*)malloc(sensor->data_length * sizeof(uint8_t));//

	for(int i=0; i<sensor->data_length; ++i){
		sensor->data[i] = data[i];
	}
}


void xbee_transmit_char(Xbee sensor, char *data){
	xbee_char_load(data, &sensor);
	xbee_length(&sensor);
	xbee_checksum_count(&sensor);
	xbee_send(&sensor);
}


////////////////////////////////////////////////////////////////////////////
/**************************************************************************/
////////////////////////////////////////////////////////////////////////////
//ODBIERANIE

//Zerowanie wartości
inline void xbee_clear(){
	xbee_clear_array(xbee_rx.mess_loaded, DATA_LENGTH);
	xbee_rx.length = 0;
	xbee_rx.checksum = 0;
}

//Zerowanie tablic
inline void xbee_clear_array(char *array, int length){
	for(int i=0; i<length; i++){
		array[i] = 0;
	}
}

//Poteowanie ktore jest potrzebne do konwersji
float xbee_pow(int x){
	int i;
	float result=1.00;
	for(i=0; i<x; i++){
		result *=10.00;
	}

	return result;
}

//konwersja danych z tablicy narazie do int, bazowo do float
float xbee_data_convert(char *tab){
	float result = 0;
	int i, max = xbee_rx.length - 12;

	for(i=0; i<max; i++){
		result += (tab[i] - 48) * xbee_pow(max - i - 1);
	}

    return result;
}


///zastanowić się jak sensownie zwracać dane
void xbee_receive(){
	xbee_clear_array(xbee_rx.data_array, DATA_ARRAY);
	xbee_rx.data_flag = 0;
	//ogarnięcie długości
	xbee_rx.length = (xbee_rx.mess_loaded[1] << 8) + xbee_rx.mess_loaded[2];
	//długość całej tablicy
	int mess_length = xbee_rx.length + 3; //3 bo dodajemy 4 elementy ale musimy odjąć jeden bo tablice sa numerowane od 0


	//sprawdzenie checksum
	for(int i=3; i<mess_length; i++){
		xbee_rx.checksum += xbee_rx.mess_loaded[i];
	}
	xbee_rx.checksum = 0xff - xbee_rx.checksum;
	//sprawdzenie czy checksum sie zgadza
	if(xbee_rx.checksum != xbee_rx.mess_loaded[mess_length]){
		//nie zgadza się czyścimy
		xbee_clear();
		return;
	}

	if(xbee_rx.length == 7){
		if(xbee_rx.mess_loaded[mess_length-1] == 0 && xbee_rx.mess_loaded[mess_length-2] == 0 && xbee_rx.mess_loaded[mess_length-3] == 0){
			xbee_rx.response = 1;
		}
	}else{ //element xbee_rx.mess_loaded[14] ma pierwszą dane aż do mess_length-1
		int k,i;
		for(i = 15, k = 0; i<mess_length; ++i, ++k){
			xbee_rx.data_array[k] = xbee_rx.mess_loaded[i];
		}
		xbee_rx.data_flag = 1;
		//debug
		//xbee_rx.data_array[k] = '\n';
		//xbee_rx.data = xbee_data_convert(xbee_rx.data_array);
	}
	xbee_clear();
}



/**************************************************/

_Bool xbee_transmit_with_response(Xbee *sensor, char *data){
	sensor->options[2] = 0x1; //Zmiana na wysylanie z potwierdzeniami

	xbee_transmit_char(*sensor, data);
	HAL_Delay(100);


	sensor->options[2] = 0x0; //Zmiana na wysylanie bez potwierdzenia


	if(xbee_rx.response){
		xbee_rx.response = 0;
		return 1;
	}else{
		xbee_rx.response = 0;
		return 0;
	}
	return 0;
}
