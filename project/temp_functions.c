#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "temp_functions.h"

/*
Функция создает массив с данными по каждому месяцу и передает этот массив следующей функции для заполнения.
После заполнения данных по каждому месяцу функция обобщает их и выводит на экран.
Функция принимает ссылку на открытый файл, строку со значением года в формате yyyy;.
*/
void year_temp (FILE *file, char *year_str) {

    char desired_value[STRING_LENGHT];                   /* переменная для записи значения в формате yyyy;mm; и передачи ее далее */
    data_month_t month_data[12] = { 0 };                 /* создаем массив структур с информацией по каждому месяцу */
    char first_digit;                                    /* первый символ для записи значения месяца в строку */
    char second_digit;                                   /* второй символ для записи значения месяца в строку */
    float av_temp = 0;                                   /* переменная для рассчита среднегодовой температуры */
    int number_of_measurments = 0;                       /* переменная для рассчета количества месяцев, по которым есть данные */
    int max_temp[12] = { 0 };                            /* массив для записи максимальных температур по месяцам */
    int min_temp[12] = { 0 };                            /* массив для записи минимальных температур по месяцам */
    int max = 0;                                         /* переменная для записи максимальной годовой температуры */
    int min = 0;                                         /* переменная для записи минимальной годовой температуры */
    
    strcpy(desired_value, year_str);                     /* копируем полученное при вызове функции значение года в другую строку */

    for (int month = 1; month <= 12; month++) {          /* счетчик количества месяцев для заполения строки */

        if ( month < 10 ) {                              /* если число месяцев имеет один разряд (т.е. меньше 10) */

            first_digit = '0';                           /* записываем 0 в первый разряд */
            second_digit = (char) month + '0';           /* записываем значения числа месяца во второй разряд */

        } else {                                         /* если число месяцев имеет два разряда (т.е. больше 10) */

            first_digit = (char) month / 10 + '0';       /* записываем значение десятков в первый разряд */
            second_digit = (char) month % 10 + '0';      /* записываем значение десятков в первый разряд */
        }

        desired_value[5] = first_digit;                  /* записываем первый разряд месяца в строку */
        desired_value[6] = second_digit;                 /* записываем второй разряд месяца в строку */
        desired_value[7] = ';';                          /* добавляем разделитель */
        desired_value[8] = '\0';                         /* добавляем знак конца строки */

        month_data[month-1] = month_temp (file, desired_value, month);     /* заполняем структуру */

        if ( month_data[month-1].days_ctr > 0 ) {                          /* если в массив были записаны данные о месячной температуре (т.е. количество найденных измерений больше 0) */
            
            number_of_measurments++;                                       /* увеличиваем количество найденных месячных измерений */
            av_temp += month_data[month-1].average_temp;                   /* прибавляем к сумме слагаемых среднюю температуру за месяц */
            max_temp[month-1] = month_data[month-1].max;                   /* записываем в массив значения максимальной температуры в определенном месяце */
            min_temp[month-1] = month_data[month-1].min;                   /* записываем в массив значения минимальной температуры в определенном месяце */
       
        } else {                                                           /* если месячных измерений не найдено, выводим ошибку в лог-файл */

            char error_str[STRING_LENGHT];
            error_str[0] = 'X';
            error_str[1] = 'X';
            error_str[2] = '.';
            error_str[3] = desired_value[5];
            error_str[4] = desired_value[6];
            error_str[5] = '.';
            error_str[6] = desired_value[0];
            error_str[7] = desired_value[1];
            error_str[8] = desired_value[2];
            error_str[9] = desired_value[3];
            error_str[10] = '\0';

            errors(1, 0, error_str);                                       /* если полностью отсутствуют месячные данные, выводим ошибку в консоль */
            continue;
        }
    } 

    av_temp /= number_of_measurments;                                      /* находим среднегодовую температуру */

    int tmp_vol = 0;                                                       /* временная переменная для вызова функции max_and_min */
    max_and_min(max_temp, number_of_measurments, &max, &tmp_vol);          /* записываем максимальную температуру из массива в переменную max */
    max_and_min(min_temp, number_of_measurments, &tmp_vol, &min);          /* записываем минимальную температуру из массива в переменную min */

    /* вывод на экран данных за год и данных за каждый месяц */
    printf("Average annual temperature for %c%c%c%c year is %3.1fº Celsius.\n\n", desired_value[0], desired_value[1], desired_value[2], desired_value[3], av_temp);
    printf("Max temperature is %3d, Min temperature is %3d.\n\n", max, min);
    printf("Monthly statistics:\n\n");

    for (int i = 0; i < 12; i++) {                                         /* счетчик элементов в массиве структур месячных данных */

        char _month_name[10];
        month_name((i+1), _month_name);                                    /* запишем в строку название месяца в соответствии с его номером */

        printf("Average monthly temperature for %10s is ", _month_name);
        
        if ( month_data[i].days_ctr > 0 ) {                                /* если количество прочитанных и записанных значений больше 1 */

            printf("%3.1fº Celsius, max temperature is %d, min temperature is %d.\n", month_data[i].average_temp, month_data[i].max, month_data[i].min);

        } else {                                                           /* если в месяце прочитанных значений нет */

            printf("not defined in the entered file, see error_log for more details\n");

        }

    }
}

