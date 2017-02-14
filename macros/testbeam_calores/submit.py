import os

name = 'Prototype'
particles = ["pionp","pionn"]
# Second data set - hcal only - +ve beam
pdata = [(2, [3720]),
	 (4, [3719]),
	 (6, [3717]),
         (8, [3716]),
         (12, [3715]),
         (16, [3714]),
         (20, [3713]),
         (30, [3712]),
         (40, [3707]),
         (50, [3706]),
         (60, [3705])
       ]

# Second data set - hcal only - -ve beam
ndata = [(2, [3721]),
         (4, [3722]),
         (6, [3723]),
         (8, [3724]),
         (12, [3725]),
         (16, [3730]),
         (24, [3731]),
         (32, [3732])
       ]
  

main = os.getcwd()
outputdir = main + "/outputdir_set2"
if not os.path.exists(outputdir):
  os.mkdir(outputdir)
jobfilename = 'jobs'
if not os.path.exists(jobfilename):
  os.mkdir(jobfilename)
files_to_be_copied = [ "Fun4All_scan.C", "Fun4All_scan.sh" ]
condor_config =  """Universe      = vanilla
Notification  = Error
Getenv        = true
Executable    = job.sh
Output        = job.out
Error         = job.err
Log           = job.log
Coresize      = 0
Queue
\n"""

for ipart in range(0,2):
 part = particles[ipart]
 if part=="pionp": data = pdata;
 if part=="pionn": data = ndata;
 for (en, runs) in data:
   cmd = "./Fun4All_scan.sh " + name + "_" + part + "_" + str(en) + " " + outputdir
   dirname = jobfilename + "/" + name + "_" + part + "_" + str(en)
   if not os.path.exists(dirname):
    os.mkdir(dirname)
   os.chdir(dirname)
   if os.path.exists("job.sh"):
     os.remove("job.sh")
   f = open('job.sh', 'w+')
   f.write("#!/bin/sh\n")
   f.write(cmd)
   f.close()
   os.system("chmod 777 job.sh")
   if not os.path.exists("condor"):
     f2 = open('condor','w')
     f2.write(condor_config)
     f2.close()
   for file in files_to_be_copied:
    if not os.path.exists(file):
     os.symlink(main+"/"+file, file)
   os.system("condor_submit condor")
   f3 = open("ndst.list",'w+')
   for run in runs:
    #filename = "/gpfs/mnt/gpfs02/sphenix/data/data01/t1044-2016a/production/Production_0715_EMCalSet2_HCalPR12/beam_0000" + str(run) + "-0000.root\n"
    #filename = " /phenix/u/jinhuang/links/sPHENIX_work/Prototype_2017/Production_0128_WithHCalCalib/beam_0000" + str(run) + "-0000.root\n"
    filename = "/gpfs/mnt/gpfs04/sphenix/user/abhisek/production/outputdir/beam_0000" + str(run) + "-0000.root\n"
    f3.write( filename )
   f3.close()
   #Finally move to main
   os.chdir(main)
