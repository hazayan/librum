#include "opened_book_service.hpp"
#include <mupdf/classes.h>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include "highlight.hpp"

using domain::entities::Highlight;

namespace application::services
{

void OpenedBookService::setUp(std::unique_ptr<IBookGetter> bookGetter)
{
    m_bookGetter = std::move(bookGetter);
}

const QList<domain::entities::Highlight>& OpenedBookService::getHighlights()
    const
{
    auto book = m_bookGetter->getBook();
    return book->getHighlights();
}

void OpenedBookService::addHighlight(
    const domain::entities::Highlight& highlight)
{
    auto book = m_bookGetter->getBook();
    book->addHighlight(highlight);

    updateBook();
}

void OpenedBookService::removeHighlight(const QUuid& uuid)
{
    auto book = m_bookGetter->getBook();
    book->removeHighlight(uuid);

    updateBook();
}

void OpenedBookService::changeHighlightColor(const QUuid& uuid,
                                             const QColor& color)
{
    auto book = m_bookGetter->getBook();
    book->changeHighlightColor(uuid, color);

    updateBook();
}

void OpenedBookService::updateBook()
{
    auto book = m_bookGetter->getBook();
    book->updateLastModified();
    m_bookGetter->updateBook(book);
}

const Highlight* OpenedBookService::getHighlightAtPoint(const QPointF& point,
                                                        int page) const
{
    auto book = m_bookGetter->getBook();
    for(auto& highlight : book->getHighlights())
    {
        if(highlight.getPageNumber() != page)
            continue;

        for(auto& rect : highlight.getRects())
        {
            if(rect.getQRect().contains(point))
                return &highlight;
        }
    }

    return nullptr;
}

const QList<domain::entities::Bookmark>& OpenedBookService::getBookmarks() const
{
    auto book = m_bookGetter->getBook();
    return book->getBookmarks();
}

void OpenedBookService::addBookmark(const domain::entities::Bookmark& bookmark)
{
    auto book = m_bookGetter->getBook();

    emit bookmarkInsertionStarted(book->getBookmarks().count());
    book->addBookmark(bookmark);
    emit bookmarkInsertionEnded();

    updateBook();
}

void OpenedBookService::renameBookmark(const QUuid& uuid,
                                       const QString& newName)
{
    auto book = m_bookGetter->getBook();

    book->renameBookmark(uuid, newName);
    emit bookmarkNameChanged(getIndexOfBookmark(uuid));

    updateBook();
}

void OpenedBookService::removeBookmark(const QUuid& uuid)
{
    auto book = m_bookGetter->getBook();

    emit bookmarkDeletionStarted(getIndexOfBookmark(uuid));
    book->removeBookmark(uuid);
    emit bookmarkDeletionEnded();

    updateBook();
}

QString OpenedBookService::getFilePath() const
{
    auto book = m_bookGetter->getBook();
    return book->getFilePath();
}

int OpenedBookService::getPageCount() const
{
    auto book = m_bookGetter->getBook();
    return book->getPageCount();
}

int OpenedBookService::getCurrentPage() const
{
    auto book = m_bookGetter->getBook();
    return book->getCurrentPage();
}

void OpenedBookService::setCurrentPage(int newCurrentPage)
{
    auto book = m_bookGetter->getBook();
    book->setCurrentPage(newCurrentPage);
}

QString OpenedBookService::getColorTheme()
{
    auto book = m_bookGetter->getBook();
    return book->getColorTheme();
}

void OpenedBookService::setColorTheme(const QString& colorTheme)
{
    auto book = m_bookGetter->getBook();
    book->setColorTheme(colorTheme);
    updateBook();
}

int OpenedBookService::getIndexOfBookmark(const QUuid& uuid) const
{
    auto book = m_bookGetter->getBook();

    auto bookmarks = book->getBookmarks();
    for(int i = 0; i < bookmarks.length(); ++i)
    {
        if(bookmarks.at(i).getUuid() == uuid)
            return i;
    }

    return -1;
}

}  // namespace application::services
