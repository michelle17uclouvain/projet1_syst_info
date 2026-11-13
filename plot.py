import pandas as pd
import matplotlib.pyplot as plt

# Lecture du fichier CSV
df = pd.read_csv("resultats.csv")

# Calcul de la moyenne et de l'écart-type par programme et par nombre de threads
stats = df.groupby(["Programme", "Threads_Totaux"])["Temps_Secondes"].agg(["mean", "std"]).reset_index()

plt.figure(figsize=(10, 6))

# Tracé d'une courbe par programme
for prog in stats["Programme"].unique():
    subset = stats[stats["Programme"] == prog]
    plt.errorbar(
        subset["Threads_Totaux"], 
        subset["mean"], 
        yerr=subset["std"], 
        marker='o', 
        capsize=5, 
        label=prog,
        linewidth=2
    )

plt.title("Temps d'exécution en fonction du nombre de threads (moyenne sur 5 runs)")
plt.xlabel("Nombre total de threads")
plt.ylabel("Temps d'exécution (secondes)")
plt.xticks([2, 4, 8, 16, 32])
plt.grid(True, linestyle="--", alpha=0.7)
plt.legend()
plt.tight_layout()

# Sauvegarde du graphique
plt.savefig("./img/performance.png", dpi=300)
print("Graphique généré avec succès : performance.png")