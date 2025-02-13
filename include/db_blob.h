/*
  db_blob.h
  BlobDB class definition

  programmer  rmcinnis
  date        april 24, 2000
*/
#ifndef DB_BLOB_H
#define DB_BLOB_H

#include <dar.h>
#include <msgcb.h>

namespace go {

/*
  blobs are made up of a header followed by a series of set size blocks.
  each block contains a fileaddr to the next block in the series.
  the header contains the size of the block, fileaddr of first garbage node, and the volume id
  a blob will span many blocks, not all of which have to be in the same blobbase
*/
typedef struct volumeaddr_tag
{
  fileaddr    addr ; // addr of next blob entry or blob-id if vid != current vid
  uint32_t       vid ; // volume id
} volumeaddr ; // struct volumeaddr

#define MAXBLOBCHUNK         (1*1024)
#define BLOBBLOCKSZ          (1*1024)

typedef struct BlobBlockHdr_tag
{
    uint16_t      tag ;
    uint16_t      sz ;      // amount of data in this block
    volumeaddr  next ;
    byte        data[2] ;
    
    bool        read ( DAR &d, fileaddr pos ) { d.seek( pos ) ; return d.read ( sizeof(*this), this ) ; }
    bool        write( DAR &d, fileaddr pos ) { d.seek( pos ) ; return d.write ( sizeof(*this), this ) ; }
} BlobBlockHdr ; // struct BlobBlockHdr

// use case: blob.block.hdr.next.addr
//           blob.block.hdr.data
typedef struct BlobBlock
{
    union block_tag{
        BlobBlockHdr   hdr ;
        byte           b[2] ;
    } block ;
                       BlobBlock() ;
    static BlobBlock  *create( uint16_t block_sz ) ;
    void               defaults()  ;
    static void        destroy( BlobBlock *b ) ;
    static uint16_t      max_data( uint16_t block_sz ) ;
    uint16_t             memcpy( const byte *src, uint16_t sz, uint16_t block_sz ) ;
    bool               read ( DAR &d, uint16_t block_sz, fileaddr pos ) ;
    bool               write( DAR &d, uint16_t block_sz, fileaddr pos ) ;
    
    // convienece methods
    uint16_t            &tag() { return block.hdr.tag ; }
    uint16_t            &data_sz() { return block.hdr.sz ; }
    volumeaddr        &next() { return block.hdr.next ; }
    byte              *data() { return block.hdr.data ; }
} BlobBlock ; // struct BlobBlock

typedef struct BlobHeader
{
    uint16_t      version ;
    uint16_t      block_sz ;
    uint32_t       vid ; // volume id
    fileaddr    garbage ; // FNULL if no garbage
    int32_t        recycle_sz ; 
    byte        filler[16] ; // round this to 32 bytes

                BlobHeader( uint16_t blocksz = BLOBBLOCKSZ ) ;
    fileaddr    append_block( DAR &d ) ;
    uint32_t       block_id( fileaddr pos ) ;
    fileaddr    block_pos( uint32_t blob_id ) ;
    fileaddr    get_block( DAR &d ) ;
    int16_t       get_sz( DAR &d, fileaddr pos, uint32_t &sz ) ;
    bool        init( DAR &d, uint32_t id = 0, uint16_t blocksz = BLOBBLOCKSZ ) ;
    bool        recycle_blob( DAR &d, fileaddr pos, uint32_t &block_bytes_freed ) ;
    bool        recycle_block( DAR &d, BlobBlockHdr &hdr, fileaddr pos ) ; // place on garbage heap
    bool        read ( DAR &d ) ;
    bool        write( DAR &d ) ;
} BlobHeader ; // struct BlobHeader

class BlobDB
{
  private :
    DAR                 _blob ;
    BlobHeader          _hdr ;
    MutexGate           _gate ;
    MsgCB               _reqVolumeDBCB ;

  protected :
    virtual int16_t       push_data( byte *dest, byte *src, uint32_t maxsz, uint16_t amt, uint32_t &tail, MsgCB *flushCB = nullptr ) ;
      
  public    :
                        BlobDB( uint32_t volume_id ) ;
    virtual            ~BlobDB() ;
    virtual void        close() ;
    virtual bool        create( const char *blobname, uint32_t vid = 0, uint16_t blocksz = BLOBBLOCKSZ ) ;
    virtual int32_t         flush() ;
    virtual uint32_t       getsz( uint32_t b_id ) ;
//    virtual uint32_t       getsz( uint32_t b_id, uint32_t &crc ) ;
    virtual bool        open( const char *blobname ) ;
    virtual uint32_t       read( uint32_t b_id, uint32_t token, uint32_t maxsz, byte *dest, int16_t &rc, MsgCB *flushCB = nullptr ) ;
    virtual bool        recycle( uint32_t b_id, uint32_t &block_bytes_freed ) ;
    virtual uint32_t       size() { return _blob.length() ; }
    virtual uint32_t       size_recycle() ;
    virtual uint32_t       write( uint32_t sz, const byte *data, uint32_t token, int16_t &rc, uint32_t &block_bytes_used ) ; // return blob-id    
    virtual uint32_t       write( const char *cstr, uint32_t token, int16_t &rc, uint32_t &block_bytes_used ) 
                        { return (cstr == nullptr)? 0L : write( strlen(cstr), (const byte*)cstr, token, rc, block_bytes_used ) ; }

    // access methods
    bool                isOpen() { return _blob.isOpen() ; }
    uint16_t              block_sz() { return _hdr.block_sz ; }
    uint32_t              &vid() { return _hdr.vid ; }
    MsgCB              &reqVolumeDBCB() { return _reqVolumeDBCB ; }
} ; // class BlobDB

} // namespace go 

#endif


