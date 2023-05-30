# calling the 'pi.x' is computationally intensive 

this case, instead of using 'hyper-threading' (i.e., not fully using logical cpu cores)

better setting the number of maximum parallel tasks (this case 'ptask' in 'laucnh.py') to physical cpu core number

-> significantly improve performance
