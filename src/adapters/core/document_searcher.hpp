#pragma once
#include "document_search_options.hpp"
#include "i_document_access.hpp"
#include "i_document_searcher.hpp"
#include "mupdf_document_searcher.hpp"

namespace adapters::core
{

class DocumentSearcher : public IDocumentSearcher
{
public:
    DocumentSearcher(IDocumentAccess* documentAccess);

    int search(const QString& text, int topPage) override;
    void clearSearch() override;
    int goToNextSearchHit() override;
    int goToPreviousSearchHit() override;

    DocumentSearchOptions* getDocumentSearchOptions() override;

private:
    DocumentSearchOptions m_documentSearchOptions;
    application::core::utils::MupdfSearchOptions convertSearchOptionsFormat(
        const DocumentSearchOptions& searchOptions) const;

    application::core::MupdfDocumentSearcher m_mupdfDocumentSearcher;
};

}  // namespace adapters::core
