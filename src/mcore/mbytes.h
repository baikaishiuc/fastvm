/*
\file		bytes.h
\brief		byte order function unit
 ----history----
\author         chengzhiyong
\date           2012-12-15
\version        0.01
\desc           create

\author         chengzhiyong
\date           2015-11-21
\version        0.02
\desc           rename bytes_xxx to mbytes_xxx

*/
#if !defined(__mbytes_h__)
#define  __mbytes_h__

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(mbytes_t_i64)
#   if defined(_MSC_VER)
#       define mbytes_t_i64  __int64
#   else
#       define mbytes_t_i64  long long
#   endif
#endif

#define mbytes_read_int_big_endian_2b(_buf)   ((((unsigned)(((unsigned char*)(_buf))[0])) << 8)  \
                                            | (((unsigned)(((unsigned char*)(_buf))[1])) << 0))
#define mbytes_read_int_little_endian_2b(_buf)((((unsigned)(((unsigned char*)(_buf))[0])) << 0)  \
                                            | (((unsigned)(((unsigned char*)(_buf))[1])) << 8))
#define mbytes_read_int_big_endian_4b(_buf)   ((((unsigned)(((unsigned char*)(_buf))[0])) << 24) \
                                            | (((unsigned)(((unsigned char*)(_buf))[1])) << 16) \
                                            | (((unsigned)(((unsigned char*)(_buf))[2])) << 8)  \
                                            | (((unsigned)(((unsigned char*)(_buf))[3])) << 0))
#define mbytes_read_int_little_endian_4b(_buf)((((unsigned)(((unsigned char*)(_buf))[0])) << 0)  \
                                            | (((unsigned)(((unsigned char*)(_buf))[1])) << 8)  \
                                            | (((unsigned)(((unsigned char*)(_buf))[2])) << 16) \
                                            | (((unsigned)(((unsigned char*)(_buf))[3])) << 24))

#define mbytes_read_int_big_endian_6b(_buf)  ((((mbytes_t_i64)mbytes_read_int_big_endian_2b(&(((unsigned char*)(_buf))[0]))) << 32) \
                                            | mbytes_read_int_big_endian_4b(&(((unsigned char*)(_buf))[2])))
#define mbytes_read_int_little_endian_6b(_buf)  ((mbytes_read_int_little_endian_4b(&(((unsigned char*)(_buf))[0]))) \
                                            | (((mbytes_t_i64)mbytes_read_int_little_endian_2b(&(((unsigned char*)(_buf))[4])))<< 32))
#define mbytes_read_int_big_endian_8b(_buf)  ((((mbytes_t_i64)mbytes_read_int_big_endian_4b(&(((unsigned char*)(_buf))[0]))) << 32) \
                                            | mbytes_read_int_big_endian_4b(&(((unsigned char*)(_buf))[4])))
#define mbytes_read_int_little_endian_8b(_buf)  ((mbytes_read_int_little_endian_4b(&(((unsigned char*)(_buf))[0]))) \
                                            | (((mbytes_t_i64)mbytes_read_int_little_endian_4b(&(((unsigned char*)(_buf))[4])))<< 32))

#define mbytes_write_int_big_endian_2b(_buf, _i_value) \
    do{\
        ((unsigned char*)(_buf))[0] = (unsigned char)(((_i_value) >> 8) & 0xff);\
        ((unsigned char*)(_buf))[1] = (unsigned char)((_i_value) & 0xff);\
    }while(0)
#define mbytes_write_int_little_endian_2b(_buf, _i_value) \
    do{\
        ((unsigned char*)(_buf))[0] = (unsigned char)((_i_value) & 0xff);\
        ((unsigned char*)(_buf))[1] = (unsigned char)(((_i_value) >> 8) & 0xff);\
    }while(0)

#define mbytes_write_int_big_endian_4b(_buf, _i_value) \
    do{\
        ((unsigned char*)(_buf))[0] = (unsigned char)(((_i_value) >> 24) & 0xff);\
        ((unsigned char*)(_buf))[1] = (unsigned char)(((_i_value) >> 16) & 0xff);\
        ((unsigned char*)(_buf))[2] = (unsigned char)(((_i_value) >> 8) & 0xff);\
        ((unsigned char*)(_buf))[3] = (unsigned char)((_i_value) & 0xff);\
    }while(0)
#define mbytes_write_int_little_endian_4b(_buf, _i_value) \
    do{\
        ((unsigned char*)(_buf))[0] = (unsigned char)((_i_value) & 0xff);\
        ((unsigned char*)(_buf))[1] = (unsigned char)(((_i_value) >> 8) & 0xff);\
        ((unsigned char*)(_buf))[2] = (unsigned char)(((_i_value) >> 16) & 0xff);\
        ((unsigned char*)(_buf))[3] = (unsigned char)(((_i_value) >> 24) & 0xff);\
    }while(0)
#define mbytes_write_int_big_endian_6b(_buf, _i64_value) \
        do{\
            unsigned high_part = (((mbytes_t_i64)_i64_value)>> 32), low_part = ((mbytes_t_i64)_i64_value) & 0xffffffff; \
            mbytes_write_int_big_endian_2b(_buf, high_part); \
            mbytes_write_int_big_endian_4b(&((char*)_buf)[2], low_part); \
        }while(0)
#define mbytes_write_int_little_endian_6b(_buf, _i64_value) \
            do{\
                unsigned high_part = (((mbytes_t_i64)_i64_value)>> 32), low_part = ((mbytes_t_i64)_i64_value) & 0xffffffff; \
                mbytes_write_int_little_endian_4b(_buf, low_part); \
                mbytes_write_int_little_endian_2b(&((char*)_buf)[4], high_part); \
            }while(0)
#define mbytes_write_int_big_endian_8b(_buf, _i64_value) \
    do{\
        unsigned high_part = (((mbytes_t_i64)_i64_value)>> 32), low_part = ((mbytes_t_i64)_i64_value) & 0xffffffff; \
        mbytes_write_int_big_endian_4b(_buf, high_part); \
        mbytes_write_int_big_endian_4b(&((char*)_buf)[4], low_part); \
    }while(0)
#define mbytes_write_int_little_endian_8b(_buf, _i64_value) \
    do{\
        unsigned high_part = (((mbytes_t_i64)_i64_value)>> 32), low_part = ((mbytes_t_i64)_i64_value) & 0xffffffff; \
        mbytes_write_int_little_endian_4b(_buf, low_part); \
        mbytes_write_int_little_endian_4b(&((char*)_buf)[4], high_part); \
    }while(0)
    


#if defined(__cplusplus)
}
#endif

#endif /* !defined(__mbytes_h__) */
