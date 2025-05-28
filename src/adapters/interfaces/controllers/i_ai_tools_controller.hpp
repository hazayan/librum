#pragma once
#include <QObject>
#include <QString>
#include "adapters_export.hpp"

namespace adapters
{

/**
 * The AuthenticationController class is exposed to the UI code and thus is the
 * "entry point" to the application's backend for application ai tool
 * operations.
 */
class ADAPTERS_EXPORT IAiToolsController : public QObject
{
    Q_OBJECT

public:
    virtual ~IAiToolsController() noexcept = default;

    Q_INVOKABLE virtual void getExplanation(const QString& text,
                                            const QString& mode) = 0;
    Q_INVOKABLE virtual void getTranslation(const QString& text,
                                            const QString& sourceLang,
                                            const QString& targetLang) = 0;

signals:
    void explanationWordReady(const QString& word);
    void explanationLimitReached();
    void explanationRequestTooLong();
    void explanationErrorOccured();

    void translationReady(const QString& translation);
    void translationLimitReached();
    void translationRequestTooLong();
    void translationErrorOccured();
};

}  // namespace adapters
