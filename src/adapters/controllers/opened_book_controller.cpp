#include "opened_book_controller.hpp"
#include <QUuid>
#include "bookmark.hpp"
#include "bookmarks_model.hpp"
#include "fz_utils.hpp"
#include "highlight.hpp"
#include "i_opened_book_service.hpp"
#include "library_book_getter.hpp"
#include "opened_book_controller.hpp"

using namespace application::core;
using application::utility::LibraryBookGetter;
using domain::entities::Bookmark;
using domain::entities::Highlight;

namespace adapters::controllers
{

OpenedBookController::OpenedBookController(
    application::IOpenedBookService* OpenedBookService,
    application::ILibraryService* libraryService) :
    m_openedBookService(OpenedBookService),
    m_libraryService(libraryService)
{
}

bool OpenedBookController::setUp(QString uuid)
{
    m_openedBookService->setUp(
        std::make_unique<LibraryBookGetter>(m_libraryService, QUuid(uuid)));

    m_bookmarksModel = std::make_unique<data_models::BookmarksModel>(
        QUuid(uuid), m_openedBookService);
    m_bookmarksProxyModel.setSourceModel(m_bookmarksModel.get());


    // Connections to bookmarks model
    connect(m_openedBookService,
            &application::IOpenedBookService::bookmarkInsertionStarted,
            m_bookmarksModel.get(),
            &data_models::BookmarksModel::startInsertingBookmark);

    connect(m_openedBookService,
            &application::IOpenedBookService::bookmarkInsertionEnded,
            m_bookmarksModel.get(),
            &data_models::BookmarksModel::endInsertingBookmark);

    connect(m_openedBookService,
            &application::IOpenedBookService::bookmarkDeletionStarted,
            m_bookmarksModel.get(),
            &data_models::BookmarksModel::startDeletingBookmark);

    connect(m_openedBookService,
            &application::IOpenedBookService::bookmarkDeletionStarted,
            m_bookmarksModel.get(),
            &data_models::BookmarksModel::endDeletingBookmark);

    connect(m_openedBookService,
            &application::IOpenedBookService::bookmarkNameChanged,
            m_bookmarksModel.get(),
            &data_models::BookmarksModel::bookmarkNameChanged);

    return true;
}

const QList<Highlight>& OpenedBookController::getHighlights() const
{
    return m_openedBookService->getHighlights();
}

void OpenedBookController::addHighlight(const Highlight& highlight)
{
    m_openedBookService->addHighlight(highlight);
}

void OpenedBookController::removeHighlight(const QUuid& uuid)
{
    m_openedBookService->removeHighlight(uuid);
}

void OpenedBookController::changeHighlightColor(const QUuid& uuid,
                                                const QColor& color)
{
    m_openedBookService->changeHighlightColor(uuid, color);
}

const Highlight* OpenedBookController::getHighlightAtPoint(const QPointF& point,
                                                           int page,
                                                           float zoom) const
{
    auto restoredPoint = utils::restoreQPoint(point, zoom);

    return m_openedBookService->getHighlightAtPoint(restoredPoint, page);
}

const QList<Bookmark>& OpenedBookController::getBookmark() const
{
    return m_openedBookService->getBookmarks();
}

QString OpenedBookController::addBookmark(const QString& name, int pageNumber,
                                          float yOffset)
{
    Bookmark bookmark(name, pageNumber, yOffset);
    m_openedBookService->addBookmark(bookmark);

    return bookmark.getUuid().toString(QUuid::WithoutBraces);
}

void OpenedBookController::renameBookmark(const QString& uuid,
                                          const QString& newName)
{
    m_openedBookService->renameBookmark(QUuid(uuid), newName);
}

void OpenedBookController::removeBookmark(const QString& uuid)
{
    m_openedBookService->removeBookmark(QUuid(uuid));
}

void OpenedBookController::goToBookmark(const QString& uuid)
{
    for(auto& bookmark : m_openedBookService->getBookmarks())
    {
        if(bookmark.getUuid() == QUuid(uuid))
        {
            emit goToBookmarkPosition(bookmark.getPageNumber(),
                                      bookmark.getYOffset());
            break;
        }
    }
}

QString OpenedBookController::getFilePath() const
{
    return m_openedBookService->getFilePath();
}

int OpenedBookController::getPageCount() const
{
    return m_openedBookService->getPageCount();
}

void OpenedBookController::setCurrentPage(int newCurrentPage)
{
    m_openedBookService->setCurrentPage(newCurrentPage);

    emit currentPageChanged(newCurrentPage);
}

int OpenedBookController::getCurrentPage() const
{
    return m_openedBookService->getCurrentPage();
}

QString OpenedBookController::getColorTheme()
{
    return m_openedBookService->getColorTheme();
}

void OpenedBookController::setColorTheme(const QString& colorTheme)
{
    m_openedBookService->setColorTheme(colorTheme);
    emit colorThemeChanged(colorTheme);
}

data_models::BookmarksProxyModel* OpenedBookController::getBookmarksModel()
{
    return &m_bookmarksProxyModel;
}

}  // namespace adapters::controllers
