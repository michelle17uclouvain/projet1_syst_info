#!/bin/bash

CSV_FILE="resultats.csv"
TOTAL_THREADS=(2 4 8 16 32)
RUNS=5

#  titres des colonnes (écrase l'ancien CSV s'il existe)
echo "Programme,Threads_Totaux,Run,Temps_Secondes" > "$CSV_FILE"

PROG1="./philosophes"
if [ -f "$PROG1" ]; then
    echo "Benchmark de $PROG1..."
    for t in "${TOTAL_THREADS[@]}"; do
        for ((r=1; r<=RUNS; r++)); do
            # Écrit le début de la ligne sans retour à la ligne (-n)
            echo -n "Philosophes,$t,$r," >> "$CSV_FILE"
            
            # /usr/bin/time ajoute automatiquement le temps (%e) et un retour à la ligne à la fin du fichier (-a)
            /usr/bin/time -f "%e" -o "$CSV_FILE" -a "$PROG1" "$t" > /dev/null
        done
    done
fi

PROG2="./rw"
if [ -f "$PROG2" ]; then
    echo "Benchmark de $PROG2..."
    for t in "${TOTAL_THREADS[@]}"; do
        half=$(( t / 2 ))
        for ((r=1; r<=RUNS; r++)); do
            echo -n "Lecteurs_Ecrivains,$t,$r," >> "$CSV_FILE"
            
            # On passe half pour le type 1 et half pour le type 2
            /usr/bin/time -f "%e" -o "$CSV_FILE" -a "$PROG2" "$half" "$half" > /dev/null
        done
    done
fi

echo "Mesures terminées et sauvegardées dans $CSV_FILE."