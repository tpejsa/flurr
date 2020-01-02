#include "flurr/resource/TextureResource.h"
#include "flurr/FlurrLog.h"
#include "flurr/utils/FileUtils.h"
#include "flurr/utils/TypeCasts.h"

#include "stb_image_aug.h"
extern "C" {
#include "stbi_DDS_aug.h"
}

#include <cassert>
#include <cstdio>
#include <vector>

namespace flurr
{

TextureResource::TextureResource(FlurrHandle a_resourceHandle, const std::string& a_resourcePath, std::size_t a_resourceDirectoryIndex, ResourceManager* a_owningManager)
  : Resource(a_resourceHandle, a_resourcePath, a_resourceDirectoryIndex, a_owningManager),
  m_texData(nullptr)
{
}

uint32_t TextureResource::getTexelSize() const
{
  return TexelSize(m_texFormat);
}

constexpr uint32_t TextureResource::TexelSize(TextureFormat a_texFormat)
{
  switch (a_texFormat)
  {
    case TextureFormat::kGrayscale:
    {
      return 1;
    }
    case TextureFormat::kRGB:
    {
      return 3;
    }
    case TextureFormat::kRGBA:
    {
      return 4;
    }
    default:
    {
      FLURR_ASSERT(false, "Unhandled texture format %u!", FromEnum(a_texFormat));
    }
  }
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
    m_texData = stbi_dds_load_from_file(textureFile, reinterpret_cast<int*>(&m_texWidth), reinterpret_cast<int*>(&m_texHeight), &numChannels, 0);
    if (!m_texData)
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
    m_texData = stbi_load_from_file(textureFile, reinterpret_cast<int*>(&m_texWidth), reinterpret_cast<int*>(&m_texHeight), &numChannels, 0);
    if (!m_texData)
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
      m_texFormat = TextureFormat::kGrayscale;
      break;
    }
    case 3:
    {
      m_texFormat = TextureFormat::kRGB;
      break;
    }
    case 4:
    {
      m_texFormat = TextureFormat::kRGBA;
      break;
    }
    default:
    {
      destroyTextureData();
      return Status::kUnsupportedFileType;
    }
  }

  // Flip image around horizontal axis
  FlipY();

  return Status::kSuccess;
}

void TextureResource::onUnload()
{
  destroyTextureData();
}

void TextureResource::destroyTextureData()
{
  if (m_texData)
    stbi_image_free(m_texData);
  m_texData = nullptr;
  m_texWidth = 0;
  m_texHeight = 0;
}

void TextureResource::FlipY()
{
  std::vector<uint8_t> tempRow(getTextureDataSize(), 0);
  const uint32_t rowSize = m_texWidth*getTexelSize();
  for (uint32_t rowIndex = 0; rowIndex < m_texHeight/2; ++rowIndex)
  {
    memcpy(tempRow.data(), m_texData + rowIndex*rowSize, rowSize);
    memcpy(m_texData + rowIndex*rowSize, m_texData + (m_texHeight-rowIndex-1)*rowSize, rowSize);
    memcpy(m_texData + (m_texHeight-rowIndex-1)*rowSize, tempRow.data(), rowSize);
  }
}

} // namespace flurr
