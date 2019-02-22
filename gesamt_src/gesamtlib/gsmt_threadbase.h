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
//  **** Module  :  GSMT_ThreadBase <interface>
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

#ifndef __GSMT_ThreadBase__
#define __GSMT_ThreadBase__

#include <time.h>
#include <pthread.h>

#include "mmdb2/mmdb_mattype.h"

// =================================================================

namespace gsmt  {

  DefineClass(ProgressBar);

  class ProgressBar  {

    public:

      ProgressBar  ();
      ~ProgressBar ();

      void reset       ( int ncells=0, char c='.' );
      void setVisible  ( bool visible );
      void setCell     ( int cellNo  , char c='#' );
      void setProgress ( int p, int total, char c='#' );
      void display     ( bool final=false );
 
      void setRVAPIProgressWidgets ( mmdb::cpstr progressBarId,
                                     mmdb::cpstr etaId );

      inline bool isVisible()  { return !hidden; }
      inline mmdb::realtype getProgress  ()  { return progress; }
      inline mmdb::cpstr    getTimeString()  { return timestr;  }

    private:
      mmdb::pstr     cells;
      mmdb::realtype progress;
      mmdb::pstr     rvapiProgressBarId;
      mmdb::pstr     rvapiTimeLabelId;
      char           timestr[40];
      char           c0;
      bool           hidden;
      int            nCells,nCell0,iclock;
      time_t         t0,t1;

  };

  DefineClass(ThreadBase);

  class ThreadBase  {

    public:

      ThreadBase ();
      virtual ~ThreadBase();

      void initThread ( PPThreadBase     thread_vector,
                        int              thread_No,
                        int              n_threads,
                        pthread_mutex_t *lock_mutex,
                        bool            *keep_running,
                        PProgressBar     progressBar,
                        int              verbosity_key );

      virtual void run();
      
      void displayProgress();
      
    protected:
      int              threadNo;  // [0..nthreads-1]
      PPThreadBase     threads;
      PProgressBar     pbar;
      pthread_mutex_t *lock;
      int              n_jobs;
      int              job_cnt;
      int              nthreads;
      int              verbosity;
      bool            *keepRunning;
      bool             finished;

  };

  void run_threads ( PPThreadBase     threads,
                     int              nthreads,
                     pthread_mutex_t *lock,
                     bool            *keepRunning,
                     PProgressBar     progressBar,
                     int              verbosity );

}

#endif
