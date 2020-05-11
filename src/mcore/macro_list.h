/*!
\file       macro_list.h
\brief      macro list unit, list operation base macro

 ----history----
\author		zhengxianwei 
\date       2008-09-03
\version    0.01
\desc       create


$Author: zhengxianwei $
$Id: macro_list.h,v 1.4 2008-09-04 02:55:07 zhengxianwei Exp $
*/
#if !defined(__macro_list_h__)
#define __macro_list_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

/*
head should defined as
struct
{
	unsigned long   counts;
	struct aaa	*list;
}xxx_head;
node should defined as
struct
{
	struct bbb	*owner;
	struct aaa	*next;
	struct aaa	*prev;
}xxx_node;
*/

/* add item to last
have copy to mparams.c, if changed plz change it*/
#define mlist_add(head, item, node)\
do \
{\
    if((head).list)\
    {\
        (item)->node.prev = ((item)->node.next = (head).list)->node.prev;\
        (head).list->node.prev = ((head).list->node.prev->node.next = (item));\
    }\
    else\
    {\
        (head).list = ((item)->node.prev = ((item)->node.next = (item)));\
    }\
    ++(head).counts;\
} while (0);

#define mlist_add_to_first(head, item, node)\
do \
{\
    if((head).list)\
    {\
        (item)->node.prev = ((item)->node.next = (head).list)->node.prev;\
        (head).list->node.prev = ((head).list->node.prev->node.next = (item));\
        (head).list = (item);\
    }\
    else\
    {\
        (head).list = ((item)->node.prev = ((item)->node.next = (item)));\
    }\
    ++(head).counts;\
} while (0);

#define mlist_add_after(head, item, node, prev_item)\
do \
{\
    (item)->node.prev =  prev_item ;\
    (item)->node.next = (prev_item)->node.next;\
    (prev_item)->node.next->node.prev = item;\
    (prev_item)->node.next = item;\
    ++(head).counts;\
} while (0);

/* don't input item as head.list 
have copy to mparams.c, if changed plz change it */
#define mlist_del(head, item, node)\
do \
{\
    if( (item)->node.prev != NULL && (item)->node.next != NULL ) \
    {\
        if((head).list == (item))\
        {\
            (head).list = ((item)->node.next == (item))?0:(item)->node.next;\
        }\
        (item)->node.prev->node.next = (item)->node.next;\
        (item)->node.next->node.prev = (item)->node.prev;\
        (item)->node.prev = NULL; \
        (item)->node.next = NULL; \
        --(head).counts;\
    }\
} while (0);

#define mlist_move_to_last(head, item, node)\
do \
{\
    if((item) != (head).list->node.prev)\
    {\
        if((head).list == (item))\
        {\
            (head).list = (item)->node.next;\
        }\
        else\
        {\
            (item)->node.prev->node.next = (item)->node.next;\
            (item)->node.next->node.prev = (item)->node.prev;\
            (item)->node.prev = ((item)->node.next = (head).list)->node.prev;\
            (head).list->node.prev = ((head).list->node.prev->node.next = (item));\
        }\
    }\
} while (0);

#define mlist_move_to_first(head, item, node)\
do \
{\
    if((item) != (head).list)\
    {\
        if((head).list->node.prev == (item))\
        {\
            (head).list = (item);\
        }\
        else\
        {\
            (item)->node.prev->node.next = (item)->node.next;\
            (item)->node.next->node.prev = (item)->node.prev;\
            (item)->node.prev = ((item)->node.next = (head).list)->node.prev;\
            (head).list->node.prev = ((head).list->node.prev->node.next = (item));\
            (head).list = (item);\
        }\
    }\
} while (0);

#define mlist_for_each(head, item, node, i) \
    for(i = 0, item = (head).list; i < (head).counts; i++, item = item->node.next)

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */
#endif /* !defined(__macro_list_h__) */
