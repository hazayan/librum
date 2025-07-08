#include "external_opened_book_controller.hpp"
#include <QUuid>
#include <memory>
#include "bookmark.hpp"
#include "external_book_getter.hpp"
#include "highlight.hpp"
#include "i_opened_book_service.hpp"

using application::utility::ExternalBookGetter;
using domain::entities::Bookmark;
using domain::entities::Highlight;

namespace adapters::controllers
{

ExternalOpenedBookController::ExternalOpenedBookController(
    application::IOpenedBookService* openedBookService) :
    m_openedBookService(openedBookService)
{
}

bool ExternalOpenedBookController::setUp(QString filePath)
{
    auto bookGetter = std::make_unique<ExternalBookGetter>(filePath);
    if(!bookGetter->bookIsValid())
        return false;

    m_openedBookService->setUp(std::move(bookGetter));
    return true;
}

const QList<Highlight>& ExternalOpenedBookController::getHighlights() const
{
    return m_emptyHighlights;
}

void ExternalOpenedBookController::addHighlight(const Highlight& highlight)
{
    Q_UNUSED(highlight);
}

void ExternalOpenedBookController::removeHighlight(const QUuid& uuid)
{
    Q_UNUSED(uuid);
}

void ExternalOpenedBookController::changeHighlightColor(const QUuid& uuid,
                                                        const QColor& color)
{
    Q_UNUSED(uuid);
    Q_UNUSED(color);
}

const Highlight* ExternalOpenedBookController::getHighlightAtPoint(
    const QPointF& point, int page, float zoom) const
{
    Q_UNUSED(point);
    Q_UNUSED(page);
    Q_UNUSED(zoom);

    return nullptr;
}

const QList<Bookmark>& ExternalOpenedBookController::getBookmark() const
{
    return m_emptyBookmarks;
}

QString ExternalOpenedBookController::addBookmark(const QString& name,
                                                  int pageNumber, float yOffset)
{
    Q_UNUSED(name);
    Q_UNUSED(pageNumber);
    Q_UNUSED(yOffset);

    return {};
}

void ExternalOpenedBookController::renameBookmark(const QString& uuid,
                                                  const QString& newName)
{
    Q_UNUSED(uuid);
    Q_UNUSED(newName);
}

void ExternalOpenedBookController::removeBookmark(const QString& uuid)
{
    Q_UNUSED(uuid);
}

void ExternalOpenedBookController::goToBookmark(const QString& uuid)
{
    Q_UNUSED(uuid);
}

QString ExternalOpenedBookController::getFilePath() const
{
    return m_openedBookService->getFilePath();
}

int ExternalOpenedBookController::getPageCount() const
{
    return m_openedBookService->getPageCount();
}

void ExternalOpenedBookController::setCurrentPage(int newCurrentPage)
{
    m_openedBookService->setCurrentPage(newCurrentPage);
    emit currentPageChanged(newCurrentPage);
}

int ExternalOpenedBookController::getCurrentPage() const
{
    return m_openedBookService->getCurrentPage();
}

QString ExternalOpenedBookController::getColorTheme()
{
    return m_openedBookService->getColorTheme();
}

void ExternalOpenedBookController::setColorTheme(const QString& colorTheme)
{
    m_openedBookService->setColorTheme(colorTheme);
    emit colorThemeChanged(colorTheme);
}

data_models::BookmarksProxyModel*
    ExternalOpenedBookController::getBookmarksModel()
{
    return nullptr;
}

}  // namespace adapters::controllers
