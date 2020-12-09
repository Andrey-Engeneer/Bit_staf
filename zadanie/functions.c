#include "functions.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h> 
#include <math.h> 

////////////Вызов функций//////////////////////

void converter(char *in_name,char *out_name)
{
	struct input_package ipackage={.line=0};
	struct output_package opackage={.shift=0};

	///////////////////////////////////////////// Запись флага в начало 

	char flag=0x7e;
	FILE *fp;
		if ((fp = fopen(out_name, "w")) == NULL)
		{
			printf("Не удалось открыть файл");
			getchar();
		}
    fwrite(&flag,1, 1, fp);  
	fclose(fp);
	fp=NULL;


	//////////////////////////// Подсчёт строк  
		 
	int lines_count=0;
	FILE *file;

		if ((file = fopen(in_name, "r")) == NULL)
		{
			printf("Не удалось открыть файл");
			getchar();
		}

		while (! feof(file))
		{
			if (fgetc(file) == '\n')
			lines_count++;
		}

	fclose(file);
	
	int i=0;
	while(i<lines_count)//количество строк
	{
	
///////////////////////////// Проверка на корректность сообщения(hex-строки)
	if(read_file(&ipackage,in_name)==0)
			{
				printf("Сообщение №%d",i+1);
				printf(" в пакете некорректно: недопустимое количество символов.Сообщение пропущено. \n");
				i++;
				continue;
			}
	
		if(transformation(&ipackage,&opackage)==0)
		{
			printf("Сообщение №%d",i+1);
			printf(" в пакете некорректно: присутствует недопустимый символ.Сообщение пропущено.\n");
			i++;
			continue;
		}

	bit_stuffing(&opackage);
	write_file(&opackage,out_name);
	i++;
	}

	if(opackage.shift!=0)
	{
		flag=flag<<(8-opackage.shift);	

		FILE *f;
		if ((f = fopen(out_name, "a")) == NULL)
		{
			printf("Не удалось открыть файл");
			getchar();
		}
		fwrite(&flag,1, 1, f);  
		fclose(f);
	}
	
}

///////////////Функция чтения////////////////////

int read_file(struct input_package *pack,char *name)
{ 
	char c;
	int koef=50;
	
	FILE *file;
		if ((file = fopen(name, "r")) == NULL)
		{
			printf("Не удалось открыть файл");
			getchar();
		}
	 
	pack->messages = (char *)calloc(koef,sizeof(char));
		if (pack->messages==0) // Если система не дала памяти под массив, выходим 
		{
			puts("Система не выделила память");
			exit(1);
			getchar();
		}
		
	int count_line=0;	
	int i=0; 
	while ((c = fgetc(file)) != EOF)
	{
		if(count_line==pack->line)
		{
			if (c == ('\n')) 
			{
				pack->length_line=i; 
				pack->line++;
				break;
			}
			pack->messages[i]=c;
			i++;
			if (c=='\r')//Исключаем случай "возврат коретки"
			{
				i--;
			}  	
		}
	
		if (c == ('\n')) 
		{
			count_line++;
		}	
	}
		if (pack->length_line==0 || pack->length_line%2!=0)
		{
			fclose(file);
			return 0;
		}
	pack->messages[pack->length_line]='\0';
	fclose(file);
	return 1;
}

////////////Трансформация сообщения////////////////

int transformation(struct input_package *in_pack,struct output_package *out_pack)
{
	int koef=500;
	out_pack->length_line=0;
	out_pack->messages = (char *)calloc(koef,sizeof(char));
	char two_symbol[2];

	if (out_pack->messages==0) // Если система не дала памяти под массив, выходим 
	{
		puts("Система не выделила память");
		exit(1);
		getchar();
	}	

	for (int i=0;i<=in_pack->length_line-1;i+=2)
	{
	two_symbol[0]=in_pack->messages[i];
	two_symbol[1]=in_pack->messages[i+1];

		if(fromHEXtoBIN(two_symbol, out_pack->messages,out_pack->length_line)==0)//Производим перевод в двоичную систему счисления
		{
			free(in_pack->messages);// очищение массива
			in_pack->messages= NULL; 
			free(out_pack->messages);
			out_pack->messages= NULL; 
			return 0;
		}
		
	out_pack->length_line++;
	}

	out_pack->messages[out_pack->length_line]='\0';
	free(in_pack->messages);
	in_pack->messages= NULL; 
	return 1;
}

