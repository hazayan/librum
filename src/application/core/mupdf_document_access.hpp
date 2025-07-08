#pragma once
#include <QPair>
#include <QString>
#include <memory>
#include <string>
#include "application_export.hpp"
#include "mupdf/classes.h"

namespace application::core
{

class APPLICATION_EXPORT MupdfDocumentAccess
{
public:
    MupdfDocumentAccess(QString filePath);

    std::unique_ptr<mupdf::FzPage> loadPage(int pageNumber);
    std::unique_ptr<mupdf::FzStextPage> loadTextPage(int pageNumber);

    int getPageCount();
    int getPageNumberFromLocation(fz_location* location);
    std::string lookupMetadata(const char* key);
    bool linkIsExternal(const char* uri);
    QPair<int, float> getPositionOfLink(const char* uri);
    mupdf::FzOutline loadTOC();

private:
    std::unique_ptr<mupdf::FzDocument> m_fzDocument = nullptr;
};

}  // namespace application::core
