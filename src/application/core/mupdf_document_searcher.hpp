#pragma once
#include <QString>
#include "mupdf/classes.h"
#include "mupdf_document_access.hpp"
#include "mupdf_search_options.hpp"

namespace application::core
{

struct SearchHit
{
    int pageNumber;
    mupdf::FzQuad rect;

    bool operator==(const SearchHit& other) const
    {
        return pageNumber == other.pageNumber && rect == other.rect;
    }
};

class MupdfDocumentSearcher
{
public:
    MupdfDocumentSearcher(MupdfDocumentAccess* mupdfDocumentAccess);

    void search(const QString& text,
                core::utils::MupdfSearchOptions searchOptions, int currentPage);
    void clearSearch();
    SearchHit firstSearchHit();
    SearchHit nextSearchHit();
    SearchHit previousSearchHit();

    const SearchHit invalidSearchHit { -1, mupdf::FzQuad() };

private:
    void extractSearchHitsFromBook(std::vector<SearchHit>& results,
                                   const char* text,
                                   core::utils::MupdfSearchOptions options,
                                   int currentPage) const;
    bool isWholeWord(const mupdf::FzStextPage& textPage,
                     const mupdf::FzQuad& quad) const;
    bool isCaseSensitive(mupdf::FzStextPage& textPage,
                         const mupdf::FzQuad& quad,
                         const QString& needle) const;
    std::vector<SearchHit> sortHitsToStartFromCurrentPage(
        const std::vector<SearchHit>& hits, int currentPage) const;

    MupdfDocumentAccess* m_mupdfDocumentAccess;
    std::vector<SearchHit> m_searchHits;
    int m_currentSearchHit;
};

}  // namespace application::core