////////////Бит-стаффинг////////////////
void bit_stuffing(struct output_package *out_pack)
{
	
	int koef=500;
	unsigned char* mas = (char *)calloc(koef,sizeof(char));
	unsigned char byte;
	unsigned char one=0x80;
	short int buff=0;
	unsigned char flag=0x7e;
	int count_one=0;
	int t=0;//номер элемента выходного массива
	int p=0;
	int k=out_pack->shift;//Количество сдвигов
	int i=0;//номер элемента входного массива
	out_pack->length_line++;
	mas[0]=flag<<(8-k);
	
	for(i=0;i<out_pack->length_line;i++)
	{
		byte=0;
		p=0;// количество сдвигов в смещенных битах

		if(count_one>=5)// проверка на переходящий доп ноль 
		{
			count_one=0;
			p++;
			k++;
		}
		
		for(int n=0;n<k-p;n++)// Запись непоместившихся на предыдущем шаге
		{
			if ((buff&(one>>n))!=0)// Подсчёт единиц идущих последовательно
				{count_one++;}
			else
				{count_one=0;}
		
			if (count_one==5)
				{
					count_one=0;
					byte+=(buff&one>>(n))>>p;
					k++;
					p++;
				}
			else
				{
					byte+=(buff&one>>(n))>>p;
				}
		}
		buff=0x00;
		
		for(int n=0;n<k;n++)// Запись непоместившихся на данном шаге
		{
			if(k<=8)
			{
				buff+=(out_pack->messages[i]&(0x01<<n))<<(8-k);
			}
			else
			{
				buff+=(out_pack->messages[i]&(0x0001<<n))<<(9-k);
			}
		}
		if (k>=8)
			{
				k-=8;
				mas[t]=byte;
				t++;
				out_pack->messages[i]=buff&0xff;
				out_pack->length_line++;
				buff=(one>>7&out_pack->messages[i])<<7;
				byte=0x00;

			}

		for(int n=0;n+k<8;n++)// Запись остатка массива
		{
		
			if ((out_pack->messages[i]&one>>(n))!=0)// Подсчёт единиц идущих последовательно
				{count_one++;}
			else
				{count_one=0;}
				
			if (count_one==5 && n+k<7)
				{	
					count_one=0;
					byte+=(out_pack->messages[i]&(one>>n))>>k;
					buff=buff>>1;
					buff+=(out_pack->messages[i]&(0x01<<k))<<(7-k);
					k++;
				}
			else
				{
					byte+=(out_pack->messages[i]&(one>>n))>>k;
				}
	
		}
		mas[t]+=byte;
		t++;
	}

	if (k==0)// Проверка на выход за пределы байта
	mas[out_pack->length_line-1]=0;
	
	mas[out_pack->length_line-1]+=flag>>k;
				
	for (i=0;i<out_pack->length_line;i++)
	{
		out_pack->messages[i]=mas[i];
	}
	out_pack->messages[out_pack->length_line]='\0';
	out_pack->shift=k;

} 


////////////Запись в файл////////////////
void write_file(struct output_package *out_pack,char *out_name)
{
FILE *fp;
	if ((fp = fopen(out_name, "a")) == NULL)
	{
		printf("Не удалось открыть файл");
		getchar();
	}

fwrite(out_pack->messages,out_pack->length_line,1,fp);
fclose(fp);
free(out_pack->messages);
}

////////////Перевод из 16-ной в 2-ую систему////////////////
int fromHEXtoBIN(char *two_symbol, unsigned char *bin, int i)
{   
for (int n=1;n>=0;n--)
{
     switch (two_symbol[n])
        {
            case '0':
            {
                bin[i] += 0x00<<(4*(1-n));
                break;
            }
            case '1':
            {
                bin[i] += 0x01<<(4*(1-n));
                break;
            }
            case '2':
            {
                 bin[i] += 0x02<<(4*(1-n));
                break;
            }
            case '3':
            {
                 bin[i] += 0x03<<(4*(1-n));
                break;
            }
            case '4':
            {
               bin[i] += 0x04<<(4*(1-n));
                break;
            }
            case '5':
            {
                 bin[i] += 0x05<<(4*(1-n));
                break;
            }
            case '6':
            {
               bin[i] += 0x06<<(4*(1-n));
                break;
            }
            case '7':
            {
                bin[i] += 0x07<<(4*(1-n));
                break;
            }
            case '8':
            {
                 bin[i] += 0x08<<(4*(1-n));
                break;
            }
            case '9':
            {
                 bin[i] += 0x09<<(4*(1-n));
                break;
            }
            case 'A':
            {
                 bin[i] += 0x0a<<(4*(1-n));
                break;
            }
            case 'B':
            {
                 bin[i] += 0x0b<<(4*(1-n));
                break;
            }
            case 'C':
            {
                bin[i] += 0x0c<<(4*(1-n));
                break;
            }
            case 'D':
            {
                 bin[i] += 0x0d<<(4*(1-n));
                break;
            }
            case 'E':
            {
                bin[i] += 0x0e<<(4*(1-n));
                break;
            }
            case 'F':
            {
                bin[i] += 0x0f<<(4*(1-n));
                break;
            }
            case 'a':
            {
                bin[i] += 0x0a<<(4*(1-n));
                break;
            }
            case 'b':
            {
                bin[i] += 0x0b<<(4*(1-n));
                break;
            }
            case 'c':
            {
                bin[i] += 0x0c<<(4*(1-n));
                break;
            }
            case 'd':
            {
                 bin[i] += 0x0d<<(4*(1-n));
                break;
            }
            case 'e':
            {
                bin[i] += 0x0e<<(4*(1-n));
                break;
            }
            case 'f':
            {
                bin[i] += 0x0f<<(4*(1-n));
                break;
            }
        	default: 
			{
			return 0;
			}
      }
}
  return 1;
}
