#pragma once
#include <QDir>
#include <QObject>
#include "i_ai_tools_gateway.hpp"
#include "i_ai_tools_service.hpp"

namespace application::services
{

class AiToolsService : public IAiToolsService
{
    Q_OBJECT

public:
    AiToolsService(IAiToolsGateway* aiToolsGateway);

    void getExplanation(const QString& text, const QString& mode) override;
    void getTranslation(const QString& text, const QString& sourceLang,
                        const QString& targetLang) override;

public slots:
    void setupUserData(const QString& token, const QString& email) override;
    void clearUserData() override;

private:
    IAiToolsGateway* m_aiToolsGateway;

    QString m_authenticationToken;
};

}  // namespace application::services
