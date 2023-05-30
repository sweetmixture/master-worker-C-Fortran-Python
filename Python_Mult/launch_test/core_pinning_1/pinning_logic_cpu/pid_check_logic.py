import multiprocessing as mp
import os,time

def get_pid():
	pid = mp.current_process().pid
	return pid

# get total number of 'logical' cpus
ptasks = mp.cpu_count()


# Worker Pool Configuration
workgroup     = [ mp.Pool(1) for i in range(ptasks) ]
worker_status = [ 'idel' for i in range(ptasks) ]
job_returns   = [ None for i in range(ptasks) ]


print(f'System CPU(logical) count: {ptasks}')

# apply tasks
for i, worker in enumerate(workgroup):
	job_returns[i] = worker.apply_async(get_pid)
	worker_status[i] = 'running'

# Get finished jobs
while 'running' in worker_status:

	for i, worker in enumerate(workgroup):

		# if status 'running'
		if worker_status[i] == 'running':

			# 1. Check if job done
			if job_returns[i].ready():
				# Change worker status to 'idle'
				worker_status[i] = 'idle'

	#print(worker_status)

# Finalise WorkerPool
for worker in workgroup:
	worker.close()
	worker.join()

# PrintOut Results
for item in job_returns:

	# Note! 'job_returns' contain objects 'AsyncApply' to retreive resultsm must use method 'get()'
	print(f'pid : {item.get()}')
