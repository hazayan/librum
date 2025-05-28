#include "ai_tools_controller.hpp"

namespace adapters::controllers
{

AiToolsController::AiToolsController(
    application::IAiToolsService* aiToolsService) :
    m_aiToolsService(aiToolsService)
{
    // Explanation
    connect(m_aiToolsService,
            &application::IAiToolsService::explanationWordReady, this,
            &AiToolsController::explanationWordReady);

    connect(m_aiToolsService,
            &application::IAiToolsService::explanationLimitReached, this,
            &AiToolsController::explanationLimitReached);

    connect(m_aiToolsService,
            &application::IAiToolsService::explanationRequestTooLong, this,
            &AiToolsController::explanationRequestTooLong);

    connect(m_aiToolsService,
            &application::IAiToolsService::explanationErrorOccured, this,
            &AiToolsController::explanationErrorOccured);

    // Translation
    connect(m_aiToolsService, &application::IAiToolsService::translationReady,
            this, &AiToolsController::translationReady);

    connect(m_aiToolsService,
            &application::IAiToolsService::translationLimitReached, this,
            &AiToolsController::translationLimitReached);

    connect(m_aiToolsService,
            &application::IAiToolsService::translationRequestTooLong, this,
            &AiToolsController::translationRequestTooLong);

    connect(m_aiToolsService,
            &application::IAiToolsService::translationErrorOccured, this,
            &AiToolsController::translationErrorOccured);
}

void AiToolsController::getExplanation(const QString& text, const QString& mode)
{
    m_aiToolsService->getExplanation(text, mode);
}

void AiToolsController::getTranslation(const QString& text,
                                       const QString& sourceLang,
                                       const QString& targetLang)
{
    m_aiToolsService->getTranslation(text, sourceLang, targetLang);
}

}  // namespace adapters::controllers
