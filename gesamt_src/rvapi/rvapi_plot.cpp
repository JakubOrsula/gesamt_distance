//
//  =================================================================
//
//    18.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_plot  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::PlotLine  - plot line class
//       ~~~~~~~~~  rvapi::Plot      - plot class
//                  rvapi::GraphData - graph data class
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#include <math.h>
#include <stdlib.h>
#include <limits>

#include "rvapi_plot.h"
#include "rvapi_node.h"
#include "rvapi_global.h"


//  =================================================================

rvapi::PlotLine::PlotLine()  {
  initPlotLine();
}

rvapi::PlotLine::~PlotLine() {}

rvapi::PlotLine::PlotLine ( const char * gdtId, const char * xsId,
                            const char * ysId )  {
  datId  = gdtId;
  xSetId = xsId;
  ySetId = ysId;
  initPlotLine();
}

void rvapi::PlotLine::initPlotLine()  {
  color         = "";
  width         = 2.5;
  xmin          = 0.0;
  xmax          = 0.0;
  ymin          = 0.0;
  ymax          = 0.0;
  style         = RVAPI_LINE_Solid;
  marker        = RVAPI_MARKER_filledCircle;
  smooth        = false;  // whether to smooth at time of plotting
  shown         = true;   // whether to be initially shown
  fill          = false;  // fill under line
  fillAndStroke = true;   // show line when filled
  fillColor     = "";     // Html fill color
  fillAlpha     = 0.1;    // fill Alpha
}

std::string & rvapi::PlotLine::lineId ( std::string & lid ) {
  lid = datId+":"+ySetId;
  return lid;
}

std::vector<std::string> & rvapi::PlotLine::styleList (
                                   std::vector<std::string> & list )  {
  list.push_back ( "solid"  );
  list.push_back ( "dashed" );
  list.push_back ( "dotted" );
  return list;
}

void rvapi::PlotLine::getLineOptions ( std::string & lineOptions )  {
std::string lid;
char        S[300];

  lineOptions = "      lineId     : '" + lineId(lid) + "',\n"
                "      show       : ";

  if (shown)  lineOptions.append ( "true,\n"  );
        else  lineOptions.append ( "false,\n" );

  sprintf ( S,"      lineWidth  : %.2f,\n",width );
  lineOptions.append ( S );
  sprintf ( S,"      xmin       : %.5g,\n"
              "      xmax       : %.5g,\n"
              "      ymin       : %.5g,\n"
              "      ymax       : %.5g,\n",
              xmin,xmax,ymin,ymax );
  lineOptions.append (
              "      color      : '"+color+"',\n" +
              std::string(S) );

  if (style=="off")
        lineOptions.append ( "      showLine   : false,\n" );
  else if (style=="bars")
        lineOptions.append ( "      renderer   : $.jqplot.rvapiBarRenderer,\n" );
  else  lineOptions.append ( "      showLine   : true,\n"
                             "      linePattern: '"+style+"',\n" );
  if (marker=="off")
    lineOptions.append ( "      showMarker : false,\n" );
  else  {
    sprintf ( S,"        size : %.2f,\n",width/2.5*9.0 );
    lineOptions.append ( "      showMarker : true,\n"
                         "      markerOptions: {\n"
                         "        show : true,\n" +
                         std::string(S) +
                         "        style: '"+marker+"'\n"
                         "      },\n" );
  }

  if (fill)  {
    sprintf ( S,"%.2f",fillAlpha );
    lineOptions.append (
              "      fill         : true,\n"
              "      fillAndStroke: " + bool2str(fillAndStroke) + ",\n"
              "      fillColor    : '" + fillColor      + "',\n"
              "      fillAlpha    : "  + std::string(S) + ",\n" );
  }

  if (style=="bars")
    lineOptions.append ( "      rendererOptions: {\n"
                         "        fillToZero  : true,\n"
                         "        barPadding  : 0,\n"
                         "        barMargin   : 3,\n"
                         "        barDirection: 'vertical',\n"
                         "        barWidth    : null,\n"
                         "        shadowOffset: 2,\n"
                         "        shadowDepth : 3,\n"
                         "        shadowAlpha : 0.25\n"
                         "      }\n" );
  else if (smooth)
    lineOptions.append ( "      rendererOptions: {\n"
                         "        smooth: true\n"
                         "      }\n" );

}

