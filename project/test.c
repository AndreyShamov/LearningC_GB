#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void make_file();

/*

Функция создает тестовый файл <data.csv> с текстовым значением в формате yyyy;mm;dd;hh;mm;temperature

Функция позволяет получить значение температуры, приблизительно равное реальной температуре за соответствующий 
месяц

Из-за использования генератора случайных чисел и srand (time(NULL)) функция каждый запуск
генерирует уникальный файл (в частности, в функции заложен 15% шанс незаписи значений от датчика).

*/

int main (int argc, char **argv){

     make_file();
}


void make_file() {
    srand (time(NULL));
    int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31 ,31, 30, 31, 30, 31};       /* массив с количеством дней в каждом месяце */
    int temperature_array[121];                                                     /* массив среднегодовых температур */
    int ctr = 0;                                                                    /* счетчик для генерации массива среднегодовых температур */
    int temperature_tmp = -30;                                                      /* переменная для записи массива со значениями температуры */
    char string[22];                                                                /* строка формата yyyy;mm;dd;hh;mm;temperature, которая будет записываться в файл */
    int array_position = 20;                                                        /* переменная для определения номера элемента в массиве среднегодовых температур */
   
    string[0] = '2';
    string[1] = '0';
    string[2] = '2';
    string[3] = '1';
    string[4] = ';';

    while(ctr < 121) {                                                              /* записываем значения возможных температур в массив */

        if(ctr < 60) {                                                              /* записывает значения от - 30 до 29 */
            
            temperature_array[ctr] = temperature_tmp++;

        } else {

            temperature_array[ctr] = temperature_tmp--;                             /* записывает значения от 30 до -30 */
        }
        
        ctr++;
    }

    FILE *file = fopen("data.csv", "w+");

    if(file == NULL) {
        printf("Unable to create a file\n");
        return;

    }

    for(int month = 1; month <= 12; month++) {                                      /* цикл перебирает значение месяца, дней в месяце, часов и минут,
                                                                                    а также считает номер элемента в массиве с годовыми температурами */
            int day_temp;
            int hour_temp;
            int final_temp;
            int min_temp;
            int rndm;

            string[5] = (month/10 + '0');                                           /* записываем или перезаписываем соответствующие значения массива */
            string[6] = (month%10 + '0');
            string[7] = ';';

            if (month > 3) {                                                        /* с марта смещаем номер элемента в массиве с годовыми температурами, 
                                                                                    на основе этого номера рассчитывается температура в каждом из часов*/
                array_position += 10;

            }

        for (int days = 1; days <= days_in_month[month-1]; days++) {                /* счетчик дней */

                    string[8] = (days/10 + '0');
                    string[9] = (days%10 + '0');
                    string[10] = ';';

                    day_temp = array_position + 4 - (rand() % 8);                  /* определяем номер в массиве с годовыми температурами,
                                                                                    на основе которого рассчитывается часовая температура*/

                for(int hours = 0; hours < 24; hours++) {                           /* счетчик часов */
                    
                    string[11] = (hours/10 + '0');
                    string[12] = (hours%10 + '0');
                    string[13] = ';';                                                     

                    if ( (hours < 8) || (hours > 21) ) {                            /* определяем номер в массиве с годовыми температурами,
                                                                                    на основе которого рассчитывается "минутная" температура*/
                        hour_temp = day_temp - (3 + (rand() % 3));                  /* цикл учитывает изменение температуры в течение дня */
                   
                    } else if ( (hours < 12) || (hours > 18) ) {

                        hour_temp = day_temp - (1 + (rand() % 3)); 
                    } else {
                        
                        hour_temp = day_temp + (rand() % 3);  

                    }


                        for (int minutes = 0; minutes < 60; minutes++) {            /* счетчик минут */

                            if ( (rand()%100) > 16) {                               /* предположим, что ~15% данных не записались */

                                string[14] = (minutes/10 + '0');
                                string[15] = (minutes%10 + '0');
                                string[16] = ';';
                            
                                min_temp = hour_temp;
                                rndm = (rand()%100);

                                if (rndm > 96) {                            /* добавим шанс того, что тепаратура в текущей минуте повысится (понизится) */
                                
                                    min_temp += 1;

                                } else if (rndm < 3) {

                                    min_temp -= 1;

                                } else if( (rndm > 10) && (rndm < 13) ) {   /* некоторый щанс, что температура повысится(понизится) 
                                                                            со следующей минуты и до конца часа */
                                    hour_temp +=1;

                                } else if( (rndm > 50) && (rndm < 53) ) {

                                    hour_temp +=1;

                                }
                            
                                final_temp = temperature_array[min_temp];   /* определяем температуру, равную числу с соответствующим номером 
                                                                            в массиве годовых температур*/
                                if (final_temp < 0) {                       /* цикл определят порядок записи символов с строку */

                                    final_temp *= -1;
                                    string[17] = '-';
                                    string[18] = (final_temp/10 + '0');
                                    string[19] = (final_temp%10 + '0');
                                    string[20] = '\n';    
                                    string[21] = '\0';  

                                } else if (final_temp > 0)  {

                                    string[17] = (final_temp/10 + '0');
                                    string[18] = (final_temp%10 + '0');
                                    string[19] = '\n';     
                                    string[20] = '\0'; 

                                } else {
                                    string[17] = '0';
                                    string[18] = '\n';     
                                    string[19] = '\0';                                  
                                }

                                fprintf(file, string);
                            }     

                        }
                }
        }

    }
    fclose(file);
    return;
}


