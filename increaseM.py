lineList = []
with open("bcsp_generate.c", 'r') as infile:
    for line in infile:
        lineList.append(line)
MsubN = int(lineList[23][10:]) + 1
lineList[23] = lineList[23][:10] + str(MsubN) + '\n'
with open("bcsp_generate.c", 'w') as outfile:
    for line in lineList:
        outfile.write(line)