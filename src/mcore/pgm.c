
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print_util.h"
#include "pgm.h"

struct pgm_mod *pgm_create(struct pgm_create_param *param)
{
#undef  func_format_s
#undef  func_format
#define func_format_s    "pgm_create(%p{filename:%s,widht:%d,height:%d}) "
#define func_format()    param, param?param->filename:NULL, param?param->width:0, param?param->height:0
    struct pgm_mod *mod = NULL;

    if (NULL == param)
    {
        print_err("err: " func_format_s " failed with invalid param(). %s:%d\r\n",
          func_format(), __FILE__, __LINE__);
        return NULL;
    }
    
    mod = (struct pgm_mod *)calloc(1, sizeof (mod[0]) + param->width * param->height * param->point_size * param->point_size);
    if (NULL == mod)
    {
        print_err("err: " func_format_s " failed with invalid param(). %s:%d\r\n",
          func_format(), __FILE__, __LINE__);
        return NULL;
    }

    mod->buf = ((char *)mod + sizeof (mod[0]));

    strcpy(mod->filename, param->filename);

    mod->width = param->width;
    mod->height = param->height;
    mod->flip = param->flip;
    mod->vflip = param->vflip;
    mod->point_size = param->point_size;
    if (!mod->point_size)
        mod->point_size = 1;

    return mod;
}

int pgm_destroy(struct pgm_mod *mod)
{
    if (mod)
        free(mod);

    return 0;
}

int pgm_set(struct pgm_mod *mod, int x, int y, int val, int size)
{
    int i, j;
    int start_pos;

    int new_y = y, new_x = x;

    if (x < 0 || (x >= mod->width) || y < 0 || (y >= mod->height))
        return 0;

    if (mod->flip)
    {
        new_x = mod->width - new_x - 1;
    }

    if (mod->vflip)
    {
        new_y = mod->height - new_y - 1;
    }

    int base_pos = new_y * mod->point_size * mod->width * mod->point_size + new_x * mod->point_size;

    for (i = 0; i < size; i++)
    {
        start_pos = base_pos + i * mod->width * mod->point_size;

        for (j = 0; j < size; j++)
        {
            mod->buf[start_pos + j] = val;
        }
    }

    return 0;
}

int pgm_save(struct pgm_mod *mod, const char *filename)
{
    FILE *fp = fopen(filename?filename:mod->filename, "wb");
    fprintf(fp, "P5\n \n%d %d\n%d\n", mod->width * mod->point_size, mod->height * mod->point_size, 255);
    fwrite(mod->buf, 1, mod->width * mod->height * mod->point_size * mod->point_size, fp);
    fclose(fp);

    return 0;
}
