import matplotlib.pyplot as plt
import numpy as np

import os
os.chdir("C:/Users/Mixcraftio/Code/Repos/Hellfire/Resultats-Analyse")

def TENSIO():
    def graphTens(t,tens,cal,zoomlvl):
        fig,tensio=plt.subplots()
        tensio.set_title("Valeurs du Tensionomètre")
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
        fig.savefig("./TRAITE/TENSIO-ZOOM"+str(zoomlvl)+".svg")

    t_brut,tens_brut=np.loadtxt("./DATA/TENSIO.TXT", delimiter=";", usecols=[0,1], unpack=True)
    tensioCal = lambda x: 2E-7*(x**3)-4E-4*(x**2)+0.295*x+3.0096

    graphTens(t_brut,tens_brut,tensioCal,0)
    t_z1=t_brut[175000:187500];tens_z1=tens_brut[175000:187500] # zoom vol
    graphTens(t_z1,tens_z1,tensioCal,1)
    t_z2=t_brut[177000:177100]-decollageTensio;tens_z2=tens_brut[177000:177100] # zoom para
    graphTens(t_z2,tens_z2,tensioCal,2)

def BMP():
    def graphBmp(t,bm,cal,zoomlvl):
        fig2,bmp=plt.subplots()
        bmp.set_title("Valeurs du BMP180")
        bmp.set_xlabel("Temps (ms)")
        color="tab:blue"
        bmp.set_ylabel("Valeur brute", color=color)
        bmp.plot(t, bm, "--", color=color)
        bmp2 = bmp.twinx()
        color2="tab:red"
        bmp2.set_ylabel("Altitude (m)", color=color2)
        bmp2.plot(t,cal(bm), color=color2)
        bmp2.grid()
        # fig2.tight_layout()
        fig2.savefig("./TRAITE/BMP-ZOOM"+str(zoomlvl)+".svg")

    t_brut,bmp_brut=np.genfromtxt("./DATA/TENSIO.TXT", delimiter=";", usecols=[0,2], unpack=True, invalid_raise=False)
    p0=bmp_brut[3450]; g=9.81 ; Cp=1006; T0=30+273.15
    bmpCal = lambda p: ((2*Cp*T0)/(7*g))*np.log(p0/p)

    graphBmp(t_brut,bmp_brut,bmpCal,0)
    t_z1=t_brut[1000:10000];bmp_z1=bmp_brut[1000:10000] # zoom 1
    graphBmp(t_z1,bmp_z1,bmpCal,1)
    t_z2=t_brut[3450:3750]-decollageTensio;bmp_z2=bmp_brut[3450:3750] # zoom vol
    graphBmp(t_z2,bmp_z2,bmpCal,2)

