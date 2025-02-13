/*
  db_blob.cpp
  BlobDB class implementation

  programmer  rmcinnis
  date        april 24, 2000
*/
#include <db_blob.h>
#include <report.h>
#ifdef LINUX
#  include <cstddef>
#endif

namespace go {

//======================================================================================
BlobBlock :: BlobBlock()
{
  defaults() ;
} // BlobBlock [constructor]

BlobBlock *BlobBlock :: create( uint16_t block_sz ) 
{
  BlobBlock *b = (BlobBlock*)new byte[ block_sz ] ;
  b->defaults() ;
  return b ;
} // BlobBlock :: create

void BlobBlock :: defaults() 
{
  block.hdr.tag = 0 ; block.hdr.sz = 0 ;
  block.hdr.next.addr = FNULL ; block.hdr.next.vid = 0 ; 
} // BlobBlock :: defaults

void BlobBlock :: destroy( BlobBlock *b )
{
  delete (byte*)b ;
} // BlobBlock :: destroy

uint16_t BlobBlock :: max_data( uint16_t block_sz )
{
  return block_sz - offsetof(BlobBlockHdr, data) ;
} // BlobBlock :: max_data

uint16_t BlobBlock :: memcpy( const byte *src, uint16_t sz, uint16_t block_sz )
{
  if (src == nullptr) return 0 ;
  data_sz() = min(max_data( block_sz ), sz) ;
  ::memcpy( data(), src, data_sz() ) ;
  return data_sz() ;
} // BlobBlock :: memcpy

bool BlobBlock :: read ( DAR &d, uint16_t block_sz, fileaddr pos ) 
{
  return d.seek( pos ) ? d.read ( block_sz, this ) : false ;
} // BlobBlock :: read

bool BlobBlock :: write( DAR &d, uint16_t block_sz, fileaddr pos ) 
{
  return d.seek( pos ) ? d.write( block_sz, this ) : false ;
} // BlobBlock :: write
    
//======================================================================================
BlobHeader :: BlobHeader( uint16_t blocksz ) 
{
  version    = 1 ;
  vid        = 0 ;
  block_sz   = blocksz ;
  garbage    = FNULL ;
  recycle_sz = 0L ;
} // BlobHeader [constructor]

fileaddr BlobHeader :: append_block( DAR &d )
{
  d.seek( 0, SEEK_END ) ;
  fileaddr pos = d.tell() ;
  // BUG FOUND: writing more data from a pointr then it point32_ts to can cause a segv
  static char  buf[4096] ;
  BlobBlock   *b = (BlobBlock*)buf ;
  b->defaults() ;
  b->write( d, block_sz, pos ) ; // reserve space
  return pos ;
} // BlobHeader :: append_block

uint32_t BlobHeader :: block_id( fileaddr pos )
{
  return (pos < (fileaddr)sizeof(*this)) ? 0 : (pos - sizeof(*this)) / block_sz ;
} // BlobHeader :: block_id

fileaddr BlobHeader :: block_pos( uint32_t blob_id )
{
  return sizeof(*this) + blob_id*block_sz ;
} // BlobHeader :: block_pos

fileaddr BlobHeader :: get_block( DAR &d )
{
  fileaddr pos = garbage ;
  if (pos != FNULL)
  {
    BlobBlockHdr hdr ;
    hdr.read( d, pos ) ; // reuse a piece of garbage
    recycle_sz = max( (recycle_sz - block_sz), 0 ) ; 
    garbage = hdr.next.addr ; // header needs to be written
    return pos ;
  }
  return append_block( d ) ;
} // BlobHeader :: get_block

int16_t BlobHeader :: get_sz( DAR &d, fileaddr pos, uint32_t &sz ) 
{
  BlobBlockHdr  hdr ;
  sz = 0 ;
  while (pos != FNULL)
  {
    if (!hdr.read( d, pos ))
      return -1 ;
    sz += hdr.sz ;
    pos = hdr.next.addr ;
  }
  return 0 ;
} // BlobHeader :: get_sz

bool BlobHeader :: init( DAR &d, uint32_t id, uint16_t blocksz ) 
{
  version    = 1 ;
  vid        = id ;
  block_sz   = blocksz ;
  garbage    = FNULL ; 
  recycle_sz = 0L ;
  memset( filler, 0x00, 16 ) ; return write(d) ; 
} // BlobHeader :: init

bool BlobHeader :: recycle_blob( DAR &d, fileaddr pos, uint32_t &block_bytes_freed )
{
  BlobBlockHdr  hdr ;
  fileaddr      gtop = pos ;
  block_bytes_freed = 0L ;
  if (pos == FNULL)  return false ;
  while (pos != FNULL)
  {
    hdr.read( d, pos ) ;
    if (hdr.next.addr == FNULL) // end of blob, point32_t at garbage
    {
      hdr.next.addr = garbage ;
      hdr.write( d, pos ) ; // write final block to disk (pts to old garbage top)
      garbage = gtop ; // new garbage top is first block in blob
      pos = FNULL ; // will drop out of loop
    }
    else
      pos = hdr.next.addr ;
    
    recycle_sz += block_sz ;
    block_bytes_freed += block_sz ;
  }
  return write( d ) ; // need to update garbage
} // BlobHeader :: recycle_blob

bool BlobHeader :: recycle_block( DAR &d, BlobBlockHdr &hdr, fileaddr pos ) // place on garbage heap
{
  hdr.next.addr = garbage ;
  garbage = pos ;
  recycle_sz += block_sz ; 
  return hdr.write( d, pos ) ;
} // BlobHeader :: recycle_block

bool BlobHeader :: read ( DAR &d )
{
  d.rewind() ;
  return d.read ( sizeof(*this), this ) ;
} // BlobHeader :: read

bool BlobHeader :: write( DAR &d )
{
  d.rewind() ;
  return d.write( sizeof(*this), this ) ;
} // BlobHeader :: write

//======================================================================================
int16_t BlobDB :: push_data( byte *dest, byte *src, uint32_t maxsz, uint16_t amt, uint32_t &tail, MsgCB *flushCB ) 
{
  if ((dest == nullptr) || (src == nullptr))
    return -1 ;
  
  uint16_t amtsent = 0, n ;
  
  while (amtsent < amt)
  {
    if ((maxsz - tail) > (uint32_t)(amt - amtsent))
      n = (amt - amtsent) ;
    else
      n = (maxsz - tail) ;
//    n = min( left, (amt - amtsent) ) ;
    memcpy( &dest[tail], &src[amtsent], n ) ;
    tail      += n ;
    amtsent   += n ;
    if (tail >= maxsz)
    { // notify callback
      if (flushCB != nullptr)
        flushCB->invoke( ArgList() << (void*)dest << (int32_t)maxsz ) ;
      tail = 0L ;
    }
  }
  return 0 ;
} // BlobDB :: push_data

BlobDB :: BlobDB( uint32_t volume_id )
{
  vid() = volume_id ;
} // BlobDB [constructor]

BlobDB :: ~BlobDB()
{
  close() ;
} // BlobDB [destructor]

void BlobDB :: close()
{
  _blob.close() ;
} // BlobDB :: close

bool BlobDB :: create( const char *blobname, uint32_t vid, uint16_t blocksz )
{
  if (blobname == nullptr) return false ;
  if (!_blob.open( blobname, "w+b" )) return false ;
  return _hdr.init(_blob, vid, blocksz) ;
} // BlobDB :: create

int32_t BlobDB :: flush()
{
  return _blob.flush() ;
} // BlobDB :: flush

uint32_t BlobDB :: getsz( uint32_t b_id )
{
  if (!isOpen()) return 0 ;
  uint32_t sz ;
  _hdr.get_sz( _blob, _hdr.block_pos( b_id ), sz ) ;
  return sz ;
} // BlobDB :: getsz

bool BlobDB :: open( const char *blobname ) 
{
  if (blobname == nullptr) return false ;
  if (!_blob.open( blobname, "r+b" )) return false ; 
  return _hdr.read(_blob) ;
} // BlobDB :: open

uint32_t BlobDB :: read( uint32_t b_id, uint32_t token, uint32_t maxsz, byte *dest, int16_t &rc, MsgCB *flushCB ) 
{
  if (dest == nullptr)
  {
    rc = -1 ;
    return 0 ;
  }
  if (!isOpen())
  {
    rc = -2 ;
    return 0 ;
  }
  fileaddr     pos = _hdr.block_pos( b_id ) ;
  BlobBlock   *blob = BlobBlock::create(_hdr.block_sz) ;
  uint32_t        cnt = 0 ;
  uint32_t        tail = 0 ;
//  uint32_t        sz_in_buf = 0 ;
  MutexScope   sc(_gate) ;

  if (pos > size())
  {
    debug_report( 2, "[BlobDB.read] block_id(%ld)  position(%ld)  bin size(%ld)\n", b_id, pos, size() ) ;
    pos = FNULL ;
  }
  
  while (pos != FNULL)
  {
//    debug_report( 2, "[BlobDB.read] position(%ld)  block size(%d)  cnt(%ld)\n", pos, _hdr.block_sz, cnt ) ;
    if (!blob->read( _blob, _hdr.block_sz, pos ))
    {
      debug_report( 2, "[BlobDB.read] read fail; block_id(%ld)  position(%ld)  bin size(%ld)  cnt(%ld)\n", b_id, pos, size(), cnt ) ;
//      printf( "pos == %ld\n", pos ) ;
      rc = -3 ;
      return cnt ; // return total bytes read
    }
    // place data in buffer (invoke flushCB if needed)
//    debug_report( 2, "[BlobDB.read] position(%ld)  blob size(%d)  tail(%ld)  next(%ld.%ld)\n",
//                  pos, blob->data_sz(), cnt, tail, blob->next().vid, blob->next().addr ) ;
    push_data( dest, blob->data(), maxsz, blob->data_sz(), tail, flushCB ) ;
    
    // increment cnt
    cnt += blob->data_sz() ;
    
    // grab next block
    pos = blob->next().addr ;
  }
  if (flushCB != nullptr)
    flushCB->invoke( ArgList() << (void*)dest << (int32_t)tail ) ;
  rc = 0 ;
  BlobBlock::destroy(blob) ;
  return cnt ;
} // BlobDB :: read

bool BlobDB :: recycle( uint32_t b_id, uint32_t &block_bytes_freed )
{
  if (!isOpen())
  {
    block_bytes_freed = 0L ;
    return false ;
  }
  MutexScope   sc(_gate) ;
  return _hdr.recycle_blob( _blob, _hdr.block_pos(b_id), block_bytes_freed ) ;
} // BlobDB :: recycle 

uint32_t BlobDB :: size_recycle() 
{
  return _hdr.recycle_sz ;
} // BlobDB :: size_recycle 

#include <report.h>
uint32_t BlobDB :: write( uint32_t sz, const byte *data, uint32_t token, int16_t &rc, uint32_t &block_bytes_used ) 
{
  rc               = -1 ;
  block_bytes_used = 0L ;
  if (!isOpen() || (sz == 0))
    return 0 ;
  
  uint32_t        cnt = 0, maxblob = BlobBlock::max_data(_hdr.block_sz), x ;
  fileaddr     pos, next = FNULL ;
  uint32_t        blob_id ;
  uint16_t       amt = 0 ;
  BlobBlock   *block = BlobBlock::create(_hdr.block_sz+1) ;
  MutexScope   sc(_gate) ;

  pos     = _hdr.get_block( _blob ) ;
  blob_id = _hdr.block_id( pos ) ;
  while (cnt < sz)
  {
//    amt = min( maxblob, (sz-cnt) ) ;
//-- i had to do this becuase microsoft's vc++ v6 woudl get an internal compiler error otherwise
    x = sz - cnt ;
    amt = (maxblob < x) ? maxblob : x ;
//--
    block->memcpy( &data[cnt], amt, _hdr.block_sz ) ;
    cnt += amt ;
    next = (cnt < sz) ? _hdr.get_block( _blob ) : FNULL ;
    block->next().addr = next ;
    block->write( _blob, _hdr.block_sz, pos  ) ;
    block_bytes_used += _hdr.block_sz ;
    pos = next ;
  }
  _hdr.write( _blob ) ; // just in case
  _blob.flush() ; // force flush - being safe
  rc = 0 ;
  BlobBlock::destroy(block) ;
  return blob_id ;
} // BlobDB :: write

} // namespace go 
