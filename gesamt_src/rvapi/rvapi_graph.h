//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_graph  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Graph - API Graph class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#ifndef RVAPI_GRAPH_H
#define RVAPI_GRAPH_H

#include "rvapi_node.h"

namespace rvapi  {

  class Plot;
  class GraphData;

  class Graph : public Node  {

    public:
      Graph ( const char * graphId,
              int  hrow     = -1,
              int  hcol     = 0,
              int  hrowSpan = 1,
              int  hcolSpan = 1
            );
      Graph ();
      ~Graph();

      virtual NODE_TYPE   type() { return NTYPE_Graph; }
      virtual bool wasModified();
      virtual bool treeWasModified();

      void addGraphData ( const char * gdataId,
                          const char * gdataTitle
                        );
      void addPlot      ( const char * plotId,
                          const char * plotTitle,
                          const char * xName,
                          const char * yName
                        );

      GraphData * getGraphData ( std::string gdataId );
      Plot      * getPlot      ( std::string plotId  );

      inline void setDefPlot   ( std::string plotid )
                                                { defplot = plotid; }
      inline void setGraphSize ( int w, int h ) { width = w; height = h; }
      
      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

    protected:
      std::vector<Plot      *> plots;
      std::vector<GraphData *> gdata;
      std::string              defplot; //!< selected by default
      int                      width,height;
      int                      xslider,yslider; //!< 0/1 for on/off

      void initGraph();
      void freeGraph();
      bool checkModified();

      virtual void make_xmli2_content  ( std::string & tag,
                                         std::string & content );

  };

}

#endif // RVAPI_GRAPH_H
