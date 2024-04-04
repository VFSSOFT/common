#include "pch.h"


#include "../MyWin.h"

TEST(MyWinTest, DeafultIconsTest_Load) {
    int err = 0;
    MyWinDeafultIcons defaultIcons;

    err = defaultIcons.Load();
    ASSERT_EQ(err, 0);

    MyArray<MyStringA>* extensions= defaultIcons.Extensions();
    MyArray<MyStringA>* iconPaths = defaultIcons.IconPaths();
    ASSERT_TRUE(extensions->Size() > 100);
    for (int i = 0; i < extensions->Size(); i++) {
        char log[4096];
        sprintf(log, "[%d] Ext=\"%s\", Path=\"%s\"\n", i, extensions->Get(i)->Deref(), iconPaths->Get(i)->Deref());
        OutputDebugStringA(log);
    }
}


TEST(MyWinTest, DeafultIconsTest_GetIcon_TxtFile) {
    int err = 0;
    MyWinDeafultIcons defaultIcons;

    err = defaultIcons.Load();
    ASSERT_EQ(err, 0);

    HICON hicon = defaultIcons.GetIcon(".txt");
    ASSERT_TRUE(hicon != NULL);
}

TEST(MyWinTest, DeafultIconsTest_GetIcon_Folder) {
    int err = 0;
    MyWinDeafultIcons defaultIcons;

    err = defaultIcons.Load();
    ASSERT_EQ(err, 0);

    HICON hicon = defaultIcons.GetFolderIcon();
    ASSERT_TRUE(hicon != NULL);
}

TEST(MyWinTest, DeafultIconsTest_GetIcon_Unknown) {
    int err = 0;
    MyWinDeafultIcons defaultIcons;

    err = defaultIcons.Load();
    ASSERT_EQ(err, 0);

    HICON hicon = defaultIcons.GetUnknownIcon();
    ASSERT_TRUE(hicon != NULL);
}

TEST(MyWinTest, DeafultIconsTest_GetAllIcon) {
    int err = 0;
    MyWinDeafultIcons defaultIcons;

    err = defaultIcons.Load();
    ASSERT_EQ(err, 0);

    MyArray<MyStringA>* extensions= defaultIcons.Extensions();
    ASSERT_TRUE(extensions->Size() > 100);
    for (int i = 0; i < extensions->Size(); i++) {
        HICON hicon = defaultIcons.GetIcon(extensions->Get(i)->DerefConst());
        //ASSERT_TRUE(hicon != NULL);
    }
}