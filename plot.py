import pandas as pd
import matplotlib.pyplot as plt

# Lecture du fichier CSV
df = pd.read_csv("resultats.csv")

# Calcul de la moyenne et de l'écart-type par programme et par nombre de threads
stats = df.groupby(["Programme", "Threads_Totaux"])["Temps_Secondes"].agg(["mean", "std"]).reset_index()

# Récupérer la liste des programmes uniques présents dans le CSV
programmes = stats["Programme"].unique()

for prog in programmes:
    # Filtrer les données pour chaque programme
    subset = stats[stats["Programme"] == prog]
    
    plt.figure(figsize=(8, 5))
    
    plt.errorbar(
        subset["Threads_Totaux"], 
        subset["mean"], 
        yerr=subset["std"], 
        marker='o', 
        color='b', 
        ecolor='r', 
        capsize=5, 
        linewidth=2
    )
    
    plt.title(f"Temps d'exécution - {prog}")
    plt.xlabel("Nombre total de threads")
    plt.ylabel("Temps d'exécution (secondes)")
    #L'axe des Y commence toujours à 0
    plt.ylim(bottom=0)
    plt.xticks([2, 4, 8, 16, 32])
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.tight_layout()
    
    # Nom du fichier de sortie dynamisé selon le programme
    filename = f"performance_{prog.lower()}.png"
    plt.savefig(f"./img/{filename}", dpi=300)
    plt.close() # Ferme la figure pour libérer la mémoire
    print(f"Graphique généré : {filename}")