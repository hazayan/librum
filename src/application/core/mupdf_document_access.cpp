#include "mupdf_document_access.hpp"

namespace application::core
{

MupdfDocumentAccess::MupdfDocumentAccess(QString filePath)
{
    auto stdFilePath = filePath.toStdString();
    m_fzDocument = std::make_unique<mupdf::FzDocument>(stdFilePath.c_str());
}

std::unique_ptr<mupdf::FzPage> MupdfDocumentAccess::loadPage(int pageNumber)
{
    return std::make_unique<mupdf::FzPage>(
        m_fzDocument->fz_load_page(pageNumber));
}

std::unique_ptr<mupdf::FzStextPage> MupdfDocumentAccess::loadTextPage(
    int pageNumber)
{
    mupdf::FzStextOptions options;
    return std::make_unique<mupdf::FzStextPage>(*m_fzDocument, pageNumber,
                                                options);
}

int MupdfDocumentAccess::getPageCount()
{
    return m_fzDocument->fz_count_pages();
}

int MupdfDocumentAccess::getPageNumberFromLocation(fz_location* location)
{
    return m_fzDocument->fz_page_number_from_location(location);
}

std::string MupdfDocumentAccess::lookupMetadata(const char* key)
{
    return m_fzDocument->fz_lookup_metadata2(key);
}

bool MupdfDocumentAccess::linkIsExternal(const char* uri)
{
    return mupdf::ll_fz_is_external_link(uri);
}

QPair<int, float> MupdfDocumentAccess::getPositionOfLink(const char* uri)
{
    float yp = 0;
    auto location = m_fzDocument->fz_resolve_link(uri, nullptr, &yp);
    int pageNumber = m_fzDocument->fz_page_number_from_location(location);

    return { pageNumber, yp };
}

mupdf::FzOutline MupdfDocumentAccess::loadTOC()
{
    return m_fzDocument->fz_load_outline();
}

}  // namespace application::core
