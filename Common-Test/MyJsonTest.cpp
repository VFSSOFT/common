#include "pch.h"

#include "../../common/MyJson.h"

TEST(MyJsonTest, EmptyObjectTest) {
  const char* jsonStr = "{}";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::objectValue);
  EXPECT_TRUE(json.Root()->ObjectValue()->Values.Size() == 0);
}

TEST(MyJsonTest, EmptyArrayTest) {
  const char* jsonStr = "[]";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::arrayValue);
  EXPECT_TRUE(json.Root()->ArrayValue()->Values.Size() == 0);
}

TEST(MyJsonTest, SingleTrueInputTest) {
  const char* jsonStr = "true";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::boolValue);
  EXPECT_TRUE(json.Root()->BoolValue() == true);
}

TEST(MyJsonTest, SingleFalseInputTest) {
  const char* jsonStr = "False";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::boolValue);
  EXPECT_TRUE(json.Root()->BoolValue() == false);
}

TEST(MyJsonTest, EmptyStringInputTest) {
  const char* jsonStr = "\"\"";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(json.Root()->StringValue()->Deref(), NULL);
}

TEST(MyJsonTest, SingleStringInputTest) {
  const char* jsonStr = "\"this is a single string\"";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(json.Root()->StringValue()->Deref(), "this is a single string");
}

TEST(MyJsonTest, SingleNumberInputTest) {
  const char* jsonStr = "-123.456";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(json.Root()->NumberValue()->Deref(), "-123.456");
}

TEST(MyJsonTest, SingleNullInputTest) {
  const char* jsonStr = "null";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
  EXPECT_TRUE(json.Root()->ValueType() == MyJsonValueType::nullValue);
}

TEST(MyJsonTest, SimpleObjectTest) {
  const char* jsonStr = 
    "{\r\n"
    "  \"name\": \"john\",\r\n"
    "  \"age\": 30,\r\n"
    "  \"car\": null\r\n"
    "}\r\n";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);

  MyJsonValue* root = json.Root();
  EXPECT_TRUE(root->ValueType() == MyJsonValueType::objectValue);

  MyJsonObject* obj = root->ObjectValue();
  EXPECT_EQ(obj->Values.Size(), 3);

  MyJsonValue* val = obj->Values.Get(0);
  EXPECT_STREQ(val->Key()->Deref(), "name");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(val->StringValue()->Deref(), "john");

  val = obj->Values.Get(1);
  EXPECT_STREQ(val->Key()->Deref(), "age");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(val->NumberValue()->Deref(), "30");

  val = obj->Values.Get(2);
  EXPECT_STREQ(val->Key()->Deref(), "car");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::nullValue);
}

TEST(MyJsonTest, SimpleArrayTest) {
    const char* jsonStr = 
    "[\r\n"
    "  \"name\",\r\n"
    "  \"age\", \r\n"
    "  \"car\",   \r\n"
    "]";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);

  MyJsonValue* root = json.Root();
  EXPECT_TRUE(root->ValueType() == MyJsonValueType::arrayValue);

  MyJsonArray* arr = root->ArrayValue();
  EXPECT_EQ(arr->Values.Size(), 3);

  MyJsonValue* val = arr->Values.Get(0);
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(val->StringValue()->Deref(), "name");

  val = arr->Values.Get(1);
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(val->StringValue()->Deref(), "age");

  val = arr->Values.Get(2);
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(val->StringValue()->Deref(), "car");
}

