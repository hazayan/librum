#include "document_access.hpp"
#include "mupdf/classes.h"

using namespace application::core;
using adapters::data_models::TOCModel;

namespace adapters::core
{

DocumentAccess::DocumentAccess(QString filePath) :
    m_mupdfDocumentAccess(filePath)
{
}

std::unique_ptr<mupdf::FzPage> DocumentAccess::loadPage(int pageNumber)
{
    return m_mupdfDocumentAccess.loadPage(pageNumber);
}

std::unique_ptr<mupdf::FzStextPage>
    adapters::core::DocumentAccess::loadTextPage(int pageNumber)
{
    return m_mupdfDocumentAccess.loadTextPage(pageNumber);
}

int DocumentAccess::getPageNumberFromLocation(fz_location* location)
{
    return m_mupdfDocumentAccess.getPageNumberFromLocation(location);
}

bool DocumentAccess::linkIsExternal(const char* uri)
{
    return m_mupdfDocumentAccess.linkIsExternal(uri);
}

std::unique_ptr<data_models::TOCModel> DocumentAccess::loadTOCModel()
{
    auto data = m_mupdfDocumentAccess.loadTOC();
    return std::make_unique<TOCModel>(data.m_internal, this);
}

MupdfDocumentAccess* DocumentAccess::getMupdfDocumentAccess()
{
    return &m_mupdfDocumentAccess;
}

DocumentPosition DocumentAccess::getPositionOfLink(const char* uri)
{
    auto positionPair = m_mupdfDocumentAccess.getPositionOfLink(uri);
    return DocumentPosition { positionPair.first, positionPair.second };
}

}  // namespace adapters::core
