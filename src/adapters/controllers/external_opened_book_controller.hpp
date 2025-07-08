#pragma once
#include <QObject>
#include <QRectF>
#include <QString>
#include "adapters_export.hpp"
#include "highlight.hpp"
#include "i_opened_book_controller.hpp"
#include "i_opened_book_service.hpp"

namespace adapters::controllers
{

/***
 * This class implements the IBookController interface to provide access to a
 * book that is not managed by the application, a so called 'external book'.
 *
 * This usecase occurs when e.g. using 'Open with Librum' from a file manager.
 */
class ADAPTERS_EXPORT ExternalOpenedBookController
    : public IOpenedBookController
{
    Q_OBJECT

public:
    ExternalOpenedBookController(
        application::IOpenedBookService* openedBookService);

    bool setUp(QString filePath) override;

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

    const QList<domain::entities::Highlight> m_emptyHighlights;
    const QList<domain::entities::Bookmark> m_emptyBookmarks;
};

}  // namespace adapters::controllers
