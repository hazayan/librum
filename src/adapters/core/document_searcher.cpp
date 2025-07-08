#include "document_searcher.hpp"
#include <QtWidgets/QApplication>
#include "document_access.hpp"
#include "fz_utils.hpp"
#include "mupdf_document_access.hpp"

using namespace application::core::utils;

namespace adapters::core
{

using application::core::MupdfDocumentAccess;

// We can not go with another approach other than dynamic casting this. The
// adapters layer is "dirty" and knows what kind of backend the application uses
// for rendering, thats why dynamic casting it is ok here. The presentation
// layer stays backend-independent and just communicates with the Interface.
DocumentSearcher::DocumentSearcher(IDocumentAccess* documentAccess) :
    m_mupdfDocumentSearcher(
        dynamic_cast<DocumentAccess*>(documentAccess)->getMupdfDocumentAccess())
{
}

int DocumentSearcher::search(const QString& text, int topPage)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    auto newSearchOptions = convertSearchOptionsFormat(m_documentSearchOptions);
    m_mupdfDocumentSearcher.search(text, newSearchOptions, topPage);
    QApplication::restoreOverrideCursor();

    auto searchHit = m_mupdfDocumentSearcher.firstSearchHit();
    if(searchHit.pageNumber == -1)
        return -1;

    emit goToPosition(searchHit.pageNumber, searchHit.rect.ul.y);
    emit selectText(searchHit.pageNumber, fzRectToQRectF(searchHit.rect));
    return searchHit.pageNumber;
}

void DocumentSearcher::clearSearch()
{
    m_mupdfDocumentSearcher.clearSearch();
}

int DocumentSearcher::goToNextSearchHit()
{
    auto searchHit = m_mupdfDocumentSearcher.nextSearchHit();
    if(searchHit == m_mupdfDocumentSearcher.invalidSearchHit)
        return -1;

    emit goToPosition(searchHit.pageNumber, searchHit.rect.ul.y);
    emit selectText(searchHit.pageNumber, fzRectToQRectF(searchHit.rect));
    return searchHit.pageNumber;
}

int DocumentSearcher::goToPreviousSearchHit()
{
    auto searchHit = m_mupdfDocumentSearcher.previousSearchHit();
    if(searchHit == m_mupdfDocumentSearcher.invalidSearchHit)
        return -1;

    emit goToPosition(searchHit.pageNumber, searchHit.rect.ul.y);
    emit selectText(searchHit.pageNumber, fzRectToQRectF(searchHit.rect));
    return searchHit.pageNumber;
}

DocumentSearchOptions* DocumentSearcher::getDocumentSearchOptions()
{
    return &m_documentSearchOptions;
}

MupdfSearchOptions DocumentSearcher::convertSearchOptionsFormat(
    const DocumentSearchOptions& searchOptions) const
{
    return MupdfSearchOptions {
        .fromStart = searchOptions.getSearchFromStart(),
        .wholeWords = searchOptions.getSearchWholeWords(),
        .caseSensitive = searchOptions.getSearchCaseSensitive(),
    };
}

}  // namespace adapters::core
