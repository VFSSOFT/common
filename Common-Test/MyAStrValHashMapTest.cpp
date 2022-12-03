#include "pch.h"

#include "../MyAStrValHashMap.h"

TEST(MyAStrValHashMapTest, OneEntryTest) {
    MyAStrValHashMap<int> map;
    int val = 0;
    MyStringA key;
    key.Set("RANDOM_VALUE");
    bool exist = map.Get(&key, &val);
    EXPECT_FALSE(exist);

    val = 123;
    map.Put(&key, val);

    val = 0;
    exist = map.Get(&key, &val);
    EXPECT_TRUE(exist);
    EXPECT_EQ(val, 123);
}

TEST(MyAStrValHashMapTest, ThousandEntriesTest) {
    MyAStrValHashMap<int> map;
    bool exist = false;
    int val = 0;

    for (int i = 1; i <= 1024; i++) {
        MyStringA key;
        key.Set("ThousandEntriesTest_");
        key.AppendInt(i);
        map.Put(&key, i * 1024);

        val = 0;
        exist = map.Get(&key, &val);
        EXPECT_TRUE(exist);
        EXPECT_EQ(val, i * 1024);
    }
  
    for (int i = 1024; i > 0; i--) {
        MyStringA key;
        key.Set("ThousandEntriesTest_");
        key.AppendInt(i);

        val = 0;
        exist = map.Get(&key, &val);
        EXPECT_TRUE(exist);
        EXPECT_EQ(val, i * 1024);
    }
}

TEST(MyAStrValHashMapTest, AddDeleteTest) {
    MyAStrValHashMap<int> map;
    bool exist = false;
    int val = 0;

    for (int i = 1; i <= 1024; i++) {
        MyStringA key;
        key.Set("AddDeleteTest_");
        key.AppendInt(i);
        map.Put(&key, i * 1024);

        val = 0;
        exist = map.Get(&key, &val);

        EXPECT_TRUE(exist);
        EXPECT_EQ(val, i * 1024);

        if (i % 8 == 0) {
            bool suc = map.Delete(&key);
            EXPECT_TRUE(suc);
            val = 0;
            exist = map.Get(&key, &val);
            EXPECT_FALSE(exist);
        }
    }
  
    for (int i = 1024; i > 0; i--) {
        MyStringA key;
        key.Set("AddDeleteTest_");
        key.AppendInt(i);

        val = 0;
        exist = map.Get(&key, &val);
        if (i % 8 == 0) {
            EXPECT_FALSE(exist);
        } else {
            EXPECT_TRUE(exist);
            EXPECT_EQ(val, i * 1024);
        }
    }
}



TEST(MyAStrValHashMapTest, ABCDEFGTest) {
    MyAStrValHashMap<int> map;
    bool exist = false;
    int val = 0;

    // ascii: 33 - 127
    for (char c = '!'; c <= '|'; c++) {
        MyStringA key;
        key.AppendChar(c);
        map.Put(&key, c);
    }
    
    for (char c = '!'; c <= '|'; c++) {
        MyStringA key;
        key.AppendChar(c);

        val = 0;
        exist = map.Get(&key, &val);
        EXPECT_TRUE(exist);
        EXPECT_EQ(val, c);
    }

}