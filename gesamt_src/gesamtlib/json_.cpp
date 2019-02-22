//  $Id: json_.cpp $
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
//  **** Module  :  json_  <implementation>
//       ~~~~~~~~~
//  **** Classes :  gsmt::JSON  - json I/O
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2017
//
//  =================================================================
//

#include <string.h>

#include "json_.h"

namespace gsmt  {

  // ----------------------------------------------------------------
  
  JSON::Value::Value()  {
    v           = NULL;
    length      = 0;
    allocLength = 0;
  }

  JSON::Value::~Value()  {
    for (int i=0;i<length;i++)
      if (v[i])
        delete[] v[i]; 
    mmdb::FreeVectorMemory ( v,0 );
  }

  void _replace_char ( mmdb::pstr S, char c1, char c2 )  {
  mmdb::pstr p = strchr ( S,c1 );
    while (p) {
      *p = c2;
      p = strchr ( p,c1 );
    }
  }

  void JSON::Value::addValue ( mmdb::cpstr val )  {
  //mmdb::pstr p;
    if (length>=allocLength)  {
      mmdb::psvector v1;
      allocLength = length + 50;
      mmdb::GetVectorMemory ( v1,allocLength,0 );
      for (int i=0;i<length;i++)  {
        v1[i] = v[i];
        v [i] = NULL;
      }
      mmdb::FreeVectorMemory ( v,0 );
      v = v1;
    }
    v[length] = NULL;
    mmdb::CreateCopy ( v[length],val );
    _replace_char ( v[length],'"','\'' );
    _replace_char ( v[length],'\\','/' );
    length++;
  }

  void JSON::Value::write ( mmdb::io::RFile f, int indent, int indent_inc )  {
  mmdb::pstr S = NULL;
  int        i,n;

    if (indent_inc>0)  {
      f.Write ( " " );
      if (length>=2)  {
        n = indent + indent_inc;
        S = new char[n+20];
        for (i=0;i<n;i++)
          S[i] = ' ';
        S[n] = char(0);
      }
    }
      
    if (length<2)  {
      f.Write ( "\"" );
      f.Write ( v[0] );
      f.Write ( "\"" );
    } else  {
      f.Write ( "[" );
      for (i=0;i<length;i++)  {
        if (i>0)
          f.Write ( "," );
        if (indent_inc>0)  {
          f.LF();
          f.Write ( S );
        }
        f.Write ( "\"" );
        f.Write ( v[i] );
        f.Write ( "\"" );
      }
      if (indent_inc>0)
        f.Write ( " " );
      f.Write ( "]" );
    }
    
    if (S)  delete[] S;

  }

  // ----------------------------------------------------------------

  JSON::Object::Object()  {
    v           = NULL;
    length      = 0;
    allocLength = 0;
  }

  JSON::Object::~Object()  {
    for (int i=0;i<length;i++)
      if (v[i])
        delete v[i]; 
    delete[] v;
  }
  
  void JSON::Object::addJSON ( JSON *json )  {
    if (length>=allocLength)  {
      JSON **v1;
      allocLength = length + 50;
      v1 = new PJSON[allocLength];
      for (int i=0;i<length;i++)  {
        v1[i] = v[i];
        v [i] = NULL;
      }
      delete[] v;
      v = v1;
    }
    v[length++] = json;
  }

  void JSON::Object::write ( mmdb::io::RFile f, int indent, int indent_inc )  {
  int  i;

    if (indent_inc>0)
      f.Write ( " " );
      
    if (length<2)  {
      v[0]->write ( f,indent+indent_inc,indent_inc );
    } else  {
      f.Write ( "[" );
      for (i=0;i<length;i++)  {
        if (i>0)
          f.Write ( "," );
        if (indent_inc>0)
          f.LF();
        v[i]->write ( f,indent+indent_inc,indent_inc );
      }
      if (indent_inc>0)
        f.Write ( " " );
      f.Write ( "]" );
    }

  }


  // ----------------------------------------------------------------

  JSON::JSON()  {
    init();
  }

  JSON::~JSON()  {
    freeMemory();
  }

  void JSON::init()  {
    
    value_key = NULL;
    value     = NULL;
    nValues   = 0;
    json_key  = NULL;
    json      = NULL;
    nJSONs    = 0;
 
    nValuesAlloc = 0;
    nJSONsAlloc  = 0;

  }

  void JSON::freeMemory()  {
  int i;
  
    for (i=0;i<nValues;i++)
      if (value[i])
        delete value[i];
    delete[] value;
    mmdb::FreeVectorMemory ( value_key,0 );
    
    for (i=0;i<nJSONs;i++)
      if (json[i])
        delete   json[i];      
    delete[] json;
    mmdb::FreeVectorMemory ( json_key,0 );
    
    init();
  
  }

