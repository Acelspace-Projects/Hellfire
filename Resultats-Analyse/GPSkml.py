import numpy as np
import simplekml

kml = simplekml.Kml()
n=np.loadtxt("./DATA/GPS.TXT", delimiter=";", dtype=str, usecols=(0), unpack=True)
lat,long=np.loadtxt("./DATA/GPS.TXT", delimiter=";", usecols=(1,2), unpack=True)

points=[]
for i in range(len(n)):
    # n[i]=n[i].split(":")# UTC+2
    print(n[i].split(":"))
    points.append({"name": n[i], "coords": (long[i], lat[i])})

s=3134 ;e=-1
# Add points to the KML
for point in points[s:e]:
    # print(point["name"], [point["coords"]])
    kml.newpoint(name=point["name"], coords=[point["coords"]])

# Add a line
kml.newlinestring(name="Path", coords=[point["coords"] for point in points])

kml.save("path.kml")
