#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#define HELP_INSTRUCTION printf("\n\t\t\t\t\t\t***INSTRUCTION***\n");\
                         printf("\nThe program reads the <*.csv> file with temprature measurments from a temperature-sensitive element\n");\
                         printf("and shows the statistic (average temperature statistic, max and min temperature) for a year and for every month.\n");\
                         printf("All measurments in <*.csv> file have to be in the following format YYYY;MM;DD;HH;mm;temperature, where:\n");\
                         printf("\n\t\t-YYYY is a year;\n");\
                         printf("\t\t-MM is a month;\n");\
                         printf("\t\t-DD is a day;\n");\
                         printf("\t\t-HH is an hour;\n");\
                         printf("\t\t-mm is a minute;\n");\
                         printf("\t\t-temperature is an amount shown on the element in Celsius degree.\n");\
                         printf("\nWhen invoked with argument -f <filename.csv>, the programm uses the *csv file as a data source.");\
                         printf("\nWhen invoked with additional argument -m MM, where MM is a months the programm shows\n the following statistic for the entered month:\n");\
                         printf("\n\t\t-average mounth temp;\n");\
                         printf("\t\t-max and min temp for the month;\n");\
                         printf("\t\t-average temp for every day in the month.\n");\
                         printf("\nWhen invoked with additional argument -d MM.DD, where MM is a month and DD is a day, the programm shows\nthe following statistic for the day in the entered month.\n");\
                         printf("\n\t\t-average day temp;\n");\
                         printf("\t\t-max and min temp for the day;\n");\
                         printf("\t\t-average temp for every hour in the day.\n");\
                         printf("\nWhen invoked with additional argument -H MM.DD/HH, where MM is a month, DD is a day, HH is an hour\nthe programm shows the statistic for the hour (average temp, max and min temp).\n");\
                         printf("\nWhen invoked with additional argument -M MM.DD/HH:mm, where MM is a month, DD is a day, HH is an hour and mm is a minute,\nthe programm shows the statistic for the entered minute.\n\n");





enum {PATH_LENGTH = 256,
      STRING_LENGHT = 25,
      MONTHS = 12,
     };

/* структура для хранения данных по каждому месяцу (структура создается по каждому месяцу) */
typedef struct temp_data_for_month {

    int av_temp_per_day[31];                       /* массив со значениями средней тепературы по каждому из дней */
    int days_ctr;                                  /* счетчик элементов в массиве av_temp_per_day */
    int temp_max[31];                              /* массив для записи максимальной температуры в каждом из дней месяца*/    
    int temp_min[31];                              /* массив для записи минимальной температуры в каждом из дней месяца*/ 
    int min;                                       /* максимальная температура в течение месяца (присваивается после сортировки массива temp_max) */ 
    int max;                                       /* минимальная температура в течение месяца (присваивается после сортировки массива temp_min) */ 
    int average_temp;                              /* среднее значение ежемесячной температуры (присваивается после сортировки массива av_temp_per_day) */ 

} data_month_t;

/* структура для хранения данных по каждому дню (для экономии памяти структура каждый раз перезаписывается)*/
typedef struct temp_data_for_day {

    int av_temp_per_hour[24];                      /* массив с информацией о средней часовой температуре */
    int hours_ctr;                                  /* счетчик элементов в массиве av_temp_per_hour */
    int temp_max[24];                              /* массив смаксимальной температурой по каждому часу */
    int temp_min[24];                              /* массив с минимальной температурой по каждому часу */
    int max;                                       /* максимальная температура в течение дня (заполняется после сортировки массива temp_max) */
    int min;                                       /* минимальная температура в течение дня (заполняется после сортировки массива temp_min) */
    int average_temp;                              /* среднее значение температуры в течение дня (присваивается после сортировки массива av_temp_per_hour) */ 

} data_day_t;

