import multiprocessing as mp
import os,sys
import datetime,time
import json

def get_time():
	time = datetime.datetime.now()
	#time_str = start_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:]
	return time

def format_time(time):
	return time.strftime("%Y-%m-%d %H:%M:%S.%f")[:]

def get_time_diff(time_1,time_2):
	diff = time_2 - time_1
	return diff

# -------------------------------------

def app_mpi_execute(app,ptasks,arg):

	stderr = 'stderr.txt'
	stdout = 'stdout.txt'
	exit_status = os.system(f'mpirun -np {ptasks} {app} {arg} 2> {stderr} > {stdout}')

	return exit_status

def job_eater(job_package):
	
	pid = mp.current_process().pid

	job_id = job_package[0]
	app = job_package[1]
	ptasks = job_package[2]

	root_dir = os.getcwd()
	os.makedirs(f'run_{job_id+1}',exist_ok=True)
	os.chdir(f'run_{job_id+1}')
	job_status = app_mpi_execute(app,ptasks,pid)
	os.chdir(root_dir)

	return job_status # os.system() call result

# -------------------------------------

if __name__ == '__main__':

	ptasks = mp.cpu_count() / 2

	try:
		n_cpus_per_workgroup = int(sys.argv[1])
	except:
		n_cpus_per_workgroup = 4

	# set number of workgroups
	n_workgroups = int(ptasks / n_cpus_per_workgroup)

	# configure workgroups
	workgroup = []
	for i in range(n_workgroups):
		workgroup.append(mp.Pool(n_cpus_per_workgroup))

	# status - async 'return'
	workgroup_job_status  = [ None for i in range(len(workgroup)) ]	# handle
	workgroup_end_t   = [ None for i in range(len(workgroup)) ]
	workgroup_delta_t = [ None for i in range(len(workgroup)) ]

	# when launch
	workgroup_start_t = [ None for i in range(len(workgroup)) ]
	workgroup_job_id  = [ None for i in range(len(workgroup)) ]

	# workgroup_status
	workgroup_status = [ 'idle' for i in range(len(workgroup)) ] 

	# app 
	#app = '/home/uccawkj/mpi/master-worker-mpi/Python_Mult/test_programs_bin/pi.x'
	app = '/home/uccawkj/Software/Python_Mult/test_programs_bin/pi.x'

	# setting out
	print(f'avail resource    : {ptasks}')
	print(f'# workgroups      : {n_workgroups}')
	#for worker in workgroup:
	#	print(worker)
	print(f'# cpus per groups : {n_cpus_per_workgroup}')
	print(f'application path  : {app}')


	# configure jobs
	total_jobs = 20

	job_package = []
	for i in range(total_jobs):
		tmp = [ total_jobs - i - 1, app, n_cpus_per_workgroup ]	# id / app / cpus
		job_package.append(tmp)
	
	# possible job configuration - for 'total_jobs = 20'
	'''
		job_package[0]  = [19, app ... ]
		job_package[1]  = [18, app ... ]
		...
		job_package[19]  = [0 , app ... ]
	'''

	# Parallel Execution - worker pool ------------------------------------------

	# initial launch
	for i,worker in enumerate(workgroup):

		# apply tasks
		workgroup_job_status[i] = worker.apply_async(job_eater,args=(job_package[total_jobs-1],))
		# update workgroup status
		workgroup_status[i] = 'running'
		# record submisstion time & job id
		workgroup_start_t[i] = get_time()
		workgroup_job_id[i] = job_package[total_jobs - 1][0]
		# subtract submitted job
		total_jobs = total_jobs - 1

	while total_jobs:

		# collection + eating jobs
		for i,job_status in enumerate(workgroup_job_status):	# 'workgroup_status' list - AsyncTask Handler
		
			if workgroup_status[i] == 'running':

				if job_status.ready():

					workgroup_job_status[i] = job_status.get()
					workgroup_end_t[i] = get_time()
					workgroup_delta_t[i] = get_time_diff(workgroup_start_t[i],workgroup_end_t[i])
					print(f'workgroup: {i} job_id: {workgroup_job_id[i]} -> job status: {workgroup_job_status[i]} || start: {workgroup_start_t[i]} end: {workgroup_end_t[i]} delta: {workgroup_delta_t[i]}')

					workgroup_job_status[i] = -1
					workgroup_status[i] = 'idle'

					if total_jobs == 0:
						break
					else:
						# relauch - non-blocking
						workgroup_job_status[i] = worker.apply_async(job_eater,args=(job_package[total_jobs-1],))	# job_package -> improved by 'pop' or something similar later
						# update workgroup status
						workgroup_status[i] = 'running'
						# record submisstion time & job id
						workgroup_start_t[i] = get_time()
						workgroup_job_id[i]  = job_package[total_jobs - 1][0]

						total_jobs = total_jobs - 1


	# last collection
	while 'running' in workgroup_status:

		for i,job_status in enumerate(workgroup_job_status):
	
			if workgroup_status[i] == 'running':
		
				if job_status.ready():

					workgroup_job_status[i] = job_status.get()
					workgroup_end_t[i] = get_time()
					workgroup_delta_t[i] = get_time_diff(workgroup_start_t[i],workgroup_end_t[i])
					print(f'workgroup: {i} job_id: {workgroup_job_id[i]} -> job status: {workgroup_job_status[i]} || start: {workgroup_start_t[i]} end: {workgroup_end_t[i]} delta: {workgroup_delta_t[i]}')

					workgroup_job_status[i] = -1
					workgroup_status[i] = 'idle'

	# ----------------------------------------------------------------------------

	# end of parallelism
	for worker,job_status in zip(workgroup,workgroup_status):
		worker.close()
		worker.join()
		

