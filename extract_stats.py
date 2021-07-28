import os

os.chdir("tests")

# We count the files in every folder that their names are like "resultsM(M size)_(test number)"
# to count how many problems have a solution for every M/N value.
#
nOfSolutions = [0 for i in range(80)]
for i in range(1500, 3500, 1):
    os.chdir("M" + str(i))
    for file in os.listdir():
        if "resultsM" + str(i) + "_" in file:
            nOfSolutions[int((i-1500)/25)] += 1
    os.chdir("..")
os.chdir("..")

# Writing the results in a file.
#
with open("nOfSolutions.txt", 'w') as outfile:
    for n in nOfSolutions:
        outfile.write(str(n/100).replace('.', ',') + ';')

# Calculating the αverage time the DPLL algorithm does for every M/N value.
#
os.chdir("tests")
timeStats = [0 for i in range(80)]
for i in range(1500, 3500, 1):
    os.chdir("M" + str(i))
    with open("resultsM" + str(i) + ".txt", 'r') as infile:
        fileData = [line for line in infile]
    index = 2
    while index < len(fileData)-1:
        additionalStep = 1 if fileData[index][0] == 'S' else 0
        timeStats[int((i-1500)/25)] += float(fileData[index+1+additionalStep][len("Time spent: "):len("Time spent: ")+6])*1000
        index += 5 + additionalStep
    os.chdir("..")
os.chdir("..")
timeStats = [round(stat/100) for stat in timeStats]

# Writing the results in a file.
#
with open("timeStatsdpll.txt", 'w') as outfile:
    for stat in timeStats:
        outfile.write(str(stat).replace('.', ',') + ';')
        
# Calculating the αverage time the WalkSat algorithm does for every M/N value.
#
os.chdir("tests")
timeStats = [0 for i in range(80)]
for i in range(1500, 3500, 1):
    os.chdir("M" + str(i))
    try:
        with open("resultswalkM" + str(i) + ".txt", 'r') as infile:
            fileData = [line for line in infile]
    except FileNotFoundError:
        os.chdir("..")
        continue
    index = 2
    while index < len(fileData)-1:
        additionalStep = 1 if fileData[index][0] == 'S' else 0
        timeStats[int((i-1500)/25)] += float(fileData[index+1+additionalStep][len("Time spent: "):len("Time spent: ")+6])*1000
        index += 6 + additionalStep
    os.chdir("..")
os.chdir("..")
timeStats = [round(stat/(nOfSolutions[i]*5)) if nOfSolutions[i] != 0 else 0 for i,stat in enumerate(timeStats)]

# Writing the results in a file.
#
with open("timeStatswalk.txt", 'w') as outfile:
    for stat in timeStats:
        if stat == 0:
            outfile.write(';')
        else:
            outfile.write(str(stat).replace('.', ',') + ';')