//
//  =================================================================
//
//    02.08.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_data  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Classes :  rvapi::Data - API Data class
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2013-2017
//
//  =================================================================
//

#ifndef RVAPI_DATA_H
#define RVAPI_DATA_H

#include "rvapi_node.h"

namespace rvapi  {

  class Data : public Node  {

    public:
      Data ( const char * datId,
             const char * dataTitle,
             int hrow     = -1,
             int hcol     = 0,
             int hrowSpan = 1,
             int hcolSpan = 1
           );
      Data ();
      ~Data();

      inline void setFoldState ( int state )  { foldState = state; }

      void addData ( std::string dataPath, std::string dataType );

      virtual NODE_TYPE type() { return NTYPE_Data; }

      virtual void write ( std::ofstream & s );
      virtual void read  ( std::ifstream & s );

      virtual void flush_html ( std::string & outDir,
                                std::string & task );

  protected:
    std::string              title; //!< to appear in the document
    std::vector<std::string> dpath;
    std::vector<std::string> dtype;
    int                      foldState;

    void initData();
    void freeData();

    std::string makeUMolTitle ( std::string path );
    int  dindex         ( const char * dataType );
    void addFile        ( std::string  & task, std::string path );
    void addYXZMapFiles ( std::string  & task, std::string name,
                          std::string pathxyz, std::string pathmtz,
                          std::string pathmap, std::string pathdmap,
                          std::string pathlib );
    void addYXZLibFiles ( std::string & task,  std::string name,
                          std::string pathxyz, std::string pathlib );
    void addDataFile    ( std::string  & task, std::string name,
                          std::string    path, int         btnKey );

    virtual void make_xmli2_content ( std::string & tag,
                                      std::string & content );

  private:
    int r; // temporary row number

  };

}

#endif // RVAPI_DATA_H
