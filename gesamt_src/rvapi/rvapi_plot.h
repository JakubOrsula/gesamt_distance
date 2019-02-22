//
//  =================================================================
//
//    18.02.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_plot  <interface>
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

#ifndef RVAPI_PLOT_H
#define RVAPI_PLOT_H

#include "rvapi_tabledata.h"

namespace rvapi  {

  class PlotLine  {

    public:

      std::string datId;   //!< Graph data Id
      std::string xSetId;  //!< Set Id for x-points
      std::string ySetId;  //!< Set Id for y-points
      std::string color;   //!< Html color
      std::string style;   //!< "off","solid","dashed","dotted","-." etc
      std::string marker;  //!< "off","circle","diamond","square",
                           /// "filledCircle","filledDiamond" or
                           /// "filledSquare"
      double          width; //!< line width ( 2.5 )
      bool             fill; //!< fill under line
      bool    fillAndStroke; //!< show line when filled
      std::string fillColor; //!< Html fill color
      double      fillAlpha; //!< fill Alpha

      double      xmin,xmax; //!< X-range of the line
      double      ymin,ymax; //!< Y-range of the line
      bool        smooth;    //!< whether to smooth at time of plotting
      bool        shown;     //!< whether to be initially shown

      PlotLine();
      PlotLine ( const char * gdtId, const char * xsId,
                 const char * ysId );
      virtual ~PlotLine();

      std::string & lineId ( std::string & lid );

      static std::vector<std::string> & styleList (
                                     std::vector<std::string> & list );

      void getLineOptions ( std::string & lineOptions );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

    protected:
      void initPlotLine();

  };


  class GraphData : public TableData  {

    public:

      GraphData ( const char * gdtId, const char * gdtTitle );
      ~GraphData();

      void putColSpec ( const char * sid, const char * name,
                        const char * header, int setNo=-1 );
      void putValue   ( const char * v, const char * sid, int index=-1 );
      void putValue   ( int   v, const char * sid, int index=-1 );
      void putValue   ( double v, const char * fmt, const char * sid,
                        int index=-1 );

      int  getSetIndex ( std::string sid );

      void resetData  ( const char * sid );

      inline std::string & getId   () { return id;    }
      inline std::string & getTitle() { return title; }
      inline int           getNofSets ()          { return setId.size(); }
      inline std::string & getSetId   ( int sNo ) { return setId  [sNo]; }
      inline std::string & getSetName ( int sNo ) { return setName[sNo]; }

      std::string getTitleEscaped   ();
      std::string getSetNameEscaped ( int sNo );

      std::string & getXmli2 ( std::string & x );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

    protected:
      std::string               id;
      std::string               title;
      std::vector<std::string>  setId;
      std::vector<std::string>  setName;

      void initGraphData();
      void freeGraphData();

  };


  class Plot  {

    public:
      Plot ( const char * pltId, const char * pltTitle,
             const char * xName, const char * yName );
      virtual ~Plot();

      void addPlotLine ( PlotLine * line );

      inline bool wasModified  ()  { return modified;  }
      inline void setUnmodified()  { modified = false; }

      inline std::string & getId   ()      { return id;           }
      inline std::string & getTitle()      { return title;        }
      inline std::string & getXName()      { return xname;        }
      inline std::string & getYName()      { return yname;        }
      inline int        getNofLines()      { return lines.size(); }
      inline PlotLine *getLine ( int lNo ) { return lines[lNo];   }
      PlotLine *getLine ( const char * datId, const char * ysetId );

      std::string getTitleEscaped();

      void      setXMin   ( double rmin ); //!< physical units, min only
      void      setXMax   ( double rmax ); //!< physical units, max only
      void      setXRange ( double rmin, double rmax ); //!< physical units
      void     setXSlider ( double smin, double smax ); //!< 0..1
      inline void setLogX ( bool logaxis )  { logx = logaxis; }
      inline void setIntX ( bool intaxis )  { xint = intaxis; }
      void      setYMin   ( double rmin ); //!< physical units, min only
      void      setYMax   ( double rmax ); //!< physical units, max only
      void      setYRange ( double rmin, double rmax ); //!< physical units
      void     setYSlider ( double smin, double smax ); //!< 0..1
      inline void setLogY ( bool logaxis )  { logy = logaxis; }
      inline void setIntY ( bool intaxis )  { yint = intaxis; }
      inline void setLegendLocation  ( const std::string legLoc )
                                        { legendLocation = legLoc;    }
      inline void setLegendPlacement ( const std::string legPlace )
                                        { legendPlacement = legPlace; }

      void clearXTicks();
      void clearYTicks();
      void addXTick   ( const double value, const std::string label );
      void addYTick   ( const double value, const std::string label );

      void getXTicks ( std::vector<double>      & values,
                       std::vector<std::string> & labels );
      void getYTicks ( std::vector<double>      & values,
                       std::vector<std::string> & labels );
      void getXmin   ( double & v, bool & on );
      void getXmax   ( double & v, bool & on );
      void getYmin   ( double & v, bool & on );
      void getYmax   ( double & v, bool & on );
      inline bool isLogX() { return logx; }
      inline bool isLogY() { return logy; }
      inline bool isIntX() { return xint; }
      inline bool isIntY() { return yint; }

      void calcRanges       ( std::vector<GraphData *> & gdata );
      void getCommonOptions ( std::string & plotOptions, bool doRanges );

      void   getPlotData ( std::vector<GraphData *> & gdata,
                           std::string              & plotData,
                           std::string              & plotOptions );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

    protected:
      std::string   id;
      std::string   title;
      std::string   xname;
      std::string   yname;
      std::string   legendLocation;
      std::string   legendPlacement;
      std::vector<double>      tick_xvalues;
      std::vector<std::string> tick_xlabels;
      std::vector<double>      tick_yvalues;
      std::vector<std::string> tick_ylabels;
      std::vector<PlotLine *>  lines;
      double        xmin,xmax,ymin,ymax;      //!< data ranges
      double        sxmin,sxmax,symin,symax;  //!< slider ranges
      int           ranges;     //!< indicates which smin/smax are set
      bool          logx,logy;  //!< make logarithmic scales if true
      bool          xint,yint;  //!< if true then the scale is integer
      bool          modified;

      void initPlot();
      void freePlot();

  };

}

#endif // RVAPI_PLOT_H