void rvapi::PlotLine::write ( std::ofstream & s )  {
  swrite ( s,datId  );
  swrite ( s,xSetId );
  swrite ( s,ySetId );
  swrite ( s,color  );
  swrite ( s,width  );
  swrite ( s,xmin   );
  swrite ( s,xmax   );
  swrite ( s,ymin   );
  swrite ( s,ymax   );
  swrite ( s,style  );
  swrite ( s,marker );
  swrite ( s,smooth );
  swrite ( s,shown  );
  swrite ( s,fill          );
  swrite ( s,fillAndStroke );
  swrite ( s,fillColor     );
  swrite ( s,fillAlpha     );
}

void rvapi::PlotLine::read ( std::ifstream & s )  {
  sread ( s,datId  );
  sread ( s,xSetId );
  sread ( s,ySetId );
  sread ( s,color  );
  sread ( s,width  );
  sread ( s,xmin   );
  sread ( s,xmax   );
  sread ( s,ymin   );
  sread ( s,ymax   );
  sread ( s,style  );
  sread ( s,marker );
  sread ( s,smooth );
  sread ( s,shown  );
  sread ( s,fill          );
  sread ( s,fillAndStroke );
  sread ( s,fillColor     );
  sread ( s,fillAlpha     );
}


//  =================================================================

rvapi::GraphData::GraphData ( const char * gdtId, const char * gdtTitle )
                : TableData()  {
  initGraphData();
  id    = gdtId;
  title = gdtTitle;
}

rvapi::GraphData::~GraphData()  {
  freeGraphData();
}

void rvapi::GraphData::initGraphData()  {}

void rvapi::GraphData::freeGraphData()  {
  setId  .clear();
  setName.clear();
}


std::string replaceAll ( std::string & line, std::string search,
                         std::string replace )  {
std::string::size_type n = 0;
std::string            L = line;
  while ((n=L.find(search,n))!= std::string::npos)  {
    L.replace( n,search.size(),replace );
    n += replace.size();
  }
  return L;
}

std::string rvapi::GraphData::getTitleEscaped()  {
  return replaceAll ( title,"'","\\'" );
}

std::string rvapi::GraphData::getSetNameEscaped ( int sNo )  {
  return replaceAll ( setName[sNo],"'","\\'" );
}


void rvapi::GraphData::putColSpec ( const char * sid,
                                    const char * name,
                                    const char * header,
                                    int setNo )  {
int sNo = setNo;

  if (sNo<0)  {
    sNo = getSetIndex ( sid );
    if (sNo<0)
      sNo = setId.size();
  }

  while ((int)setId.size()<=sNo)  {
    setId  .push_back("");
    setName.push_back("");
  }
  setId  [sNo] = sid;
  setName[sNo] = name;

  putVertHeader ( header,"",sNo );

}

int rvapi::GraphData::getSetIndex ( std::string sid )  {
int k = -1;
  for (int i=0;(i<(int)setId.size()) && (k<0);i++)
    if (setId.at(i)==sid)
      k = i;
  return k;
}

void rvapi::GraphData::putValue ( const char * v, const char *sid,
                                  int index )  {
int sNo = getSetIndex ( sid );
  if (sNo>=0)  {
    if (index>=0)  TableData::putValue ( v,sNo,index );
             else  TableData::putValue ( v,sNo,table[sNo].size() );
  }
}

void rvapi::GraphData::putValue ( int v, const char *sid, int index )  {
int sNo = getSetIndex ( sid );
  if (sNo>=0)  {
    if (index>=0)  TableData::putValue ( v,sNo,index );
             else  TableData::putValue ( v,sNo,table[sNo].size() );
  }
}

void rvapi::GraphData::putValue ( double v, const char * fmt,
                                 const char *sid, int index )  {
int sNo = getSetIndex ( sid );
  if (sNo>=0)  {
    if (index>=0)  TableData::putValue ( v,fmt,sNo,index );
             else  TableData::putValue ( v,fmt,sNo,table[sNo].size() );
  }
}

