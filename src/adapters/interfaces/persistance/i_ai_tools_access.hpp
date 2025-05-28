#pragma once
#include <QObject>
#include <QString>
#include "adapters_export.hpp"

namespace adapters
{

/**
 * The IAiToolsAccess class makes the API calls to the server.
 */
class ADAPTERS_EXPORT IAiToolsAccess : public QObject
{
    Q_OBJECT

public:
    virtual ~IAiToolsAccess() noexcept = default;

    virtual void getExplanation(const QString& authToken, const QString& query,
                                const QString& mode) = 0;
    virtual void getTranslation(const QString& authToken, const QString& text,
                                const QString& sourceLang,
                                const QString& targetLang) = 0;

signals:
    void explanationWordReceived(const QString& word);
    void explanationErrorOccured(int errorCode);
    void translationErrorOccured(int errorCode);
    void translationReceived(const QString& translation);
};

}  // namespace adapters
