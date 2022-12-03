#include "pch.h"

#include "../../common/MyDataPacket.h"

TEST(MyDataPacketTest, CommonTest) {
  MyDataPacket pkt;

  EXPECT_EQ(pkt.Available(), 0);

  EXPECT_EQ(pkt.HasLine(), false);
}

TEST(MyDataPacketTest, ReadLinesTest) {
  MyDataPacket pkt;

  const char* data = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nUser-Agent: VFSSOFT/1.0\r\nContent-Length: 0\r\n";
  int dataLen = strlen(data);
  pkt.Write(data, dataLen);

  EXPECT_EQ(pkt.Available(), dataLen);

  EXPECT_EQ(pkt.HasLine(), true);
  const char* ptr = pkt.ReadLine();
  EXPECT_STREQ(ptr, "GET / HTTP/1.1");

  EXPECT_EQ(pkt.HasLine(), true);
  ptr = pkt.ReadLine();
  EXPECT_STREQ(ptr, "Host: www.baidu.com");

  EXPECT_EQ(pkt.HasLine(), true);
  ptr = pkt.ReadLine();
  EXPECT_STREQ(ptr, "User-Agent: VFSSOFT/1.0");

  EXPECT_EQ(pkt.HasLine(), true);
  ptr = pkt.ReadLine();
  EXPECT_STREQ(ptr, "Content-Length: 0");

  EXPECT_EQ(pkt.HasLine(), false);
  ptr = pkt.ReadLine();
  EXPECT_STREQ(ptr, "");
}

TEST(MyDataPacketTest, ReadBytesTest) {
  MyDataPacket pkt;

  const char* data = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nUser-Agent: VFSSOFT/1.0\r\nContent-Length: 0\r\n";
  int dataLen = strlen(data);
  pkt.Write(data, dataLen);

  EXPECT_EQ(pkt.Available(), dataLen);

  EXPECT_EQ(pkt.HasLine(), true);
  const char* ptr = pkt.ReadLine();
  EXPECT_STREQ(ptr, "GET / HTTP/1.1");

  MyBuffer buf;
  pkt.ReadBytes(5, &buf);
  EXPECT_STREQ("Host:", buf.Deref());
}

TEST(MyDataPacketTest, Hex2DecimalTest) {
  const char* data = "E0E0";
  UINT64 num = MyDataPacket::Hex2Decimal(data, -1);
  EXPECT_EQ(0xe0e0, num);

  data = "12345";
  num = MyDataPacket::Hex2Decimal(data, -1);
  EXPECT_EQ(0x12345, num);

  data = "ff";
  num = MyDataPacket::Hex2Decimal(data, -1);
  EXPECT_EQ(0xff, num);
}

