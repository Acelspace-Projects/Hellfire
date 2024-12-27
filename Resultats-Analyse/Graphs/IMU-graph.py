import matplotlib.pyplot as plt
import numpy as np

decollageImu=1680060

t,temp,ax,ay,az,gx,gy,gz,mx,my,mz=np.loadtxt("../DATA/IMU-FIXED.TXT", delimiter=";", unpack=True)
deb=0;fin=-1
# deb=167000;fin=179000 # zoom vol
# deb=167950;fin=178500 # zoom simu
# deb=167950;fin=170000 # zoom poussee et un peu plus
t=t[deb:fin]-decollageImu
ax=ax[deb:fin];ay=ay[deb:fin];az=az[deb:fin]
gx=gx[deb:fin];gy=gy[deb:fin];gz=gz[deb:fin]

fig4,accel=plt.subplots()
accel.set_title("Données brutes de l'IMU")
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
# fig4.tight_layout()
fig4.savefig("./OUT/IMU.svg")

trajecto=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
vGlobal=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
angleSelf=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])
wSelf=np.array([np.array([0.0,0.0,0.0]) for i in range(len(t))])

a=np.array([np.array([ay[i],az[i],ax[i]]) for i in range(len(t))])
wDotSelf=np.array([np.array([gy[i],gz[i],gx[i]]) for i in range(len(t))])

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

x=np.array([trajecto[i,0] for i in range(len(t))])
y=np.array([trajecto[i,1] for i in range(len(t))])
z=np.array([trajecto[i,2] for i in range(len(t))])
fig5=plt.figure()
traj=fig5.add_subplot(projection='3d')
traj.set_title("Trajectographie")
traj.plot3D(x,y,z)
traj.set_xlabel('x')
traj.set_ylabel('y')
traj.set_zlabel('z')
fig5.savefig("./OUT/TRAJ.svg")

wx=np.array([wSelf[i,0] for i in range(len(t))])
wy=np.array([wSelf[i,1] for i in range(len(t))])
wz=np.array([wSelf[i,2] for i in range(len(t))])
vx=np.array([vGlobal[i,0] for i in range(len(t))])
vy=np.array([vGlobal[i,1] for i in range(len(t))])
vz=np.array([vGlobal[i,2] for i in range(len(t))])
fig5,vit=plt.subplots()
vit.set_title("Données du LSM6DSOX")
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
fig5.savefig("./OUT/TRAJ2.svg")

# Export des données
export=""
for r in range(len(t)):
    for i in trajecto[r]:
        export+=str(i)+";"
    for i in angleSelf[r][:-1]:
        export+=str(i)+";"
    export+=str(angleSelf[r][-1])+"\n"

f = open("./OUT/VOL-BLEND.txt", "w")
f.write(export)
f.close()
print("Saved")

plt.show()