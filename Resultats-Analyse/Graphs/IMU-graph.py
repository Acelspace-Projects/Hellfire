import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv("../DATA/IMU-FIXED-TRIM.TXT", sep=";", engine="python")
df = df.dropna()
t, temp, ax, ay, az, gx, gy, gz, mx, my, mz = df.iloc[:, 0].values, df.iloc[:, 1].values, df.iloc[:, 2].values, df.iloc[:, 3].values, df.iloc[:, 4].values, df.iloc[:, 5].values, df.iloc[:, 6].values, df.iloc[:, 7].values, df.iloc[:, 8].values, df.iloc[:, 9].values, df.iloc[:, 10].values
gx, gy, gz = gx * np.pi / 180, gy * np.pi / 180, gz * np.pi / 180

fig,accel=plt.subplots()
accel.set_title("Données brutes d'acceleration de l'IMU")
accel.set_xlabel("Temps (ms)")
accel.set_ylabel("Accélération (m.s-2)")
accel.plot(t,ax,label="ax")
accel.plot(t,ay,label="ay")
accel.plot(t,az,label="az")
accel.legend()
fig.savefig("./OUT/Accel.svg")

fig,gyro=plt.subplots()
gyro.set_title("Données brutes de vitesse angulaire de l'IMU")
gyro.set_xlabel("Temps (ms)")
gyro.set_ylabel("Vitesse angulaire (°.s-1)")
gyro.plot(t,gx,label="gx")
gyro.plot(t,gy,label="gy")
gyro.plot(t,gz,label="gz")
gyro.grid()
gyro.legend()
fig.savefig("./OUT/Gyro.svg")

# Prepare arrays
N = len(t)
wGlobal = np.zeros((N, 3))
angleGlobal = np.zeros((N, 3))
aGlobal = np.zeros((N, 3))
vGlobal = np.zeros((N, 3))
pGlobal = np.zeros((N, 3))

wSelf = np.array([gx, gy, gz]).T
aSelf = np.array([ax, ay, az]).T

# Vecteur attitude initial (capteur monté verticalement + fusée tournée de 80°)
angleGlobal[0] = [-np.pi/2 - 80*np.pi/180, 0, 0]

# --- Variables pour le filtrage ---

# Correction du biais gyro avec les 500 premières valeurs
gyro_bias = np.mean(wSelf[:500], axis=0)

# Facteur de fusion pour correction gyro
alpha = 0.01

# Variables de rectification de la vitesse 
g0 = 9.81
threshold = 0.5  # Déviation acceptable par rapport à 9.81
decay = 0.98 # Facteur de diminution

def Euler_SingleStep(xDot, xi, h):
    xf = xi + xDot * h
    return xf

def selfToGlobal(val, rot):
    theta, psi, phi = rot
    A = [[np.cos(psi)*np.cos(phi)-np.sin(psi)*np.cos(theta)*np.sin(phi), -np.cos(psi)*np.sin(phi)-np.sin(psi)*np.cos(theta)*np.cos(phi), np.sin(psi)*np.sin(theta)],
         [np.sin(psi)*np.cos(phi)+np.cos(psi)*np.cos(theta)*np.sin(phi), -np.sin(psi)*np.sin(phi)+np.cos(psi)*np.cos(theta)*np.cos(phi), -np.cos(psi)*np.sin(theta)],
         [np.sin(theta)*np.sin(phi),                                     np.sin(theta)*np.cos(phi),                                      np.cos(theta)]]
    return np.dot(A, val)

for i in range(1, N):
    h = (t[i] - t[i-1]) * 1e-3

    # --- Correction du biais gyro ---
    corrected_w = wSelf[i-1] - gyro_bias
    wGlobal[i-1] = selfToGlobal(corrected_w, angleGlobal[i-1])
    angleGlobal[i] = Euler_SingleStep(wGlobal[i-1], angleGlobal[i-1], h)

    # --- Correction gyro par fusion ---
    a_norm = np.linalg.norm(aSelf[i-1])
    if abs(a_norm - g0) < threshold:
        ax_, ay_, az_ = aSelf[i-1]
        pitch_est = np.arctan2(ax_, np.sqrt(ay_**2 + az_**2))
        roll_est = np.arctan2(ay_, az_)
        angleGlobal[i][0] = (1 - alpha) * angleGlobal[i][0] + alpha * roll_est
        angleGlobal[i][1] = (1 - alpha) * angleGlobal[i][1] + alpha * pitch_est

    # --- Acceleration vers repère global ---
    aGlobal[i-1] = selfToGlobal(aSelf[i-1], angleGlobal[i-1]) - np.array([0, 0, 9.81])

    # --- Rectification de la vitesse pour les petites accélérations ---
    if abs(a_norm - g0) < threshold:
        vGlobal[i-1] *= decay

    vGlobal[i] = Euler_SingleStep(aGlobal[i-1], vGlobal[i-1], h)
    pGlobal[i] = Euler_SingleStep(vGlobal[i-1], pGlobal[i-1], h)

# Tracé de la trajectographie
x, y, z = pGlobal[:, 0], pGlobal[:, 1], pGlobal[:, 2]
fig2 = plt.figure()
traj = fig2.add_subplot(projection='3d')
traj.set_title("Trajectographie")
traj.plot3D(x, y, z)
traj.set_xlabel('x')
traj.set_ylabel('y')
traj.set_zlabel('z')
fig2.savefig("./OUT/TRAJ.svg")

# Export data
export = ""
for r in range(N):
    export += str(t[r]) + ";"
    export += ";".join(map(str, pGlobal[r])) + ";"
    export += ";".join(map(str, angleGlobal[r])) + "\n"

with open("./OUT/VOL-BLEND.txt", "w", encoding="UTF-8") as f:
    f.write(export)

plt.show()