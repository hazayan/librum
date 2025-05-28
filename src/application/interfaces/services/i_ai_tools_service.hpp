#pragma once
#include <QObject>
#include <QString>
#include "application_export.hpp"

namespace application
{

class APPLICATION_EXPORT IAiToolsService : public QObject
{
    Q_OBJECT

public:
    virtual ~IAiToolsService() noexcept = default;

    virtual void getExplanation(const QString& text, const QString& mode) = 0;
    virtual void getTranslation(const QString& text, const QString& sourceLang,
                                const QString& targetLang) = 0;

public slots:
    virtual void setupUserData(const QString& token, const QString& email) = 0;
    virtual void clearUserData() = 0;

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

}  // namespace application
