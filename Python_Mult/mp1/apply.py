import multiprocessing as mp
import os

# Define your function that will be run by each process
def run_process(pool_id,Ncpu):
	# Create a directory with the pool_id as its name
	os.makedirs(f"pool_{pool_id}", exist_ok=True)
	# Change to the new directory
	os.chdir(f"pool_{pool_id}")
	# Run your application using mpirun with the pool_id as an argument
	os.system(f"mpirun -np {Ncpu} /home/uccawkj/Software/Python_Mult/test_programs_bin/pi.x {pool_id} > output.txt")
	# Return the pool_id along with any other information you want to keep track of
	return (pool_id, "completed")


'''
	using: 'apply_async' ... concurrenc (yes) | Blocking (yes) | Ordered-results (no)
'''

if __name__ == '__main__':

	parallel_tasks = mp.cpu_count()
	print(f'Avail Resource: {parallel_tasks}')

	# Define the number of pools and processes per pool
	num_pools = 1
	num_processes = parallel_tasks / num_pools

	# Create a dictionary to store the mapping between pool_id and process object
	process_dict = {}

	# Create the pool and start the processes
	pool = mp.Pool(processes=num_pools)
	for i in range(num_pools):
		# Submit the process to the pool and store the process object in the dictionary
		process_dict[i] = pool.apply(run_process, args=(i,num_processes,))

	# Wait for all processes to complete
	pool.close()
	pool.join()

	# Retrieve the results from the process objects and update the dictionary
	for i in range(num_pools):
		result = process_dict[i].get()
		process_dict[i] = result

	# Print the mapping between pool_id and job status
	print(process_dict)

