#ifndef _API_H_
#define _API_H_

//Header file which does generic Get/Set API.
#define API_DECLARE_GET(sname,sfield,sfieldtype) \
sfieldtype sname##_get_##sfield ( sname * _ptr )

#define API_DECLARE_SET(sname,sfield,sfieldtype) \
void sname##_set_##sfield ( sname * _ptr, sfieldtype _value );

#define API_DEFINE_GET(sname,sfield,sfieldtype) \
sfieldtype sname##_get_##sfield ( sname * _ptr ) { \
        if(_ptr) return _ptr->sfield; { sfieldtype a = {0}; return a; } \
    }

#define API_DEFINE_SET(sname,sfield,sfieldtype) \
void sname##_set_##sfield ( sname * _ptr, sfieldtype _value ) { \
        if(_ptr) _ptr->sfield = _value; \
}

#define API_DECLARE_GET_SET(sname,sfield,sfieldtype) \
API_DECLARE_GET(sname,sfield,sfieldtype); \
API_DECLARE_SET(sname,sfield,sfieldtype)

#define API_DEFINE_GET_SET(sname,sfield,sfieldtype) \
API_DEFINE_GET(sname,sfield,sfieldtype); \
API_DEFINE_SET(sname,sfield,sfieldtype)

#endif //_API_H_
