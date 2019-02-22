//  $Id: memio_.h $
// =================================================================
//  This code is distributed under the terms and conditions of the
//  CCP4 Program Suite Licence Agreement as 'Part 2' (Annex 2)
//  software. A copy of the CCP4 licence can be obtained by writing
//  to CCP4, Research Complex at Harwell, Rutherford Appleton
//  Laboratory, Didcot OX11 0FA, UK, or from
//  http://www.ccp4.ac.uk/ccp4license.php.
//  =================================================================
//
//    03.02.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  memio_  <interface>
//       ~~~~~~~~~
//  **** Classes :  gsmt::MemIO  - buffered I/O
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2010-2015
//
//  =================================================================
//

#ifndef __GSMT_MEMIO_H
#define __GSMT_MEMIO_H

#include "mmdb2/mmdb_io_file.h"

namespace gsmt  {

  DefineClass(MemIO);

  class MemIO  {

    public:

      enum { Ok,CantOpenFile,EmptyFile,ReadError,WriteError,MemReadError,
             CompressionError,UncompressionError,SizeError };

      MemIO();
      virtual ~MemIO();

      void  setCompressionLevel ( int compressionLevel ); // 0,1,2,3

      int   read  ( mmdb::cpstr  fileName,
                    mmdb::io::GZ_MODE gzMode=mmdb::io::GZM_NONE );
      int   read  ( mmdb::io::RFile f );

      int   write ( mmdb::cpstr  fileName,
                    mmdb::io::GZ_MODE gzMode=mmdb::io::GZM_NONE );
      int   write ( mmdb::io::RFile f );

      int   length() { return buffer_length; }

      void  reset();
      void  free ();

      void *get_buffer        ( const int length );
      void  set_buffer_length ( const int len )  { buffer_length = len; }
      void  take_buffer       ( mmdb::pstr * buf, int * length );
      void  get_buffer        ( mmdb::pstr * buf, int * length );

      void  write_buffer ( const void * src,  const int length  );
      bool  read_buffer  ( void       * dest, const int length,
                           bool * Ok = NULL);

      void  put_integer   ( int        I );
      void  put_ishort    ( int        I );  // saves as short
      void  put_ibyte     ( int        I );  // saves as byte
      void  put_byte      ( mmdb::byte B );
      void  put_short     ( short      I );
      void  put_long      ( long       I );
      void  put_word      ( mmdb::word W );
      void  put_real      ( mmdb::realtype  R );
      void  put_float     ( mmdb::realtype  R );  // stores single precision
      void  put_shortreal ( mmdb::shortreal R );
      void  put_line      ( mmdb::cpstr     L );  // for static strings
      void  put_string    ( mmdb::cpstr     L );  // for dynamic strings
      void  put_bool      ( bool   B );

      bool  get_integer   ( int        & I, bool * Ok = NULL );
      bool  get_ishort    ( int        & I, bool * Ok = NULL );
      bool  get_ibyte     ( int        & I, bool * Ok = NULL );
      bool  get_byte      ( mmdb::byte & B, bool * Ok = NULL );
      bool  get_short     ( short      & I, bool * Ok = NULL );
      bool  get_long      ( long       & I, bool * Ok = NULL );
      bool  get_word      ( mmdb::word      & W, bool * Ok = NULL );
      bool  get_real      ( mmdb::realtype  & R, bool * Ok = NULL );
      bool  get_float     ( mmdb::realtype  & R, bool * Ok = NULL );
      bool  get_shortreal ( mmdb::shortreal & R, bool * Ok = NULL );
      bool  get_line      ( mmdb::pstr        L, bool * Ok = NULL );
      bool  get_string    ( mmdb::pstr      & L, bool * Ok = NULL );
      bool  get_bool      ( bool   & B, bool * Ok = NULL );

    private:
      int           compression_level;
      mmdb::bvector buffer,compression_buffer;
      int           buffer_length,buffer_pos,alloc_length,chunk_size;
      int           compression_alloc;

  };

}  // namespace gsmt

#endif // __GSMT_MEMIO_H