/* 
Функция записи массива с информацией о температуре по каждому месяцу.
Функция принимает ссылку на открытый файл, строку со значением в формате yyyy;mm;, 
номер месяца, данные по которому читаются.
Функция добавляет в строку значения дня, передает для заполнения структуру и строку в формате YYYY;mm;dd; ,
обрабатывает полученные данные и заполняет структуру с месячными данными.  
*/

data_month_t month_temp (FILE *file, char *desired_value, int month) {

    data_month_t month_data = { 0 };                                            /* структура с месячной информацией (возвращается) */
    data_day_t day_data = { 0 };                                                /* структура с информацией по каждому дню месяца  (перезаписывается) */
    char first_digit;                                                           /* первый символ для записи значения дня в строку */
    char second_digit;                                                          /* второй символ для записи значения дня в строку */
    int days_in_month[13] = {0, 31, 28, 31, 30, 31, 30, 31 ,31, 30, 31, 30, 31};   /* массив с информацией о количестве дней в месяце (в феврале 28 дней) */

    
    for (int day = 1; day <= days_in_month[month]; day++) {          /* счетчик дней от 1 до крайнего числа месяца */

        if ( day < 10 ) {                                            /* если число дней имеет один разряд (т.е. оно меньше 10) */

            first_digit = '0';                                       /* записываем 0 в первый разряд */
            second_digit = (char) day + '0';                         /* записываем значения числа месяца во второй разряд */

        } else {                                                     /* если число месяцев имеет два разряда (т.е. больше 10) */

            first_digit = (char) day / 10 + '0';                     /* записываем значение десятков в первый разряд */
            second_digit = (char) day % 10 + '0';                    /* записываем значение единиц во второй разряд */
        }            

        desired_value[8] = first_digit;                          /* записываем первый разряд дня в строку */
        desired_value[9] = second_digit;                         /* записываем второй разряд дня в строку */
        desired_value[10] = ';';                                 /* добавляем разделитель */
        desired_value[11] = '\0';                                /* добавляем символ конца строки */

        memset(&day_data, 0, sizeof(data_day_t));
        day_temp(file, desired_value, &day_data);                /* передаем структуру для заполнения в массив day_temp */

        if (day_data.hours_ctr > 0) {                            /* если в отношении дня найдены данные (т.е. количество найденных измерений больше 0) */

            month_data.days_ctr ++;                              /* увеличиваем количество записанных дней в структуре month_dat*/
            month_data.average_temp += day_data.average_temp;    /* записываем в соответствующую ячейку массива av_temp_per_day структуры month_data среднюю дневную температуру */
            month_data.temp_max[day-1] = day_data.max;           /* записываем в соответствующую ячейку массива av_temp_per_day структуры month_data максимальную дневную температуру */
            month_data.temp_min[day-1] = day_data.min;           /* записываем в соответствующую ячейку массива av_temp_per_day структуры month_data минимальную дневную температуру */
        
        } else {                                                 /* пишем ошибку в лог, если в отношении конкретного дня не было найдено измерений */

            char error_str[STRING_LENGHT];
            error_str[0] = desired_value[8];
            error_str[1] = desired_value[9];
            error_str[2] = '.';
            error_str[3] = desired_value[5];
            error_str[4] = desired_value[6];
            error_str[5] = '.';
            error_str[6] = desired_value[0];
            error_str[7] = desired_value[1];
            error_str[8] = desired_value[2];
            error_str[9] = desired_value[3];
            error_str[10] = '\0';

            errors(1, 0, error_str);                            /* если полностью отсутствуют дневные данные, выводим ошибку в консоль */
            continue;
        }
    }

            int max_min_tmp = 0;
            month_data.average_temp /= month_data.days_ctr; 
            /* month_data.average_temp = average_temp(month_data.av_temp_per_day, month_data.days_ctr);  записываем среднемесячную температуру в переменную average_temp массива month_data */
            max_and_min(month_data.temp_max, month_data.days_ctr, &month_data.max, &max_min_tmp);      /* записываем максимальную месячную температуру в переменную max массива month_data */
            max_and_min(month_data.temp_min, month_data.days_ctr, &max_min_tmp, &month_data.min);      /* записываем минимальную месячную температуру в переменную min массива month_data */
            
            #ifdef DEBUG
            debug_file(5, month_data.max, month_data.min, month_data.average_temp, desired_value);
            debug_file(6, month_data.days_ctr, 0, 0, " ");
            #endif 

            return month_data;
}