void rvapi::GraphData::resetData ( const char *sid )  {
int sNo = getSetIndex ( sid );
  if (sNo>=0)
    TableData::resetData ( sNo );
}


std::string & rvapi::GraphData::getXmli2 ( std::string & x )  {
size_t nrows;

  if (table.size()>0)  {
    x.append ( "<data>\n" );
    nrows = 0;
    for (unsigned long j=0;j<table.size();j++)
      nrows = std::max ( nrows,table[j].size() );
    for (unsigned long i=0;i<nrows;i++)  {
      for (unsigned long j=0;j<table.size();j++)
         if (i<table[j].size())
               x.append ( "   " + table[j][i]->geti2XMLText() );
         else  x.append ( "   ." );
      x.append ( "\n" );
    }
    x.append ( "</data>\n" );
  }

  return x;

}


void rvapi::GraphData::write ( std::ofstream & s )  {
int n = setId.size();

  swrite ( s,id    );
  swrite ( s,title );
  swrite ( s,n     );
  for (int i=0;i<n;i++)  {
    swrite ( s,setId  [i] );
    swrite ( s,setName[i] );
  }

  TableData::write ( s );

}

void rvapi::GraphData::read ( std::ifstream & s )  {
std::string si,sn;
int         n;

  freeGraphData();

  sread ( s,id    );
  sread ( s,title );
  sread ( s,n     );
  for (int i=0;i<n;i++)  {
    sread ( s,si );
    sread ( s,sn );
    setId  .push_back ( si );
    setName.push_back ( sn );
  }

  TableData::read ( s );

}



//  =================================================================


#define _RANGE_None    0x00
#define _RANGE_X_Min   0x01
#define _RANGE_X_Max   0x02
#define _RANGE_Y_Min   0x04
#define _RANGE_Y_Max   0x08
#define _RANGE_SX      0x10
#define _RANGE_SY      0x20

rvapi::Plot::Plot ( const char * pltId, const char * pltTitle,
                    const char * xName, const char * yName )  {
  initPlot();
  id    = pltId;
  title = pltTitle;
  xname = xName;
  yname = yName;
}

rvapi::Plot::~Plot()  {
  freePlot();
}

void rvapi::Plot::initPlot()  {
  legendLocation  = "ne";
  legendPlacement = "";
  xmin     = 0.0;
  xmax     = 0.0;
  ymin     = 0.0;
  ymax     = 0.0;
  sxmin    = 0.0;
  sxmax    = 0.0;
  symin    = 0.0;
  symax    = 0.0;
  ranges   = _RANGE_None;
  logx     = false;
  logy     = false;
  xint     = false;
  yint     = false;
  modified = true;
  clearXTicks();
  clearYTicks();
}

void rvapi::Plot::freePlot()  {
  for (int i=0;i<(int)lines.size();i++)
    if (lines.at(i))
      delete lines.at(i);
  clearXTicks();
  clearYTicks();
  modified = true;
}


std::string rvapi::Plot::getTitleEscaped()  {
  return replaceAll ( title,"'","\\'" );
}

void rvapi::Plot::addPlotLine ( PlotLine * line )  {
  lines.push_back ( line );
  modified = true;
}

rvapi::PlotLine *rvapi::Plot::getLine ( const char * datId,
                                        const char * ysetId )  {
std::string lineId = datId;
std::string lid;
PlotLine   *line   = NULL;
int         nLines = lines.size();
  lineId.append ( ":"    );
  lineId.append ( ysetId );
  for (int k=0;(k<nLines) && (!line);k++)
    if (lines.at(k)->lineId(lid)==lineId)
      line = lines.at(k);
  return line;
}


void rvapi::Plot::setXMin  ( double rmin )  {
// physical units
  xmin     = rmin;
  ranges   = (ranges | _RANGE_X_Min) & (~_RANGE_X_Max);
  modified = true;
}

void rvapi::Plot::setXMax  ( double rmax )  {
// physical units
  xmax     = rmax;
  ranges   = (ranges | _RANGE_X_Max) & (~_RANGE_X_Min);
  modified = true;
}

