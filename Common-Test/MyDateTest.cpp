#include "pch.h"

#include "../../common/MyDate.h"

TEST(MyDateTest, ParseDateWithFormat) {
  int err = 0;
  MyDate date;
  err = date.Parse("MM/dd/yyyy", "05/29/2015");
  EXPECT_EQ(err, 0);
  EXPECT_EQ(date.Year(), 2015);
  EXPECT_EQ(date.Month(), 5);
  EXPECT_EQ(date.Day(), 29);
}
TEST(MyDateTest, ParseDateWithFormat1) {
  int err = 0;
  MyDate date;
  err = date.Parse("MM/dd/yyyy HH:mm", "05/29/2015 05:50");
  EXPECT_EQ(err, 0);
  EXPECT_EQ(date.Year(), 2015);
  EXPECT_EQ(date.Month(), 5);
  EXPECT_EQ(date.Day(), 29);
  EXPECT_EQ(date.Hour(), 5);
  EXPECT_EQ(date.Minute(), 50);

}
TEST(MyDateTest, ParseDateWithFormat2) {
  int err = 0;
  MyDate date;
  err = date.Parse("MM/dd/yyyy hh:mm tt", "05/29/2015 05:50 AM");
  EXPECT_EQ(err, 0);
  EXPECT_EQ(date.Year(), 2015);
  EXPECT_EQ(date.Month(), 5);
  EXPECT_EQ(date.Day(), 29);
  EXPECT_EQ(date.Hour(), 5);
  EXPECT_EQ(date.Minute(), 50);
}

TEST(MyDateTest, ParseDateWithFormat3) {
  int err = 0;
  MyDate date;
  err = date.Parse("MM/dd/yyyy H:mm", "05/29/2015 5:50");
  EXPECT_EQ(err, 0);
  EXPECT_EQ(date.Year(), 2015);
  EXPECT_EQ(date.Month(), 5);
  EXPECT_EQ(date.Day(), 29);
  EXPECT_EQ(date.Hour(), 5);
  EXPECT_EQ(date.Minute(), 50);
}

TEST(MyDateTest, ParseDateWithFormat4) {
  int err = 0;
  MyDate date;
  err = date.Parse("MM/dd/yyyy HH:mm:ss", "05/29/2015 05:50:06");
  EXPECT_EQ(err, 0);
  EXPECT_EQ(date.Year(), 2015);
  EXPECT_EQ(date.Month(), 5);
  EXPECT_EQ(date.Day(), 29);
  EXPECT_EQ(date.Hour(), 5);
  EXPECT_EQ(date.Minute(), 50);
  EXPECT_EQ(date.Second(), 6);
}

TEST(MyDateTest, ParseDateWithFormat5) {
  int err = 0;
  MyDate date;
  err = date.Parse("yyyy-MM-ddTHH:mm:ss.ffffff", "2015-05-16T05:50:06.719922");
  EXPECT_EQ(err, 0);
  EXPECT_EQ(date.Year(), 2015);
  EXPECT_EQ(date.Month(), 5);
  EXPECT_EQ(date.Day(), 16);
  EXPECT_EQ(date.Hour(), 5);
  EXPECT_EQ(date.Minute(), 50);
  EXPECT_EQ(date.Second(), 6);
  EXPECT_EQ(date.MS(), 719922);
}