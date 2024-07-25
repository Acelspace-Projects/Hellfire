import os
os.chdir("C:/Users/Mixcraftio/Code/Repos/Hellfire/Resultats-Analyse")

f=open("./BRUT/IMU.TXT")
text=f.read()
ntext=""
count=-1
c2=0
for c in text:
  if c == ".":
    ntext+="."
    count=1
  else:
    if c == "\t":
      ntext+=";"
    elif c == "\n":
      ntext+="\n"
      count=-1
      c2+=1
      if c2%10000 == 0:
        p=c2/2682202*100
        print(p)
    else:
      ntext+=c
    if count == 0:
      count=-1
      ntext+=";"
    elif count>=0:
      count-=1
  
f2=open("./TRAITE/IMU.TXT","w")
f2.write(ntext)