void rvapi::Plot::setXRange  ( double rmin, double rmax )  {
// physical units
  xmin     = std::min<double>(rmin,rmax);
  xmax     = std::max<double>(rmin,rmax);
  ranges   = ranges | _RANGE_X_Min | _RANGE_X_Max;
  modified = true;
}

void rvapi::Plot::setXSlider ( double smin, double smax )  {
// 0..1
  sxmin  = std::min<double>(smin,smax);
  sxmax  = std::max<double>(smin,smax);
  sxmin  = std::max<double>(sxmin,0.0);
  if (sxmax>1.0)  {
    sxmin /= sxmax;
    sxmax  = 1.0;
  }
  ranges   = ranges | _RANGE_SX;
  modified = true;
}


void rvapi::Plot::setYMin  ( double rmin )  {
// physical units
  ymin     = rmin;
  ranges   = (ranges | _RANGE_Y_Min) & (~_RANGE_Y_Max);
  modified = true;
}

void rvapi::Plot::setYMax  ( double rmax )  {
// physical units
  ymax     = rmax;
  ranges   = (ranges | _RANGE_Y_Max) & (~_RANGE_Y_Min);
  modified = true;
}

void rvapi::Plot::setYRange  ( double rmin, double rmax )  {
// physical units
  ymin     = std::min<double>(rmin,rmax);
  ymax     = std::max<double>(rmin,rmax);
  ranges   = ranges | _RANGE_Y_Min | _RANGE_Y_Max;
  modified = true;
}

void rvapi::Plot::setYSlider ( double smin, double smax )  {
// 0..1
  symin  = std::min<double>(smin,smax);
  symax  = std::max<double>(smin,smax);
  symin  = std::max<double>(symin,0.0);
  if (symax>1.0)  {
    symin /= symax;
    symax  = 1.0;
  }
  ranges   = ranges | _RANGE_SY;
  modified = true;
}

void rvapi::Plot::clearXTicks()  {
  tick_xvalues.clear();
  tick_xlabels.clear();
  modified = true;
}

void rvapi::Plot::clearYTicks()  {
  tick_yvalues.clear();
  tick_ylabels.clear();
  modified = true;
}

void rvapi::Plot::addXTick ( const double value,
                             const std::string label ) {
  tick_xvalues.push_back ( value );
  tick_xlabels.push_back ( label );
  modified = true;
}

void rvapi::Plot::addYTick ( const double value,
                             const std::string label ) {
  tick_yvalues.push_back ( value );
  tick_ylabels.push_back ( label );
  modified = true;
}

void rvapi::Plot::getXTicks ( std::vector<double>      & values,
                              std::vector<std::string> & labels )  {
  values = tick_xvalues;
  labels = tick_xlabels;
}

void rvapi::Plot::getYTicks ( std::vector<double>      & values,
                              std::vector<std::string> & labels )  {
  values = tick_yvalues;
  labels = tick_ylabels;
}

void rvapi::Plot::getXmin ( double & v, bool & on )  {
  v  = xmin;
  on = (ranges & _RANGE_X_Min);
}

void rvapi::Plot::getXmax ( double & v, bool & on )  {
  v  = xmax;
  on = (ranges & _RANGE_X_Max);
}

void rvapi::Plot::getYmin ( double & v, bool & on )  {
  v  = ymin;
  on = (ranges & _RANGE_Y_Min);
}

void rvapi::Plot::getYmax ( double & v, bool & on )  {
  v  = ymax;
  on = (ranges & _RANGE_Y_Max);
}

