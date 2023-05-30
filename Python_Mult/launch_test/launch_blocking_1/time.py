import datetime

def record_start_end_time():
    start_time = datetime.datetime.now()
    start_time_str = start_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
    print("Start Date and Time:", start_time_str)

    # Simulate some processing time
    for _ in range(10000000):
        pass

    end_time = datetime.datetime.now()
    end_time_str = end_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
    print("End Date and Time:", end_time_str)

    elapsed_time = end_time - start_time
    print("Elapsed Time:", elapsed_time)

record_start_end_time()

