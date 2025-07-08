#pragma once
#include <QObject>
#include "adapters_export.hpp"
#include "document_search_options.hpp"

namespace adapters
{

class ADAPTERS_EXPORT IDocumentSearcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(core::DocumentSearchOptions* searchOptions READ
                   getDocumentSearchOptions CONSTANT)

public:
    virtual ~IDocumentSearcher() noexcept = default;

    Q_INVOKABLE virtual int search(const QString& text, int topPage) = 0;
    Q_INVOKABLE virtual void clearSearch() = 0;
    Q_INVOKABLE virtual int goToNextSearchHit() = 0;
    Q_INVOKABLE virtual int goToPreviousSearchHit() = 0;

    virtual core::DocumentSearchOptions* getDocumentSearchOptions() = 0;

signals:
    void goToPosition(int pageNumber, int yOffset);
    void selectText(int pageNumber, const QRectF& rect);
};

}  // namespace adapters
