import multiprocessing as mp

def aFunction(x):	# 'x' iterable with item 'z'

	return [[len(z),z] for z in x]

if __name__ == '__main__':

	# Get # of avail cpu cores
	ptasks = mp.cpu_count()
	print(f'# of avail resource {ptasks}')

	# Create ap pool that has # workers
	pool = mp.Pool(processes=ptasks)

	# read the file in 
	f = open('stopWord.txt','r')
	l = f.readlines()
	f.close()

	# remove new line character
	d = [ w.strip() for w in l ]
	print(d)
	# get the result asynchronously
	#result = pool.apply_async(aFunction,args=([d],))
	result = pool.apply_async(aFunction,[d])

	print(result.get())
