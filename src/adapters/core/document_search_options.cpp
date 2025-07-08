#include "document_search_options.hpp"

namespace adapters::core
{

DocumentSearchOptions::DocumentSearchOptions()
{
}

bool DocumentSearchOptions::getSearchWholeWords() const
{
    return m_searchWholeWords;
}

void DocumentSearchOptions::setSearchWholeWords(bool newSearchWholeWords)
{
    m_searchWholeWords = newSearchWholeWords;
    emit searchWholeWordsChanged();
}

bool DocumentSearchOptions::getSearchCaseSensitive() const
{
    return m_searchCaseSensitive;
}

void DocumentSearchOptions::setSearchCaseSensitive(bool newCaseSensitive)
{
    m_searchCaseSensitive = newCaseSensitive;
    emit searchCaseSensitiveChanged();
}

bool DocumentSearchOptions::getSearchFromStart() const
{
    return m_searchFromStart;
}

void DocumentSearchOptions::setSearchFromStart(bool newSearchFromStart)
{
    m_searchFromStart = newSearchFromStart;
    emit searchFromStartChanged();
}

}  // namespace adapters::core
