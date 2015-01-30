<?php

  $worker = new GearmanWorker();
  $worker->addServer('127.0.0.1','4730');
  $worker->addFunction('rtb_log_write', 'rtb_log_write_function');

  while ($worker->work());

  function rtb_log_write_function($job)
  {
      //  For example purpose
      echo "Received job: {$job->handle()}" . PHP_EOL; 
      return strrev($job->workload());
  }
  ?>
