import os
import multiprocessing as mp

def run_process(pool_id,Ncpu):
	
	os.makedirs(f"pool_{pool_id}", exist_ok=True)
	os.chdir(f"pool_{pool_id}")
	os.system(f"mpirun -np {Ncpu} /home/uccawkj/Software/Python_Mult/test_programs_bin/pi.x {pool_id} > output.txt")
	return (pool_id, "completed")

if __name__ == '__main__':
	ptasks = mp.cpu_count()
	print('available cpus = {}'.format(ptasks))

	num_pools = 20
	num_processes = ptasks / num_pools

	# create pool
	pool = mp.Pool(processes=num_pools)

	process_dict = {}
	for i in range(num_pools):
		process_dict[i] = pool.apply_async(run_process,args=(i,num_processes,))

	pool.close()
	pool.join()

	for i in range(num_pools):
		result = process_dict[i].get()
		process_dict[i] = result

	print(process_dict)