  void JSON::addValue ( mmdb::cpstr key, mmdb::cpstr val ) {
  int i;
  
    if (nValues>=nValuesAlloc)  {
      mmdb::psvector  value_key1;
      Value         **value1;
      nValuesAlloc += 20;
      mmdb::GetVectorMemory ( value_key1,nValuesAlloc,0 );
      value1 = new PValue[nValuesAlloc];
      for (i=0;i<nValues;i++)  {
        value_key1[i] = value_key[i];
        value1    [i] = value    [i];
        value_key [i] = NULL;
        value     [i] = NULL;
      }
      for (i=nValues;i<nValuesAlloc;i++)  {
        value_key1[i] = NULL;
        value1    [i] = NULL;
      }
      mmdb::FreeVectorMemory ( value_key,0 );
      delete[] value;
      value_key = value_key1;
      value     = value1;
    }
    
    int n = -1;
    for (i=0;(i<nValues) && (n<0);i++)
      if (!strcmp(value_key[i],key))
        n = i;
        
    if (n<0)  {
      n = nValues;
      mmdb::CreateCopy ( value_key[n],key );
      value[n] = new Value();
      nValues++;
    }
    
    value[n]->addValue ( val );
  
  }

  void JSON::addValue ( mmdb::cpstr key, int val )  {
  char S[100];
    sprintf ( S,"%i",val );
    addValue ( key,S );
  }

  void JSON::addValue ( mmdb::cpstr key, mmdb::realtype val,
                        mmdb::cpstr format )  {
  char S[100];
    sprintf ( S,format,val );
    addValue ( key,S );
  }
  
  void JSON::addJSON  ( mmdb::cpstr key, JSON *obj )  {
  int i;
  
    if (nJSONs>=nJSONsAlloc)  {
      mmdb::psvector  json_key1;
      Object        **json1;
      nJSONsAlloc += 20;
      mmdb::GetVectorMemory ( json_key1,nJSONsAlloc,0 );
      json1 = new PObject[nJSONsAlloc];
      for (i=0;i<nJSONs;i++)  {
        json_key1[i] = json_key[i];
        json1    [i] = json    [i];
        json_key [i] = NULL;
        json     [i] = NULL;
      }
      for (i=nJSONs;i<nJSONsAlloc;i++)  {
        json_key1[i] = NULL;
        json1    [i] = NULL;
      }
      mmdb::FreeVectorMemory ( json_key,0 );
      if (json)
        delete[] json;
      json_key = json_key1;
      json     = json1;
    }
    
    int n = -1;
    for (i=0;(i<nJSONs) && (n<0);i++)
      if (!strcmp(json_key[i],key))
        n = i;
        
    if (n<0)  {
      n = nJSONs;
      mmdb::CreateCopy ( json_key[n],key );
      json[n] = new Object();
      nJSONs++;
    }
    
    json[n]->addJSON ( obj );
    
  }
  
  bool JSON::write ( mmdb::cpstr fileName, int indent_inc )  {
  mmdb::io::File f;
  
    f.assign ( fileName,true,false );
    if (f.rewrite())  {
      write ( f,0,indent_inc );    
      return true;
    } else
      return false;
  
  }

  void JSON::write ( mmdb::io::RFile f, int indent, int indent_inc )  {
  mmdb::pstr S = NULL;
  int        i,j,n,m,k;
  bool       first;
  
    if (indent_inc>0)  {
      n = 0;
      for (i=0;i<nValues;i++)
        if (value_key[i])
          n = mmdb::IMax(n,strlen(value_key[i]));
      for (i=0;i<nJSONs;i++)
        if (json_key[i])
          n = mmdb::IMax(n,strlen(json_key[i]));
      n += 3;
    } else
      n = -1;

    S = new char[indent+20];
    m = 0;
    if (indent_inc>0)  {
      while (m<indent)
        S[m++] = ' ';
      S[m] = char(0);
      f.Write ( S );
      f.Write ( "{\n" );
      j = indent + indent_inc;
      while (m<j)
        S[m++] = ' ';
      S[m] = char(0);
    } else  {
      S[0] = char(0);
      f.Write ( "{" );
    }

    first = true;
    for (i=0;i<nValues;i++)
      if (value_key[i] && value[i])  {
        if (!first)  {
          f.Write ( "," );
          if (indent_inc>0)
            f.LF();
        }
        if (indent_inc>0)
          f.Write ( S );
        f.Write ( "\"" );
        f.Write ( value_key[i] );
        f.Write ( "\"" );
        if (indent_inc>0)  {
          k = strlen(value_key[i]) + 2;
          while (k<n)  {
            f.Write ( " " );
            k++;
          } 
        }
        f.Write ( ":" );
        value[i]->write ( f,indent+indent_inc,indent_inc );
        first = false;
      }

    for (i=0;i<nJSONs;i++)
      if (json_key[i] && json[i])  {
        if (!first)  {
          f.Write ( "," );
          if (indent_inc>0)
            f.LF();
        }
        if (indent_inc>0)
          f.Write ( S );
        f.Write ( "\"" );
        f.Write ( json_key[i] );
        f.Write ( "\"" );
        if (indent_inc>0)  {
          k = strlen(json_key[i]) + 2;
          while (k<n)  {
            f.Write ( " " );
            k++;
          } 
        }
        f.Write ( ":" );
        json[i]->write ( f,indent+indent_inc,indent_inc );
        first = false;
      }
      
    if (indent_inc>0)  {
      f.LF();
      S[indent] = char(0);
      f.Write ( S ); 
      f.Write ( "}" );
    } else
      f.Write ( "}" );
      
    if (S)  delete[] S;

  }

}  // namespace gsmt
