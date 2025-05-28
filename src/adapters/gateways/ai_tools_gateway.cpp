#include "ai_tools_gateway.hpp"

namespace adapters::gateways
{

AiToolsGateway::AiToolsGateway(IAiToolsAccess* aiToolsService) :
    m_aiToolsAccess(aiToolsService)
{
    // Explanation
    connect(m_aiToolsAccess, &IAiToolsAccess::explanationWordReceived, this,
            &AiToolsGateway::explanationWordReady);

    connect(m_aiToolsAccess, &IAiToolsAccess::explanationErrorOccured, this,
            &AiToolsGateway::explanationErrorOccured);

    // Translation
    connect(m_aiToolsAccess, &IAiToolsAccess::translationReceived, this,
            &AiToolsGateway::translationReady);

    connect(m_aiToolsAccess, &IAiToolsAccess::translationErrorOccured, this,
            &AiToolsGateway::translationErrorOccured);
}

void AiToolsGateway::getExplanation(const QString& authToken,
                                    const QString& text, const QString& mode)
{
    m_aiToolsAccess->getExplanation(authToken, text, mode);
}

void AiToolsGateway::getTranslation(const QString& authToken,
                                    const QString& text,
                                    const QString& sourceLang,
                                    const QString& targetLang)
{
    m_aiToolsAccess->getTranslation(authToken, text, sourceLang, targetLang);
}

}  // namespace adapters::gateways