TEST(MyJsonTest, Json2Test) {
  const char* jsonStr = 
    "{\r\n"
    "  \"name\": \"john\",\r\n"
    "  \"age\": 30,\r\n"
    "  \"car\": null\r\n"
    "}\r\n";
  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);

  MyJsonValue* root = json.Root();
  EXPECT_TRUE(root->ValueType() == MyJsonValueType::objectValue);

  MyJsonObject* obj = root->ObjectValue();
  EXPECT_EQ(obj->Values.Size(), 3);

  MyJsonValue* val = obj->Values.Get(0);
  EXPECT_STREQ(val->Key()->Deref(), "name");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::stringValue);
  EXPECT_STREQ(val->StringValue()->Deref(), "john");

  val = obj->Values.Get(1);
  EXPECT_STREQ(val->Key()->Deref(), "age");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(val->NumberValue()->Deref(), "30");

  val = obj->Values.Get(2);
  EXPECT_STREQ(val->Key()->Deref(), "car");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::nullValue);
}

TEST(MyJsonTest, Json3Test) {
    const char* jsonStr = 
    "{\r\n"
    "    \"glossary\": {\r\n"
    "        \"title\": \"example glossary\",\r\n"
		"                  \"GlossDiv\": {\r\n"
    "        \"title\": \"S\",\r\n"
		"	                       \"GlossList\": {\r\n"
    "            \"GlossEntry\": {\r\n"
    "                \"ID\": \"SGML\",\r\n"
		"			                                \"SortAs\": \"SGML\",\r\n"
		"			                                \"GlossTerm\": \"Standard Generalized Markup Language\",\r\n"
		"			                                \"Acronym\": \"SGML\",\r\n"
		"			                                \"Abbrev\": \"ISO 8879:1986\",\r\n"
		"			                                \"GlossDef\": {\r\n"
    "                    \"para\": \"A meta-markup language, used to create markup languages such as DocBook.\",\r\n"
		"				                                  \"GlossSeeAlso\": [\"GML\", \"XML\"]\r\n"
    "                },\r\n"
		"			                              \"GlossSee\": \"markup\"\r\n"
    "            }\r\n"
    "        }\r\n"
    "    }\r\n"
    "}\r\n"
       "}";

  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);

  MyJsonValue* val = json.Root();
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::objectValue);
  MyJsonObject* obj = val->ObjectValue();
  EXPECT_EQ(obj->Values.Size(), 1);
  val = obj->Values.Get(0);
  EXPECT_STREQ(val->Key()->Deref(),"glossary");
  EXPECT_TRUE(val->ValueType() == MyJsonValueType::objectValue);
  
  obj = val->ObjectValue();
  EXPECT_EQ(obj->Values.Size(), 2);

  val = obj->Values.Get(0);
  EXPECT_STREQ(val->Key()->Deref(), "title");
  EXPECT_STREQ(val->StringValue()->Deref(), "example glossary");
  val = obj->Values.Get(1);
  EXPECT_STREQ(val->Key()->Deref(), "GlossDiv");


  MyJsonValue* glossListValue = val->FindChildByName("GlossList");
  EXPECT_TRUE(glossListValue != NULL);

  MyJsonValue* glossEntryValue = glossListValue->FindChildByName("GlossEntry");
  EXPECT_TRUE(glossEntryValue != NULL);
  EXPECT_TRUE(glossEntryValue->ValueType() == MyJsonValueType::objectValue);
  
  obj = glossEntryValue->ObjectValue();
  EXPECT_EQ(obj->Values.Size(), 7);
  EXPECT_STREQ(obj->Values.Get(0)->Key()->Deref(), "ID");
  EXPECT_STREQ(obj->Values.Get(0)->StringValue()->Deref(), "SGML");
  EXPECT_STREQ(obj->Values.Get(1)->Key()->Deref(), "SortAs");
  EXPECT_STREQ(obj->Values.Get(1)->StringValue()->Deref(), "SGML");
  EXPECT_STREQ(obj->Values.Get(2)->Key()->Deref(), "GlossTerm");
  EXPECT_STREQ(obj->Values.Get(2)->StringValue()->Deref(), "Standard Generalized Markup Language");

  MyJsonValue* glossDefValue = glossEntryValue->FindChildByName("GlossDef");
  EXPECT_TRUE(glossDefValue != NULL);


  MyJsonValue* glossSeeAlsoValue = glossDefValue->FindChildByName("GlossSeeAlso");
  EXPECT_TRUE(glossSeeAlsoValue != NULL);
  EXPECT_TRUE(glossSeeAlsoValue->ValueType() == MyJsonValueType::arrayValue);
  MyJsonArray* arr = glossSeeAlsoValue->ArrayValue();
  EXPECT_EQ(arr->Values.Size(), 2);
  EXPECT_STREQ(arr->Values.Get(0)->StringValue()->Deref(), "GML");
  EXPECT_STREQ(arr->Values.Get(1)->StringValue()->Deref(), "XML");
}