void rvapi::Plot::calcRanges ( std::vector<GraphData *> & gdata )  {
//double margin;
bool rangeXMin   = (ranges & _RANGE_X_Min)  == 0x00;
bool rangeXMax   = (ranges & _RANGE_X_Max)  == 0x00;
bool rangeYMin   = (ranges & _RANGE_Y_Min)  == 0x00;
bool rangeYMax   = (ranges & _RANGE_Y_Min)  == 0x00;
bool rangeSX     = (ranges & _RANGE_SX) == 0x00;
bool rangeSY     = (ranges & _RANGE_SY) == 0x00;
std::string excl = "nan,-nan,inf,-inf";

  if (rangeXMin)  xmin =  std::numeric_limits<double>::max();
  if (rangeXMax)  xmax = -std::numeric_limits<double>::max();

  if (rangeYMin)  ymin =  std::numeric_limits<double>::max();
  if (rangeYMax)  ymax = -std::numeric_limits<double>::max();

  for (int k=0;k<(int)lines.size();k++)  {
    PlotLine  *line = lines[k];
    GraphData *g    = NULL;
    for (int i=0;(i<(int)gdata.size()) && (!g);i++)
      if (gdata[i]->getId()==line->datId)
        g = gdata[i];
    if (g)  {
      int xset = g->getSetIndex ( line->xSetId );
      int yset = g->getSetIndex ( line->ySetId );
      line->xmin = std::numeric_limits<double>::max();
      line->xmax = -line->xmin;
      line->ymin = std::numeric_limits<double>::max();
      line->ymax = -line->ymin;
      if ((xset>=0) && (yset>=0))  {
        int nPoints = std::min ( g->table[xset].size(),
                                 g->table[yset].size() );
                                 
        for (int n=0;n<nPoints;n++)
          if ((excl.find(g->table[xset][n]->text,0)==std::string::npos) &&
              (excl.find(g->table[yset][n]->text,0)==std::string::npos))  {
            double x = strtod(g->table[xset][n]->text.c_str(),NULL);
            line->xmin = std::min ( line->xmin,x );
            line->xmax = std::max ( line->xmax,x );
            double y = strtod(g->table[yset][n]->text.c_str(),NULL);
            line->ymin = std::min ( line->ymin,y );
            line->ymax = std::max ( line->ymax,y );
          }
                                 
        /*
        for (int n=0;n<nPoints;n++)
          if (g->table[xset][n]->text.compare("nan"))  {
            double x = strtod(g->table[xset][n]->text.c_str(),NULL);
            line->xmin = std::min ( line->xmin,x );
            line->xmax = std::max ( line->xmax,x );
            if (g->table[yset][n]->text.compare("nan"))  {
              double y = strtod(g->table[yset][n]->text.c_str(),NULL);
              line->ymin = std::min ( line->ymin,y );
              line->ymax = std::max ( line->ymax,y );
            }
          }
        */
        if (rangeXMin)  xmin = std::min ( xmin,line->xmin );
        if (rangeXMax)  xmax = std::max ( xmax,line->xmax );
        if (rangeYMin)  ymin = std::min ( ymin,line->ymin );
        if (rangeYMax)  ymax = std::max ( ymax,line->ymax );
      }
    }
  }

/*
  if (rangeX)  {
    if (logx)  {
      xmin = log(xmin);
      xmax = log(xmax);
    }
    margin = 0.05*(xmax-xmin);
    xmin -= margin;
    xmax += margin;
    if (logx)  {
      xmin = exp(xmin);
      xmax = exp(xmax);
    }
  }
  if (rangeY)  {
    if (logy)  {
      ymin = log(ymin);
      ymax = log(ymax);
    }
    margin = 0.05*(ymax-ymin);
    ymin -= margin;
    ymax += margin;
    if (logy)  {
      ymin = exp(ymin);
      ymax = exp(ymax);
    }
  }
*/

  if (rangeSX)  {
    sxmin = 0.0;
    sxmax = 1.0;
  }
  if (rangeSY)  {
    symin = 0.0;
    symax = 1.0;
  }

}

std::string get_ticks ( std::vector<double>      & tick_values,
                        std::vector<std::string> & tick_labels )  {
std::string ticks;

  if (tick_values.size()<=0)
    return "";

  ticks = "       ticks: [";
  for (unsigned int i=0;i<tick_values.size();i++)  {
    char S[100];
    if (i>0)  ticks.append ( "," );
    sprintf ( S,"[%.4g,'",tick_values[i] );
    ticks.append ( S + tick_labels[i] + "']" );
  }
  ticks.append ( "],\n" );

  return ticks;

}

