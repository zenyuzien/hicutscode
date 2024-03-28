import sys
import math
import glob
import os
import numpy as np
from collections import Counter


def prfx_ol(SA, DA, SA_siz, DA_siz):
    SA_bi = (str(bin(int(SA.split('.')[0]))[2:].zfill(8))+str(bin(int(SA.split('.')[1]))[2:].zfill(8))+str(bin(int(SA.split('.')[2]))[2:].zfill(8))+str(bin(int(SA.split('.')[3]))[2:].zfill(8)))
    DA_bi = (str(bin(int(DA.split('.')[0]))[2:].zfill(8))+str(bin(int(DA.split('.')[1]))[2:].zfill(8))+str(bin(int(DA.split('.')[2]))[2:].zfill(8))+str(bin(int(DA.split('.')[3]))[2:].zfill(8)))
    n1 = int(min(SA_siz, DA_siz))
    if(SA_bi[0:n1]==DA_bi[0:n1]):
        return 1
    else:
        return 0

def prt_ol(PRT1, PRT2):
    P10 = PRT1.split('/')[0]
    P11 = PRT1.split('/')[1]
    P20 = PRT2.split('/')[0]
    P21 = PRT2.split('/')[1]
    if((P10==P20) or (P11=='0x00') or (P21=='0x00')):
        return 1
    else:
        return 0

def rng_ol(RNG1, RNG2):
    if(RNG1[0] in range(RNG2[0], RNG2[1])) or (RNG1[1] in range(RNG2[0], RNG2[1])) or (RNG2[0] in range(RNG1[0], RNG1[1])) or (RNG2[1] in range(RNG1[0], RNG1[1])):
        return 1
    else:
        return 0
# j = range(0,4)
# print(j[3])

path = sys.argv[1]
if os.path.exists(path+'_rngStatistics.txt'):
    os.remove(path+'_rngStatistics.txt')

