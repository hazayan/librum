#pragma once
#include <QAbstractItemModel>
#include "adapters_export.hpp"
#include "i_document_access.hpp"
#include "toc_item.hpp"

namespace adapters::data_models
{

class ADAPTERS_EXPORT TOCModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TOCModel(fz_outline* outline, IDocumentAccess* documentAccess,
                      QObject* parent = nullptr);
    ~TOCModel();

    enum Roles
    {
        TitleRole = Qt::DisplayRole,
        PageNumberRole,
        YOffsetRole,
    };

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    int columnCount(const QModelIndex& parent) const override;

private:
    void setupModelData(fz_outline* outline);
    adapters::data_models::TOCItem* getTOCItemFromOutline(fz_outline* outline);

    adapters::data_models::TOCItem* m_rootItem;
    IDocumentAccess* m_documentAccess;
};

}  // namespace adapters::data_models