/*
Функция заполненяет структуру day_data
Функция принимает ссылку на открытый файл file, строку со значениями YYYY;mm;dd; , структуру для заполнения
*/
void day_temp (FILE *file, char *desired_value, data_day_t *day_data) {

    static int byte_num = 0;                                                   /* переменная для хранения количества прочитанных байт */
    static int str_num = 0;                                                    /* переменная для хранения количества прочитанных строк */
    data_hour_t hour_data;                                                     /* структура для хранения данных по часу */
    char first_digit;                                                          /* первый символ для записи значения часа в строку */
    char second_digit;                                                         /* второй символ для записи значения часа в строку */

    for (int hours = 0; hours < 24; hours++) {                                 /* счетчик количества часов (от 0 до 23) */       

        if ( hours < 10 ) {                                                    /* если число часов имеет один разряд (т.е. оно меньше 10) */

            first_digit = '0';                                                 /* записываем 0 в первый разряд */
            second_digit = (char) hours + '0';                                 /* записываем значения числа месяца во второй разряд */

        } else {                                                               /* если число часов имеет два разряда (т.е. больше 10) */

            first_digit = (char) hours / 10 + '0';                             /* записываем значение десятков в первый разряд */
            second_digit = (char) hours % 10 + '0';                            /* записываем значение единиц во второй разряд */
        }  

        desired_value[11] = first_digit;                                       /* записываем первый разряд часа в строку */                              
        desired_value[12] = second_digit;                                      /* записываем второй разряд часа в строку */
        desired_value[13] = ';';                                               /* добавляем разделитель */
        desired_value[14] = '\0';                                              /* добавляем символ конца строки */

        memset(&hour_data, 0, sizeof(data_hour_t));                            /* заполняем нулями структуру */        
        hour_temp(file, &byte_num, desired_value, &hour_data, &str_num);       /* передаем структуру hour_data для заполнения в функцию hour_temp */


        if (hour_data.temp_ctr > 0) {                                          /* если значения часовой температуры записаны (т. е. количество найденных измерений больше 0) */
            
            day_data->average_temp += hour_data.average_temp;                  /* записываем в соответствующую ячейку массива av_temp_per_hour структуры day_data значение средней часовой температуры */
            day_data->temp_max[hours] = hour_data.max;                         /*  записываем массив с максимальной температурой */
            day_data->temp_min[hours] = hour_data.min;
            day_data->hours_ctr ++;                                            /*  записываем массивы с минимальной температурой */

        } else {

            char error_str[STRING_LENGHT];
            error_str[0] = desired_value[8];
            error_str[1] = desired_value[9];
            error_str[2] = '.';
            error_str[3] = desired_value[5];
            error_str[4] = desired_value[6];
            error_str[5] = '.';
            error_str[6] = desired_value[0];
            error_str[7] = desired_value[1];
            error_str[8] = desired_value[2];
            error_str[9] = desired_value[3];
            error_str[10] = ' ';
            error_str[11] = desired_value[11];
            error_str[12] = desired_value[12];
            error_str[13] = 'H';
            error_str[14] = '\0';

            errors(1, 0, error_str);                                                /* если отсутствуют данные по дню, выводим ошибку в лог-файл */
            continue;
        } 
    }

    int _tmp = 0;                                                                   /* временная переменная для вызова функции max_and_min */
    max_and_min(day_data->temp_max, day_data->hours_ctr,  &day_data->max,  &_tmp);     
    max_and_min(day_data->temp_min, day_data->hours_ctr,  &_tmp,  &day_data->min);

    day_data->average_temp /= day_data->hours_ctr;
 
    #ifdef DEBUG
    debug_file(4, day_data->max, day_data->min, day_data->average_temp, desired_value);
    debug_file(7, day_data->hours_ctr, 0, 0, " ");
    #endif 
}

/* 
Функция чтения и записи в структуру значения температуры в конкретном часе.  
Принимает открытый файл с информацией, указатель на позицию в файле с которой следует начать чтение строк,
массив для поиска значений минут и температуры в формате YYYY;mm;dd;hh;, структуру для записи значений,
номер строки (необходим для вывода ошибок в лог-файл)
*/ 

