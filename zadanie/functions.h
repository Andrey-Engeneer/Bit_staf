
#ifndef FUNCTIONS
#define FUNCTIONS
#include <stdio.h>
#include <stdlib.h>

struct input_package
{
	char *messages; // массив для считывания 
	int line;// номер строки	
	int length_line; //длина строки
};

struct output_package
{
	unsigned char *messages; // массив для записи 	
	int length_line; //длина строки
	int shift;//сдвиг бит
};
int fromHEXtoBIN(char *two_symbol, unsigned char *bin, int i);// Перевод из 16-ой в 2-ую систему
void converter(char *in_name,char *out_name); //Вызов функций
int read_file(struct input_package *pack,char *name);// чтение файла
int transformation(struct input_package *in_pack,struct output_package *out_pack); // прeoбразование сообщения
void bit_stuffing(struct output_package *out_pack);  //бит-стаффинг
void write_file(struct output_package *out_pack,char *out_name); // запись в файл 

#endif // FUNCTIONS

