//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_node  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Node - API base node class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//


#ifndef RVAPI_NODE_H
#define RVAPI_NODE_H

#include <vector>
#include <string>
#include <fstream>

#include "rvapi_defs.h"

namespace rvapi  {
    
  extern int getRVAPIError();
  extern void setRVAPIError ( int error_code );

  class Node  {

    public:
      Node ( const char * nodeId,
             int hrow     = -1,
             int hcol     = 0,
             int hrowSpan = 1,
             int hcolSpan = 1
           );
      Node ();
      virtual ~Node();

      virtual NODE_TYPE       type()  { return NTYPE_None; }

      inline  std::string & nodeId()  { return Id;         }
      virtual bool      wasCreated()  { return created;    }
      virtual bool     wasModified()  { return modified;   }
      virtual bool treeWasModified();
      virtual void setTreeModified();
      virtual bool treeWasCreated ();
      virtual void setTreeCreated ();
      virtual void setTreeFinished(); // unmodified, not created

      inline int n_nodes()   { return (int)nodes.size(); }
      inline Node * node ( int n ) { return nodes[n]; }

      inline void setBeforeId ( std::string bId ) { beforeId = bId; }

      void   addNode  ( Node *node );

      Node * findNode ( std::string & nodeId );  //!< full tree search
      Node * findNode ( const char *  nodeId );  //!< full tree search
      Node * findNode ( const char *  nodeId, NODE_TYPE t );  //!< full tree search

      inline int  getRow() { return row; }
      void  setPosition ( int hrow, int hcol,
                          int hrowSpan, int hcolSpan );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html  ( std::string & outDir,
                                 std::string & task );

      virtual void flush_xmli2 ( std::string & outDir,
                                 std::string & xml );

    protected:
      std::string         Id;       //!< unique node Id
      std::string         beforeId; //!< if not empty, specifies the
                                    //!< node to insert this one before
                                    //!< in html pages
      Node *              parent;   //!< parent (holder) node
      int                 row;      //!< cell row in parent's grid
      int                 col;      //!< cell column in parent's grid
      int                 rowSpan;  //!< row span in parent's grid
      int                 colSpan;  //!< row span in parent's grid
      std::vector<Node *> nodes;    //!< list of children

      void initNode();
      void freeNode();

      Node * addNode    ( Node *node, std::string & holderId );
      Node * addNode    ( Node *node, const char *  holderId );
      void   removeNode ( Node *node );

      std::string & gridPosition ( std::string & pos );
      std::string & gridPosition ( std::string & pos,
                                   int hrow,     int hcol,
                                   int hrowSpan, int hcolSpan );

      std::string & gridPosition_xmli2 ( std::string & pos );
      std::string & gridPosition_xmli2 ( std::string & pos,
                                         int hrow,     int hcol,
                                         int hrowSpan, int hcolSpan );

      std::string & getNodeFName ( std::string & fname,
                                   std::string  prefix,
                                   const char * suffix );

      inline void setParent  ( Node * holder )  { parent = holder; }
      inline void setModified  () { modified = true; }
      inline void clearModified() { modified = false; created = false; }

      std::string & makeContent ( std::string & fname,
                                  std::string & data,
                                  std::string & outDir,
                                  const char  * contentName );
      std::string & makeContent ( std::string & fname,
                                  std::string & data,
                                  std::string & outDir,
                                  std::string   contentName );

      std::string & addContent  ( std::string & fname,
                                  std::string & data,
                                  std::string & outDir,
                                  const char  * contentName );
      std::string & addContent  ( std::string & fname,
                                  std::string & data,
                                  std::string & outDir,
                                  std::string   contentName );

      virtual void make_xmli2_content  ( std::string & tag,
                                         std::string & content );

    private:
      static int idcnt;
      bool   modified; //!< true if node was modified since last flush
      bool   created;  //!< true if node was created since last flush

      void genId ( std::string & gId );

  };

  void swrite ( std::ofstream & s, int   n );
  void swrite ( std::ofstream & s, double v );
  void swrite ( std::ofstream & s, bool  b );
  void swrite ( std::ofstream & s, std::string   text );

  void sread ( std::ifstream & s, int   & n );
  void sread ( std::ifstream & s, double & v );
  void sread ( std::ifstream & s, bool  & b );
  void sread ( std::ifstream & s, std::string & text );

  std::string bool2str   ( bool   b );
  std::string double2str ( double d, const char *fmt );

}

#endif // RVAPI_NODE_H
