//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_graph  <implemenation>
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

#include "rvapi_graph.h"
#include "rvapi_plot.h"
#include "rvapi_tasks.h"

rvapi::Graph::Graph ( const char *graphId,
                      int hrow, int hcol,
                      int hrowSpan, int hcolSpan  )
            : Node(graphId,hrow,hcol,hrowSpan,hcolSpan)  {
  initGraph();
}

rvapi::Graph::Graph() : Node()  {
  initGraph();
}

rvapi::Graph::~Graph()  {
  freeGraph();
}

void rvapi::Graph::initGraph()  {
  width   = 605;
  height  = 500;
  xslider = true;
  yslider = true;
}

void rvapi::Graph::freeGraph()  {

  for (int i=0;i<(int)plots.size();i++)
    if (plots.at(i))
      delete plots.at(i);

  for (int i=0;i<(int)gdata.size();i++)
    if (gdata.at(i))
      delete gdata.at(i);

}

bool rvapi::Graph::wasModified()  {
  return checkModified();
}

bool rvapi::Graph::treeWasModified()  {
  checkModified();
  return Node::treeWasModified();
}


void rvapi::Graph::addGraphData ( const char * gdataId,
                                  const char * gdataTitle )  {
  if (!getGraphData(gdataId))  {
    gdata.push_back ( new GraphData(gdataId,gdataTitle) );
    setModified();
  }
}

void rvapi::Graph::addPlot ( const char * plotId,
                             const char * plotTitle,
                             const char * xName,
                             const char * yName )  {
  if (!getPlot(plotId))  {
    plots.push_back ( new Plot(plotId,plotTitle,xName,yName) );
    setModified();
  }

}

rvapi::GraphData * rvapi::Graph::getGraphData ( std::string gdataId ) {
GraphData * gdt = NULL;
  for (int i=0;(i<(int)gdata.size()) && (!gdt);i++)
    if (gdata.at(i)->getId()==gdataId)
      gdt = gdata.at(i);
  return gdt;
}

rvapi::Plot * rvapi::Graph::getPlot ( std::string plotId )  {
Plot * plt = NULL;
  for (int i=0;(i<(int)plots.size()) && (!plt);i++)
    if (plots.at(i)->getId()==plotId)
      plt = plots.at(i);
  return plt;
}


void rvapi::Graph::write ( std::ofstream & s )  {
int np = plots.size();
int ng = gdata.size();
int key,i;

  swrite ( s,defplot );
  swrite ( s,width   );
  swrite ( s,height  );
  swrite ( s,xslider );
  swrite ( s,yslider );

  swrite ( s,np );
  swrite ( s,ng );

  for (i=0;i<np;i++)
    if (plots.at(i))  {
      key = 1;
      swrite ( s,key );
      plots.at(i)->write ( s );
    } else  {
      key = 0;
      swrite ( s,key );
    }

  for (i=0;i<ng;i++)
    if (gdata.at(i))  {
      key = 1;
      swrite ( s,key );
      gdata.at(i)->write ( s );
    } else  {
      key = 0;
      swrite ( s,key );
    }

  Node::write ( s );

}

void rvapi::Graph::read  ( std::ifstream & s )  {
Plot      *plot;
GraphData *gd;
int        np,ng,key,i;

  freeGraph();

  sread ( s,defplot );
  sread ( s,width   );
  sread ( s,height  );
  sread ( s,xslider );
  sread ( s,yslider );

  sread ( s,np );
  sread ( s,ng );

  for (i=0;i<np;i++)  {
    sread ( s,key );
    if (key>0)  {
      plot = new Plot ( "","","","" );
      plot->read ( s );
      plots.push_back ( plot );
    }
  }

  for (i=0;i<ng;i++)  {
    sread ( s,key );
    if (key>0)  {
      gd = new GraphData ( "","" );
      gd->read ( s );
      gdata.push_back ( gd );
    }
  }

  Node::read ( s );

}


bool rvapi::Graph::checkModified()  {
  if (!wasCreated())  {
    for (int i=0;(i<(int)gdata.size()) && (!Node::wasModified());i++)
      if (gdata[i]->wasModified())
        setModified();
    for (int i=0;(i<(int)plots.size()) && (!Node::wasModified());i++)
      if (plots[i]->wasModified())
        setModified();
  }
  return Node::wasModified();
}

void rvapi::Graph::flush_html ( std::string & outDir,
                                std::string & task )  {
std::string graphData;
std::string content;
std::string pos;
char        S[300];

  checkModified();

  if ((plots.size()>0) && Node::wasModified())  {

    sprintf ( S,
      "    width  : %i,\n"
      "    height : %i,\n"
      "    hslider: %i,\n"
      "    vslider: %i\n",
      width,height,xslider,yslider );

    graphData.append ( "{\n"
                       "  graphOptions: {\n"
                       "    defplot: '" + defplot + "',\n"
                       "    curplot: '" + defplot + "',\n" +
                       std::string(S) +
                       "  },\n"
                       "\n"
                       "  graphData: [\n\n"
                     );


    for (int i=0;i<(int)plots.size();i++)  {
      std::string plotData;
      std::string plotOptions;
      Plot *p = plots[i];
      p->getPlotData ( gdata,plotData,plotOptions );

      if (i>0)
        graphData.append ( ",\n\n" );
      graphData.append ( "    { id: '" + p->getId() +"',\n" );
      graphData.append ( "      title: '" + p->getTitle() +"',\n" );
      graphData.append ( "      plotData: [\n" + plotData + "  ],\n" );
      graphData.append ( "      plotOptions: {\n" + plotOptions + "  }\n" );
      graphData.append ( "    }" );

    }

    graphData.append ( "\n  ]\n}" );
    graphData = makeContent ( content,graphData,outDir,"graph_data" );

    add_graph ( task,nodeId(),parent->nodeId(),graphData,
                gridPosition(pos) );

    for (int i=0;i<(int)gdata.size();i++)
      gdata[i]->setUnmodified();
    for (int i=0;i<(int)plots.size();i++)
      plots[i]->setUnmodified();

  }

  Node::flush_html ( outDir,task );

}


