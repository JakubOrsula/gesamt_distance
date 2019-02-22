//  $Id: json_.h $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
//  =================================================================
//
//    26.01.17   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  json_  <interface>
//       ~~~~~~~~~
//  **** Classes :  gsmt::JSON  - json I/O
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2017
//
//  =================================================================
//

#ifndef __GSMT_JSON_H
#define __GSMT_JSON_H

#include "mmdb2/mmdb_io_file.h"

namespace gsmt  {

  DefineClass(JSON);

  class JSON  {
  
    DefineClass(Value);
    class Value  {
      public:
        mmdb::psvector  v;
        int        length;
        Value ();
        ~Value();
        void addValue ( mmdb::cpstr val );
        void write    ( mmdb::io::RFile f, int indent, int indent_inc );
      private:
        int allocLength;
    };

    DefineClass(Object);
    class Object  {
      public:
        JSON   **v;
        int length;
        Object ();
        ~Object();
        void addJSON ( JSON *json );
        void write   ( mmdb::io::RFile f, int indent, int indent_inc );
       private:
        int allocLength;
    };

    public:

      JSON();
      virtual ~JSON();
      
      void addValue ( mmdb::cpstr key, mmdb::cpstr val );
      void addValue ( mmdb::cpstr key, int val );
      void addValue ( mmdb::cpstr key, mmdb::realtype val,
                      mmdb::cpstr format );
      
      void addJSON  ( mmdb::cpstr key, JSON *obj );
      
      bool write    ( mmdb::cpstr fileName, int indent_inc );
      void write    ( mmdb::io::RFile f, int indent, int indent_inc ); // internal use

    protected:
      mmdb::psvector value_key;
      Value        **value;
      int            nValues;
      mmdb::psvector json_key;
      Object       **json;
      int            nJSONs;
      
      void  init();
      void  freeMemory();
      
    private:
      int nValuesAlloc;
      int nJSONsAlloc;

  };

}  // namespace gsmt

#endif // __GSMT_JSON_H
