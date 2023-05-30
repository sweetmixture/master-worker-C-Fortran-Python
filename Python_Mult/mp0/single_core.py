import multiprocessing as mp
import sys,subprocess

num_cpus_per_node = 40
num_node = 1
resource = num_cpus_per_node * num_node

rapp='bash /home/uccawkj/PAX/Python_Mult/MultiProcessPool_basic/sys_call.sh'

def run_system_call(command):
	pid = mp.current_process().pid
	command_with_pid = f"{command} {pid}"
	subprocess.run(command_with_pid, shell=True)

commands = [ rapp for i in range(160) ]
print(commands)

#with mp.Pool(processes=resource) as pool:
with mp.Pool(processes=1) as pool:
	
	cpu_cnt = mp.cpu_count()
	print("cpu_cnt:{}".format(cpu_cnt))
	
	'''
		map(target,args) : resposible for an interable - here 'commands' which is a list
	'''
	pool.map(run_system_call, commands)
