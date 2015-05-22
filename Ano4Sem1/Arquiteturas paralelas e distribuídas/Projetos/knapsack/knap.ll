#@ class = regular
#@ shell = /usr/bin/sh
#@ node = 1
#@ tasks_per_node = 16
#@ network.LAPI = csss,not_shared,us 
#@ wall_clock_limit = 0:15:00
#@ notification = always
#@ job_type = parallel
#@ output = $(host).$(jobid).$(stepid).out
#@ error = $(host).$(jobid).$(stepid).err
#@ account_no = mp309
#@ queue

./knap-lapi

