# ProcessorScheduling
Implements backtracking algorithm to solve the processor scheduling problem; given number of processors and a list of runtimes, will find the best maximum workload. In other words, will assign the runtimes to the processors in a way such that the biggest workload is minimized. In order to make backtracking more efficient, five heuristics have been implemented. They are: 

  A. Always pick an unassigned task with the longest run-time to assign next; or, equivalently, sort the tasks in decreasing order of run-time and then assign them in sequence.
  
  B. Compute a lower bound on the maximum workload (by adding the run-times of the individual tasks, dividing by the number of processors, and rounding up to the nearest integer) and stop searching if a complete assignment with that workload is ever found.

  C. Keep track of the smallest maximum workload found previously (initially this is the workload for the -lwd assignment as noted above) and ignore any partial assignment that makes the workload for some processor at least this large.

  D. Not assign a task to a processor with the same (current) workload as a lower-numbered processor.

  E. Not assign succeeding tasks with the same run-time to processors with lower numbers.
