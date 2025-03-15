import numpy as np
import pandas as pd

dataFile = pd.read_csv("../../Resultats-Analyse/DATA/TENSIO-TRIM.TXT", sep=";", usecols=[0, 2], engine="python")
dataFile = dataFile.dropna()
t_brut, bmp_brut = dataFile.iloc[:, 0].values, dataFile.iloc[:, 1].values

export = "timestamp,UP\n"
for i in range(len(t_brut)):
    export += str(int(t_brut[i])) + "," + str(int(bmp_brut[i])) + "\n"

# Ecriture dans le fichier
with open("./bmp-data.csv", "w") as f:
    f.write(export)