#pragma once
#include <QObject>
#include "adapters_export.hpp"
#include "i_ai_tools_controller.hpp"
#include "i_ai_tools_service.hpp"

namespace adapters::controllers
{

class ADAPTERS_EXPORT AiToolsController : public IAiToolsController
{
    Q_OBJECT

public:
    AiToolsController(application::IAiToolsService* aiToolsService);

    void getExplanation(const QString& text, const QString& mode) override;
    void getTranslation(const QString& text, const QString& sourceLang,
                        const QString& targetLang) override;

private:
    application::IAiToolsService* m_aiToolsService;
};

}  // namespace adapters::controllers
