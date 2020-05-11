#ifdef __cplusplus
extern "C" {
#endif

#ifndef __pgm_h__
#define __pgm_h__

    struct pgm_mod;

    struct pgm_create_param
    {
        char filename[64];
        int width;
        int height;
        int flip;
        int vflip;
        int point_size;
    };

struct pgm_mod
{
    char filename[64];
    int width;
    int height;
    int flip;
    int vflip;
    int point_size;
    char *buf;
};


    struct pgm_mod *pgm_create(struct pgm_create_param *param);
    int pgm_destroy(struct pgm_mod *mod);
    int pgm_set(struct pgm_mod *mod, int x, int y, int val, int size);
    int pgm_save(struct pgm_mod *mod, const char *filename);

#endif

#ifdef __cplusplus
}
#endif