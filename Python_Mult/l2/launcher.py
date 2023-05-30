import multiprocessing as mp
import os,sys
import datetime,time
import json

def get_time():
	start_time = datetime.datetime.now()
	time_str = start_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:]
	return time_str

def get_time_diff(time_1,time2):
	diff = time_2 - time_1
	return diff.total_seconds()

# -------------------------------------

def app_mpi_execute(app,ptasks):

	stderr = 'stderr.txt'
	stdout = 'stdout.txt'
	exit_status = os.system(f'mpirun -np {ptasks} {app} 2> {stderr} > {stdout}')

	return exit_status

def job_eater(job_package):
	
	job_id = job_package[0]
	app = job_package[1]
	ptasks = job_package[2]

	root_dir = os.getcwd()
	os.makedirs(f'run_{job_id+1}',exist_ok=True)
	os.chdir(f'run_{job_id+1}')
	job_status = app_mpi_execute(app,ptasks)
	os.chdir(root_dir)

	return job_status

# -------------------------------------

if __name__ == '__main__':

	ptasks = mp.cpu_count()

	try:
		n_cpus_per_workgroup = sys.argv[1]
	except:
		n_cpus_per_workgroup = 4

	# set number of workgroups
	n_workgroups = int(ptasks / n_cpus_per_workgroup)

	# configure workgroups
	workgroup = []
	for i in range(n_workgroups):
		workgroup.append(mp.Pool(n_cpus_per_workgroup))
	workgroup_status = [ None for i in range(len(workgroup)) ]

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
	total_jobs = 10
	#jobs_for_workgroups = [ int(total_jobs / n_workgroups) for i in range(n_workgroups) ]
	#jobs_for_workgroups[-1] = jobs_for_workgroups[-1] + total_jobs%n_workgroups
	#print(total_jobs,jobs_for_workgroups)

	job_package = []
	for i in range(total_jobs):
		tmp = [ i, app, n_cpus_per_workgroup ]
		job_package.append(tmp)
	
	# Parallel Execution - worker pool ------------------------------------------
	while total_jobs:

		for i,worker in enumerate(workgroup):
			workgroup_status[i] = worker.apply_async(job_eater,args=(job_package[total_jobs-1],))
			total_jobs = total_jobs - 1
			#print(job_status.get())	# return '0' - success

		#for job_status in workgroup_status:
		#	job_status.get()

	# ----------------------------------------------------------------------------

	# end of parallelism
	for worker,job_status in zip(workgroup,workgroup_status):
		worker.close()
		worker.join()
		

