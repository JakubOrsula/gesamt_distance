//
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
// =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  ----------------------------------------------------------------
//
//  **** Module  :  GSMT_ThreadBase <implementation>
//       ~~~~~~~~~
//  **** Project :  GESAMT
//       ~~~~~~~~~
//  **** Classes :  gsmt::ProgressBar
//       ~~~~~~~~~  gsmt::ThreadBase
//
//  (C) E. Krissinel 2008-2017
//
// =================================================================
//

#include <string.h>
#include <stdio.h>

#include "rvapi/rvapi_interface.h"

#include "gsmt_threadbase.h"
#include "gsmt_base.h"
#include "gsmt_utils.h"

// =================================================================

gsmt::ProgressBar::ProgressBar()  {
  rvapiProgressBarId = NULL;
  rvapiTimeLabelId   = NULL;
  progress   = 0.0;
  strcpy ( timestr,"--:--:--" );
  hidden = false;
  cells  = NULL;
  nCells = 0;
  reset ( 100,'.' );
}

gsmt::ProgressBar::~ProgressBar()  {
  if (cells)               delete[] cells;
  if (rvapiProgressBarId)  delete[] rvapiProgressBarId;
  if (rvapiTimeLabelId)    delete[] rvapiTimeLabelId;
}

void gsmt::ProgressBar::setRVAPIProgressWidgets (
                                          mmdb::cpstr progressBarId,
                                          mmdb::cpstr etaId ) {
  mmdb::CreateCopy ( rvapiProgressBarId,progressBarId );
  mmdb::CreateCopy ( rvapiTimeLabelId  ,etaId         );
}

void gsmt::ProgressBar::reset ( int ncells, char c )  {
  if (ncells>0)  {
    if (cells)  delete[] cells;
    cells = new char[ncells+1];
    cells[ncells] = char(0);
    nCells = ncells;
  }
  progress = 0.0;
  strcpy ( timestr,"--:--.--" );
  for (int i=0;i<nCells;i++)
    cells[i] = c;
  c0     = c;
  nCell0 = -1;
  t0     = time(NULL);
  t1     = t0;
  iclock = 0;

  if (rvapiProgressBarId)  {
    rvapi_set_progress_value ( rvapiProgressBarId,2,nCells );
    rvapi_reset_label        ( rvapiTimeLabelId,timestr );
    rvapi_flush();
  }

}

void gsmt::ProgressBar::setVisible ( bool visible )  {
  hidden = (!visible) || rvapiProgressBarId;
}

void gsmt::ProgressBar::setCell ( int cellNo, char c )  {
  if ((cellNo>=0) && (cellNo<nCells))  {
    if (cells[cellNo]!=c)  {
      cells[cellNo] = c;
      display ( false );
    }
  }
}

static char sclock[] = "-+o=";// "|/-\\";

void gsmt::ProgressBar::setProgress ( int p, int total, char c )  {
int     cellNo,ihour,imin,isec;
time_t  t = time ( NULL );

  if (total>0)  {
    progress  = p;
    progress /= total;
  } else
    progress  = 1.0;
  cellNo = mmdb::mround ( progress*nCells ) - 1;
  if ((t-t1>=1) || (total<=0))  {
    for (int i=nCell0+1;i<=cellNo;i++)
      cells[i] = c;
    if (!sclock[iclock])
      iclock = 0;
    if (progress>0.0)  {
      getHours ( (1.0/progress-1.0)*(t-t0),ihour,imin,isec );
      sprintf  ( timestr,"%02i:%02i.%02i",ihour,imin,isec );
    } else
      strcpy   ( timestr,"00:00.00" );    
    if (!hidden)  {
      printf ( "\r %3.0f%% %s  time left: %s %1c  ",
               100.0*progress,cells,timestr,sclock[iclock++] );
      if (total<=0)
        printf ( "\n\n" );
      fflush ( stdout );
    }
    nCell0 = cellNo;
    t1 = t;
    if (rvapiProgressBarId)  {
      rvapi_set_progress_value ( rvapiProgressBarId,3,cellNo+1 );
      rvapi_reset_label        ( rvapiTimeLabelId,timestr );
      rvapi_flush();
    } 
  }
}

