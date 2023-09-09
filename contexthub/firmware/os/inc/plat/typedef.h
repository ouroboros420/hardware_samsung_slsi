#ifndef __TYPE_DEF_H_
#define __TYPE_DEF_H_

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

#define __raw_write8(v, a)   (*(volatile unsigned char *)(a) = (v))
#define __raw_write16(v, a)  (*(volatile unsigned short *)(a) = (v))
#define __raw_write32(v, a)  (*(volatile unsigned int *)(a) = ((unsigned int)v))
#define __raw_writel(v, a)   __raw_write32(v, a)
#define __raw_read8(a)       (*(volatile unsigned char *)(a))
#define __raw_read16(a)      (*(volatile unsigned short *)(a))
#define __raw_read32(a)      (*(volatile unsigned int *)(a))
#define __raw_readl(a)       __raw_read32(a)

/** Read Data from Register */
#define REG_READ32(addr)     REG_UINT32(addr)
#define REG_READ16(addr)     REG_UINT16(addr)
#define REG_READ8(addr)      REG_UINT8(addr)

/** Write Data to Register */
#define REG_WRITE32(addr,data)    REG_UINT32(addr)=(unsigned int)(data)
#define REG_WRITE16(addr,data)    REG_UINT16(addr)=(unsigned short)(data)
#define REG_WRITE8(addr,data)     REG_UINT8(addr)=(unsigned char)(data)

#define REG_SET32(addr,data)      REG_UINT32(addr)=(REG_UINT32(addr)|((unsigned int)(data)))
#define REG_SET16(addr,data)      REG_UINT16(addr)=(REG_UINT16(addr)|((unsigned short)(data)))
#define REG_SET8(addr,data)       REG_UINT8(addr)=(REG_UINT8(addr)|(unsigned char)(data))

#define REG_CLEAR32(addr,data)    REG_UINT32(addr)=(REG_UINT32(addr)&(~((unsigned int)(data))))
#define REG_CLEAR16(addr,data)    REG_UINT16(addr)=(REG_UINT16(addr)&(~((unsigned short)(data))))
#define REG_CLEAR8(addr,data)     REG_UINT8(addr)=(REG_UINT8(addr)&(~((unsigned char)(data))))

#define RD(addr)                  REG_UINT32(addr)
#define WR(addr,data)             REG_WRITE32(addr,data)

#endif	// __TYPE_DEF_H_