TEST(MyJsonTest, Json4Test) {
  const char* jsonStr = 
    "{\"menu\": {\r\n"
    "\"id\": \"file\",\r\n"
    "\"value\": \"File\",\r\n"
    "\"popup\": {\r\n"
    "  \"menuitem\": [\r\n"
    "    {\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},\r\n"
    "    {\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},\r\n"
    "    {\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}\r\n"
    "  ]\r\n"
    "}\r\n"
    "}}\r\n";

  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  EXPECT_EQ(ret, 0);
}

TEST(MyJsonTest, Json5Test) {
  const char* jsonStr = 
    "{\"widget\": {\r\n"
    "    \"debug\": \"on\",\r\n"
    "    \"window\": {\r\n"
    "        \"title\": \"Sample Konfabulator Widget\",\r\n"
    "        \"name\": \"main_window\",\r\n"
    "        \"width\": 500,\r\n"
    "        \"height\": 500\r\n"
    "    },\r\n"
    "    \"image\": { \r\n"
    "        \"src\": \"Images/Sun.png\",\r\n"
    "        \"name\": \"sun1\",\r\n"
    "        \"hOffset\": 250,\r\n"
    "        \"vOffset\": 250,\r\n"
    "        \"alignment\": \"center\"\r\n"
    "    },\r\n"
    "    \"text\": {\r\n"
    "        \"data\": \"Click Here\",\r\n"
    "        \"size\": 36,\r\n"
    "        \"style\": \"bold\",\r\n"
    "        \"name\": \"text1\",\r\n"
    "        \"hOffset\": 250,\r\n"
    "        \"vOffset\": 100,\r\n"
    "        \"alignment\": \"center\",\r\n"
    "        \"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\"\r\n"
    "    }\r\n"
    "}}  \r\n";

  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();
  EXPECT_EQ(ret, 0);

  // Ensure all the numbers are parsed correctly
  MyJsonValue* width = json.Find("widget/window/width");
  EXPECT_TRUE(width->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(width->NumberValue()->Deref(), "500");

  MyJsonValue* height = json.Find("widget/window/height");
  EXPECT_TRUE(height->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(height->NumberValue()->Deref(), "500");

  MyJsonValue* hOffset = json.Find("widget/image/hOffset");
  EXPECT_TRUE(hOffset->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(hOffset->NumberValue()->Deref(), "250");

  MyJsonValue* vOffset = json.Find("widget/image/vOffset");
  EXPECT_TRUE(vOffset->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(vOffset->NumberValue()->Deref(), "250");

  hOffset = json.Find("widget/text/hOffset");
  EXPECT_TRUE(hOffset->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(hOffset->NumberValue()->Deref(), "250");

  vOffset = json.Find("widget/text/vOffset");
  EXPECT_TRUE(vOffset->ValueType() == MyJsonValueType::numberValue);
  EXPECT_STREQ(vOffset->NumberValue()->Deref(), "100");

}

TEST(MyJsonTest, Json6Test) {
  const char* jsonStr = 
    "{\"web-app\": {\r\n"
    "  \"servlet\": [ \r\n"  
    "    {\r\n"
    "      \"servlet-name\": \"cofaxCDS\",\r\n"
    "      \"servlet-class\": \"org.cofax.cds.CDSServlet\",\r\n"
    "      \"init-param\": {\r\n"
    "        \"configGlossary:installationAt\": \"Philadelphia, PA\",\r\n"
    "        \"configGlossary:adminEmail\": \"ksm@pobox.com\",\r\n"
    "        \"configGlossary:poweredBy\": \"Cofax\",\r\n"
    "        \"configGlossary:poweredByIcon\": \"/images/cofax.gif\",\r\n"
    "        \"configGlossary:staticPath\": \"/content/static\",\r\n"
    "        \"templateProcessorClass\": \"org.cofax.WysiwygTemplate\",\r\n"
    "        \"templateLoaderClass\": \"org.cofax.FilesTemplateLoader\",\r\n"
    "        \"templatePath\": \"templates\",\r\n"
    "        \"templateOverridePath\": \"\",\r\n"
    "        \"defaultListTemplate\": \"listTemplate.htm\",\r\n"
    "        \"defaultFileTemplate\": \"articleTemplate.htm\",\r\n"
    "        \"useJSP\": false,\r\n"
    "        \"jspListTemplate\": \"listTemplate.jsp\",\r\n"
    "       \"jspFileTemplate\": \"articleTemplate.jsp\",\r\n"
    "       \"cachePackageTagsTrack\": 200,\r\n"
    "       \"cachePackageTagsStore\": 200,\r\n"
    "       \"cachePackageTagsRefresh\": 60,\r\n"
    "       \"cacheTemplatesTrack\": 100,\r\n"
    "       \"cacheTemplatesStore\": 50,\r\n"
    "       \"cacheTemplatesRefresh\": 15,\r\n"
    "       \"cachePagesTrack\": 200,\r\n"
    "       \"cachePagesStore\": 100,\r\n"
    "       \"cachePagesRefresh\": 10,\r\n"
    "       \"cachePagesDirtyRead\": 10,\r\n"
    "       \"searchEngineListTemplate\": \"forSearchEnginesList.htm\",\r\n"
    "       \"searchEngineFileTemplate\": \"forSearchEngines.htm\",\r\n"
    "       \"searchEngineRobotsDb\": \"WEB-INF/robots.db\",\r\n"
    "       \"useDataStore\": true,\r\n"
    "       \"dataStoreClass\": \"org.cofax.SqlDataStore\",\r\n"
    "       \"redirectionClass\": \"org.cofax.SqlRedirection\",\r\n"
    "       \"dataStoreName\": \"cofax\",\r\n"
    "       \"dataStoreDriver\": \"com.microsoft.jdbc.sqlserver.SQLServerDriver\",\r\n"
    "       \"dataStoreUrl\": \"jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon\",\r\n"
    "       \"dataStoreUser\": \"sa\",\r\n"
    "       \"dataStorePassword\": \"dataStoreTestQuery\",\r\n"
    "       \"dataStoreTestQuery\": \"SET NOCOUNT ON;select test='test';\",\r\n"
    "       \"dataStoreLogFile\": \"/usr/local/tomcat/logs/datastore.log\",\r\n"
    "       \"dataStoreInitConns\": 10,\r\n"
    "       \"dataStoreMaxConns\": 100,\r\n"
    "       \"dataStoreConnUsageLimit\": 100,\r\n"
    "       \"dataStoreLogLevel\": \"debug\",\r\n"
    "       \"maxUrlLength\": 500}},\r\n"
    "   {\r\n"
    "     \"servlet-name\": \"cofaxEmail\",\r\n"
    "     \"servlet-class\": \"org.cofax.cds.EmailServlet\",\r\n"
    "     \"init-param\": {\r\n"
    "     \"mailHost\": \"mail1\",\r\n"
    "     \"mailHostOverride\": \"mail2\"}},\r\n"
    "   {\r\n"
    "     \"servlet-name\": \"cofaxAdmin\",\r\n"
    "     \"servlet-class\": \"org.cofax.cds.AdminServlet\"},\r\n"
    "\r\n"
    "   {\r\n"
    "     \"servlet-name\": \"fileServlet\",\r\n"
    "     \"servlet-class\": \"org.cofax.cds.FileServlet\"},\r\n"
    "   {\r\n"
    "     \"servlet-name\": \"cofaxTools\",\r\n"
    "     \"servlet-class\": \"org.cofax.cms.CofaxToolsServlet\",\r\n"
    "     \"init-param\": {\r\n"
    "       \"templatePath\": \"toolstemplates/\",\r\n"
    "       \"log\": 1,\r\n"
    "       \"logLocation\": \"/usr/local/tomcat/logs/CofaxTools.log\",\r\n"
    "       \"logMaxSize\": \"\",\r\n"
    "       \"dataLog\": 1,\r\n"
    "       \"dataLogLocation\": \"/usr/local/tomcat/logs/dataLog.log\",\r\n"
    "       \"dataLogMaxSize\": \"\",\r\n"
    "       \"removePageCache\": \"/content/admin/remove?cache=pages&id=\",\r\n"
    "       \"removeTemplateCache\": \"/content/admin/remove?cache=templates&id=\",\r\n"
    "       \"fileTransferFolder\": \"/usr/local/tomcat/webapps/content/fileTransferFolder\",\r\n"
    "       \"lookInContext\": 1,\r\n"
    "       \"adminGroupID\": 4,\r\n"
    "       \"betaServer\": true}}],\r\n"
    " \"servlet-mapping\": {\r\n"
    "   \"cofaxCDS\": \"/\",\r\n"
    "   \"cofaxEmail\": \"/cofaxutil/aemail/*\",\r\n"
    "   \"cofaxAdmin\": \"/admin/*\",\r\n"
    "   \"fileServlet\": \"/static/*\",\r\n"
    "   \"cofaxTools\": \"/tools/*\"},\r\n"
    "\r\n"
    " \"taglib\": {\r\n"
    "   \"taglib-uri\": \"cofax.tld\",\r\n"
    "   \"taglib-location\": \"/WEB-INF/tlds/cofax.tld\"}}}\r\n"
  ;

  MyJson json;
  json.JsonStr()->Set(jsonStr, strlen(jsonStr));
  int ret = json.Parse();

  printf("%s\n", json.LastErrorMessage()->Deref());

  EXPECT_EQ(ret, 0);
}

TEST(MyJsonTest, BuildEmptyObjectTest) {
  int ret = 0;
  MyJson json;
  ret = json.InitRootNode(MyJsonValueType::objectValue);
  EXPECT_EQ(ret, 0);

  MyStringA retJson;
  ret = json.ToString(&retJson, true);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), "{}");

  retJson.Reset();
  ret = json.ToString(&retJson, false);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), "{\r\n}");
}

TEST(MyJsonTest, BuildObjectTest) {
  const char* jsonStrCompact = 
    "{\"StringValue\":\"String\",\"NumberValue\":1,\"TrueValue\":true,\"NullValue\":null}";
  const char* jsonStrIndented = 
    "{\r\n"
    "  \"StringValue\":\"String\",\r\n"
    "  \"NumberValue\":1,\r\n"
    "  \"TrueValue\":true,\r\n"
    "  \"NullValue\":null\r\n"
    "}";

  int ret = 0;
  MyJson json;
  ret = json.InitRootNode(MyJsonValueType::objectValue);
  EXPECT_EQ(ret, 0);

  MyJsonValue* obj = json.Root();
  obj->AddStringChild("StringValue", "String");
  obj->AddNumberChild("NumberValue", "1");
  obj->AddBoolChild("TrueValue", true);
  obj->AddNullChild("NullValue");

  MyStringA retJson;
  ret = json.ToString(&retJson, true);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), jsonStrCompact);

  retJson.Reset();
  ret = json.ToString(&retJson, false);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), jsonStrIndented);
}