void rvapi::Plot::getCommonOptions ( std::string & plotOptions,
                                     bool doRanges )  {
char S[300];

  plotOptions =
    "   plotId: '" + getId() + "',\n"
    "   axesDefaults: {\n"
    "     tickRenderer: $.jqplot.CanvasAxisTickRenderer,\n"
    "     labelRenderer: $.jqplot.CanvasAxisLabelRenderer,\n"
    "     tickOptions: {\n"
    "       fontSize: '12pt'\n"
    "     }\n"
    "   },\n"
    "   axes:{\n"
    "     xaxis:{\n"
    "       label: '" + xname + "',\n" +
      get_ticks(tick_xvalues,tick_xlabels) +
    "       tickOptions: {\n"
    "         ticks: [],\n"
    "         show : true\n"
    "       },\n";

  if (logx)  plotOptions.append (
    "       renderer         : $.jqplot.LogAxisRenderer,\n"
    "       log              : true,\n"
    "       tickDistribution : 'power',\n"
    "       pad              : 1.025" );
       else  plotOptions.append (
    "       renderer : $.jqplot.LinearAxisRenderer,\n"
    "       log      : false,\n"
    "       pad      : 1.025" );

  if ((ranges & _RANGE_X_Min) || doRanges)  {
    sprintf ( S,",\n"
                "       min   : %.4g",xmin );
    plotOptions.append ( S );
  }

  if ((ranges & _RANGE_X_Max) || doRanges)  {
    sprintf ( S,",\n"
                "       max   : %.4g",xmax );
    plotOptions.append ( S );
  }

  if (doRanges)  {
    sprintf ( S,
        ",\n"
        "       smin  : %.4g,\n"
        "       smax  : %.4g,\n"
        "       smin0 : %.4g,\n"
        "       smax0 : %.4g",
        sxmin,sxmax,sxmin,sxmax
             );
    plotOptions.append ( S );
  }

  plotOptions.append (
    "\n"
    "     },\n"
    "     yaxis:{\n"
    "       label: '" + yname + "',\n" +
        get_ticks(tick_yvalues,tick_ylabels) +
    "       tickOptions: {\n"
    "         ticks: [],\n"
    "         show : true\n"
    "       },\n" );

  if (logy)  plotOptions.append (
    "       renderer         : $.jqplot.LogAxisRenderer,\n"
    "       log              : true,\n"
    "       tickDistribution : 'even',\n"
    "       pad              : 1.025" );
       else  plotOptions.append (
    "       renderer : $.jqplot.LinearAxisRenderer,\n"
    "       log      : false,\n"
    "       pad      : 1.025" );

  if ((ranges & _RANGE_Y_Min) || doRanges)  {
    sprintf ( S,",\n"
                "       min   : %.4g",ymin );
    plotOptions.append ( S );
  }

  if ((ranges & _RANGE_Y_Max) || doRanges)  {
    sprintf ( S,",\n"
                "       max   : %.4g",ymax );
    plotOptions.append ( S );
  }

  if (doRanges)  {
    sprintf ( S,
        ",\n"
        "       smin  : %.4g,\n"
        "       smax  : %.4g,\n"
        "       smin0 : %.4g,\n"
        "       smax0 : %.4g",
        symin,symax,symin,symax
             );
    plotOptions.append ( S );
  }

  plotOptions.append (
    "\n"
    "     }\n"
    "   },\n"
    "   highlighter: {\n"
    "     show: true,\n"
    "     tooltipFormatString: '%.4g',\n"
    "     useAxesFormatters: false,\n"
    "     sizeAdjust: 7.5\n"
    "   },\n"
    "   cursor: {\n"
    "     show: true,\n"
    "     zoom: true,\n"
    "     looseZoom: true,\n"
    "     constrainOutsideZoom: false,\n"
    "     constrainZoomTo: 'none',\n"
    "     tooltipFormatString: '%.4g, %.4g',\n"
    "     useAxesFormatters: false,\n"
    "     showTooltip: true,\n"
    "     followMouse: true,\n"
    "     showTooltipOutsideZoom: true\n"
//----       "     constrainOutsideZoom: false\n"
//    "     useAxesFormatters: false,\n"
//            "     tooltipAxesGroups: ['xaxis','yaxis'],\n"
//    "     tooltipLocation:'sw'\n"
    "   },\n" );
  if (!legendLocation.empty())  {
    plotOptions.append (
      "   legend: {\n"
      "     show: true,\n"
      "     location: '" + legendLocation + "'" );
    if (!legendPlacement.empty())
          plotOptions.append ( ",\n     placement: '" +
                               legendPlacement + "'\n" );
    else  plotOptions.append ( "\n" );
  } else
    plotOptions.append (
      "   legend: {\n"
      "     show: false\n" );
  plotOptions.append ( "   }" );

}

