#pragma once
#include <QObject>

namespace adapters::core
{

class DocumentSearchOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool searchWholeWords READ getSearchWholeWords WRITE
                   setSearchWholeWords NOTIFY searchWholeWordsChanged)
    Q_PROPERTY(bool searchCaseSensitive READ getSearchCaseSensitive WRITE
                   setSearchCaseSensitive NOTIFY searchCaseSensitiveChanged)
    Q_PROPERTY(bool searchFromStart READ getSearchFromStart WRITE
                   setSearchFromStart NOTIFY searchFromStartChanged)

public:
    DocumentSearchOptions();

    bool getSearchWholeWords() const;
    void setSearchWholeWords(bool newSearchWholeWords);

    bool getSearchCaseSensitive() const;
    void setSearchCaseSensitive(bool newSearchCaseSensitive);

    bool getSearchFromStart() const;
    void setSearchFromStart(bool newSearchFromStart);

signals:
    void searchWholeWordsChanged();
    void searchCaseSensitiveChanged();
    void searchFromStartChanged();

private:
    bool m_searchWholeWords = false;
    bool m_searchCaseSensitive = false;
    bool m_searchFromStart = false;
};

}  // namespace adapters::core