/* структура для хранения данных по каждому часу (для экономии памяти структура каждый раз перезаписывается)*/
typedef struct temp_data_for_hour {

    int temp_values[70];                           /* массив с информацией о средней температуре в каждой из минут */
    int temp_ctr;                                  /* счетчик элементов в массиве temp_values */
    int max;                                       /* переменная для записи максимальной температуры в часе (после сортировки массива temp_value) */
    int min;                                       /* переменная для записи минимальной температуры в часе (после сортировки массива temp_value) */

} data_hour_t; 

int read_two_digits (const int option, const char first_d, const char second_d, const int add_parametr);
void errors (const int error_num, int line_num, const char *line);
void max_and_min(const int *array, const int elem_numb, int *max, int *min);

void year_temp (FILE *file, char *year_str);
void month_temp (FILE *file, char *desired_value, data_month_t month_data, const int month);
void day_temp (FILE *file, char *desired_value, data_day_t *day_data);
void hour_temp (FILE *file, int *byte_num, char *desired_value, data_hour_t *hour_data, int *str_num);

void minute_temp_option (FILE *file, const char *desired_value, const char *path);

/* 
Функция для рассчета средней температуры (ищет самый большой и самый маленький элемент переданного массива).

Принимает целочисленный массив и количество элементов в нем.

Возвращает среднее значение (сумма элементов / количество элементов) в целых числах
*/

int average_temp(const int *temp_measurments, const int numbers) {

    int av_temp = 0;                                                            /* переменная для рассчета средней температуры*/

        for (int counter = 0; counter < numbers; counter++) {                   /* счетчик элементов переданного массива */

            av_temp += temp_measurments[counter];                               /* складываем элементы массива */
        }

        av_temp /= numbers;                                                     /* находим среднее значение температуры */

    return av_temp;
}

/*
Функция создает массив с данными по каждому месяцу и передает этот массив следующей функции для заполнения.
После заполнения данных по каждому месяцу функция обобщает их и выводит на вывод.

Функция принимает ссылку на открытый файл, строку со значением года в формате YYYY;
*/

