#include <gtest/gtest.h>
#include <flurr.h>

#include <cstdio>
#include <map>
#include <memory>
#include <thread>

using flurr::CurrentTime;
using flurr::TrimString;
using flurr::SplitString;
using flurr::FromString;
using flurr::ToString;
using flurr::ConfigFile;
using flurr::Status;
using flurr::ObjectFactory;
using flurr::FlurrCore;
using flurr::FlurrHandle;
using flurr::INVALID_HANDLE;
using flurr::ResourceListener;
using flurr::ResourceManager;
using flurr::ResourceOperationResult;
using flurr::ResourceOperationType;
using flurr::ResourceType;
using flurr::ResourceState;

class FlurrTest : public ::testing::Test
{

protected:

  FlurrTest()
  {
    // You can do set-up work for each test here.
  }

  virtual ~FlurrTest()
  {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp()
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown()
  {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

// Test time utils
TEST_F(FlurrTest, FlurrTimeUtils)
{
  double startTime = CurrentTime();
  constexpr long long kSleepTime = 1;
  std::this_thread::sleep_for(std::chrono::seconds(kSleepTime));
  EXPECT_TRUE(CurrentTime() - startTime > kSleepTime) << "Current time is off.";
}

// Test string utils
TEST_F(FlurrTest, FlurrStringUtils)
{
  // Test string trimming
  EXPECT_TRUE(TrimString(" abc   ") == "abc");
  EXPECT_TRUE(TrimString("  abc") == "abc");
  EXPECT_TRUE(TrimString("abc ") == "abc");
  EXPECT_TRUE(TrimString("abc") == "abc");
  EXPECT_TRUE(TrimString("   ") == "");
  EXPECT_TRUE(TrimString(" ") == "");
  EXPECT_TRUE(TrimString("") == "");

  // Test string splitting
  auto&& tokens = SplitString("abc def ghi", ' ');
  EXPECT_TRUE(tokens.size() == 3 &&
    tokens[0] == "abc" &&
    tokens[1] == "def" &&
    tokens[2] == "ghi");
  tokens = SplitString(" abc def ghi  ", ' ');
  EXPECT_TRUE(tokens.size() == 5 &&
    tokens[0] == "" &&
    tokens[1] == "abc" &&
    tokens[2] == "def" &&
    tokens[3] == "ghi" &&
    tokens[4] == "");
  tokens = SplitString("abc", ' ');
  EXPECT_TRUE(tokens.size() == 1 && tokens[0] == "abc");
  tokens = SplitString("  abc=def  =ghi", '=');
  EXPECT_TRUE(tokens.size() == 3 &&
    tokens[0] == "  abc" &&
    tokens[1] == "def  " &&
    tokens[2] == "ghi");

  // Test string serialization/deserialization
  EXPECT_TRUE(ToString<std::string>("abc def") == "abc def");
  EXPECT_TRUE(ToString(123) == "123");
  EXPECT_TRUE(FromString<std::string>("abc def") == "abc def");
  EXPECT_TRUE(FromString<int>("123") == 123);
  EXPECT_TRUE(FromString<double>(ToString(123.321)) == 123.321);
  EXPECT_TRUE(FromString<bool>(ToString(true)) == true);
}

// Test configuration file reader
TEST_F(FlurrTest, FlurrConfigFile)
{
  // Can we read the config file?
  ConfigFile cfgFile;
  EXPECT_TRUE(cfgFile.readFromFile("TestFlurr.cfg") == Status::kSuccess);

  // Verify config settings
  int optionInt = 0;
  std::string optionStr = "";
  float optionFloat = 0.f;
  bool optionBool = false;
  std::string resourceDir = "";
  bool logEnabled = true;
  std::string rendererType = "";
  std::string textureFiltering = "";
  int anisotropyLevel = 0;
  EXPECT_TRUE(cfgFile.readIntValue("", "optionInt", optionInt) &&
    123 == optionInt);
  EXPECT_TRUE(cfgFile.readStringValue("", "optionString", optionStr) &&
    "abc" == optionStr);
  EXPECT_TRUE(cfgFile.readFloatValue("", "optionFloat", optionFloat) &&
    12.35f == optionFloat);
  EXPECT_TRUE(cfgFile.readBoolValue("", "optionBool", optionBool) &&
    true == optionBool);
  EXPECT_TRUE(cfgFile.readStringValue("General", "resourceDirectory", resourceDir) &&
    "./resources" == resourceDir);
  EXPECT_TRUE(cfgFile.readBoolValue("General", "logEnabled", logEnabled) &&
    false == logEnabled);
  EXPECT_TRUE(cfgFile.readStringValue("Renderer", "rendererType", rendererType) &&
    "OGL3" == rendererType);
  EXPECT_TRUE(cfgFile.readStringValue("Renderer", "textureFiltering", textureFiltering) &&
    "Anisotropic" == textureFiltering);
  EXPECT_TRUE(cfgFile.readIntValue("Renderer", "anisotropyLevel", anisotropyLevel) &&
    16 == anisotropyLevel);
}

enum class TestObjectType
{
  kTest1 = 0,
  kTest2,
  kTestInvalid
};

class BaseTestObject
{
public:
  BaseTestObject(uint32_t a_objId, const std::string& a_objName)
    : m_objectId(a_objId), m_objectName(a_objName)
  {
  }
  uint32_t ObjectId() const { return m_objectId; }
  const std::string& ObjectName() const { return m_objectName; }
  virtual TestObjectType TypeId() const = 0;
private:
  uint32_t m_objectId;
  std::string m_objectName;
};

class Test1Object : public BaseTestObject
{
public:
  Test1Object(uint32_t a_objId, const std::string& a_objName)
    : BaseTestObject(a_objId, a_objName)
  {
  }
  TestObjectType TypeId() const override { return TestObjectType::kTest1; }
};

class Test2Object : public BaseTestObject
{
public:
  Test2Object(uint32_t a_objId, const std::string& a_objName)
    : BaseTestObject(a_objId, a_objName)
  {
  }
  TestObjectType TypeId() const override { return TestObjectType::kTest2; }
};

// Test object factory
TEST_F(FlurrTest, FlurrObjectFactories)
{
  using TestObjectFactory = ObjectFactory<BaseTestObject, TestObjectType, TestObjectType::kTestInvalid, uint32_t, std::string>;
  TestObjectFactory testObjFactory;
  EXPECT_TRUE(TestObjectFactory::kInvalidTypeId == TestObjectType::kTestInvalid);

  // Test class registration
  TestObjectFactory::CreateCallbackType test1ObjCreateCallback =
    [](uint32_t a_objId, const std::string& a_objName)
    {
      return new Test1Object(a_objId, a_objName);
    };
  TestObjectFactory::CreateCallbackType test2ObjCreateCallback =
    [](uint32_t a_objId, const std::string& a_objName)
    {
      return new Test2Object(a_objId, a_objName);
    };
  testObjFactory.registerClass(TestObjectType::kTest1, "Test1Object", test1ObjCreateCallback);
  testObjFactory.registerClass(TestObjectType::kTest2, "Test2Object", test2ObjCreateCallback);
  EXPECT_TRUE(testObjFactory.isClassRegistered(TestObjectType::kTest1));
  EXPECT_TRUE(testObjFactory.isClassRegistered(TestObjectType::kTest2));
  EXPECT_TRUE(testObjFactory.isClassRegistered("Test1Object"));
  EXPECT_TRUE(testObjFactory.isClassRegistered("Test2Object"));
  EXPECT_TRUE(testObjFactory.getTypeIdByName("Test1Object") == TestObjectType::kTest1);
  EXPECT_TRUE(testObjFactory.getTypeIdByName("Test2Object") == TestObjectType::kTest2);
  EXPECT_TRUE(testObjFactory.getTypeIdByName("Test3Object") == TestObjectFactory::kInvalidTypeId);
  const auto& testObjTypes = testObjFactory.getRegisteredClassTypes();
  ASSERT_TRUE(testObjTypes.size() == 2);

  // Test object creation
  auto&& test1Obj1 = std::unique_ptr<BaseTestObject>(testObjFactory.createObject(TestObjectType::kTest1, 1, "Test1Obj1"));
  auto&& test1Obj2 = std::unique_ptr<BaseTestObject>(testObjFactory.createObject("Test1Object", 2, "Test1Obj2"));
  auto&& test2Obj3 = std::unique_ptr<BaseTestObject>(testObjFactory.createObject(TestObjectType::kTest2, 3, "Test2Obj3"));
  ASSERT_TRUE(test1Obj1);
  ASSERT_TRUE(test1Obj2);
  ASSERT_TRUE(test2Obj3);
  EXPECT_TRUE(test1Obj1->TypeId() == TestObjectType::kTest1);
  EXPECT_TRUE(test2Obj3->TypeId() == TestObjectType::kTest2);
  EXPECT_TRUE(test1Obj1->ObjectId() == 1);
  EXPECT_TRUE(test2Obj3->ObjectId() == 3);
}

class TestResourceListener : public ResourceListener
{
public:
  ResourceOperationResult getOperationResult(FlurrHandle a_resourceHandle) const {
    auto operationResultIt = m_operationResults.find(a_resourceHandle);
    return operationResultIt != m_operationResults.end() ? operationResultIt->second : ResourceOperationResult{INVALID_HANDLE, ResourceOperationType::kLoad, Status::kFailed};
  }
  void onResourceOperation(const ResourceOperationResult& a_result, ResourceManager* a_resourceManager) override
  {
    m_operationResults[a_result.resourceHandle] = a_result;
  }
private:
  std::map<FlurrHandle, ResourceOperationResult> m_operationResults;
};

// Test resource management
TEST_F(FlurrTest, FlurrResourceManager)
{
  EXPECT_TRUE(FlurrCore::Get().init() == Status::kSuccess);
  auto* resourceManager = FlurrCore::Get().getResourceManager();
  ASSERT_TRUE(resourceManager);
  EXPECT_TRUE(resourceManager->isRunning());

  // Test resource creation
  static const std::string kShaderPath1 = "resources/shaders/LitPhong.vert";
  static const std::string kShaderPath2 = "resources/shaders/LitPhong.frag";
  static const std::string kShaderPath3 = "resources/shaders/blahblah.vert";
  FlurrHandle shaderResourceHandle1 = INVALID_HANDLE;
  FlurrHandle shaderResourceHandle2 = INVALID_HANDLE;
  FlurrHandle shaderResourceHandle3 = INVALID_HANDLE;
  EXPECT_TRUE(resourceManager->createResource(ResourceType::kShader, kShaderPath1, shaderResourceHandle1)
    == Status::kSuccess);
  EXPECT_TRUE(resourceManager->createResource(ResourceType::kShader, kShaderPath2, shaderResourceHandle2)
    == Status::kSuccess);
  EXPECT_TRUE(resourceManager->createResource(ResourceType::kShader, kShaderPath3, shaderResourceHandle3)
    == Status::kResourceFileNotFound);
  EXPECT_TRUE(shaderResourceHandle1 != INVALID_HANDLE);
  EXPECT_TRUE(shaderResourceHandle3 == INVALID_HANDLE);
  EXPECT_TRUE(resourceManager->hasResource(shaderResourceHandle1));
  auto* shaderResource1 = resourceManager->getResource(shaderResourceHandle1);
  auto* shaderResource2 = resourceManager->getResource(shaderResourceHandle2);
  EXPECT_TRUE(shaderResource1);
  EXPECT_TRUE(shaderResource2);
  EXPECT_TRUE(shaderResource1 == resourceManager->getResource(kShaderPath1));
  EXPECT_TRUE(shaderResource1->getResourceHandle() == shaderResourceHandle1);
  EXPECT_TRUE(shaderResource1->getResourcePath() == kShaderPath1);
  EXPECT_TRUE(shaderResource1->getResourceFullPath() == ("./" + kShaderPath1));
  EXPECT_TRUE(shaderResource1->getResourceState() == ResourceState::kCreated);

  // Test resource loading
  TestResourceListener testListener;
  resourceManager->addListener(&testListener);
  static constexpr int kUpdateTimeMSec = 100;
  static constexpr float kUpdateTimeSec = kUpdateTimeMSec/1000.0f;
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec));
  FlurrCore::Get().update(kUpdateTimeSec);
  EXPECT_TRUE(Status::kSuccess == resourceManager->loadResource(shaderResourceHandle1));
  EXPECT_TRUE(Status::kSuccess == resourceManager->loadResource(shaderResourceHandle2));
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec*10));
  FlurrCore::Get().update(kUpdateTimeSec*10);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle1).resourceHandle == shaderResourceHandle1);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle1).status == Status::kSuccess);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle2).resourceHandle == shaderResourceHandle2);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle2).status == Status::kSuccess);
  EXPECT_TRUE(shaderResource1->getResourceState() == ResourceState::kLoaded);
  EXPECT_TRUE(shaderResource2->getResourceState() == ResourceState::kLoaded);

  // Test resource unloading
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec));
  FlurrCore::Get().update(kUpdateTimeSec);
  EXPECT_TRUE(Status::kSuccess == resourceManager->unloadResource(shaderResourceHandle1));
  EXPECT_TRUE(Status::kSuccess == resourceManager->unloadResource(shaderResourceHandle2));
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec));
  FlurrCore::Get().update(kUpdateTimeSec);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle1).resourceHandle == shaderResourceHandle1);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle1).status == Status::kSuccess);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle2).resourceHandle == shaderResourceHandle2);
  EXPECT_TRUE(testListener.getOperationResult(shaderResourceHandle2).status == Status::kSuccess);
  EXPECT_TRUE(shaderResource1->getResourceState() == ResourceState::kCreated);
  EXPECT_TRUE(shaderResource2->getResourceState() == ResourceState::kCreated);
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec));
  FlurrCore::Get().update(kUpdateTimeSec);
  EXPECT_TRUE(Status::kSuccess == resourceManager->loadResource(shaderResourceHandle1));
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec*10));
  FlurrCore::Get().update(kUpdateTimeSec*10);
  EXPECT_TRUE(shaderResource1->getResourceState() == ResourceState::kLoaded);
  EXPECT_TRUE(shaderResource2->getResourceState() == ResourceState::kCreated);

  // Test resource destroying
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec));
  FlurrCore::Get().update(kUpdateTimeSec);
  EXPECT_TRUE(Status::kSuccess == resourceManager->destroyResource(shaderResourceHandle2));
  std::this_thread::sleep_for(std::chrono::milliseconds(kUpdateTimeMSec));
  FlurrCore::Get().update(kUpdateTimeSec);
  EXPECT_FALSE(resourceManager->hasResource(shaderResourceHandle2));
  EXPECT_TRUE(resourceManager->hasResource(shaderResourceHandle1));
  EXPECT_TRUE(Status::kSuccess == resourceManager->destroyResource(shaderResourceHandle1));
  EXPECT_FALSE(resourceManager->hasResource(shaderResourceHandle1));

  // Test shutdown
  EXPECT_TRUE(resourceManager->createResource(ResourceType::kShader, kShaderPath1, shaderResourceHandle1)
    == Status::kSuccess);
  FlurrCore::Get().shutdown();
  EXPECT_FALSE(resourceManager->hasResource(shaderResourceHandle1));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  std::getchar();
  return result;
}