void rvapi::Graph::make_xmli2_content ( std::string & tag,
                                        std::string & content )  {
std::vector<double>      values;
std::vector<std::string> labels;
double                   v;
bool                     on;

  tag = "graph";

  content.clear();

  for (unsigned int i=0;i<plots.size();i++)  {
    Plot *p = plots[i];
    if (p)  {

      content.append ( "<plot id=\"" + p->getId() + "\">\n" +
                       "<title>" + p->getTitle() + "</title>\n" +
                       "<xname>" + p->getXName() + "</xname>\n" +
                       "<yname>" + p->getYName() + "</yname>\n"
                     );
      for (int j=0;j<p->getNofLines();j++)  {
        PlotLine *pline = p->getLine ( j );
        if (pline)
          content.append (
            "<plotline dataset=\"" + pline->datId +
                               "\" xset=\"" + pline->xSetId +
                               "\" yset=\"" + pline->ySetId + "\">\n" +
            "<color>"  + pline->color  + "</color>\n"  +
            "<style>"  + pline->style  + "</style>\n"  +
            "<marker>" + pline->marker + "</marker>\n" +
            "<width>" + double2str(pline->width,"%.2f") + "</width>\n" +
            "<fill>"   + bool2str(pline->fill) + "</fill>\n" +
            "<fillandstroke>" + bool2str(pline->fillAndStroke)
                              + "</fillandstroke>\n" +
            "<fillcolor>" + pline->fillColor + "</fillcolor>\n" +
            "<fillalpha>" + double2str(pline->fillAlpha,"%.2f")
                                             + "</fillalpha>\n" +
            "<smooth>"    + bool2str(pline->smooth) + "</smooth>\n" +
            "<show>"      + bool2str(pline->shown)  + "</show>\n"   +
            "</plotline>\n"
          );
      }

      p->getXmin ( v,on );
      content.append ( "<xmin on=\"" + bool2str(on) + "\">"
                               + double2str(v,NULL) + "</xmin>\n" );
      p->getXmax ( v,on );
      content.append ( "<xmax on=\"" + bool2str(on) + "\">"
                               + double2str(v,NULL) + "</xmax>\n" );
      p->getYmin ( v,on );
      content.append ( "<ymin on=\"" + bool2str(on) + "\">"
                               + double2str(v,NULL) + "</ymin>\n" );
      p->getYmax ( v,on );
      content.append ( "<ymax on=\"" + bool2str(on) + "\">"
                               + double2str(v,NULL) + "</ymax>\n" );

      content.append ( "<logx>" + bool2str(p->isLogX()) + "</logx>\n" +
                       "<logy>" + bool2str(p->isLogY()) + "</logy>\n" +
                       "<intx>" + bool2str(p->isIntX()) + "</intx>\n" +
                       "<inty>" + bool2str(p->isIntY()) + "</inty>\n" );

      p->getXTicks ( values,labels );
      if (values.size()>0)  {
        content.append ( "<custom_xvalues>\n" );
        for (unsigned int j=0;j<values.size();j++)
          content.append ( "  " + double2str(values[j],NULL) );
        content.append ( "\n</custom_xvalues>\n" );
      } else
        content.append ( "<custom_xvalues></custom_xvalues>\n" );

      if (labels.size()>0)  {
        content.append ( "<custom_xlabels>\n" );
        for (unsigned int j=0;j<labels.size();j++)
          content.append ( "  " + labels[j] );
        content.append ( "\n</custom_xlabels>\n" );
      } else
        content.append ( "<custom_xlabels></custom_xlabels>\n" );

      p->getYTicks ( values,labels );
      if (values.size()>0)  {
        content.append ( "<custom_yvalues>\n" );
        for (unsigned int j=0;j<values.size();j++)
          content.append ( "  " + double2str(values[j],NULL) );
        content.append ( "\n</custom_yvalues>\n" );
      } else
        content.append ( "<custom_yvalues></custom_yvalues>\n" );

      if (labels.size()>0)  {
        content.append ( "<custom_ylabels>\n" );
        for (unsigned int j=0;j<labels.size();j++)
          content.append ( "  " + labels[j] );
        content.append ( "\n</custom_ylabels>\n" );
      } else
        content.append ( "<custom_ylabels></custom_ylabels>\n" );

      content.append ( "</plot>\n" );

    }

  }

  for (unsigned int i=0;i<gdata.size();i++)  {
    GraphData *gd = gdata[i];
    if (gd)  {
      content.append ( "<graphdata id=\"" + gd->getId() + "\">\n" +
                       "<title>" + gd->getTitle() + "</title>\n"  +
                       "<setids separator=\"?\">\n" );
      for (int j=0;j<gd->getNofSets();j++)  {
        if (j>0)  content.append ( "?" );
        content.append ( gd->getSetId(j) );
      }
      content.append ( "\n</setids>\n"
                       "<setnames separator=\"?\">\n" );
      for (int j=0;j<gd->getNofSets();j++)  {
        if (j>0)  content.append ( "?" );
        content.append ( gd->getSetName(j) );
      }
      content.append ( "\n</setnames>\n" );
      gd->getXmli2 ( content );
      content.append ( "</graphdata>\n" );
    }
  }

}

