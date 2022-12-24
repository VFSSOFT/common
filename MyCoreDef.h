#ifndef MY_CORE_DEF_H_
#define MY_CORE_DEF_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <stdarg.h>

#ifdef _WIN32
#define WIN32
#pragma warning(disable: 4267)
#pragma warning(disable: 4996)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef UCHAR
#define UCHAR short
#endif

#ifndef INT64
#define INT64 long long
#endif

#ifndef UINT64
#define UINT64 unsigned long long
#endif 

// Program Utilities MACROS
#define PARAM_IN 
#define PARAM_OUT
#define PARAM_INOUT
#define PARAM_READONLY
#define PARAM_NULLABLE
#define PARAM_NOTNULL

#define PRIVATE_FUNCTION

#define DECLARE_ERRCODE int errCode = 0;
#define CHECK_ERRCODE(x) if ((errCode = (x))) return errCode;
#define CHECK_ERRCODE_GOTO(x, label) if (errCode = (x)) goto label;

#define STR_LEN(x) (((x) == NULL || *(x) == 0) ? 0 : strlen((x)))
#define STR_IS_EMPTY(x) ((x) == NULL || *(x) == 0)
#define STR_EQUALS(s1, s2) (strcmp(s1, s2) == 0)
#define STR_EQUALS_INGORE_CASE(s1, s2) (stricmp(s1, s2) == 0)

#define SET_FLAG(x, f, onOff) ((x) = (onOff) ? ((x) | (f)) : ((x) &~(f)))
#define IS_FLAG_ON(x, f) (((x) & (f)) != 0)

#define BOOL_STR(x) ((x) ? "True" : "False")

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)


#define MY_LAST_ERROR_DECL                                                 \
  public:                                                                  \
    inline int LastErrorCode() { return m_LastErrorCode; }                 \
    inline MyStringA* LastErrorMessage() { return &m_LastErrorMessage;}    \
                                                                           \
  protected:                                                               \
    inline int LastError(int errCode, const char* errMsg) {                \
      m_LastErrorCode = errCode;                                           \
      m_LastErrorMessage.Set(errMsg);                                      \
      return m_LastErrorCode;                                              \
    }                                                                      \
                                                                           \
    inline int LastError(int errCode, MyStringA* errMsg) {                 \
      m_LastErrorCode = errCode;                                           \
      m_LastErrorMessage.Set(errMsg->Deref());                             \
      return m_LastErrorCode;                                              \
    }                                                                      \
                                                                           \
  private:                                                                 \
    int       m_LastErrorCode;                                             \
    MyStringA m_LastErrorMessage;                                          

#define INIT_LAST_ERROR          \
    m_LastErrorCode = 0;         \
    m_LastErrorMessage.Reset();  \
                                 \


#define MY_ERR_BASE                   0x08000000
#define MY_ERR_OUT_OF_MEMORY          (MY_ERR_BASE + 1)
#define MY_ERR_INVALID_PARAMETERS     (MY_ERR_BASE + 2)
#define MY_ERR_INDEX_OUT_OF_BOUNDARY  (MY_ERR_BASE + 3)
#define MY_ERR_FATAL_ERROR            (MY_ERR_BASE + 4)
#define MY_ERR_OP_TIMEOUT             (MY_ERR_BASE + 5)
#define MY_ERR_NOT_ENOUGH_DATA        (MY_ERR_BASE + 6)
#define MY_ERR_INVALID_OPERATION      (MY_ERR_BASE + 7)
#define MY_ERR_SOCKET_EOF             (MY_ERR_BASE + 8)


#define MY_ERR_BASE_FILE_PATH         (MY_ERR_BASE + 0x07000000)
#define MY_ERR_FILE_PATH_NO_PARENT    (MY_ERR_BASE_FILE_PATH + 1)

#define MY_ERR_BASE_NETWORK           (MY_ERR_BASE + 0x06000000)
#define MY_ERR_NETWORK_SYS_ERR        (MY_ERR_BASE_NETWORK + 1)
#define MY_ERR_NETWORK_DNS_ERR        (MY_ERR_BASE_NETWORK + 2)
#define MY_ERR_NETWORK_SSL_ERR        (MY_ERR_BASE_NETWORK + 3)
#define MY_ERR_HTTP_UNKNOWN_ENCODING  (MY_ERR_BASE_NETWORK + 4)
#define MY_ERR_INVALID_URL            (MY_ERR_BASE_NETWORK + 5)
#define MY_ERR_CONNECT_TIMEOUT        (MY_ERR_BASE_NETWORK + 6)
#define MY_ERR_HTTP_INVALID_RESPONSE  (MY_ERR_BASE_NETWORK + 7)
#define MY_ERR_HTTP_INVALID_URL       (MY_ERR_BASE_NETWORK + 8)
#define MY_ERR_HTTP_CHUNK_ENCODING    (MY_ERR_BASE_NETWORK + 9)
#define MY_ERR_HTTP_COMPRESSION       (MY_ERR_BASE_NETWORK + 10)
#define MY_ERR_HTTP_TOO_MANY_REDIRECT (MY_ERR_BASE_NETWORK + 11)


#define MY_ERR_BASE_CRYPTO             (MY_ERR_BASE + 0x05000000)
#define MY_ERR_CRYPTO_INVALID_HASH_ALG (MY_ERR_BASE_CRYPTO + 1)
#define MY_ERR_CRYPTO_INVALID_HMAC_ALG (MY_ERR_BASE_CRYPTO + 2)
#define MY_ERR_CRYPTO_ERROR            (MY_ERR_BASE_CRYPTO + 3)

#define MY_ERR_BASE_ENCODING           (MY_ERR_BASE + 0x04000000)
#define MY_ERR_ENCODING_XML            (MY_ERR_BASE_ENCODING + 1)
#define MY_ERR_ENCODING_JSON           (MY_ERR_BASE_ENCODING + 2)

#define MY_ERR_BASE_FILE               (MY_ERR_BASE + 0x03000000)
#define MY_ERR_FILE_DIR_NOT_EMPTY      (MY_ERR_BASE_FILE + 3)
#define MY_ERR_FILE_NOT_FOUND          (MY_ERR_BASE_FILE + 4)
#define MY_ERR_FILE_INVALID_PATH       (MY_ERR_BASE_FILE + 5)
#define MY_ERR_FILE_ALREADY_EXIST      (MY_ERR_BASE_FILE + 6)


#if _WIN32

#define MY_INVALID_SOCKET INVALID_SOCKET

#else

#define MY_INVALID_SOCKET -1

#endif

#endif // MY_CORE_DEF_H_