TEST(MyJsonTest, BuildArrayTest) {
  const char* jsonStrCompact = 
    "[\"A\",\"B\",true,false,null,123,\"C\"]";
  const char* jsonStrIndented = 
    "[\r\n"
    "  \"A\",\r\n"
    "  \"B\",\r\n"
    "  true,\r\n"
    "  false,\r\n"
    "  null,\r\n"
    "  123,\r\n"
    "  \"C\"\r\n"
    "]";

  int ret = 0;
  MyJson json;
  ret = json.InitRootNode(MyJsonValueType::arrayValue);
  EXPECT_EQ(ret, 0);

  MyJsonValue* arr = json.Root();
  arr->AddStringChild("", "A");
  arr->AddStringChild("", "B");
  arr->AddBoolChild("", true);
  arr->AddBoolChild("", false);
  arr->AddNullChild("");
  arr->AddNumberChild("", "123");
  arr->AddStringChild("", "C");

  MyStringA retJson;
  ret = json.ToString(&retJson, true);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), jsonStrCompact);

  retJson.Reset();
  ret = json.ToString(&retJson, false);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), jsonStrIndented);
}

TEST(MyJsonTest, BuildComplexJsonTest) {
  const char* jsonStrCompact = 
    "{\"menu\":{\"id\":\"file\",\"value\":\"File\",\"popup\":{\"menuitem\":[{\"value\":\"New\",\"onclick\":\"CreateNewDoc()\"},{\"value\":\"Open\",\"onclick\":\"OpenDoc()\"},{\"value\":\"Close\",\"onclick\":\"CloseDoc()\"}]}}}";
  const char* jsonStrIndented = 
    "{\r\n"
    "  \"menu\":{\r\n"
    "    \"id\":\"file\",\r\n"
    "    \"value\":\"File\",\r\n"
    "    \"popup\":{\r\n"
    "      \"menuitem\":[\r\n"
    "        {\r\n"
    "          \"value\":\"New\",\r\n"
    "          \"onclick\":\"CreateNewDoc()\"\r\n"
    "        },\r\n"
    "        {\r\n"
    "          \"value\":\"Open\",\r\n"
    "          \"onclick\":\"OpenDoc()\"\r\n"
    "        },\r\n"
    "        {\r\n"
    "          \"value\":\"Close\",\r\n"
    "          \"onclick\":\"CloseDoc()\"\r\n"
    "        }\r\n"
    "      ]\r\n"
    "    }\r\n"
    "  }\r\n"
    "}";

  int ret = 0;
  MyJson json;
  ret = json.InitRootNode(MyJsonValueType::objectValue);
  EXPECT_EQ(ret, 0);

  MyJsonValue* root = json.Root();
  MyJsonValue* menu = root->AddObjectChild("menu");
  menu->AddStringChild("id", "file");
  menu->AddStringChild("value", "File");
  MyJsonValue* popup = menu->AddObjectChild("popup");
  MyJsonValue* menuItem = popup->AddArrayChild("menuitem");
  MyJsonValue* item1 = menuItem->AddObjectChild("");
  item1->AddStringChild("value", "New");
  item1->AddStringChild("onclick", "CreateNewDoc()");
  MyJsonValue* item2 = menuItem->AddObjectChild("");
  item2->AddStringChild("value", "Open");
  item2->AddStringChild("onclick", "OpenDoc()");
  MyJsonValue* item3 = menuItem->AddObjectChild("");
  item3->AddStringChild("value", "Close");
  item3->AddStringChild("onclick", "CloseDoc()");

  MyStringA retJson;
  ret = json.ToString(&retJson, true);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), jsonStrCompact);

  retJson.Reset();
  ret = json.ToString(&retJson, false);
  EXPECT_EQ(ret, 0);
  EXPECT_STREQ(retJson.Deref(), jsonStrIndented);
}



//int main() {
//}