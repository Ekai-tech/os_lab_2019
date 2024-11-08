CC=gcc 
CFLAGS = -I. -Wall -pthread  # Добавляем -pthread для поддержки потоков  # Опции компилятора: -I. указывает на текущую директорию для поиска заголовочных файлов

.PHONY: all clean  # Объявление псевдонимов для целей, которые не соответствуют файлам

# Цель по умолчанию, которая собирает все исполняемые файлы
all: parallel_min_max zombie_process parallel_sum

# Правило для сборки исполняемого файла parallel_min_max
parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h 
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS) 

parallel_sum: parallel_sum.o utils.o # Зависимости для сборки parallel_sum
	$(CC) -o parallel_sum parallel_sum.o utils.o $(CFLAGS)

parallel_sum.o: parallel_sum.c utils.h # Зависимости для сборки объекта parallel_sum.o
	$(CC) -c parallel_sum.c $(CFLAGS)

utils.o: utils.c utils.h # Зависимости для сборки объекта utils.o
	$(CC) -c utils.c $(CFLAGS)

# Правило для сборки исполняемого файла zombie_process
zombie_process: zombie_process.o utils.o 
	$(CC) -o zombie_process $(CFLAGS) zombie_process.o utils.o 	


# Правило для компиляции объекта find_min_max.o из find_min_max.c
find_min_max.o : utils.h find_min_max.h 
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS) 

# Правило для компиляции объекта zombie_process.o из zombie_process.c
zombie_process.o: zombie_process.c find_min_max.h utils.h
	$(CC) -c zombie_process.c $(CFLAGS)

# Цель для очистки скомпилированных файлов и исполняемых программ
clean : 
	rm -f utils.o find_min_max.o parallel_min_max zombie_process zombie_process.o parallel_sum.o parallel_sum