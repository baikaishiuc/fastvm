
#ifndef __mheap_h__
#define __mheap_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

	
	struct mheap {
		int(*compare_func)(void *ref, const void *lhs, const void *rhs);
		int len;
		int size;
		void *ref;
		void **ptab;
	};

	/* 这个mheap实现不会自动调整大小，自己看着办吧 */
	struct mheap*	mheap_new(int size, void *ref, int (* compare_func)(void *ref, const void *lhs, const void *rhs));
	void			mheap_delete(struct mheap *);

	void*			mheap_min(struct mheap *);
	void*			mheap_delmin(struct mheap *);
	int				mheap_insert(struct mheap *, void *elm);
	int				mheap_isempty(struct mheap *h);
	void			mheap_deckey(struct mheap *h, void *key);
	int				mheap_sort(struct mheap *h);


#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif

