import re,sys
import numpy as np

a = open('dirfile.txt','w')

#This is your naming scheme
#target = "c:/GeSiM/NpC16 V2.15.49/Workplate/agindudetstingillumina/temp.txt"
#target = "c:/GeSiM/NPC16V2.15.53/dset20140903_115537_data/dset_track20140903_115537.txt"
target = sys.argv[1]

#Cool regular expression = split
ar = re.split("/",target)

#numpy array action
ar = np.array(ar)
#print ar
id = np.where(ar=='NPC16V2.15.57')


#first reference directory
posfrom = id[0][0]+1

#then loop through to create the string
ct = 0 
forstr = ''
for i in range(posfrom,len(ar)):
	if i < len(ar)-1:
	 ct = ct+1
	 forstr = forstr + '<dir'+str(ct)+'>'+ar[i]+'</dir'+str(ct)+'>'
	else:
	 forstr = forstr + '<fle>'+re.sub('.txt','',ar[i])+'</fle>'

forstr = forstr + "<num>"+str(ct)+"</num>"

a.write(forstr)
a.close()


