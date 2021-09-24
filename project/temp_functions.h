#pragma once 

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

/* #define DEBUG */		                                /* если опция определена, пррограмма будет записывать данные для отладки в файл */                     

/* секция с описанием функционала программы */
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

/* структура для хранения данных по каждому месяцу (структура создается для каждого месяца) */
typedef struct temp_data_for_month {

    int days_ctr;                                  /* счетчик элементов в массивах temp_max, temp_min */
    int temp_max[31];                              /* массив для записи максимальной температуры в каждом из дней месяца*/    
    int temp_min[31];                              /* массив для записи минимальной температуры в каждом из дней месяца*/ 
    int min;                                       /* максимальная температура в течение месяца (присваивается после сортировки массива temp_max) */ 
    int max;                                       /* минимальная температура в течение месяца (присваивается после сортировки массива temp_min) */ 
    float average_temp;                            /* среднее значение ежемесячной температуры (присваивается после заполнения и обработки массива av_temp_per_day) */ 

} data_month_t;

/* структура для хранения данных по каждому дню (для экономии памяти структура каждый раз перезаписывается)*/
typedef struct temp_data_for_day {

    int hours_ctr;                                 /* счетчик элементов в массивах temp_max, temp_min */
    int temp_max[24];                              /* массив с максимальным значением температуры в каждом часе дня */
    int temp_min[24];                              /* массив с минимальным значением температуры в каждом часе дня */
    int max;                                       /* максимальная температура в течение дня (заполняется после сортировки массива temp_max) */
    int min;                                       /* минимальная температура в течение дня (заполняется после сортировки массива temp_min) */
    float average_temp;                            /* среднее значение температуры в течение дня (присваивается после заполнения и обработки массива av_temp_per_hour) */ 

} data_day_t;

/* структура для хранения данных по каждому часу (для экономии памяти структура каждый раз перезаписывается)*/
typedef struct temp_data_for_hour {

    int temp_values[70];                           /* массив с информацией о средней температуре в каждой из минут часа */
    int temp_ctr;                                  /* счетчик элементов в массиве temp_values */
    int max;                                       /* переменная для записи максимальной температуры в часе (присваивается после заполнения и обработки массива temp_value) */
    int min;                                       /* переменная для записи минимальной температуры в часе (присваивается после заполнения и обработки массива temp_value) */
    float average_temp;                            /* среднее значение температуры в течение часа */ 

} data_hour_t; 

/* сопутствующие функции */
void errors (const int error_num, int line_num, const char *line);
int read_arg(FILE *file, const int arg_num, const char *arg_value, const char *path);
float average_temp(const int *temp_measurments, const int array_size);
void max_and_min(const int *array, const int elem_numb, int *max, int *min);
void month_name(const int month_num, char *name); 
int read_two_digits (const int option, const char first_d, const char second_d, const int add_parametr);

#ifdef DEBUG
void debug_file(const int option, const int arg_1, const int arg_2, const float arg_3, const char *string);
#endif 

/* функции, обеспечивающие функционал программы */
void year_temp (FILE *file, char *year_str);
data_month_t month_temp (FILE *file, char *desired_value, const int month);
void day_temp (FILE *file, char *desired_value, data_day_t *day_data);
void hour_temp (FILE *file, int *byte_num, char *desired_value, data_hour_t *hour_data, int *str_num);
void minute_temp_option (FILE *file, const char *desired_value, const char *path, const char *month_name);

