#pragma once
#include <QObject>
#include <QRectF>
#include <QString>
#include "adapters_export.hpp"
#include "bookmarks_model.hpp"
#include "i_library_service.hpp"
#include "i_opened_book_controller.hpp"
#include "i_opened_book_service.hpp"

namespace adapters::controllers
{

class ADAPTERS_EXPORT OpenedBookController : public IOpenedBookController
{
    Q_OBJECT

public:
    OpenedBookController(application::IOpenedBookService* openedbookService,
                         application::ILibraryService* libraryService);

    bool setUp(QString uuid) override;

    const QList<domain::entities::Highlight>& getHighlights() const override;
    void addHighlight(const domain::entities::Highlight& highlight) override;
    void removeHighlight(const QUuid& uuid) override;
    void changeHighlightColor(const QUuid& uuid, const QColor& color) override;
    const domain::entities::Highlight* getHighlightAtPoint(
        const QPointF& point, int page, float zoom) const override;

    const QList<domain::entities::Bookmark>& getBookmark() const override;
    QString addBookmark(const QString& name, int pageNumber,
                        float yOffset) override;
    void renameBookmark(const QString& uuid, const QString& newName) override;
    void removeBookmark(const QString& uuid) override;
    void goToBookmark(const QString& uuid) override;

    QString getFilePath() const override;
    int getPageCount() const override;

    void setCurrentPage(int newCurrentPage) override;
    int getCurrentPage() const override;

    QString getColorTheme() override;
    void setColorTheme(const QString& colorTheme) override;

    data_models::BookmarksProxyModel* getBookmarksModel() override;

private:
    application::IOpenedBookService* m_openedBookService;
    application::ILibraryService* m_libraryService;

    std::unique_ptr<data_models::BookmarksModel> m_bookmarksModel;
    data_models::BookmarksProxyModel m_bookmarksProxyModel;
};

}  // namespace adapters::controllers
