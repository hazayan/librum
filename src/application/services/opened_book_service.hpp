#pragma once
#include <memory>
#include "i_book_getter.hpp"
#include "i_opened_book_service.hpp"
#include "mupdf/classes.h"

namespace application::services
{

class OpenedBookService : public IOpenedBookService
{
public:
    void setUp(std::unique_ptr<IBookGetter> bookGetter) override;

    const QList<domain::entities::Highlight>& getHighlights() const override;
    void addHighlight(const domain::entities::Highlight& highlight) override;
    void removeHighlight(const QUuid& uuid) override;
    void changeHighlightColor(const QUuid& uuid, const QColor& color) override;
    const domain::entities::Highlight* getHighlightAtPoint(
        const QPointF& point, int page) const override;

    const QList<domain::entities::Bookmark>& getBookmarks() const override;
    void addBookmark(const domain::entities::Bookmark& bookmark) override;
    void renameBookmark(const QUuid& uuid, const QString& newName) override;
    void removeBookmark(const QUuid& uuid) override;

    QString getFilePath() const override;
    int getPageCount() const override;
    int getCurrentPage() const override;
    void setCurrentPage(int newCurrentPage) override;

    QString getColorTheme() override;
    void setColorTheme(const QString& colorTheme) override;

    void updateBook() override;

private:
    int getIndexOfBookmark(const QUuid& uuid) const;

    std::unique_ptr<IBookGetter> m_bookGetter;
    std::unique_ptr<mupdf::FzDocument> m_fzDocument = nullptr;
    float m_zoom = 1;
};

}  // namespace application::services
