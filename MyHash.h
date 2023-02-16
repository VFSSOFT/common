#ifndef MY_HASH_H_
#define MY_HASH_H_

#define MYHASH_ALG_SHA256 1
#define MYHASH_ALG_SHA1   2

class MyHash {
public:
  MyHash() {}
  virtual ~MyHash() {}

  static int CalcHash(int hashAlg, const char* data, int dataLen, unsigned char** out, unsigned int* outLen);

};

#endif // MY_HASH_H_