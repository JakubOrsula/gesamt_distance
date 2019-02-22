//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_radar  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Radar - radar chart class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#ifndef RVAPI_RADAR_H
#define RVAPI_RADAR_H

#include "rvapi_node.h"

namespace rvapi  {

  class Radar : public Node  {

    public:
      Radar ( const char * radarId,
              const char * radarTitle,
              int hrow     = -1,
              int hcol     = 0,
              int hrowSpan = 1,
              int hcolSpan = 1
            );
      Radar ();
      virtual ~Radar();

      virtual NODE_TYPE type()   { return NTYPE_Radar; }

      inline void setFoldState ( int state )  { foldState = state; }

      void  addProperty  ( const char * name, const double value );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

    protected:
      std::string              title;
      std::vector<std::string> names;
      std::vector<double>      values;
      int                      width;
      int                      height;
      double                   maxValue;
      int                      nLevels;
      int                      translateX;
      int                      translateY;
      int                      extraWidthX;
      int                      extraWidthY;
      int                      foldState;  //!< -1,0,+1 for initialy
                                           /// folded, not foldable and
                                           /// initially unfolded

      void initRadar();
      void freeRadar();

  };

}

#endif // RVAPI_RADAR_H