void gsmt::ProgressBar::display ( bool final )  {
UNUSED_ARGUMENT(final);
int i,n=0;

  for (i=0;i<nCells;i++)
    if (cells[i]!=c0)  n++;

  if (!hidden)  {
    printf ( "\r %3.0f%% %s \n",
             100.0*mmdb::realtype(n)/mmdb::realtype(nCells-1),cells );
    fflush ( stdout );
  }
  
  if (rvapiProgressBarId)  {
    rvapi_set_progress_value ( rvapiProgressBarId,3,n );
    rvapi_reset_label        ( rvapiTimeLabelId,"Finished" );
    rvapi_flush();
  } 

}


// =================================================================

gsmt::ThreadBase::ThreadBase()  {
  threadNo    = 0;
  threads     = NULL;
  pbar        = NULL;
  lock        = NULL;
  n_jobs      = 0;
  job_cnt     = 0;
  nthreads    = 1;
  verbosity   = 1;
  keepRunning = NULL;
  finished    = false;
}

gsmt::ThreadBase::~ThreadBase()  {}

void gsmt::ThreadBase::run()  {}

void gsmt::ThreadBase::displayProgress()  {
int ndone;

  if (lock)
    pthread_mutex_lock ( lock );

  if (threads && pbar)  {
    ndone = 0;
    for (int i=0;i<nthreads;i++)
      ndone += threads[i]->job_cnt;
    pbar->setProgress ( ndone,n_jobs );
  }
  
  if (lock)
    pthread_mutex_unlock ( lock );

}

void gsmt::ThreadBase::initThread ( PPThreadBase     thread_vector,
                                    int              thread_No,
                                    int              n_threads,
                                    pthread_mutex_t *lock_mutex,
                                    bool            *keep_running,
                                    PProgressBar     progressBar,
                                    int              verbosity_key )  {
  threads      = thread_vector;
  nthreads     = n_threads;
  threadNo     = thread_No;
  if (!threadNo)  pbar = progressBar;  // only in one thread!
            else  pbar = NULL;
  lock         = lock_mutex;
  job_cnt      = 0;
  verbosity    = verbosity_key;
  keepRunning  = keep_running; 
}



void *run_thread ( void *arg )  {
  (gsmt::PThreadBase(arg))->run();
  return NULL;
}

void gsmt::run_threads ( PPThreadBase     threads,
                         int              nthreads,
                         pthread_mutex_t *lock,
                         bool            *keepRunning,
                         PProgressBar     progressBar,
                         int              verbosity )  {
pthread_t      *tid;
pthread_attr_t  attr;
time_t          t0 = time(NULL);
int             n, ihour,imin,isec;

  if (verbosity>=0)  {
    if (nthreads==1)
          printf ( " ... 1 thread will be used\n\n" );
    else  printf ( " ... %i threads will be used\n\n",nthreads );
  }

  if (progressBar)  {
    progressBar->reset      ( pbar_len     );
    progressBar->setVisible ( verbosity==1 );
  }
  
  // Prepare threads

  tid  = new pthread_t[nthreads];
  pthread_attr_init ( &attr );
  pthread_attr_setdetachstate ( &attr,PTHREAD_CREATE_JOINABLE );

  // Run threads
  for (n=0;n<nthreads;n++)  {
    threads[n]->initThread ( threads,n,nthreads,lock,keepRunning,
                             progressBar,verbosity );
    if (n>0)
      pthread_create ( &(tid[n]),&attr,run_thread,threads[n] );
  }

  run_thread  ( threads[0] );

  // Wait for threads to finish and merge hits

  for (n=1;n<nthreads;n++)  {
    pthread_join ( tid[n],NULL );
    threads[0]->displayProgress();
  }

  if (keepRunning)  {
    if (progressBar)
      progressBar->setProgress ( 1,0 );
  }

  if (verbosity>=0)  {
    getHours ( time(NULL)-t0,ihour,imin,isec );
    printf   ( " ... total time:  %02i:%02i.%02i\n",ihour,imin,isec );
  }
  
  // Release resources
  
  pthread_attr_destroy ( &attr );
  delete[] tid;
  
}

