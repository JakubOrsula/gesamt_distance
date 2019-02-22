//
//  =================================================================
//
//    06.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_progressbar  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::ProgressBar - API Progress Bar class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#ifndef RVAPI_PROGRESSBAR_H
#define RVAPI_PROGRESSBAR_H

#include "rvapi_node.h"

namespace rvapi  {

  class ProgressBar : public Node  {

    public:

      enum PBAR_KEY { Hide=0,Show=1,Range=2,Value=3 };

      ProgressBar ( const char * pbarId,
                    int hrow     = -1,
                    int hcol     = 0,
                    int hrowSpan = 1,
                    int hcolSpan = 1
                  );
      ProgressBar ();
      ~ProgressBar();

      virtual NODE_TYPE type() { return NTYPE_ProgressBar; }

      void setProgressBar ( PBAR_KEY key, int value );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string tasks;
    int         range;
    int         pbr_width;

    void initProgressBar();
    void freeProgressBar();

  };

}

#endif // RVAPI_TEXT_H
