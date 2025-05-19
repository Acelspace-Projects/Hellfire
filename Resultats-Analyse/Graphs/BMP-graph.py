import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

decollageTensio=1752882

# t_brut,bmp_brut=np.genfromtxt("../DATA/TENSIO-TRIM.TXT", delimiter=";", usecols=[0,2], unpack=True, invalid_raise=False)
df = pd.read_csv("../DATA/TENSIO-TRIM.TXT", sep=";", usecols=[0, 2], engine="python")
df = df.dropna()
t_brut, bmp_brut = df.iloc[:, 0].values, df.iloc[:, 1].values
p0=bmp_brut[3450]; g=9.81; Cp=1006; T0=30+273.15; R=8.314; M=29e-3
# altitude = lambda p: ((2*Cp*T0)/(7*g))*np.log(p0/p)
altitude = lambda p: -((R*T0)/(M*g))*np.log(p/p0)



def graphBmp(t,bm,cal,zoomlvl):
    fig2,bmp=plt.subplots()
    bmp.set_title("Données du BMP180")
    bmp.set_xlabel("Temps (ms)")
    color="tab:blue"
    bmp.set_ylabel("Valeur brute (Pa)", color=color)
    bmp.plot(t, bm, "--", color=color)
    bmp2 = bmp.twinx()
    color2="tab:red"
    bmp2.set_ylabel("Altitude (m)", color=color2)
    bmp2.plot(t,cal(bm), color=color2)
    bmp2.grid()
    # fig2.tight_layout()
    fig2.savefig("./OUT/BMP-ZOOM"+str(zoomlvl)+".svg")

graphBmp(t_brut,bmp_brut,altitude,0)

t_z2=t_brut[3450:3750]-decollageTensio;bmp_z2=bmp_brut[3450:3750] # zoom vol
graphBmp(t_z2,bmp_z2,altitude,1)



calib=altitude(bmp_z2)
vit=[(calib[i+1] - calib[i])/(t_z2[i+1] - t_z2[i])*1e3 for i in range(len(calib)-1)]

montee=t_brut[3500:3542]-decollageTensio;calib_montee=altitude(bmp_brut[3500:3542])
vit_montee=[(calib_montee[i+1] - calib_montee[i])/(montee[i+1] - montee[i])*1e3 for i in range(len(calib_montee)-1)]

descente=t_brut[3538:3720]-decollageTensio;calib_descente=altitude(bmp_brut[3538:3720])
vit_descente=[(calib_descente[i] - calib_descente[i-1])/(descente[i] - descente[i-1])*1e3 for i in range(1,len(calib_descente))]

print("\nVitesse moyenne de descente: " + str(np.mean(vit_descente))+ " m.s-1")

fig3,vites=plt.subplots()
vites.set_title("Vitesse z calculée des données du BMP180")
vites.set_xlabel("Temps (ms)")
vites.set_ylabel("Vitesse (m.s-1)")
vites.plot(t_z2[:-1],vit, label="Vitesse z calculée")
vites.hlines(np.mean(vit_descente), xmin=descente[0], xmax=descente[-1], color="tab:orange", label="Moyenne de la vitesse z de descente")
vites.set_ylim((-50,200))
vites.grid()
vites.legend()
fig3.savefig("./OUT/BMP-VIT.svg")

plt.show()