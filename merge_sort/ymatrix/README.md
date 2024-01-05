# solution files description
  task1.cc     implementation of task1, parallel sequential scan of the row
               use about 2 * ncpu threads.

  task2.cc     implementation of task2, find out the ordered three ranges if 
               exist parallelly by binary like searching, and then print each
               range.

  task3.cc     implementation of task3, search method is same as task2, using
               a more merge step to output the rows order by row->b

  row.cc       common threaded boundary search code for task2 & task3 and the
               row printer

  row.h        row & range defination

  other files and scripts are for testing and benchmark