#pragma once
#include <QByteArray>
#include <QObject>
#include <QString>
#include "application_export.hpp"

namespace application
{

/**
 * The AiToolsGateway class acts as a layer of abstraction before the
 * AiToolsAccess class. It maps the data provided by the application to
 * the data type required by the API.
 */
class APPLICATION_EXPORT IAiToolsGateway : public QObject
{
    Q_OBJECT

public:
    virtual ~IAiToolsGateway() noexcept = default;

    virtual void getExplanation(const QString& authToken, const QString& text,
                                const QString& mode) = 0;
    virtual void getTranslation(const QString& authToken, const QString& text,
                                const QString& sourceLang,
                                const QString& targetLang) = 0;

signals:
    void explanationWordReady(const QString& explanation);
    void explanationErrorOccured(const int code);
    void translationReady(const QString& translation);
    void translationErrorOccured(const int code);
};

}  // namespace application
