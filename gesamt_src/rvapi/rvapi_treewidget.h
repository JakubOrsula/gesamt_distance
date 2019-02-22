//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_treewidget  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::TreeWidget - API Tree class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//

#ifndef RVAPI_TREEWIDGET_H
#define RVAPI_TREEWIDGET_H

#include "rvapi_node.h"

namespace rvapi  {

  class TreeWidget : public Node  {

    public:
      TreeWidget ( const char * treeId,
                   const char * treeTitle,
                   int  hrow     = -1,
                   int  hcol     = 0,
                   int  hrowSpan = 1,
                   int  hcolSpan = 1
                 );
      TreeWidget ();
      ~TreeWidget();

      virtual NODE_TYPE type() { return NTYPE_TreeWidget; }

      // Composing the tree: 1) add nodes as usual width addNode()
      // 2) set parents and titles for all nodes using setTreeNode().
      // Root nodes should have empty parent Id (""). All children of
      // same parent will be placed in the same order as they were
      // added to the tree in (1).

      void setTreeNode (
         const char * leafId,    //!< to be shown when leaf is selected
         const char * leafTitle, //!< leaf title
         const char * leafOpen,  //!< leaf open status
         const char * parId      //!< parent Id (same as of parent Node)
                       );

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html  ( std::string & outDir,
                                 std::string & task );

    protected:
      std::string               title;
      std::vector<std::string>  leaf_titles;
      std::vector<std::string>  leaf_open;
      std::vector<std::string>  leaf_parents;

      void initTreeWidget();
      void freeTreeWidget();
      void completeStructure();

      std::string & make_tree ( std::string & tree,
                                std::string parentId,
                                std::string & outDir );

      virtual void make_xmli2_content ( std::string & tag,
                                        std::string & content );

  };

}

#endif // RVAPI_GRAPHWIDGET_H