void hour_temp (FILE *file, int *byte_num, char *desired_value, data_hour_t *hour_data, int *str_num) {

    char tmp[STRING_LENGHT];                                                                /* строка для хранения данных, полученных из файла */
    int elem = 0;                                                                           /* счетчик количества прочитанных элементов */
    int minutes;                                                                            /* переменная для записи значения минут */
    int temperature;                                                                        /* переменная для записи значения температуры */
    int is_negative = 0;                                                                    /* переменная для работы с отрицательным значением температуры */
    int byte;                                                                               /* переменная для записи позиции в файле */
    int str_number = *str_num;                                                              /* переменная для записи количества прочитанных строк (для вывода сообщений об ошибке в лог-файл) */
                            
    fseek(file, *byte_num, SEEK_SET);                                                       /* устанавливаем указатель в нужную позицию в файле */                              

    while ( fgets(tmp, STRING_LENGHT, file) != NULL ) {                                     /* читаем очередную строку */
        
        str_number++;                                                                       /* увеличиваем значение переменной, записывающей номер строки (переменная нужна для логирования ошибок ) */
                                                                                           
        if( memcmp(tmp, desired_value, 14) == 0 ) {                                         /* ищем совпадения со строкой в формате yyyy;mm;dd;HH; */
   
            if ( (minutes = read_two_digits(4, tmp[14], tmp[15], 0)) == -1) {               /* проверяем корректность значения минут в файле */
        
                errors(5, str_number, tmp);                                                 /* если функция возвращает -1 (т. е. ошибку), записываем ее в лог и переходим к следующей строке */                       
                continue; 
            }    

            if (tmp[17] == '-') {
                
                is_negative = 1;                                                            /* если значение температуры начинается с - (т.е. оно отрицательное), переменной присваивается значение 1 */

            } else {

                is_negative = 0;
            }    


            if (tmp[18] == '\n') {                                                          /* если значение температуры состоит из 1 символа (например, 0) */

                temperature = read_two_digits(5, '0', tmp[17], 0);                          /* отправляем для проверки один символ */

            } else { 
                
                temperature = read_two_digits(5, tmp[17 + is_negative], tmp[18 + is_negative], 0); 
            }
                
            
            if (temperature == -1) {                                                        /* если функция read_two_digits возвращает -1 (т. е. ошибку), записываем ошибку в лог и переходим к следующей строке файла */ 
                       
                errors(6, str_number, tmp);                                                                                        
                continue; 
            }    

            elem++;                                                                         /* увеличиваем количество успешно прочитанных элементов, если значение минут и температуры соответствует требованиям */

            if (is_negative == 1) temperature *= -1;                                        /* если значение температуры отрицательное, домножим число на -1 */

            if (elem <= 70) {                                                               /* проверяем не переполнится ли массив значений с температурой */
                   
                    hour_data->temp_values[elem-1] = temperature;                           /* записываем в структуру полученную температуру */
                    hour_data->temp_ctr = elem;                                             /* записываем количество элементов в массиве с измерениями температуры */

                    #ifdef DEBUG
                    debug_file(2, minutes, hour_data->temp_values[elem-1], 0, desired_value);  /* запись в журнале работы программы в случае компиляции с опцией DEBUG */
                    debug_file(3, str_number, hour_data->temp_ctr, 0, " ");
                    #endif

            } else {

                    errors(10, str_number, tmp);                                            /* выводим ошибку, если количество измерений в час превышает 70 */

            }

        } else if (elem > 0) {                                                              /* если после совпавших значений прочитано значение, которое не совпадает вплоть до часов */
            
            max_and_min(hour_data->temp_values, hour_data->temp_ctr, &hour_data->max, &hour_data->min);   /* вызываем функцию поиска максимального и минимального значения и записываем эти значения в переменные min и max*/
            hour_data->average_temp = average_temp(hour_data->temp_values, hour_data->temp_ctr);          /* записываем значение средней температуры в переменную average_temp*/
            
            #ifdef DEBUG
            debug_file(1, hour_data->max, hour_data->min, hour_data->average_temp, desired_value);        /* запись в журнале работы программы в случае компиляции с опцией DEBUG */
            #endif 

                *str_num = str_number - 1;                                                  /* записываем значение количества полученных строк */

                if ( (byte = ftell(file)) == -1) {                                          /* запрашиваем текущую позицию в файле */
            
                    return;                                                                 /* возвращаемся без записи если функция ftell выдала ошибку */
            
                } else {

                    *byte_num = byte;                                                       /* записываем текущую позицию если ftell вернул какое-либо значение и выходим из функции */
                    
                    #ifdef DEBUG
                    debug_file(0, *str_num, *byte_num, 0, " ");                             /* запись в журнале работы программы в случае компиляции с опцией DEBUG */
                    #endif

                    return;
                }
        }

                                          
    }
}




/*
Функция для поиска температуры в конкретную минуту времени (опция -M)
Функция принимает ссылку на открытый файл, строку в формате yyyy;mm;dd;hh;mm; , которая уже проверена на соответствие формату
Функция проверяет только значение температуры
*/
void minute_temp_option (FILE *file, const char *desired_value, const char *path, const char *month_name) {

    int temperature;                                                                            /* переменная для записи значений температуры */
    char tmp[STRING_LENGHT];                                                                    /* переменная для записи прочитанной строки */
    int is_negative = 0;                                                                        /* переменная для обозначения отрицательных значений */
    int str_num = 0;                                                                            /* переменная для подсчета строки (для логирования найденныз ошибок) */
    int n = 0;                                                                                  /* счетчик найденных значений (помогает определить было ли найдено исходное значение) */

    while ( fgets(tmp, STRING_LENGHT, file) != NULL ) {                                         /* читаем строку в файле и записываем ее в строку tmp */

        str_num++;                                                                              /* увеличиваем значение строки при успешном чтении */

        if ( memcmp(tmp, desired_value, 17) == 0) {                                             /* ищем совпадения со строкой в формате YYYY;mm;dd;HH;MM; (т.е. со значениями, которые введены пользователем) */

            n++;                                                                                /* если совпадение найдено, увеличиваем значение счетчика */
    
            if (tmp[17] == '-') is_negative = 1;                                                /* если значение температуры начинается с - (т.е. оно отрицательное), переменной i присваивается значение 1 */

            break;                                                                              /* выходим из цикла, если нашли искомое значение (читаем только первое значение, соответствующее введенному искомому значению в формате YYYY;mm;dd;HH;MM ) */
        } 
    }                                               

    if (n == 0) {                                                                               /* если программа прошла по всему файлу и не нашла строки с указанными данными в формате YYYY;mm;dd;HH;MM; */

        printf("The programm cannot find the temperature for %c%c of %s, %c%c%c%c %c%c:%c%c in file %s.\n", desired_value[8], desired_value[9],             
            month_name, desired_value[0], desired_value[1], desired_value[2], desired_value[3], desired_value[11], desired_value[12],  
            desired_value[14], desired_value[15], path);                                         /* выводим ошибку, если совпадений не найдено */
        
        return;                                                                                                     /* выходим из функции */
    }


    if ( (temperature = read_two_digits(5, tmp[17 + is_negative], tmp[18 + is_negative], 0)) == -1) {      /* проверяем корректность значения температуры в найденной строке */
        
        errors(6, str_num, tmp);                                                                           /* выводим ошибку в лог, если функция read_two_digits вернула значение -1 (т.е. ошибку) */
        printf("Wrong temperature format for the. Try to read line %d manually", str_num);                 /* говорим о найденной ошибку пользователю */
        return;                                                                                            /* выходим из функции */
    }                                                                            

    if (is_negative  == 1) temperature *= -1;                                                              /* если значение температуры было прочитано выше как отрицательное, домножим число на -1 */

        printf("At %c%c of %s, %c%c%c%c %c%c:%c%c the temperature was %dº Celsius.\n", desired_value[8], desired_value[9],
            month_name, desired_value[0], desired_value[1], desired_value[2], desired_value[3], 
            desired_value[11], desired_value[12], desired_value[14], desired_value[15], temperature);              /* выводим найденное значение, если не обнаружены ошибки */
        
    return;
}

/* 
Функция записи ошибок в форматировании файла с данными в лог-файл. 
Принимает номер ошибки, номер строки и саму строку.
*/
void errors (const int error_num, int line_num, const char *line) {

    FILE *log = fopen("errors_log", "a+");                                      /* создаем/ открываем лог-файл */

    if (log == NULL) {                                                          /* выводим в консоль ошибку, если не удается открыть/ создать лог-файл*/

        printf("Uneable to create/open the log file.");
        return;
    }

    switch(error_num) {                                                         /* вывод ошибки в соответствии с ее кодом */

        case 0: fprintf(log, "Internal error in function %s\n", line); break;
        case 1: fprintf(log, "No data with respect to %s\n", line); break;
        case 5: fprintf(log, "Problem with minutes in line %d:\n%s", (line_num + 1), line); break;
        case 6: fprintf(log, "Problem with temperature value in line %d:%s\n", (line_num + 1), line); break;
        case 9: fprintf(log, "Problem with format in line %d:\n%s", (line_num + 1), line); break;
        case 10: fprintf(log, "Measurement frequency exceeded in line %d:\n%s", (line_num + 1), line); break;                /* превышена частота измерений */
    }
        
    fclose(log);                                                                /* закрываем лог-файл */

}

