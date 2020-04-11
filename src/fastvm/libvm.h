
#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __libvm_h__
#define __libvm_h__

#define vm_error_noabort    VM_SET_STATE(_vm_error_noabort)
#define vm_error            VM_SET_STATE(_vm_error)
#define vm_warning          VM_SET_STATE(_vm_warning)


#endif

#if defined(__cplusplus)
}
#endif