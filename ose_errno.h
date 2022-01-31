#ifndef OSE_ERRNO
#define OSE_ERRNO

#ifdef __cplusplus
extern "C" {
#endif

enum ose_errno
{
    OSE_ERR_NONE = 0,
    OSE_ERR_ELEM_TYPE,
    OSE_ERR_ITEM_TYPE,
    OSE_ERR_ELEM_COUNT,
    OSE_ERR_ITEM_COUNT,
    OSE_ERR_RANGE,
    OSE_ERR_UNKNOWN_TYPETAG,
};

#define ose_errno_set(b, e) ose_context_set_status(b, e)
#define ose_errno_get(b) ose_context_get_status(b)

#ifdef __cplusplus
}
#endif

#endif