f = open(path+'_rngStatistics.txt', "a")
# l = ('fname \t SA_rng \t DA_rng \t SP_rng \t DP_rng \t SP_prfx \t DP_prfx \t PRT_rng \t single_rl \t max_ol \t mxOL_indx \t OL_allGroup \t Total_rl')
# f.write(l+'\n')
for fd in glob.glob(path+'/*.rl'):
    # print(fd)
    fname = fd.split('.rl')[0]
    fname = fname.split('/')[1]
    # print(fname)
    fil = open(fd, "r")
    i = 0
    SP_rng = 0
    DP_rng = 0
    SP_prfxRng = 0
    DP_prfxRng = 0
    single_rl = 0
    SA = []
    DA = []
    SA_siz = []
    DA_siz = []
    PRT_siz = []
    SP_1 = []
    DP_1 = []
    SP_2 = []
    DP_2 = []
    PRT = []
    toDRng = 0
    oneDRng = 0
    for lines in fil:
        line = lines.split()
        # print(line)
        SA_temp = line[0].split('@')[1]
        SA.insert(i, SA_temp.split('/')[0])
        DA.insert(i, line[1].split('/')[0])
        SA_siz.insert(i, line[0].split('/')[1])
        DA_siz.insert(i, line[1].split('/')[1])
        SP_1.insert(i, line[2])
        SP_2.insert(i, line[4])
        n1 = int(SP_1[i])
        n = int(SP_2[i])+1
        S_rng = 0
        if((n & (n-1) == 0) and n != 0) and ((n1==0) or (n1 & (n1-1) == 0) and n1 != 0):
            SP_prfxRng = SP_prfxRng+1
        elif SP_1[i] != SP_2[i]:
            SP_rng = SP_rng+1
            S_rng = 1
        # print(n1, n, SP_prfxRng, SP_rng)
        DP_1.insert(i, line[5])
        DP_2.insert(i, line[7])
        n1 = int(DP_1[i])
        n = int(DP_2[i])+1
        D_rng = 0
        if((n & (n-1) == 0) and n != 0) and ((n1==0) or (n1 & (n1-1) == 0) and n1 != 0):
            DP_prfxRng = DP_prfxRng+1
        elif DP_1[i] != DP_2[i]:
            DP_rng = DP_rng+1
            D_rng = 1
        if(S_rng==1) and (D_rng==1):
            toDRng = toDRng+1
        elif((S_rng==1) and (D_rng==0)) or ((S_rng==0) and (D_rng==1)):
            oneDRng = oneDRng+1
        PRT.insert(i, line[8])
        PRT_siz.insert(i, line[8].split('/')[1])
        if((SA_siz[i]!='32') and (DA_siz[i]!='32') and (SP_1[i]==SP_2[i]) and (DP_1[i]==DP_2[i]) and (PRT_siz[i]=='0xFF')):
            single_rl = single_rl + 1
        i=i+1

    # print(SA)
    # print(DA)
    # print(SP_1)
    # print(DP_1)
    # print(SP_2)
    # print(DP_2)
    # print(PRT)
    rl_siz = i
    rl_OL = []
    rl_cnt = []
    m = 0
    ol_grp = {m: [0]}
    NOL_list = list(range(0,rl_siz))
    # try:
    #     NOL_list.remove(92)
    # except ValueError:
    #     pass
    # print(NOL_list)
    # ol_grp[1] = [3,4,5]
    # ol_grp[2] = [0,3,5,6,7,8,9]
    # ol_grp[1].append(10)
    # # del ol_grp[1]
    # tmp = ol_grp[1] + list(set(ol_grp[2]) - set(ol_grp[1]))
    # print(tmp)

    # def intersection(lst1, lst2): 
    #     return len(list(set(lst1) & set(lst2)))
  
    # # Driver Code 
    # lst1 = [15, 9, 10, 56, 23, 78, 5, 4, 9] 
    # lst2 = [9, 4, 5,36, 47, 26, 45, 87] 
    # print(intersection(lst1, lst2))
    
    # print(10 not in [x for v in ol_grp.values() for x in v])
        
    for j in range(0,rl_siz):
        # print(i)
        # rl_OL[j] = 0
        rl_OL.insert(j,0)
        rl_cnt.insert(j,0)
        SAol_past = 0
        DAol_past = 0
        if(j not in ol_grp[m]):
            m = m+1
            ol_grp[m] = [j]
        for k in range(0,j):
            if(k<j):                
                SAol_past = prfx_ol(SA[j], SA[k], SA_siz[j], SA_siz[k])
                DAol_past = prfx_ol(DA[j], DA[k], DA_siz[j], DA_siz[k])
                SP_past = rng_ol([int(SP_1[j]), int(SP_2[j])], [int(SP_1[k]), int(SP_2[k])])
                DP_past = rng_ol([int(DP_1[j]), int(DP_2[j])], [int(DP_1[k]), int(DP_2[k])])
                PRT_past = prt_ol(PRT[j], PRT[k])
                rl_OL_tmp = SAol_past * DAol_past * SP_past * DP_past * PRT_past
                if(rl_OL_tmp == 1):
                    rl_OL[j] = 1
                    rl_cnt[j] = rl_cnt[j]+1
                    try:
                        NOL_list.remove(j)
                    except ValueError:
                        pass
                    # print(len(ol_grp), ol_grp)
                    if(k not in ol_grp[m]):
                        ol_grp[m].append(k)
                else:
                    # rl_OL[j] = 0
                    SAol_past = 0
                    DAol_past = 0
            # if(k>9):
            #     print(j,':',k,': -->', rl_OL_tmp, '\t', SAol_past, DAol_past, SP_past, DP_past, PRT_past, '\t', rl_cnt[j], rl_OL[j])
            # else:
            #     print(j,':',k,': -->', rl_OL_tmp, '\t\t', SAol_past, DAol_past, SP_past, DP_past, PRT_past, '\t', rl_cnt[j], rl_OL[j])
            # print(SAol_past, DAol_past, SP_past, DP_past, PRT_past)
            print(i, j, k)          

    # print(ol_grp)
    # print(rl_cnt.index(max(rl_cnt)))
    # print(rl_cnt)
    j1 = np.array(rl_cnt)
    j2 = (len(np.sort(j1[j1<(rl_siz/4)])))
    SA_prfx = rl_siz - (SA_siz.count('32'))
    DA_prfx = rl_siz - (DA_siz.count('32'))
    PRT_prfx = PRT_siz.count('0x00')
    line1 = ('fname\t\tSA_rng\tDA_rng\tSP_rng\tDP_rng\tSP_prfx\tDP_prfx\t1D_rng\t2D_rng\tPRT_rng\tsingle_rl\tmax_ol\tmxOL_indx\tHighPr_rl\tTotal_rl\n')
    line2 = (str(fname) + '\t' + str(SA_prfx) + '\t\t' + str(DA_prfx) + '\t\t' + str(SP_rng) + '\t\t' + str(DP_rng) + '\t\t' + str(SP_prfxRng) + '\t\t' + str(DP_prfxRng) + '\t\t' +str(oneDRng) + '\t\t' + str(toDRng) + '\t\t' + str(PRT_prfx) + '\t\t' + str(single_rl) + '\t\t\t' + str(max(rl_cnt)) + '\t\t' + str(rl_cnt.index(max(rl_cnt))+1) + '\t\t\t' + str(rl_OL.count(0)) + '\t\t\t' + str(rl_siz)+'\n\n')
    # print(line1)
    # print(line2)
    f.write(line1)
    f.write(line2)

    f.write('Multimatch(n/4)\t\t>n/4\n')
    l=(str(j2)+'\t\t\t'+str(rl_siz-j2)+'\n\n')
    f.write(l)
    f.write('Overlap count\n')
    l = (str(rl_cnt)+'\n\n')
    f.write(l)
    f.write('overlap possibiltty\n')
    l = (str(rl_OL)+'\n\n')
    f.write(l)
    f.write('overlap combinations\n')
    f.write(str(ol_grp)+'\n\n')
    f.write('High Priority List\n')
    f.write(str(NOL_list)+'\n\n\n\n')