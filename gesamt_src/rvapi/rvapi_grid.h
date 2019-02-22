//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_grid  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Grid - API grid class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#ifndef RVAPI_GRID_H
#define RVAPI_GRID_H

#include "rvapi_node.h"

namespace rvapi  {

  class GridCell  {

    public:
      int  row;
      int  col;
      int  width;
      int  height;
      bool modified;

      GridCell ();
      ~GridCell();

      void write ( std::ofstream & s );
      void read  ( std::ifstream & s );

  };

  class Grid : public Node  {

    public:
      Grid ( const char * gridId,
             bool filling,
             int  hrow     = -1,
             int  hcol     = 0,
             int  hrowSpan = 1,
             int  hcolSpan = 1
           );
      Grid ();
      ~Grid();

      virtual NODE_TYPE type() { return NTYPE_Grid; }

      void setCellStretch ( int width, int height, int grow, int gcol );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::vector<GridCell *> cells;
    bool                    compact;

    void initGrid();
    void freeGrid();

  };

}

#endif // RVAPI_GRID_H
