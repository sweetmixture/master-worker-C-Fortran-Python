# SuperFastPython.com
# example of setting the default number of workers in the process pool
from multiprocessing.pool import Pool
from multiprocessing import active_children
import multiprocessing as mp
 
# protect the entry point
if __name__ == '__main__':
	# create a process pool with the default number of workers
	pool = Pool()
	# report the status of the process pool
	print(pool)
	# report the number of processes in the pool
	print(mp.cpu_count())
	print(pool._processes)
	# report the number of active child processes
	children = active_children()
	print(len(children))
