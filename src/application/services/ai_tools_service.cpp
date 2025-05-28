#include "ai_tools_service.hpp"
#include "error_code.hpp"

namespace application::services
{

AiToolsService::AiToolsService(IAiToolsGateway* aiToolsGateway) :
    m_aiToolsGateway(aiToolsGateway)
{
    // Explanation
    connect(m_aiToolsGateway, &IAiToolsGateway::explanationWordReady, this,
            &AiToolsService::explanationWordReady);

    connect(
        m_aiToolsGateway, &IAiToolsGateway::explanationErrorOccured, this,
        [this](int code)
        {
            if(code == static_cast<int>(
                           error_codes::ErrorCode::AiExplanationLimitReached))
            {
                emit explanationLimitReached();
            }
            else if(code ==
                    static_cast<int>(error_codes::ErrorCode::AiRequestTooLong))
            {
                emit explanationRequestTooLong();
            }
        });

    // Translation
    connect(m_aiToolsGateway, &IAiToolsGateway::translationReady, this,
            &AiToolsService::translationReady);

    connect(m_aiToolsGateway, &IAiToolsGateway::translationErrorOccured, this,
            [this](int code)
            {
                if(code == static_cast<int>(
                               error_codes::ErrorCode::TranslationLimitReached))
                {
                    emit translationLimitReached();
                }
                else if(code ==
                        static_cast<int>(
                            error_codes::ErrorCode::TranslationRequestTooLong))
                {
                    emit translationRequestTooLong();
                }
                else
                {
                    emit translationErrorOccured();
                }
            });
}

void AiToolsService::getExplanation(const QString& text, const QString& mode)
{
    m_aiToolsGateway->getExplanation(m_authenticationToken, text, mode);
}

void AiToolsService::getTranslation(const QString& text,
                                    const QString& sourceLang,
                                    const QString& targetLang)
{
    m_aiToolsGateway->getTranslation(m_authenticationToken, text, sourceLang,
                                     targetLang);
}

void AiToolsService::setupUserData(const QString& token, const QString& email)
{
    Q_UNUSED(email);

    m_authenticationToken = token;
}

void AiToolsService::clearUserData()
{
    m_authenticationToken.clear();
}

}  // namespace application::services
