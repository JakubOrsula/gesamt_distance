//
//  =================================================================
//
//    18.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_loggraph  <implemenation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::LogGraph - API LogGraph class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include "rvapi_loggraph.h"
#include "rvapi_plot.h"
#include "rvapi_tasks.h"

rvapi::LogGraph::LogGraph ( const char *graphId,
                            int hrow, int hcol,
                            int hrowSpan, int hcolSpan  )
               : Graph(graphId,hrow,hcol,hrowSpan,hcolSpan)  {
  initLogGraph();
}

rvapi::LogGraph::LogGraph() : Graph()  {
  initLogGraph();
}

rvapi::LogGraph::~LogGraph()  {
  freeLogGraph();
}

void rvapi::LogGraph::initLogGraph()  {}
void rvapi::LogGraph::freeLogGraph()  {}

void rvapi::LogGraph::getLogData ( Plot         *p,
                                   GraphData    *g,
                                   std::string & plotData,
                                   std::string & plotOptions )  {
std::string excl = "nan,-nan,inf,-inf";

  p->getCommonOptions ( plotOptions,false );
  plotData.clear();

  int nLines = p->getNofLines();
  for (int k=0;k<nLines;k++)  {
    PlotLine *line = p->getLine ( k );
//          if (getGraphData(line->datId)==g)  {
    if (g->getId()==line->datId)  {
      int xset = g->getSetIndex ( line->xSetId );
      int yset = g->getSetIndex ( line->ySetId );
      if ((xset>=0) && (yset>=0))  {
        if (plotData.empty())  {
          plotData.append ( "  " );
          plotOptions.append ( ",\n   series:[\n    " );
        } else  {
          plotData.append ( "," );
          plotOptions.append ( ",\n    " );
        }
        plotData.append ( "[\n   " );
        int nPoints = std::min ( g->table[xset].size(),
                                 g->table[yset].size() );
        bool wasData = false;
        for (int n=0;n<nPoints;n++)
//          if ((g->table[xset][n]->text.find("nan",0)==std::string::npos) &&
//              (g->table[yset][n]->text.find("nan",0)==std::string::npos))  {
          if ((excl.find(g->table[xset][n]->text,0)==std::string::npos) &&
              (excl.find(g->table[yset][n]->text,0)==std::string::npos))  {
            if (wasData)
              plotData.append ( ",\n   " );
            plotData.append ( "[" + g->table[xset][n]->text + "," +
                                    g->table[yset][n]->text + "]" );
            wasData = true;
          }
        plotData.append ( "  \n  ]" );
        std::string lineOptions;
        line->getLineOptions ( lineOptions );
        plotOptions.append ( " {label      : '" + g->getSetNameEscaped(yset) +
                                              "',\n" + lineOptions );
        plotOptions.append ( "     }" );
      }
    }
  }

  plotOptions.append ( "\n   ]\n" );

}

void rvapi::LogGraph::flush_html ( std::string & outDir,
                                   std::string & task )  {
std::string treeData;
std::string plotData;
std::string plotOptions;
std::string gLeaf;
std::string content;
std::string pos;

  if (wasModified())  {

    for (int j=0;j<(int)plots.size();j++)
      plots[j]->calcRanges ( gdata );

    for (int i=0;i<(int)gdata.size();i++)  {
      GraphData *g = gdata[i];
      gLeaf.clear();

      for (int j=0;j<(int)plots.size();j++)  {
        Plot *p = plots[j];
        getLogData ( p,g,plotData,plotOptions );

        if (!plotData.empty())  {
          if (gLeaf.empty())
               gLeaf = " { label: '" + g->getTitleEscaped() + "',\n"
                       "   id: '"    + g->getId()    + "',\n"
                       "   children: [\n";
          else gLeaf.append ( "},\n" );
          gLeaf.append ( "      { label: '" + p->getTitleEscaped() + "',\n"
                         "         id: '"   + p->getId()    + "',\n"
                         " plotData: [\n"   + plotData      +
                         " \n ],\n"
                         " plotOptions: {\n" + plotOptions  +
                         " }\n" );
        }

      }

      if (!gLeaf.empty())  {
        if (treeData.empty())  treeData = "[\n";
                         else  treeData.append ( ",\n" );
        treeData.append ( gLeaf + "}\n   ]\n}" );
      }

    }

    if (!treeData.empty())
      treeData.append ( "\n]" );
    if (treeData.size()>0)  //inline_size_threshold)
      treeData = makeContent ( content,treeData,outDir,"loggraph_data" );

    add_log_graph ( task,nodeId(),parent->nodeId(),treeData,
                    gridPosition(pos) );

    for (int i=0;i<(int)gdata.size();i++)
      gdata[i]->setUnmodified();
    for (int i=0;i<(int)plots.size();i++)
      plots[i]->setUnmodified();

  }

  Node::flush_html ( outDir,task );

}