#ifdef DEBUG
/* 
Функция вывода в файл прочитанной информации для поиска ошибок.
Функция может принимает номер опции, а также может принимать:
2 аргумента в формате int, аргумент в формате float, строку.

*/
void debug_file(const int option, const int arg_1, const int arg_2, const float arg_3, const char *string) {
    
    FILE *debug = fopen("read_values", "a+");                                     /* создаем/ открываем файл */

    if (debug == NULL) {                                                             /* выводим в консоль ошибку, если не удается открыть/ создать файл*/

        printf("Uneable to create/open the debug file.");
        return;
    }

    switch(option) {                                                              /* вывод информации в соответствии с номером полученной опции */

        case 0: fprintf(debug, " Current line: %d, Read bytes: %d", arg_1, arg_2); break;
        case 1: fprintf(debug, "\n\tHOUR:%s, max = %d, min = %d, av = %f", string, arg_1, arg_2, arg_3); break;
        case 2: fprintf(debug, "\nMIN:%s%2d, temp = %4d", string, arg_1, arg_2); break;
        case 3: fprintf(debug, " Current line: %5d, Current element: %5d", arg_1, arg_2); break;
        case 4: fprintf(debug, "\n\tDAY:%s, max = %d, min = %d, av = %f", string, arg_1, arg_2, arg_3); break;
        case 5: fprintf(debug, "\n\t\tMONTH:%s, max = %d, min = %d, av = %f", string, arg_1, arg_2, arg_3); break;
        case 6: fprintf(debug, " Number of read days: %d", arg_1); break;  
        case 7: fprintf(debug, " Number of read hours: %d", arg_1); break;       
    }

    fclose(debug);                                                                /* закрываем файл */
}

#endif /* DEBUG */

