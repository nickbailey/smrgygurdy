from artifastring.violin_instrument import *
from artifastring.monowav import *

BUFSIZE = 1024

violin = ViolinInstrument()
out = MonoWav("output.wav")
bowforce = 0.4
bowvelocity = 1

#violin.finger(0, 0.3)

for a in xrange(10):
	violin.reset()
	bowvelocity+=0.1
	violin.bow(0, 0.1, bowforce, bowvelocity)
	for b in xrange(500):
		buf = out.request_fill(BUFSIZE)
		violin.wait_samples(buf, BUFSIZE)
	
		
