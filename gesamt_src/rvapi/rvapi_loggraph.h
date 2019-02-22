//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_loggraph  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::LogGraph - API LogGraph class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_LOGGRAPH_H
#define RVAPI_LOGGRAPH_H

#include "rvapi_graph.h"

namespace rvapi  {

  class Plot;
  class GraphData;

  class LogGraph : public Graph  {

    public:
      LogGraph ( const char * graphId,
                 int  hrow     = -1,
                 int  hcol     = 0,
                 int  hrowSpan = 1,
                 int  hcolSpan = 1
               );
      LogGraph ();
      ~LogGraph();

      virtual NODE_TYPE type() { return NTYPE_LogGraph; }

      virtual void flush_html  ( std::string & outDir,
                                 std::string & task );

    protected:
      void initLogGraph();
      void freeLogGraph();

      void getLogData ( Plot         *p,
                        GraphData    *g,
                        std::string & plotData,
                        std::string & plotOptions );

  };

}

#endif // RVAPI_LOGGRAPH_H
