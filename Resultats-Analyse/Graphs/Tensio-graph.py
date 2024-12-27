import matplotlib.pyplot as plt
import numpy as np

decollageTensio=1752882

def graphTens(t,tens,cal,zoomlvl):
    fig,tensio=plt.subplots()
    tensio.set_title("Données du Tensionomètre")
    tensio.set_xlabel("Temps (ms)")
    color="tab:blue"
    tensio.set_ylabel("Valeur brute (0-1023)", color=color)
    tensio.plot(t, tens, "--", color=color)
    tensio2 = tensio.twinx()
    color2="tab:red"
    tensio2.set_ylabel("Valeur calibrée (kg)", color=color2)
    tensio2.plot(t,cal(tens), color=color2)
    tensio2.grid()
    # fig.tight_layout()
    fig.savefig("./OUT/TENSIO-ZOOM"+str(zoomlvl)+".svg")

t_brut,tens_brut=np.loadtxt("../DATA/TENSIO.TXT", delimiter=";", usecols=[0,1], unpack=True)
tensioCal = lambda x: 2E-7*(x**3)-4E-4*(x**2)+0.295*x+3.0096

graphTens(t_brut,tens_brut,tensioCal,0)
t_z1=t_brut[175000:187500];tens_z1=tens_brut[175000:187500] # zoom vol
graphTens(t_z1,tens_z1,tensioCal,1)
t_z2=t_brut[177000:177100]-decollageTensio;tens_z2=tens_brut[177000:177100] # zoom para
graphTens(t_z2,tens_z2,tensioCal,2)

plt.show()