CC = gcc
CFLAGS = -Wall -Wextra -O0 -pthread
LDFLAGS = -lpthread

EXECS = philosophes rw

all: $(EXECS)

# Règle générique pour compiler chaque fichier .c en exécutable
%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Lance la compilation, puis le script de mesure, puis le script de graphique
plot: all
	chmod +x script.sh
	./script.sh
	python3 plot.py

clean:
	rm -f $(EXECS) *.csv *.png