/* 
Функция, управляющая работой программы в зависимости от введенной пользователем опции. 
Принимает открытый файл, номер, соответствующий аргументу (-f = 0, -m = 1, -d = 2, -h = 3, -M = 4), 
массив со значением аргумента и путь/название файла
Возвращает -1 в случае некорректного ввода файла, 0 или соответствующее значение > 0 в остальных случаях
*/
int read_arg(FILE *file, const int arg_num, const char *arg_value, const char *path) {

    char data[STRING_LENGHT] = { 0 };                                                               /* строка для записи введенных пользователем значений в установленном формате (YYYY;mm;dd;HH;MM;) */
    strcat(data, "2021;");                                                                          /* добавляем в строку информацию о годе */

/* если иные аргументы отсутствуют, программа запускается в стандартном режиме (вывод годовой информации и информации по каждому месяцу) */
  
   if (arg_num == 0) {

       year_temp (file, data);
       return 0;
   }
   
/* если пользователь ввел иные аргументы, программа проверяет корректность введенных пользователем данных, вызывает соответствующую функцию и выводит данные */

    int month;                                                                                  /* переменная для записи введенного пользователем месяца */

    if ( (month = read_two_digits(1, arg_value[0], arg_value[1], 0)) == -1) return -1;          /* проверяем корректность ввода пользователем месяца, если функция read_two_digits возвращает ошибку (-1), выходим из функции */
                                                                                                    
    data[5] = arg_value[0];                                                                     /* если месяц введен корректно, записываем его в строку */
    data[6] = arg_value[1];
    data[7] = ';';
    data[8] = '\0';

    char _month_name[10];
    month_name(month, _month_name);                                                             /* запишем в строку название месяца, соответствующее его номеру */

/* Вывод информации при вводе пользователем опции -m (месячная температура) */
    if (arg_num == 1) {

        data_month_t month_data = { 0 };                                                         /* создаем структуру для записи месячных значений */
        month_data = month_temp(file, data, month);                                              /* передаем структуру и строку в формате YYYY;mm; в функцию для чтения значений из файла */

       printf("Average temperature for %s of %c%c%c%c is %3.1fº Celsius.\n", _month_name, 
       data[0], data[1], data[2], data[3], month_data.average_temp);
       printf("Maximum temperature is %dº Celsius, minimum temperature is %dº Celsius.\n", month_data.max, month_data.min);
       
       return 0;
   }

    int day;                                                                                      /* переменная для записи введенного пользователем дня */

    if ( (day = read_two_digits(6, arg_value[3], arg_value[4], month)) == -1) return -1;          /* проверяем корректность ввода пользователем значения дня, выхооодим если функция вернула -1 (т.е. ошибку) */
                                                                                 
    data[8] = arg_value[3];                                                                       /* если день введен корректно, записываем его в строку */
    data[9] = arg_value[4];
    data[10] = ';';
    data[11] = '\0';

/* Вывод информации при вводе пользователем опции -d (дневная температура) */
    if (arg_num == 2) {

        data_day_t day_data = { 0 };                                                             /* структура для записи дневной информации */

        day_temp(file, data, &day_data);

        if (day_data.hours_ctr == 0) {                                                           /* если функция day_temp не записаписала ни одного значения */

            printf("No measurments found for %s\n", arg_value);                                  /* выводим ошибку и выходим из функции */
            return 1;            
        }

        printf("Average temperature for %c%c of %s, %c%c%c%c was %3.1fº Celsius.\n", data[8], data[9], _month_name, 
        data[0], data[1], data[2], data[3], day_data.average_temp); 

        printf("Maximum temperature is %dº Celsius, minimum temperature is %dº Celsius.\n", day_data.max, day_data.min);

        return 0;
   }

    int hour;                                                                                    /* переменная для ввода введенного пользователем часа */
    
    if ( (hour = read_two_digits(3, arg_value[6], arg_value[7], 0)) == -1) return -1;            /* проверяем корректность ввода значения часа, выходим из функции если read_two_digits вернула -1 (т. е. ошибку) */

    data[11] = arg_value[6];                                                                     /* если день введен корректно, записываем его в строку */
    data[12] = arg_value[7];
    data[13] = ';';
    data[14] = '\0';

 /* Вывод информации при вводе пользователем опции -H (часовая температура) */   
    if (arg_num == 3) {

        data_hour_t hour_temperature = { 0 };                                                    /* структура для записи дневной информации */
        int tmp_1 = 0;                                                                           /* переменная - заглушка для передачи поинтера в функцию */
        int tmp_2 = 0;                                                                           /* переменная - заглушка для передачи поинтера в функцию */
        
        hour_temp(file, &tmp_1, data, &hour_temperature, &tmp_2);                                /* передаем информацию в функцию для записи прочитанных значений */
        
        if (hour_temperature.temp_ctr == 0) {                                                    /* если после вызова функции не нашлось искомого значения */

            printf("No measurments found for %s\n", arg_value);                                  /* выводим ошибку и выходим из функции */
            return 1;

        }

        printf("Average temperature for %c%c of %s, %c%c%c%c from %c%c:00 to %c%c:59 was %3.1fº Celsius.\n", data[8], data[9], _month_name,
        data[0], data[1], data[2], data[3],data[11], data[12], data[11], data[12], hour_temperature.average_temp); 

        printf("Maximum temperature is %dº Celsius, minimum temperature is %dº Celsius.\n", hour_temperature.max, hour_temperature.min);

        return 0;
   }


    int minute;                                                                                  /* переменная для записи минут, введенных пользователем */
    
    if ( (minute = read_two_digits(4, arg_value[9], arg_value[10], 0)) == -1) return -1;         /* проверяем корректность ввода значения минут, выходим из функции если read_two_digit вернула -1 (т.е. ошибку) */

    data[14] = arg_value[9];                                                                     /* если минуты введены корректно, записываем их в строку */
    data[15] = arg_value[10];
    data[16] = ';';
    data[17] = '\0';

    minute_temp_option(file, data, path, _month_name);                                            /* вызываем функцию, которая находит и выводит в консоль значение температуры в конкретную минуту */

    return 0;
}

/* 
Функция рассчета средней температуры.
Принимает целочисленный массив и количество элементов в нем.
Возвращает среднее значение (сумма элементов / количество элементов) в форме десятичной дроби.
*/

float average_temp(const int *temp_measurments, const int array_size) {

    float temp_sum = 0;                                                /* переменная для рассчета суммы элементов массива */

        for (int ctr = 0; ctr < array_size; ctr++) {                   /* счетчик элементов переданного массива */

            temp_sum += temp_measurments[ctr];                         /* складываем элементы массива */
        }

        temp_sum = temp_sum / (float) array_size;                      /* находим среднее значение температуры */

    return temp_sum;
}

