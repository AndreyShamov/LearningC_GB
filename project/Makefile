TEST += test
SRCS += main.c
SRCS += temp_functions.c
APPNAME += main

all:

	gcc $(SRCS) -o $(APPNAME)

test:
		-gcc test.c -o $(TEST)
		./$(TEST)
		gcc $(SRCS) -o $(APPNAME)
		./$(APPNAME) -f data.csv

clean:
		rm -f $(APPNAME) $(TEST)
		rm -f errors_log
		rm -f data.csv
		rm -f read_values
