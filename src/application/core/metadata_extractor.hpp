#pragma once
#include <memory>
#include "i_metadata_extractor.hpp"
#include "mupdf/classes.h"
#include "mupdf_document_access.hpp"

namespace application::core
{

class MetadataExtractor : public IMetadataExtractor
{
public:
    bool setup(const QString& filePath) override;
    domain::value_objects::BookMetaData getBookMetaData() override;
    QImage getBookCover() override;

private:
    QString getDocumentInfo(const char* key);
    QImage getCover();
    QString getTitleFromPath();
    QString getDocumentSize();
    QString getBookExtension();
    double roundToPrecisionOf2(double raw);

    std::unique_ptr<MupdfDocumentAccess> m_mupdfDocumentAccess;
    QString m_filePath;
};

}  // namespace application::core