/* 
Функция поиска максимального и минимального элементов в массиве.
Функция принимает массив, количество элементов в массиве, ссылки на переменные max и min для записи полученных значений.
*/
void max_and_min(const int *array, const int array_size, int *max, int *min) {

    int max_tmp = array[0];                                                 /* присваиваем переменной значение первого элемента */
    int min_tmp = array[0];                                                 /* присваиваем переменной значение первого элемента */

    for (int ctr = 1; ctr < array_size; ctr++) {                              /* счетчик элементов переданного массива (начинается с 1, поскольку ранее переменным присвоено значение нулевого элемента массива) */
        
        if ( max_tmp < array[ctr] ) {                                       /* если элемент прочитанный элемент массива больше, чем найденный ранее элеммент */
            
            max_tmp = array[ctr];                                           /* записываем найденный элемент в качестве нового максимума */
            continue;                                                       /* переходим на следующую итерацию цикла */
        }

        if ( min_tmp > array [ctr]) min_tmp = array[ctr];                   /* если прочитанный элемент массива меньше предыдущего минимума, записываем его в качестве нового минимума */
    }

    *max = max_tmp;                                                         /* записываем максимум в переданную переменную */
    *min = min_tmp;                                                         /* записываем минимум в переданную переменную */

}

/*
Функция возвращает строку с названием месяца, соответствующего его порядковому номеру. 
*/
void month_name(const int month_num, char *name) {


    if ( (month_num >= 1) &&  (month_num <= 12) ) {                         /* если полученно корректное название месяца */
        
        char months_names[12][10] = {"January\0", "February\0", "March\0", "April\0", "May\0", "June\0", "July\0", "August\0", "September\0", "October\0", "November\0", "December\0"};     /* массив с названиями месяцев */
        
        strcpy(name, months_names[month_num - 1]);                          /* записываем название месяца */
                               
        return;
    }                                                                                                                 

}

/* 
Функция читает два переданных ей элемента массива и проверяет их на корректность.
Опции для проверки: 1 - месяц, 2 - день (без передачи месяца), 3 - час, 4 - минута, 5 - температура, 6 - день (с указанием месяца)
Аргумент add_parametr применяется с опцией 5 (передается номер месяца) для корректности отражения количества дней в месяце.
Функция возвращает -1 в случае ошибки и прочитанное число в случае корректного заполнения.  
*/

int read_two_digits (const int option, const char first_d, const char second_d, const int add_parametr) {

    int tmp_num = 0;                                         /* переменная для записи возвращаемого значения (в зависимости от опции: месяц, день, минута, температура) */     


/* блок, проверяющий являются ли два переданных символа числами */

        if( (first_d >= '0') && (first_d <= '9') ) {         /* проверяем является ли первый переданный символ числом */
                                
            tmp_num = ( (int)first_d - '0') * 10;            /* вставляем прочитанный символ в разряд десятков числа */
        
        } else {

            return -1;                                        /* возвращаем -1 если первый символ не является числом */
        }


        if( (second_d >= '0') && (second_d <= '9') ) {        /* проверяем является ли второй переданный символ числом */
                                
            tmp_num += ((int)second_d - '0');                 /* вставляем прочитанный символ в разряд единиц числа  */
        
        } else {

            return -1;                                        /* возвращаем -1 если второй символ не является числом */
        }

/* блок проверки полученного из символов числа в соответствии с номером переданной опции*/

    if ( option == 0 ) return tmp_num;                          /* сразу возвращаем число, если функция вызвана без опций */

    int values[5][2] = {1, 12, 1, 31, 0, 23, 0, 59, 0, 99};     /* двумерный массив со значениями для сравнения полученного числа */

    for (int i = 1; i < 6; i++) {                               /* подбираем номер опции */

        if (i == option) {                                      /* входим в цикл проверки, если номер опции совпадает со значением счетчика */

            if ( (tmp_num >= values[i-1][0]) && (tmp_num <= values[i-1][1])) {      /* цикл сравнивает полученное число с соответствующими элементами двумерного массива */

                return tmp_num;                                                     /* возвращаем прочитанное число, если проочитанноое число находится в нужном диапазоне */

            } else {                                                                /* возвращаем -1 в случае, если прочитанное число находится за пределами диапазона */

                return -1;
            }
        }
    }

/* проверка дней с учетом номера месяца при вызове опции 5 (на всякий случай в феврале 29 дней) */

    if ( (option == 6) && (add_parametr >= 1) && (add_parametr <= 12)) {                                        /* проверка номера опции и переданного параметра */

        int days_in_month[12] = {31, 29, 31, 30, 31, 30, 31 ,31, 30, 31, 30, 31};                               /* массив с количеством дней в каждом месяце */

        if ( (tmp_num >= 1) && (tmp_num <= days_in_month[add_parametr-1])) {                                    /* проверка полученного числа в диапазоне от 1 до последнего дня месяца*/

            return tmp_num;

        } else {

            return -1;
        }

    }

    return -1;                                                                     /* возвращаем -1 функция вызвана с неверной опцией */
}

