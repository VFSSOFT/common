#ifndef _MY_FILE_ATTRIBUTES_H_
#define _MY_FILE_ATTRIBUTES_H_

#include "MyCoreDef.h"

#ifdef _WIN32

#define MY_FILE_ATTRIBUTES_HAS_ATTR(attrs, attr) (((attrs) & attr) != 0)
#define MY_FILE_ATTRIBUTES_SET_ATTR(attrs, onoff, attr) {                          \
                                                          (attrs) = (onoff)        \
                                                            ? (attrs) | attr       \
                                                            : (attrs & (~(attr))); \
                                                        }


class MyFileAttributes {
public:
  MyFileAttributes(UINT64 attr) {
    m_Attributes = attr;
  }

  inline UINT64 RawAttributes() { return m_Attributes; }
  inline void SetRawAttributes(UINT64 attrs) { m_Attributes = attrs; }

  inline BOOL IsArchive() {  return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_ARCHIVE); }
  inline void SetArchive(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_ARCHIVE); }
  
  inline BOOL IsCompressed() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_COMPRESSED); }
  inline void SetCompressed(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_COMPRESSED); }

  inline BOOL IsDevice() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_DEVICE); }
  inline void SetDevice(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_DEVICE); }

  inline BOOL IsEncrypted() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_ENCRYPTED); }
  inline void SetEncrypted(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_ENCRYPTED); }

  inline BOOL IsHidden() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_HIDDEN); }
  inline void SetHidden(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_HIDDEN); }

  inline BOOL IsNormal() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_NORMAL); }
  inline void SetNormal(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_NORMAL); }

  inline BOOL IsReadonly() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_READONLY); }
  inline void SetReadonly(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_READONLY); }

  inline BOOL IsReparsePoint() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_REPARSE_POINT); }
  inline void SetReparsePoint(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_REPARSE_POINT); }

  inline BOOL IsSparseFile() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_SPARSE_FILE); }
  inline void SetSparseFile(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_SPARSE_FILE); }

  inline BOOL IsSystem() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_SYSTEM); }
  inline void SetSystem(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_SYSTEM); }

  inline BOOL IsTemporary() { return MY_FILE_ATTRIBUTES_HAS_ATTR(m_Attributes, FILE_ATTRIBUTE_TEMPORARY); }
  inline void SetTemporary(BOOL val) { MY_FILE_ATTRIBUTES_SET_ATTR(m_Attributes, val, FILE_ATTRIBUTE_TEMPORARY); }

private:
  UINT64 m_Attributes;
};
#endif // _WIN32

#endif // _MY_FILE_ATTRIBUTES_H_