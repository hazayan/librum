#pragma once
#include <QObject>
#include "i_ai_tools_access.hpp"
#include "i_ai_tools_gateway.hpp"

namespace adapters::gateways
{

class AiToolsGateway : public application::IAiToolsGateway
{
    Q_OBJECT

public:
    AiToolsGateway(IAiToolsAccess* aiToolsService);

    void getExplanation(const QString& authToken, const QString& text,
                        const QString& mode) override;
    void getTranslation(const QString& authToken, const QString& text,
                        const QString& sourceLang,
                        const QString& targetLang) override;

private:
    IAiToolsAccess* m_aiToolsAccess;
};

}  // namespace adapters::gateways
