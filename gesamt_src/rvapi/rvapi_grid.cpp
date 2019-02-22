//
//  =================================================================
//
//    06.03.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_grid  <implementation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Grid - API Grid class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#include "rvapi_grid.h"
#include "rvapi_tasks.h"

rvapi::GridCell::GridCell()  {
  row      = 0;
  col      = 0;
  width    = 0;
  height   = 0;
  modified = true;
}

rvapi::GridCell::~GridCell()  {}

void rvapi::GridCell::write ( std::ofstream & s )  {
  swrite ( s,modified );
  swrite ( s,row      );
  swrite ( s,col      );
  swrite ( s,width    );
  swrite ( s,height   );
}

void rvapi::GridCell::read ( std::ifstream & s )  {
  sread ( s,modified );
  sread ( s,row      );
  sread ( s,col      );
  sread ( s,width    );
  sread ( s,height   );
//  modified = true;
}


rvapi::Grid::Grid ( const char *gridId, bool filling,
                    int hrow, int hcol, int hrowSpan, int hcolSpan  )
          : Node(gridId,hrow,hcol,hrowSpan,hcolSpan)  {
  initGrid();
  compact = !filling;
}

rvapi::Grid::Grid() : Node()  {
  initGrid();
}

rvapi::Grid::~Grid()  {
  freeGrid();
}

void rvapi::Grid::initGrid()  {
  compact = false;
}

void rvapi::Grid::freeGrid()  {
  for (unsigned int i=0;i<cells.size();i++)
    if (cells[i])  delete cells[i];
  cells.clear();
}

void rvapi::Grid::setCellStretch ( int width, int height,
                                   int grow,  int gcol )  {
GridCell *cell = new GridCell();
  cell->width    = width;
  cell->height   = height;
  cell->row      = grow;
  cell->col      = gcol;
  cell->modified = true;
  cells.push_back ( cell );
}

void rvapi::Grid::write ( std::ofstream & s )  {
int nn = cells.size();
int key;
  swrite ( s,compact );
  swrite ( s,nn      );
  for (unsigned int i=0;i<cells.size();i++)
    if (cells[i])  {
      key = 1;
      swrite ( s,key );
      cells[i]->write ( s );
    } else  {
      key = 0;
      swrite ( s,key );
    }
  Node::write ( s );
}

void rvapi::Grid::read ( std::ifstream & s )  {
GridCell *cell;
int       nn,key;
  freeGrid();
  sread ( s,compact );
  sread ( s,nn      );
  for (int i=0;i<nn;i++)  {
    sread ( s,key );
    if (key>0)  {
      cell = new GridCell();
      cell->read ( s );
      cells.push_back ( cell );
    }
  }
  Node::read ( s );
}

void rvapi::Grid::flush_html ( std::string & outDir,
                               std::string & task )  {
std::string pos;
  if (wasCreated())  {
    if (compact)
      nest_grid_compact ( task,nodeId(),parent->nodeId(),
                          gridPosition(pos) );
    else  nest_grid     ( task,nodeId(),parent->nodeId(),
                          gridPosition(pos) );
  }
  for (unsigned int i=0;i<cells.size();i++)
    if (cells[i])  {
      if (cells[i]->modified)  {
        set_cell_stretch ( task,nodeId(),
                           cells[i]->width,cells[i]->height,
                           cells[i]->row  ,cells[i]->col );
        cells[i]->modified = false;
      }
    }
  Node::flush_html ( outDir,task );
}