void year_temp (FILE *file, char *year_str) {

    char desired_value[STRING_LENGHT];                   /* переменная для записи значения в формате YYYY;MM; */
    data_month_t month_data[12] = { 0 };                 /* создаем массив структур с информацией по каждому месяцу */
    char first_digit;                                    /* первый символ для записи значения месяца в строку */
    char second_digit;                                   /* второй символ для записи значения месяца в строку */
    int av_temp = 0;                                     /* переменная для рассчита среднегодовой температуры */
    int number_of_measurments = 0;                       /* переменная для рассчета количества месяцев, по которым есть данные */
    int max_temp[12] = {0};                              /* массив для записи максимальных температур по месяцам */
    int min_temp[12] = {0};                              /* массив для записи минимальных температур по месяцам */
    int max;                                             /* переменная для записи максимальной температуры */
    int min;                                             /* переменная для записи минимальной температуры */
    char months[12][10] = {"January\0", "February\0", "March\0", "April\0", "May\0", "June\0", "July\0", "August\0", "September\0", "October\0", "November\0", "December\0"};     /* массив с названиями месяцов */

    strcpy(desired_value, year_str);                     /* копируем значение года в переменную */

    for (int month = 1; month <= 12; month++) {          /* счетчик количества месяцев для заполения структур */

        if ( month > 10 ) {                              /* если число месяцев имеет один разряд (т.е. меньше 10) */

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

        month_temp (file, desired_value, month_data[month-1], month);     /* передаем структуру для записи в функцию month_temp */

        if ( month_data[month-1].days_ctr > 0 ) {                          /* если в массив были записаны данные о месячной температуре (т.е. количество найденных измерений больше 0) */
            number_of_measurments++;                                       /* увеличиваем количество слогаемых */
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

            errors(1, 0, error_str);
            continue;
        }
    } 

    av_temp /= number_of_measurments;                                      /* находим среднегодовую температуру */

    /* вывод на экран данных за год и данных за каждый месяц */
    printf("Average annual temperature for %c%c%c%c year is %dº Celsius.\n\n", desired_value[0], desired_value[1], desired_value[2], desired_value[3], av_temp);
    printf("Max temperature is %d, Min temperature is %d.\n\n", max, min);
    printf("Monthly statistics:\n\n");

    for (int i = 0; i < 12; i++) {                                         /* счетчик количества месяцев */

        printf("Average monthly temperature for %s is ", months[i - 1]);
        
        if ( month_data[i-1].days_ctr > 0 ) {                              /* если количество прочитанных и записанных значений больше 1 */

            printf("%dº Celsius, max temperature is %d, min temperature is %d\n", month_data[i-1].average_temp, month_data[i-1].max, month_data[i-1].min);

        } else {                                                           /* если в месяце прочитанных значений нет */

            printf("not defined in the entered file, see error_log for more details\n");

        }

    }
}

/* 
Функция записи массива с информацией о температуре по каждому месяцу.

Функция принимает ссылку на открытый файл, строку со значением в формате YYYY;mm; , 
ссылку на соответствующий месяцу элемент структуры, номер месяца

Функция добавляет в строку значения дня, передает для заполнения структуру и строку в формате YYYY;mm;dd; ,
обрабатывает полученные данные и заполняет структуру с месячными данными  
*/

void month_temp (FILE *file, char *desired_value, data_month_t month_data, int month) {

    data_day_t day_data;                                                     /* структура с информацией по каждому дню месяца  (перезаписывается) */
    char first_digit;                                                        /* первый символ для записи значения дня в строку */
    char second_digit;                                                       /* второй символ для записи значения дня в строку */
    int days_in_month[12] = {31, 29, 31, 30, 31, 30, 31 ,31, 30, 31, 30, 31};   /* массив с информацией о количестве дней в месяце (в феврале 29) */

    
    for (int day = 1; day <= days_in_month[month-1]; day++) {        /* счетчик дней от 1 до крайнего числа месяца */

        if ( day > 10 ) {                                            /* если число дней имеет один разряд (т.е. оно меньше 10) */

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

        memset(&day_data, 0, sizeof(day_data));                  /* заполняем нулями структуру */
        day_temp(file, desired_value, &day_data);                 /* передаем структуру для заполнения в массив day_temp */

        if (day_data.hours_ctr != 0) {                           /* если в отношении дня найденны данные (т.е. количество найденных измерений больше 1) */

            month_data.days_ctr += 1;                   /* увеличиваем количество записанных дней в структуре month_dat*/
            month_data.av_temp_per_day[day-1] = average_temp(day_data.av_temp_per_hour, day_data.hours_ctr);   /* записываем в соответствующую ячейку массива av_temp_per_day структуры month_data среднюю дневную температуру */
            month_data.temp_max[day-1] = day_data.max;  /* записываем в соответствующую ячейку массива av_temp_per_day структуры month_data максимальную дневную температуру */
            month_data.temp_min[day-1] = day_data.min;  /* записываем в соответствующую ячейку массива av_temp_per_day структуры month_data минимальную дневную температуру */
        
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

            errors(1, 0, error_str);
            continue;
        }
    }

            month_data.average_temp = average_temp(month_data.av_temp_per_day, month_data.days_ctr);                 /* записываем среднемесячную температуру в переменную average_temp массива month_data */
            max_and_min(month_data.av_temp_per_day, month_data.days_ctr, &month_data.max, &month_data.min);      /* записываем максимальную и минимальную месячную температуры в переменные min и max массива month_data */
            return;
}

/*
Функция заполненяет структуру day_data

Функция принимает ссылку на открытый файл file, строку со значениями YYYY;mm;dd; , структуру для заполнения
*/
void day_temp (FILE *file, char *desired_value, data_day_t *day_data) {

    static int byte_num = 0;                                                   /* переменная для хранения количества прочитанных байт */
    static int str_num = 0;                                                    /* переменная для хранения количества прочитанных строк */
    data_hour_t hour_data;                                                     /* первый символ для записи значения часа в строку */
    char first_digit;                                                          /* второй символ для записи значения часа в строку */
    char second_digit;

    for (int hours = 0; hours < 24; hours++) {                                 /* счетчик количества часов */       

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

        errors(7, 0,  desired_value);
        memset(&hour_data, 0, sizeof(hour_data));                              /* заполняем нулями структуру */        
        hour_temp(file, &byte_num, desired_value, &hour_data, &str_num);       /* передаем структуру hour_data для заполнения в функцию hour_temp */


        if (hour_data.temp_ctr > 0) {                                          /* если значения часовой температуры записаны */
            
            day_data->av_temp_per_hour[hours - 1] = average_temp(hour_data.temp_values, hour_data.temp_ctr);                             /* записываем в соответствующую ячейку массива av_temp_per_hour структуры day_data значение средней часовой температуры */
            day_data->temp_max[hours - 1] = hour_data.max;                                                                               /*  записываем массив с максимальной температурой */
            day_data->temp_min[hours - 1] = hour_data.max;
            day_data->hours_ctr ++;                                                                               /*  записываем массивы с минимальной температурой */
            errors(16, 0, "Done!");

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

            errors(1, 0, error_str);
            continue;
        } 
    }

    int _tmp = 0;
    max_and_min(day_data->temp_max, day_data->hours_ctr,  &day_data->max,  &_tmp);     
    max_and_min(day_data->temp_min, day_data->hours_ctr,  &_tmp,  &day_data->min);

    day_data->average_temp = average_temp(day_data->av_temp_per_hour, day_data->hours_ctr);                                 /* записываем максимальную и минимальную дневную температуры */

}

/* 
Функция поиска максимального и минимального элементов в массиве

Функция принимает массив, количество элементов в массиве, ссылки на переменные max и min для записи полученных значений
*/
void max_and_min(const int *array, const int elem_numb, int *max, int *min) {

    int max_tmp = array[0];
    int min_tmp = array[0];

    for (int i = 1; i < elem_numb; i++) {
        
        if ( max_tmp < array[i] ) {
            
            max_tmp = array[i];
            continue;
        }

        if( min_tmp > array [i]) min_tmp = array[i];
    }

    *max = max_tmp;
    *min = min_tmp;

}

/*
Функция для поиска температуры в конкретную минуту времени (опция -M)

Функция принимает ссылку на открытый файл, строку в формате yyyy;mm;dd;hh;mm; , которая уже проверена на соответствие формату
Функция проверяет только значение температуры
*/
void minute_temp_option (FILE *file, const char *desired_value, const char *path) {

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

    char string[STRING_LENGHT];                                                                 /* строка для записи значения в фоормате dd.mm.yyyy HH:MM, которое будет выводиться */
    string[0] = desired_value[8];
    string[1] = desired_value[9];
    string[2] = '.';
    string[3] = desired_value[5];
    string[4] = desired_value[6];
    string[5] = '.';
    string[6] = desired_value[0];
    string[7] = desired_value[1];
    string[8] = desired_value[2];
    string[9] = desired_value[3];
    string[10] = ' ';
    string[11] = desired_value[11];
    string[12] = desired_value[12];   
    string[13] = ':';
    string[14] = desired_value[14];
    string[15] = desired_value[15];
    string[16] = '\0';

    if (n == 0) {                                                                               /* если программа прошла по всему файлу и не нашла строки с указанными данными в формате YYYY;mm;dd;HH;MM; */

        printf("The programm cannot find the temperature for %s in file %s.\n", string, path);  /* выводим ошибку */
        return;                                                                                 /* выходим из функции */
    }


    if ( (temperature = read_two_digits(5, tmp[17 + is_negative], tmp[18 + is_negative], 0)) == -1) {      /* проверяем корректность ввода значения температуры */
        
        errors(6, str_num, tmp);                                                                           /* выводим ошибку в лог, если функция read_two_digits вернула значение -1 (т.е. ошибку) */
        printf("Wrong temperature format for the %s. Try to read line %d manually", string, str_num);      /* говорим о найденной ошибку пользователю */
        return;                                                                                            /* выходим из функции */
    }                                                                            

    if (is_negative  == 1) temperature *= -1;                                                              /* если значение температуры было прочитано выше как отрицательное, домножим число на -1 */

    printf("At %s the temperature was %dº Celsius.\n", string, temperature);                               /* выводим найденное значение, если не обнаружены ошибки */
        
    return;
}

/* 
Функция чтения и записи в структуру значения минут и температуры в конкретном часе.  

Принимает открытый файл с информацией, указатель на позицию в файле с которой следует начать чтение строк,
массив для поиска значений минут и температуры в формате YYYY;mm;dd;hh;, структуру для записи значений,
номер строки (необходим для вывода ошибок в лог-файл)
*/ 

void hour_temp (FILE *file, int *byte_num, char *desired_value, data_hour_t *hour_data, int *str_num) {

    char tmp[STRING_LENGHT];                                                                /* строка для хранения данных, полученных из файла */
    int elem = 0;                                                                           /* запись количества прочитанных элементов */
    int minutes;                                                                            /* переменная для записи значения минут */
    int temperature;                                                                        /* переменная для записи значения температуры */
    int is_negative = 0;                                                                    /* переменная для работы с отрицательным значением температуры */
    int byte;                                                                               /* переменная для записи позиции в файле */
                            
    fseek(file, *byte_num, SEEK_SET);                                                       /* устанавливаем указатель в нужную позицию в файле */                              

    while ( fgets(tmp, STRING_LENGHT, file) != NULL ) {                                     /* читаем очередную строку */
        
        *str_num++;                                                                         /* увеличиваем значение переменной, записывающей номер строки (переменная нужна для логирования ошибок ) */
                                                                                           
        if( memcmp(tmp, desired_value, 14) == 0 ) {                                         /* ищем совпадения со строкой в формате YYYY;mm;dd;HH; */
   
            if ( (minutes = read_two_digits(4, tmp[14], tmp[15], 0)) == -1) {               /* проверяем корректность значения минут в файле */
        
                errors(5, *str_num, tmp);                                                   /* если функция возвращает -1 (т. е. ошибку), записываем ее в лог и переходим к следующей строке */                       
                continue; 
            }    


            

            if (tmp[17] == '-') {
                
                is_negative = 1;                                            /* если значение температуры начинается с - (т.е. оно отрицательное), переменной i присваивается значение 1 */

            } else {

                is_negative = 0;
            }    


            if (tmp[18] == '\n') {                                                          /* если значение температуры состоит из 1 символа (например, 0) */

                temperature = read_two_digits(5, '0', tmp[17], 0);                          /* отправляем для проверки один символ */

            } else { 
                
                temperature = read_two_digits(5, tmp[17 + is_negative], tmp[18 + is_negative], 0); 
            }
                
            if (temperature == -1) {                                                         /* проверяем корректность значения температуры */
                       
                errors(6, *str_num, tmp);                                                    /* если функция возвращает -1 (т. е. ошибку), записываем ее в лог и переходим к следующей строке файла */                                     
                continue; 
            }    

            elem++;                                                                         /* увеличиваем количество элементов, если значение минут и температуры соответствует требованиям */

            if (is_negative == 1) temperature *= -1;                                        /* если значение температуры отрицательное, домножим число на -1 */

            if (elem <= 70) {                                                               /* проверяем не переполнится ли массив значений с температурой */
                   
                    hour_data->temp_values[elem-1] = temperature;                           /* записываем в структуру полученную температуру */
                    hour_data->temp_ctr = elem;                                             /* записываем количество элементов в массиве с измерениями температуры */
                    printf("Temp = %d, num = %d\n", hour_data->temp_values[elem-1],hour_data->temp_ctr);

            } else {

                    errors(10, *str_num, tmp);                                              /* выводим ошибку, если количество измерений в час превышает 70 */

            }

        } else if (elem > 0) {                                                              /* если после совпавших значений прочитано значение, которое не совпадает вплоть до часов */
            
            max_and_min(hour_data->temp_values, hour_data->temp_ctr, &hour_data->max, &hour_data->min);   /* вызываем функцию поиска максимального и минимального значения и записываем эти значения в переменные min и max*/
            
                if ( (byte = ftell(file)) == -1) {                                          /* запрашиваем текущую позицию в файле */
            
                    return;                                                                 /* возвращаемся без записи если функция ftell выдала ошибку */
            
                } else {

                    *byte_num = byte;                                                       /* записываем текущую позицию если ftell вернул какое-либо значение и выходим из функции */
                    return;
                }
        }
    }
}

/* 
функция записи ошибок с выводом строки в лог-файл. Принимает номер ошибки, номер строки и саму строку

Помимо этого, лог-файл используется для вывода ошибок самой программы для ее отладки
*/
void errors (const int error_num, int line_num, const char *line) {

    FILE *log = fopen("errors_log", "a+");
    line_num += 1;

    switch(error_num) {

        case 0: fprintf(log, "Internal error in function %s\n", line); break;
        case 1: fprintf(log, "No data with respect to %s\n", line); break;
        case 5: fprintf(log, "Problem with minutes in line %d:\n%s", line_num, line); break;
        case 6: fprintf(log, "Problem with temperature value in line %d:%s\n", line_num, line); break;

        case 7: fprintf(log, "\n%s:", line); break;
        case 16: fprintf(log, "%s", line); break;
            
        case 9: fprintf(log, "Problem with format in line %d:\n%s", line_num, line); break;
        case 10: fprintf(log, "Measurement frequency exceeded in line %d:\n%s", line_num, line); break;                /* превышена частота измерений */

    }
        
    fclose(log);

}

/* 
Функция читает два переданных ей элемента массива и проверяет их на корректность.
Опции для проверки: 1 - месяц, 2 - день (без передачи месяца), 3 - час, 4 - минута, 5 - температура, 6 - день (с указанием месяца)
Аргумент add_parametr применяется с опцией 5 (передается номер месяца) для корректности отражения количества дней в месяце.
Функция возвращает -1 в случае ошибки и прочитанное число в случае корректного заполнения.  
*/

int read_two_digits (const int option, const char first_d, const char second_d, const int add_parametr) {

    int tmp_num = 0;                                         /* переменная для записи числа внутри функции */     

/* проверяем корректность переданной опции, если опция некорректная, запишем это в лог */

if ( (option < 0) || (option > 6) ) {

    errors(0, 0, "read_two_digits, wrong option number");
    return -1;
}

/* блок, проверяющий являются ли два переданных символа числами */

        if( (first_d >= '0') && (first_d <= '9') ) {         /* проверяем является ли первый переданный символ числом */
                                
            tmp_num = ( (int)first_d - '0') * 10;            /* вставляем прочитанный символ в разряд десятков числа */
        
        } else {

            errors(16, 0, "Ошибка 1!");
            return -1;                                        /* возвращаем -1 если первый символ не является числом */
        }


        if( (second_d >= '0') && (second_d <= '9') ) {        /* проверяем является ли второй переданный символ числом */
                                
            tmp_num += ((int)second_d - '0');                 /* вставляем прочитанный символ в разряд единиц числа  */
        
        } else {

            char st[9];
            st[0] = 'd';
            st[1] = 'i';
            st[2] = 'g';
            st[3] = ':';
            st[4] = ' ';
            st[5] = second_d;
            st[6] = '!';
            st[7] = '\0';
            errors(16, 0, st);
            return -1;                                        /* возвращаем -1 если второй символ не является числом */
        }

/* блок проверки полученного из символов числа в соответствии с номером переданной опции*/

    if ( option == 0 ) return tmp_num;                          /* сразу возвращаем число, если функция вызвана без опций */

    int values[5][2] = {1, 12, 1, 31, 0, 23, 0, 59, 0, 99};     /* двумерный массив со значениями для сравнения полученного числа */

    for (int i = 1; i < 6; i++) {                               /* подбираем номер опции */

        if (i == option) {                                      /* входим в цикл проверки, если номер опции совпадает */

            if ( (tmp_num >= values[i-1][0]) && (tmp_num <= values[i-1][1])) {      /* цикл сравнивает полученное число с соответствующими элементами двумерного массива */

                printf("Опция %d. Сравнили %d с %d и %d, успешно\n", i, tmp_num, values[i-1][0], values[i-1][1]);
                return tmp_num;

            } else {

                printf("Опция %d. Сравнили %d с %d и %d, ошибка\n", i, tmp_num, values[i-1][0], values[i-1][1]);
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

    return -1;
}


/* 

Функция проверки корректности введенных аргументов. 

Принимает открытый файл, номер, соответствующий аргументу (-f = 0, -m = 1, -d = 2, -h = 3, -M = 4), 
массив со значением аргумента и путь/название файла

Возвращает -1 в случае некорректного ввода файла, 0 или соответствующее значение > 0 в остальных случаях

*/

int read_arg(FILE *file, const int arg_num, const char *arg_value, const char *path) {

    char data[STRING_LENGHT] = {0};
    strcat(data, "2021;");

/* если иные аргументы отсутствуют, программа запускается в стандартном режиме */

   if (arg_num == 0) {

       year_temp (file, data);
       return 0;
   }
   
/* если пользователь ввел иные аргументы, программа проверяет их корректность и вызывает следующую функцию */

    int month;  

    if ( (month = read_two_digits(1, arg_value[0], arg_value[1], 0)) == -1) return -1;              /* проверяем корректность ввода значения месяца */
                                                                                                    
    data[5] = arg_value[0];                                                                     /* если месяц введен корректно, записываем его в строку */
    data[6] = arg_value[1];
    data[7] = ';';
    data[8] = '\0';


    if (arg_num == 1) {

       printf("Получен аргумент -m %d\n", month);
       return 0;
   }

    int day;

    if ( (day = read_two_digits(6, arg_value[3], arg_value[4], month)) == -1) return -1;            /* проверяем корректность ввода значения дня */
                                                                                 
    data[8] = arg_value[3];                                                                     /* если день введен корректно, записываем его в строку */
    data[9] = arg_value[4];
    data[10] = ';';
    data[11] = '\0';

/* Вывод информации при вводе пользователем опции -d (дневная температура) */
    if (arg_num == 2) {

        data_day_t day_data = { 0 };


        day_temp(file, data, &day_data);

        if (day_data.hours_ctr == 0) {                                                           /* если функция day_temp не запи списала ни одного значения */

            printf("No measurments found for %s\n", arg_value);                                 /* выводим ошибку и выходим из функции */
            return 1;            
        }

        printf("Average temperature for %c%c.%c%c.%c%c%c%c was %dº Celsius.\n", data[8], data[9], data[5], 
        data[6],  data[0], data[1], data[2], data[3], day_data.average_temp); 

        printf("Maximum temperature is %dº Celsius, minimum temperature is %dº Celsius.\n", day_data.max, day_data.min);

        return 0;
   }

    int hour; 
    
    if ( (hour = read_two_digits(3, arg_value[6], arg_value[7], 0)) == -1) return -1;            /* проверяем корректность ввода значения часа */

    data[11] = arg_value[6];                                                                     /* если день введен корректно, записываем его в строку */
    data[12] = arg_value[7];
    data[13] = ';';
    data[14] = '\0';

 /* Вывод информации при вводе пользователем опции -H (часовая температура) */   
    if (arg_num == 3) {

        data_hour_t hour_temperature = { 0 };
        int av_temp = 0;
        int tempr = 0;
        int str_num = 0;

        
        hour_temp(file, &tempr, data, &hour_temperature, &str_num);

        printf("temp_ctr = %d\n", hour_temperature.temp_ctr);
        
        if (hour_temperature.temp_ctr == 0) {                                                     /* если после вызова функции не нашлось искомого значения */

            printf("No measurments found for %s\n", arg_value);                                   /* выводим ошибку и выходим из функции */
            return 1;

        }

        av_temp = average_temp(hour_temperature.temp_values, hour_temperature.temp_ctr);

        printf("Average temperature for %c%c.%c%c.%c%c%c%c from %c%c:00 to %c%c:59 was %dº Celsius.\n", data[8], data[9], data[5], 
        data[6],  data[0], data[1], data[2], data[3],data[11], data[12], data[11], data[12], av_temp); 

        printf("Maximum temperature is %dº Celsius, minimum temperature is %dº Celsius.\n", hour_temperature.max, hour_temperature.min);

        return 0;
   }


    int minute;     
    if ( (minute = read_two_digits(4, arg_value[9], arg_value[10], 0)) == -1) return -1;         /* проверяем корректность ввода значения часа */

    data[14] = arg_value[9];                                                                     /* если день введен корректно, записываем его в строку */
    data[15] = arg_value[10];
    data[16] = ';';
    data[17] = '\0';

    minute_temp_option(file, data, path);                                                          /* вызываем функцию, которая находит и выводит значение температуры в конкретную минуту */

    return 0;
}


/* функция main читает аргументы, открывает файл с данными и передает аргументы для проверки в функцию read_arg */
int main(int argc, char **argv) {

    char path[PATH_LENGTH] = { 0 };
    extern char *optarg;
    extern int optind;
    extern int enterr;
    extern int optout;
    const char *opts = "f:m:d:H:M:h"; 

    int ret, ret_2;
    int arg_num = 0;                             /* переменная для записи значения аргумента: -m = 1, -d = 2, -h = 3, -M = 4 */    
    char arg_value[15] = {0};                    /* перемменная для записи значения, переданного аргументами -m -d -h -M */                                                           
    int f_arg = 0;                               /* переменная для проверки наличия аргумента -f */



    if (argc == 1) {

        printf("No argument added. Run the programm with argument -h for a help.\n");
        return 1;
    }

            while ( (ret = getopt(argc, argv, opts)) != -1)  {

                 switch(ret) {

                    case 'h': {
                        HELP_INSTRUCTION
                        return 0;  
                    }

                    case 'f': {
                        
                        strncpy(path, optarg, PATH_LENGTH); 
                        f_arg = 1; 
                        break;
                    }                   
                    
                    case 'M': {

                        strncpy(arg_value, optarg, 15);
                        arg_num = 4;
                        break;

                    }

                    case 'H': {

                        if(arg_num < 3) {                                  /* защита от перезаписывания */ 
                            strncpy(arg_value, optarg, 15);
                            arg_num = 3;
                        }

                        break;
                    }

                    case 'd': {

                        if(arg_num < 2) {                                 /* защита от перезаписывания */ 
                            strncpy(arg_value, optarg, 15);
                            arg_num = 2;
                        }

                        break;
                    }

                    case 'm': {

                        if(arg_num < 1) {                                 /* защита от перезаписывания */                         
                            strncpy(arg_value, optarg, 15);
                            arg_num = 1;
                        }

                        break;
                    }                
                
                    case '?': {       

                        printf("Wrong argument. Run the programm with argument -h for a help.\n");
                        return 1;
                    }

                }

            }
    
    if (f_arg == 0) {

        printf("Argument -f <filename.csv> is obligatory. Run the programm with argument -h for a help.\n");
        return 1;
    
    } else {

        FILE *file = fopen(path, "r");                      

        if(file == NULL) {                                  
            printf("Unable to open a file\n");
        return -1;
        }

        if ( (ret_2 = read_arg(file, arg_num, arg_value, path)) == -1) {

            printf("Wrong argument: %s. Run the programm with argument -h for a help.\n", arg_value);
            fclose(file);
            return -1;

        } 

        fclose(file);
        return 0;

    }
}