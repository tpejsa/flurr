#include "flogger/OGLVertexArray.h"
#include "flogger/OGLVertexBuffer.h"
#include "flurr/FlurrLog.h"

namespace flurr
{

OGLVertexArray::OGLVertexArray(FlurrHandle a_arrayHandle)
  : VertexArray(a_arrayHandle),
  m_oglVaoId(0)
{
}

OGLVertexArray::~OGLVertexArray()
{
  if (m_oglVaoId)
    glDeleteVertexArrays(1, &m_oglVaoId);
}

Status OGLVertexArray::onCreateArray()
{
  glGenVertexArrays(1, &m_oglVaoId);
  glBindVertexArray(m_oglVaoId);

  for (std::size_t attributeIndex = 0; attributeIndex < getAttributeBuffersCount(); ++attributeIndex)
  {
    const GLuint oglAttributeIndex = static_cast<GLuint>(attributeIndex);
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<OGLVertexBuffer*>(getAttributeBuffer(attributeIndex))->getOGLVertexBufferId());
    std::size_t attributeSize = getAttributeBuffer(attributeIndex)->getAttributeSize();
    glVertexAttribPointer(oglAttributeIndex,
      static_cast<GLint>(attributeSize)/sizeof(float), GL_FLOAT,
      GL_FALSE, 0, 0);
    glEnableVertexAttribArray(oglAttributeIndex);
  }

  return Status::kSuccess;
}

void OGLVertexArray::onDestroyArray()
{
  if (m_oglVaoId)
    glDeleteVertexArrays(1, &m_oglVaoId);
}

Status OGLVertexArray::onDrawArray()
{
  // Get number of indices to draw
  auto* indexBuffer = getIndexBuffer();
  std::size_t numIndices = indexBuffer->getDataSize() / sizeof(uint32_t);

  // Draw the elements
  glBindVertexArray(m_oglVaoId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<OGLVertexBuffer*>(indexBuffer)->getOGLVertexBufferId());
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
  return Status::kSuccess;
}

} // namespace flurr
