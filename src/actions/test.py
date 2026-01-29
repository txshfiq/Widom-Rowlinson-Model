import signac

project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

# Fetch the jobs matching the fixed parameters
jobs = project.find_jobs({'M': 8, 'L': 20})

# Sort them based on parameter 'c'
# usage of lambda allows you to specify the key to sort by
jobs_sorted = sorted(jobs, key=lambda job: job.sp.z)

for job in jobs_sorted:
    print(f"c: {job.sp.z} -> Job ID: {job.id}")