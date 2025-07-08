#pragma once
#include <QObject>
#include "adapters_export.hpp"
#include "i_document_access.hpp"
#include "mupdf_document_access.hpp"
#include "toc_model.hpp"

using namespace application::core;

namespace adapters::core
{

class ADAPTERS_EXPORT DocumentAccess : public IDocumentAccess
{
public:
    DocumentAccess(QString filePath);

    std::unique_ptr<mupdf::FzPage> loadPage(int pageNumber) override;
    std::unique_ptr<mupdf::FzStextPage> loadTextPage(int pageNumber) override;

    int getPageNumberFromLocation(fz_location* location) override;
    core::DocumentPosition getPositionOfLink(const char* uri) override;
    bool linkIsExternal(const char* uri) override;
    std::unique_ptr<data_models::TOCModel> loadTOCModel() override;

    MupdfDocumentAccess* getMupdfDocumentAccess();

private:
    MupdfDocumentAccess m_mupdfDocumentAccess;
    std::unique_ptr<data_models::TOCModel> m_TOCModel;
};

}  // namespace adapters::core
