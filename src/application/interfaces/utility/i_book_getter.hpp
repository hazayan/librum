#pragma once
#include <QUuid>
#include "application_export.hpp"
#include "book.hpp"

namespace application
{

/**
 * This is an interface to get a pointer to a book from any source, either the
 * in-memory library or the file system. We need this because the book service
 * needs to access the book to add highlights, bookmarks, etc. But it doesn't
 * know if the book is in the library or on the file system (in the case of an
 * external book). So it uses this interface to get a book pointer.
 */
class APPLICATION_EXPORT IBookGetter
{
public:
    virtual ~IBookGetter() noexcept = default;

    virtual domain::entities::Book* getBook() = 0;
    virtual void setUuid(const QUuid& uuid) = 0;
    virtual void updateBook(domain::entities::Book* book) = 0;
    virtual bool bookIsValid() = 0;
};

}  // namespace application
