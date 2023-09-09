#define __ALIGN_UP(x, a)                    (((x) + ((a) - 1)) & ~((a) - 1))

/* SBWC */
/* w: width, h: height, b: block size, a: alignment, s: stride, e: extra */
#define SBWC_STRIDE(w, b, a)                __ALIGN_UP(((b) * (((w) + 31) / 32)), a)
#define SBWC_HEADER_STRIDE(w)               ((((((w) + 63) / 64) + 15) / 16) * 16)
#define SBWC_Y_SIZE(s, h, e)                (((s) * ((__ALIGN_UP((h), 16) + 3) / 4)) + e)
#define SBWC_CBCR_SIZE(s, h, e)             (((s) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + e)
#define SBWC_Y_HEADER_SIZE(w, h, a)         __ALIGN_UP(((SBWC_HEADER_STRIDE(w) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 256), a)
#define SBWC_CBCR_HEADER_SIZE(w, h)         ((SBWC_HEADER_STRIDE(w) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 128)

/* SBWC 32B align */
#define SBWC_8B_STRIDE(w)                   SBWC_STRIDE(w, 128, 1)
#define SBWC_10B_STRIDE(w)                  SBWC_STRIDE(w, 160, 1)

#define SBWC_8B_Y_SIZE(w, h)                SBWC_Y_SIZE(SBWC_8B_STRIDE(w), h, 64)
#define SBWC_8B_Y_HEADER_SIZE(w, h)         SBWC_Y_HEADER_SIZE(w, h, 32)
#define SBWC_8B_CBCR_SIZE(w, h)             SBWC_CBCR_SIZE(SBWC_8B_STRIDE(w), h, 64)
#define SBWC_8B_CBCR_HEADER_SIZE(w, h)      SBWC_CBCR_HEADER_SIZE(w, h)

#define SBWC_10B_Y_SIZE(w, h)               SBWC_Y_SIZE(SBWC_10B_STRIDE(w), h, 64)
#define SBWC_10B_Y_HEADER_SIZE(w, h)        __ALIGN_UP((((__ALIGN_UP((w), 32) * __ALIGN_UP((h), 16) * 2) + 256) - SBWC_10B_Y_SIZE(w, h)), 32)
#define SBWC_10B_CBCR_SIZE(w, h)            SBWC_CBCR_SIZE(SBWC_10B_STRIDE(w), h, 64)
#define SBWC_10B_CBCR_HEADER_SIZE(w, h)     (((__ALIGN_UP((w), 32) * __ALIGN_UP((h), 16)) + 256) - SBWC_10B_CBCR_SIZE(w, h))

/* SBWC 256B align */
#define SBWC_256_8B_STRIDE(w)               SBWC_STRIDE(w, 128, 256)
#define SBWC_256_10B_STRIDE(w)              SBWC_STRIDE(w, 256, 1)

#define SBWC_256_8B_Y_SIZE(w, h)            SBWC_Y_SIZE(SBWC_256_8B_STRIDE(w), h, 0)
#define SBWC_256_8B_Y_HEADER_SIZE(w, h)     SBWC_Y_HEADER_SIZE(w, h, 256)
#define SBWC_256_8B_CBCR_SIZE(w, h)         SBWC_CBCR_SIZE(SBWC_256_8B_STRIDE(w), h, 0)
#define SBWC_256_8B_CBCR_HEADER_SIZE(w, h)  SBWC_CBCR_HEADER_SIZE(w, h)

#define SBWC_256_10B_Y_SIZE(w, h)           SBWC_Y_SIZE(SBWC_256_10B_STRIDE(w), h, 0)
#define SBWC_256_10B_Y_HEADER_SIZE(w, h)    SBWC_Y_HEADER_SIZE(w, h, 256)
#define SBWC_256_10B_CBCR_SIZE(w, h)        SBWC_CBCR_SIZE(SBWC_256_10B_STRIDE(w), h, 0)
#define SBWC_256_10B_CBCR_HEADER_SIZE(w, h) SBWC_CBCR_HEADER_SIZE(w, h)

/* SBWC Lossy align macros */
#define SBWCL_8B_STRIDE(w, r)               (((128 * (r)) / 100) * (((w) + 31) / 32))
#define SBWCL_10B_STRIDE(w, r)              (((160 * (r)) / 100) * (((w) + 31) / 32))

#define SBWCL_8B_Y_SIZE(w, h, r)            ((SBWCL_8B_STRIDE(w, r) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 64)
#define SBWCL_8B_CBCR_SIZE(w, h, r)         ((SBWCL_8B_STRIDE(w, r) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 64)

#define SBWCL_10B_Y_SIZE(w, h, r)           ((SBWCL_10B_STRIDE(w, r) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 64)
#define SBWCL_10B_CBCR_SIZE(w, h, r)        ((SBWCL_10B_STRIDE(w, r) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 64)

#define SBWCL_8B_CBCR_BASE(base, w, h, r)   ((base) + SBWCL_8B_Y_SIZE(w, h, r))
#define SBWCL_10B_CBCR_BASE(base, w, h, r)  ((base) + SBWCL_10B_Y_SIZE(w, h, r))

/* SBWC Lossy v2.7 32B/64B align */
#define SBWCL_32_STRIDE(w)             (96 * (((w) + 31) / 32))
#define SBWCL_64_STRIDE(w)             (128 * (((w) + 31) / 32))
#define SBWCL_HEADER_STRIDE(w)         ((((((w) + 63) / 64) + 15) / 16) * 16)

#define SBWCL_32_Y_SIZE(w, h)          (SBWCL_32_STRIDE(w) * (((h) + 3) / 4))
#define SBWCL_32_CBCR_SIZE(w, h)       (SBWCL_32_STRIDE(w) * ((((h) / 2) + 3) / 4))

#define SBWCL_64_Y_SIZE(w, h)          (SBWCL_64_STRIDE(w) * (((h) + 3) / 4))
#define SBWCL_64_CBCR_SIZE(w, h)       (SBWCL_64_STRIDE(w) * ((((h) / 2) + 3) / 4))

#define SBWCL_Y_HEADER_SIZE(w, h)      ((SBWCL_HEADER_STRIDE(w) * (((h) + 3) / 4)) + 64)
#define SBWCL_CBCR_HEADER_SIZE(w, h)   ((SBWCL_HEADER_STRIDE(w) * ((((h) / 2) + 3) / 4)) + 64)

#define SBWCL_32_CBCR_BASE(base, w, h) ((base) + SBWCL_32_Y_SIZE(w, h) + SBWCL_Y_HEADER_SIZE(w, h))
#define SBWCL_64_CBCR_BASE(base, w, h) ((base) + SBWCL_64_Y_SIZE(w, h) + SBWCL_Y_HEADER_SIZE(w, h))