def IMU():
    t,temp,ax,ay,az,gx,gy,gz,mx,my,mz=np.loadtxt("./DATA/IMU-FIXED.TXT", delimiter=";", unpack=True)
    # deb=0;fin=-1
    # deb=167000;fin=179000 # zoom vol
    deb=167950;fin=178500 # zoom simu
    # deb=167950;fin=170000 # zoom poussee et un peu plus
    t=t[deb:fin]-decollageImu
    ax=ax[deb:fin];ay=ay[deb:fin];az=az[deb:fin]
    gx=gx[deb:fin];gy=gy[deb:fin];gz=gz[deb:fin]

    fig3,accel=plt.subplots()
    accel.set_title("Valeurs de l'IMU")
    accel.set_xlabel("Temps (ms)")
    accel.set_ylabel("Accélération (m.s-2)")
    accel.plot(t,ax,label="ax")
    accel.plot(t,ay,label="ay")
    accel.plot(t,az,label="az")
    # accel.plot(t,gx,label="gx")
    # accel.plot(t,gy,label="gy")
    # accel.plot(t,gz,label="gz")
    accel.grid()
    accel.legend()
    # fig3.tight_layout()
    fig3.savefig("./TRAITE/IMU.svg")

    trajecto=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
    vGlobal=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
    angleSelf=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
    wSelf=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
    a=np.array([np.array([ay[i],az[i],ax[i]]) for i in range(len(t))]);wDotSelf=np.array([np.array([gy[i],gz[i],gx[i]]) for i in range(len(t))])
    
    # theta=45 # Angle de lancer de la fusée (en deg)
    # elevation=80 # Angle d'élévation de la rampe (en deg)
    # phi=90-elevation
    # theta=theta*np.pi/180
    # phi=phi*np.pi/180

    def Euler_SingleStep(xDot,xi):
        xf=xi+xDot*h
        return xf

    def acceleration():
        # https://fr.wikipedia.org/wiki/Angles_d%27Euler

        # print("CALC")
        axSelf=a[i,0];aySelf=a[i,1];azSelf=a[i,2]
        aSelf=np.array([axSelf,aySelf,azSelf])
        anglexSelf=angleSelf[i+1,0];angleySelf=angleSelf[i+1,1];anglezSelf=angleSelf[i+1,2]
        psi=anglezSelf;theta=angleySelf;phi=anglexSelf
        Matrix=[[np.cos(psi)*np.cos(phi)-np.sin(psi)*np.cos(theta)*np.sin(phi), -np.cos(psi)*np.sin(phi)-np.sin(psi)*np.cos(theta)*np.cos(phi), np.sin(psi)*np.sin(theta)],
                [np.sin(psi)*np.cos(phi)+np.cos(psi)*np.cos(theta)*np.sin(phi), -np.sin(psi)*np.sin(phi)+np.cos(psi)*np.cos(theta)*np.cos(phi), -np.cos(psi)*np.sin(theta)],
                [np.sin(theta)*np.sin(phi),                                     np.sin(theta)*np.cos(phi),                                      np.cos(theta)]]
        P=np.array(Matrix)
        # accel=np.array([axSelf,aySelf,azSelf])
        accel=np.matmul(P,aSelf)
        return accel

    time=0
    for i in range(len(t)-1):
        h=(t[i+1]-t[i])/1e3

        wSelf[i+1]=Euler_SingleStep(wDotSelf[i],wSelf[i])
        angleSelf[i+1]=Euler_SingleStep(wSelf[i+1],angleSelf[i])

        accel=acceleration()
        vGlobal[i+1]=Euler_SingleStep(accel,vGlobal[i])
        trajecto[i+1]=Euler_SingleStep(vGlobal[i+1],trajecto[i])

        time+=h

    print(time,t[-1]-t[0])
    x=np.array([trajecto[i,0] for i in range(len(t))])
    y=np.array([trajecto[i,1] for i in range(len(t))])
    z=np.array([trajecto[i,2] for i in range(len(t))])
    fig4=plt.figure()
    traj=fig4.add_subplot(projection='3d')
    traj.set_title("Trajectographie")
    traj.plot3D(x,y,z,"r")
    traj.set_xlabel('x')
    traj.set_ylabel('y')
    traj.set_zlabel('z')
    fig4.savefig("./TRAITE/TRAJ.svg")

    wx=np.array([wSelf[i,0] for i in range(len(t))])
    wy=np.array([wSelf[i,1] for i in range(len(t))])
    wz=np.array([wSelf[i,2] for i in range(len(t))])
    vx=np.array([vGlobal[i,0] for i in range(len(t))])
    vy=np.array([vGlobal[i,1] for i in range(len(t))])
    vz=np.array([vGlobal[i,2] for i in range(len(t))])
    fig5,vit=plt.subplots()
    vit.set_title("Valeurs de Simulation")
    vit.set_xlabel("Temps (ms)")
    vit.set_ylabel("Vitesse (m.s-1)")
    vit.plot(t,wx,label="wx")
    vit.plot(t,wy,label="wy")
    vit.plot(t,wz,label="wz")
    vit.plot(t,vx,label="vx")
    vit.plot(t,vy,label="vy")
    vit.plot(t,vz,label="vz")
    vit.grid()
    vit.legend()
    # fig5.tight_layout()
    fig5.savefig("./TRAITE/TRAJ2.svg")

    # Export des données
    export=""
    for r in range(len(t)):
        for i in trajecto[r]:
            export+=str(i)+";"
        for i in angleSelf[r][:-1]:
            export+=str(i)+";"
        export+=str(angleSelf[r][-1])+"\n"

    f = open("./TRAITE/VOL-BLEND.txt", "w")
    f.write(export)
    f.close()
    print("Saved")

decollageTensio=1752882
decollageImu=1680060
# TENSIO()
# BMP()
IMU()
# plt.show()