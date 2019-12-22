#include "flurr/resource/TextureResource.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/FileUtils.h"

#include "stb_image_aug.h"
extern "C" {
#include "stbi_DDS_aug.h"
}

#include <cstdio>

namespace flurr
{

TextureResource::TextureResource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager)
  : Resource(a_resourceHandle, a_resourcePath, a_resourceDirectoryIndex, a_owningManager),
  m_textureData(nullptr)
{
}

Status TextureResource::onLoad(const std::string& a_fullPath)
{
  // Make sure it's a supported file extension
  const auto&& ext = GetFileExtension(a_fullPath);
  if ("dds" != ext && "jpg" != ext && "png" != ext)
  {
    FLURR_LOG_ERROR("Texture file %s has unsupported extension!", a_fullPath.c_str());
    return Status::kUnsupportedFileType;
  }

  // Open texture file
  FILE* textureFile = fopen(a_fullPath.c_str(), "rb");
  FLURR_ASSERT(textureFile, "Failed to open texture file %s!", a_fullPath.c_str());

  int numChannels = 0;
  if (ext == "dds")
  {
    // Make sure this a DDS file
    if (!stbi_dds_test_file(textureFile))
    {
      FLURR_LOG_ERROR("File %s in not a valid .dds file!", a_fullPath.c_str());
      fclose(textureFile);
      return Status::kOpenFileError;
    }

    // Load texture from the file
    m_textureData = stbi_dds_load_from_file(textureFile, reinterpret_cast<int*>(&m_textureWidth), reinterpret_cast<int*>(&m_textureHeight), &numChannels, 0);
    if (!m_textureData)
    {
      FLURR_LOG_ERROR("Failed to load texture from file %s!", a_fullPath.c_str());
      fclose(textureFile);
      return Status::kReadFileError;
    }
  }
  else
  {
    // Make sure this a JPG or PNG file
    if (ext == "jpg" && !stbi_jpeg_test_file(textureFile) ||
      ext == "png" && !stbi_png_test_file(textureFile))
    {
      FLURR_LOG_ERROR("File %s in not a valid .%s file!", a_fullPath.c_str(), ext.c_str());
      fclose(textureFile);
      return Status::kOpenFileError;
    }

    // Load texture from the file
    m_textureData = stbi_load_from_file(textureFile, reinterpret_cast<int*>(&m_textureWidth), reinterpret_cast<int*>(&m_textureHeight), &numChannels, 0);
    if (!m_textureData)
    {
      FLURR_LOG_ERROR("Failed to load texture from file %s!", a_fullPath.c_str());
      fclose(textureFile);
      return Status::kReadFileError;
    }
  }

  // Set texture format
  switch (numChannels)
  {
    case 2:
    {
      m_textureFormat = TextureFormat::kGrayscale;
      break;
    }
    case 3:
    {
      m_textureFormat = TextureFormat::kBGR;
      break;
    }
    case 4:
    {
      m_textureFormat = TextureFormat::kBGRA;
      break;
    }
    default:
    {
      destroyTextureData();
      return Status::kUnsupportedFileType;
    }
  }

  return Status::kSuccess;
}

void TextureResource::onUnload()
{
  destroyTextureData();
}

void TextureResource::destroyTextureData()
{
  if (m_textureData) delete[] m_textureData;
  m_textureData = nullptr;
  m_textureWidth = 0;
  m_textureHeight = 0;
}

} // namespace flurr