void rvapi::Plot::getPlotData ( std::vector<GraphData *> & gdata,
                                std::string  & plotData,
                                std::string  & plotOptions )  {
std::string excl = "nan,-nan,inf,-inf";

  calcRanges ( gdata );

  getCommonOptions ( plotOptions,true );
  plotData.clear();

  int nLines = lines.size();
  for (int k=0;k<nLines;k++)  {
    PlotLine  *line = lines[k];
    GraphData *g    = NULL;
    for (int i=0;(i<(int)gdata.size()) && (!g);i++)
      if (gdata[i]->getId()==line->datId)
        g = gdata[i];
    if (g)  {
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
//          if (g->table[xset][n]->text.compare("nan") &&
 //             g->table[yset][n]->text.compare("nan"))  {
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

  plotData   .append ( "\n" );
  plotOptions.append ( "\n   ]\n" );

}

void rvapi::Plot::write ( std::ofstream & s )  {
int nl = lines.size();
int key,i;

  swrite ( s,modified );
  swrite ( s,id     );
  swrite ( s,title  );
  swrite ( s,xname  );
  swrite ( s,yname  );
  swrite ( s,nl     );
  swrite ( s,xmin   );
  swrite ( s,xmax   );
  swrite ( s,ymin   );
  swrite ( s,ymax   );
  swrite ( s,sxmin  );
  swrite ( s,sxmax  );
  swrite ( s,symin  );
  swrite ( s,symax  );
  swrite ( s,ranges );
  swrite ( s,logx   );
  swrite ( s,logy   );
  swrite ( s,xint   );
  swrite ( s,yint   );

  for (i=0;i<nl;i++)
    if (lines.at(i))  {
      key = 1;
      swrite ( s,key );
      lines.at(i)->write ( s );
    } else  {
      key = 0;
      swrite ( s,key );
    }

  nl = tick_xvalues.size();
  swrite ( s,nl );
  for (i=0;i<nl;i++)  {
    swrite ( s,tick_xvalues[i] );
    swrite ( s,tick_xlabels[i] );
  }

  nl = tick_yvalues.size();
  swrite ( s,nl );
  for (i=0;i<nl;i++)  {
    swrite ( s,tick_yvalues[i] );
    swrite ( s,tick_ylabels[i] );
  }

}

void rvapi::Plot::read ( std::ifstream & s )  {
PlotLine   *line;
std::string label;
double      value;
int         nl,key,i;

  freePlot();

  sread ( s,modified );
  sread ( s,id     );
  sread ( s,title  );
  sread ( s,xname  );
  sread ( s,yname  );
  sread ( s,nl     );
  sread ( s,xmin   );
  sread ( s,xmax   );
  sread ( s,ymin   );
  sread ( s,ymax   );
  sread ( s,sxmin  );
  sread ( s,sxmax  );
  sread ( s,symin  );
  sread ( s,symax  );
  sread ( s,ranges );
  sread ( s,logx   );
  sread ( s,logy   );
  sread ( s,xint   );
  sread ( s,yint   );

  for (i=0;i<nl;i++)  {
    sread ( s,key );
    if (key>0)  {
      line = new PlotLine();
      line->read ( s );
      lines.push_back ( line );
    }
  }

  sread ( s,nl );
  for (i=0;i<nl;i++)  {
    sread ( s,value );
    sread ( s,label );
    addXTick ( value,label );
  }

  sread ( s,nl );
  for (i=0;i<nl;i++)  {
    sread ( s,value );
    sread ( s,label );
    addYTick ( value,label );
  }

//  modified = true;

}

