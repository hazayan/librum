#pragma once
#include <QObject>
#include "adapters_export.hpp"
#include "document_position.hpp"
#include "mupdf/classes.h"

namespace adapters
{

// Forward declaration to avoid circular dependency
namespace data_models
{
class TOCModel;
}  // namespace data_models

class ADAPTERS_EXPORT IDocumentAccess : public QObject
{
    Q_OBJECT

public:
    virtual ~IDocumentAccess() noexcept = default;

    virtual std::unique_ptr<mupdf::FzPage> loadPage(int pageNumber) = 0;
    virtual std::unique_ptr<mupdf::FzStextPage> loadTextPage(
        int pageNumber) = 0;

    virtual int getPageNumberFromLocation(fz_location* location) = 0;
    virtual bool linkIsExternal(const char* uri) = 0;
    virtual core::DocumentPosition getPositionOfLink(const char* uri) = 0;
    virtual std::unique_ptr<data_models::TOCModel> loadTOCModel() = 0;
};

}  // namespace adapters
