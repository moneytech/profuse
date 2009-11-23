#ifndef __ENDIAN_H__
#define __ENDIAN_H__

// utlities to read/write bytes.

#include <stdint.h>

namespace LittleEndian {


    uint8_t Read8(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return (p[0]);        
    }
    
    uint16_t Read16(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return p[0] | (p[1] << 8);        
    }

    uint32_t Read24(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return (p[0]) | (p[1] << 8) | (p[2] << 16);        
    }


    uint32_t Read32(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return (p[0]) | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);  
    }
   
    
    uint8_t Read8(const void *vp, unsigned offset)
    {
        return Read8(offset + (const uint8_t *)vp);       
    }    
    
    uint16_t Read16(const void *vp, unsigned offset)
    {
        return Read16(offset + (const uint8_t *)vp);       
    }    
    
    uint32_t Read24(const void *vp, unsigned offset)
    {
        return Read24(offset + (const uint8_t *)vp);       
    }    
    
    uint32_t Read32(const void *vp, unsigned offset)
    {
        return Read32(offset + (const uint8_t *)vp);       
    }        


    // write
    void Write8(void *vp, uint8_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = x;        
    }

    void Write16(void *vp, uint16_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = (x) & 0xff;
        p[1] = (x >> 8) & 0xff;   
    }

    void Write24(void *vp, uint32_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = (x) & 0xff;
        p[1] = (x >> 8) & 0xff;     
        p[2] = (x >> 16) & 0xff;     
    }
    
    void Write32(void *vp, uint32_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = (x) & 0xff;
        p[1] = (x >> 8) & 0xff;     
        p[2] = (x >> 16) & 0xff;   
        p[3] = (x >> 24) & 0xff;        
    }
    
    void Write8(void *vp, unsigned offset, uint8_t x)
    {
        Write8(offset + (uint8_t *)vp, x);
    }

    void Write16(void *vp, unsigned offset, uint16_t x)
    {
        Write16(offset + (uint8_t *)vp, x);
    }

    void Write24(void *vp, unsigned offset, uint32_t x)
    {
        Write24(offset + (uint8_t *)vp, x);
    }

    void Write32(void *vp, unsigned offset, uint32_t x)
    {
        Write32(offset + (uint8_t *)vp, x);
    }

}


namespace BigEndian {


    uint8_t Read8(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return p[0];        
    }
    
    uint16_t Read16(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return (p[0] << 8) | (p[1]);        
    }

    uint32_t Read24(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return (p[0] << 16) | (p[1] << 8) | (p[2]);        
    }


    uint32_t Read32(const void *vp)
    {
        const uint8_t *p = (const uint8_t *)vp;
        return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);  
    }
   
    
    uint8_t Read8(const void *vp, unsigned offset)
    {
        return Read8(offset + (const uint8_t *)vp);       
    }    
    
    uint16_t Read16(const void *vp, unsigned offset)
    {
        return Read16(offset + (const uint8_t *)vp);       
    }    
    
    uint32_t Read24(const void *vp, unsigned offset)
    {
        return Read24(offset + (const uint8_t *)vp);       
    }    
    
    uint32_t Read32(const void *vp, unsigned offset)
    {
        return Read32(offset + (const uint8_t *)vp);       
    }        



    // write
    void Write8(void *vp, uint8_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = x;        
    }

    void Write16(void *vp, uint16_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = (x >> 8) & 0xff;
        p[1] = (x) & 0xff;   
    }

    void Write24(void *vp, uint32_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = (x >> 16) & 0xff;
        p[1] = (x >> 8) & 0xff;     
        p[2] = (x) & 0xff;     
    }
    
    void Write32(void *vp, uint32_t x)
    {
        uint8_t *p = (uint8_t *)vp;
        p[0] = (x >> 24) & 0xff;
        p[1] = (x >> 16) & 0xff;     
        p[2] = (x >> 8) & 0xff;   
        p[3] = (x) & 0xff;        
    }
    
    void Write8(void *vp, unsigned offset, uint8_t x)
    {
        Write8(offset + (uint8_t *)vp, x);
    }

    void Write16(void *vp, unsigned offset, uint16_t x)
    {
        Write16(offset + (uint8_t *)vp, x);
    }

    void Write24(void *vp, unsigned offset, uint32_t x)
    {
        Write24(offset + (uint8_t *)vp, x);
    }

    void Write32(void *vp, unsigned offset, uint32_t x)
    {
        Write32(offset + (uint8_t *)vp, x);
    }


}



#endif