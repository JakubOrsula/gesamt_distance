//
//  =================================================================
//
//    04.12.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_treewidget  <implemenation>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::TreeWidget - API Tree class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2014
//
//  =================================================================
//

#include "rvapi_treewidget.h"
#include "rvapi_tasks.h"
#include "rvapi_global.h"

rvapi::TreeWidget::TreeWidget ( const char *treeId,
                                const char *treeTitle,
                                int hrow, int hcol,
                                int hrowSpan, int hcolSpan  )
                 : Node(treeId,hrow,hcol,hrowSpan,hcolSpan)  {
  initTreeWidget();
  title = treeTitle;
}

rvapi::TreeWidget::TreeWidget() : Node()  {
  initTreeWidget();
}

rvapi::TreeWidget::~TreeWidget()  {
  freeTreeWidget();
}

void rvapi::TreeWidget::initTreeWidget()  {}

void rvapi::TreeWidget::freeTreeWidget()  {
  leaf_titles .clear();
  leaf_open   .clear();
  leaf_parents.clear();
}

void rvapi::TreeWidget::completeStructure()  {
  while (leaf_titles.size()<=nodes.size())
    leaf_titles.push_back("");
  while (leaf_open.size()<=nodes.size())
    leaf_open.push_back(RVAPI_TREE_NODE_Auto);
  while (leaf_parents.size()<=nodes.size())
    leaf_parents.push_back("");
}


void rvapi::TreeWidget::setTreeNode (
           const char * leafId,    // to be shown when leaf is selected
           const char * leafTitle, // leaf title
           const char * leafOpen,  // leaf open status
           const char * parId      // parent Id (same as of parent Node)
                                    )  {
int n = -1;

  completeStructure();

  for (int i=0;(i<(int)nodes.size()) && (n<0);i++)
    if (nodes[i])  {
      if (nodes[i]->nodeId()==leafId)
        n = i;
    }

  if (n>=0)  {
    leaf_titles [n] = leafTitle;
    leaf_open   [n] = leafOpen;
    leaf_parents[n] = parId;
    setModified();
  }

}



std::string & rvapi::TreeWidget::make_tree ( std::string & tree,
                                             std::string parentId,
                                             std::string & outDir  )  {
std::string branch;
std::string node_task;
std::string fname;

  tree.clear();

  for (int i=0;(i<(int)nodes.size());i++)
    if (leaf_parents[i]==parentId)  {
      if (tree.empty()) tree = "[\n{";
                   else tree.append ( ",{\n" );
      tree.append ( " label: '" + leaf_titles[i] +
                    "', id: '"  + nodes[i]->nodeId() + "-leaf'" );
      node_task.clear();
      nodes[i]->setTreeCreated();
      nodes[i]->flush_html ( outDir,node_task );
      node_task = makeContent ( fname,node_task,outDir,
                                nodes[i]->nodeId() + ".tsk" );
      tree.append ( ", task: '" + node_task + "'" );
      tree.append ( ", openMode: '" + leaf_open[i] + "'" );
      make_tree ( branch,nodes[i]->nodeId(),outDir );
      if (!branch.empty())
        tree.append  ( ",\n children: " + branch );
      tree.append ( "\n}" );
    }

  if (!tree.empty())
    tree.append ( "\n]" );

  return tree;

}

void rvapi::TreeWidget::write ( std::ofstream & s )  {
int n = leaf_titles.size();
  swrite ( s,title );
  swrite ( s,n     );
  for (int i=0;i<n;i++)  {
    swrite ( s,leaf_titles [i] );
    swrite ( s,leaf_open   [i] );
    swrite ( s,leaf_parents[i] );
  }
  Node::write ( s );
}

void rvapi::TreeWidget::read ( std::ifstream & s )  {
std::string t;
std::string o;
std::string p;
int         n;
  freeTreeWidget();
  sread ( s,title );
  sread ( s,n     );
  for (int i=0;i<n;i++)  {
    sread ( s,t );
    sread ( s,o );
    sread ( s,p );
    leaf_titles .push_back ( t );
    leaf_open   .push_back ( o );
    leaf_parents.push_back ( p );
  }
  Node::read ( s );
}

void rvapi::TreeWidget::flush_html ( std::string & outDir,
                                     std::string & task )  {

  if (!wasCreated())
    for (unsigned int i=0;i<nodes.size();i++)
      if (nodes[i]->treeWasCreated())  {
        setModified();
        nodes[i]->setTreeCreated();
      }

  if (wasModified())  {

    completeStructure();

    std::string treeData;
    std::string fname;
    std::string pos;

    make_tree ( treeData,"",outDir );

    if (treeData.size()>0)  //inline_size_threshold)
      treeData = makeContent ( fname,treeData,outDir,"tree_data" );

    add_tree_widget ( task,nodeId(),title,parent->nodeId(),treeData,
                      gridPosition(pos) );

  } else  {

    for (unsigned int i=0;i<nodes.size();i++)
      nodes[i]->flush_html ( outDir,task );

  }

  clearModified();

}

void rvapi::TreeWidget::make_xmli2_content ( std::string & tag,
                                             std::string & content )  {
  tag = "tree";
  content = "<title>" + title + "</title>\n";
  for (unsigned int i=0;i<leaf_titles.size();i++)
    content.append ( "<leaf title=\"" + leaf_titles [i] +
                         "\" open=\"" + leaf_open   [i] +
                       "\" parent=\"" + leaf_parents[i] +
                     "\"></leaf>\n" );
}
