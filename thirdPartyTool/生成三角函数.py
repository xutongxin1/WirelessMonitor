import numpy as np
nlist =range(1,10)
a = np.array(nlist)

sin = np.sin(a*np.pi/180)
np.around(sin,3)
print (sin)
np.savetxt("result.txt", sin,fmt="%.4f")