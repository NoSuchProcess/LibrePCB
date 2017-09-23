/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * http://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_DOMELEMENT_H
#define LIBREPCB_DOMELEMENT_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include <QtWidgets>
#include <QDomElement>
#include <sexpresso/sexpresso/sexpresso.hpp>
#include "../exceptions.h"
#include "filepath.h"

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {

class DomDocument;

/*****************************************************************************************
 *  Class DomElement
 ****************************************************************************************/

/**
 * @brief The DomElement class represents one element in a DOM tree
 *
 * Each #DomElement represents either a text element or an element with childs.
 * Example:
 * @code{.xml}
 * <root_element>                   <!-- element with childs (cannot include text) -->
 *     <child>                      <!-- element with childs (cannot include text) -->
 *         <text>Some Text</text>   <!-- text element (cannot contain childs) -->
 *     </child>
 *     <empty_child></empty_child>  <!-- could be a text element or an element with childs -->
 * </root_element>
 * @endcode
 *
 * @author ubruhin
 * @date 2015-02-01
 */
class DomElement final
{
        Q_DECLARE_TR_FUNCTIONS(DomElement)

    public:

        // Constructors / Destructor

        /**
         * @brief Constructor to create a new DOM element
         *
         * @param name      The tag name of the element
         * @param text      The text if a text element should be created
         */
        explicit DomElement(const QString& name, const QString& text = QString()) noexcept;

        /**
         * @brief Destructor (destroys all child elements)
         */
        ~DomElement() noexcept;


        // General Methods

        /**
         * @brief Get the DOM document of this DOM element
         *
         * @param docOfTree     If true and this element is not the root element, this
         *                      method will try to return the document of the whole tree.
         *
         * @retval DomDocument*  A pointer to the DOM document of this DOM element.
         * @retval nullptr          If "docOfTree == false" and this element has no
         *                          document or is not the root element in the document.
         * @retval nullptr          If the whole DOM tree of this element has no document.
         */
        DomDocument* getDocument(bool docOfTree) const noexcept;

        /**
         * @brief Set the document of this element
         *
         * @warning This method should be called only from class #DomDocument!
         *
         * @param doc   The document or nullptr
         */
        void setDocument(DomDocument* doc) noexcept;

        /**
         * @brief Get the filepath of the DOM documents file (if available)
         *
         * @return The filepath of the documents file (invalid if no document available)
         *
         * @note If no document is available or the document is not saved to disc (newly
         *       created document), this method will return an invalid #FilePath object!
         */
        FilePath getDocFilePath() const noexcept;

        /**
         * @brief Get the parent element
         *
         * @retval DomElement*   Pointer to the parent element
         * @retval nullptr          If this element has no parent
         */
        DomElement* getParent() const noexcept {return mParent;}

        /**
         * @brief Get the tag name of this element
         *
         * @return The tag name
         */
        const QString& getName() const noexcept {return mName;}

        /**
         * @brief Set the tag name of this element
         *
         * @param name  The new name (see #isValidTagName() for allowed characters)
         */
        void setName(const QString& name) noexcept {
            Q_ASSERT(isValidTagName(name));
            mName = name;
        }


        // Text Handling Methods

        /**
         * @brief Set the text of this text element
         *
         * @tparam T    The value of this type will be converted to a QString.
         *
         * @warning This method must be called only on elements without child elements!
         *
         * @param value         The value in the template type T
         */
        template <typename T>
        void setText(const T& value) noexcept {
            Q_ASSERT(mChilds.isEmpty() == true);
            mText = objectToString(value);
        }

        /**
         * @brief Get the text of this text element in the specified type
         *
         * @tparam T    The text will be converted to this type.
         *
         * @param throwIfEmpty  If true and the text is empty, an exception will be thrown.
         *                      If false and the text is empty, defaultValue will be returned.
         *
         * @retval T            The text of this text element in the template type T
         * @retval defaultValue If the text is empty and "throwIfEmpty == false"
         *
         * @throw Exception     If this is not a text element
         * @throw Exception     If converting the text into the type T was not successful
         * @throw Exception     If the text is empty and "throwIfEmpty == true"
         */
        template <typename T>
        T getText(bool throwIfEmpty, const T& defaultValue = T()) const {
            if (hasChilds()) {
                throw FileParseError(__FILE__, __LINE__, getDocFilePath(), -1, -1, mName,
                                     tr("A node with child elements cannot have a text."));
            }
            try {
                return stringToObject<T>(mText, throwIfEmpty, defaultValue);
            } catch (const Exception& e) {
                throw FileParseError(__FILE__, __LINE__, getDocFilePath(), -1, -1, QString(),
                    QString(tr("Text \"%1\" in node \"%2\" is invalid: %3"))
                    .arg(mText, mName, e.getMsg()));
            }
        }


        // Attribute Handling Methods

        /**
         * @brief Set or add an attribute to this element
         *
         * @tparam T        The text will be converted in this type.
         *
         * @param name      The tag name (see #isValidTagName() for allowed characters)
         * @param value     The attribute value
         */
        template <typename T>
        void setAttribute(const QString& name, const T& value) noexcept {
            mAttributes.insert(name, objectToString(value));
        }

        /**
         * @brief Check whether this element has a specific attribute or not
         *
         * @param name  The tag name (see #isValidTagName() for allowed characters)
         *
         * @retval true     If the attribute exists
         * @retval false    If the attribute does not exist
         */
        bool hasAttribute(const QString& name) const noexcept;

        /**
         * @brief Get the value of a specific attribute in the specified type
         *
         * @tparam T    The value will be converted in this type.
         *
         * @param name          The tag name (see #isValidTagName() for allowed characters)
         * @param throwIfEmpty  If true and the value is empty, an exception will be thrown
         *                      If false and the value is empty, defaultValue will be returned.
         *
         * @retval T            The value of this text element in the template type T
         * @retval defaultValue If the value is empty and "throwIfEmpty == false"
         *
         * @throw Exception     If the specified attribute does not exist
         * @throw Exception     If converting the value into the type T was not successful
         * @throw Exception     If the value is empty and "throwIfEmpty == true"
         */
        template <typename T>
        T getAttribute(const QString& name, bool throwIfEmpty, const T& defaultValue = T()) const {
            if (!mAttributes.contains(name)) {
                throw FileParseError(__FILE__, __LINE__, getDocFilePath(), -1, -1, QString(),
                    QString(tr("Attribute \"%1\" not found in node \"%2\".")).arg(name, mName));
            }
            QString value = mAttributes.value(name);
            try {
                return stringToObject<T>(value, throwIfEmpty, defaultValue);
            } catch (const Exception& e) {
                throw FileParseError(__FILE__, __LINE__, getDocFilePath(), -1, -1, QString(),
                    QString(tr("Invalid attribute %1=\"%2\" in node \"%3\": %4"))
                    .arg(name, value, mName, e.getMsg()));
            }
        }


        // Child Handling Methods
        const QList<DomElement*>& getChilds() const noexcept {return mChilds;}

        /**
         * @brief Get all childs with a specific tag name
         *
         * @param name      Element tag name of the childs to be returned
         *
         * @return A list of childs
         */
        QList<DomElement*> getChilds(const QString& name) const noexcept;

        /**
         * @brief Check whether this element has childs or not
         *
         * @retval true     If this element has child elements
         * @retval false    If this element has no child elements
         */
        bool hasChilds() const noexcept {return !mChilds.isEmpty();}

        /**
         * @brief Get the child count of this element
         *
         * @return  The count of child elements
         */
        int getChildCount() const noexcept {return mChilds.count();}

        /**
         * @brief Remove a child element from the DOM tree
         *
         * @param child         The child to remove
         * @param deleteChild   If true, this method will also delete the child object
         */
        void removeChild(DomElement* child, bool deleteChild) noexcept;

        /**
         * @brief Append a child to the end of the child list of this element
         *
         * @param child     The child to append
         */
        void appendChild(DomElement* child) noexcept;

        /**
         * @brief Create and append the new child to the end of the child list of this element
         *
         * @param name  The tag name of the element to create and append
         *
         * @return The created and appended child element
         */
        DomElement* appendChild(const QString& name) noexcept;

        /**
         * @brief Create a new text child and append it to the list of childs
         *
         * @tparam T        This type will be converted to the text string.
         *
         * @param name      The tag name (see #isValidTagName() for allowed characters)
         * @param value     The attribute value which will be converted to a QString
         */
        template <typename T>
        DomElement* appendTextChild(const QString& name, const T& value) noexcept {
            QScopedPointer<DomElement> child(new DomElement(name, objectToString(value)));
            appendChild(child.data());
            return child.take();
        }

        /**
         * @brief Get the first child element of this element
         *
         * @param throwIfNotFound   If true and this element has no childs, an exception
         *                          will be thrown
         *
         * @retval DomElement*   The first child element
         * @retval nullptr          If there is no child and "throwIfNotFound == false"
         *
         * @throw Exception If "throwIfNotFound == true" and there is no child element
         */
        DomElement* getFirstChild(bool throwIfNotFound = false) const;

        /**
         * @brief Get the first child element with a specific name
         *
         * @param name              The tag name of the child to search
         * @param throwIfNotFound   If true and this element has no childs with the
         *                          specified name, an exception will be thrown
         *
         * @retval DomElement*   The first child element with the specified name
         * @retval nullptr          If there is no such child and "throwIfNotFound == false"
         *
         * @throw Exception If "throwIfNotFound == true" and there is no such child element
         */
        DomElement* getFirstChild(const QString& name, bool throwIfNotFound) const;

        /**
         * @brief Get the first child element with a specific path/name (recursive)
         *
         * This method is very useful to search a child in a DOM tree recursively.
         *
         * Example:
         * @code{.cpp}
         * // "root" contains the root node of a DOM document (or any other node in a DOM tree)
         * DomElement* root = doc.getRoot();
         * // get the text of the first "category" child of the element "meta/categories".
         * // the return value of getFirstChild() is always a valid pointer as the method
         * // would throw an exception of the specified path or child does not exist.
         * QString value1 = root->getFirstChild("meta/categories/category", true, true)->getText();
         * // the next line does the same, but the character "*" defines that the first
         * // child of "meta/categories" is returned, no matter what tag name it has.
         * // Note: the white space between '/' and '*' is added to avoid compiler warnings
         * // ("...within comment") and must not be added when using this method.
         * QString value2 = root->getFirstChild("meta/categories/ *", true, true)->getText();
         * @endcode
         *
         * @param pathName              The path + name to the child to search. As child
         *                              name you can use "*" to specify that any child
         *                              name is allowed.
         * @param throwIfPathNotExist   If true and the specified path (the left part of
         *                              the last slash in pathName) does not exist
         * @param throwIfChildNotFound  If true and the specified child (the right part of
         *                              the last slash in pathName) does not exist
         *
         * @retval DomElement*   The first child element with the specified path/name
         * @retval nullptr          If there is no such path and "throwIfPathNotExist == false"
         * @retval nullptr          If there is no such child and "throwIfChildNotFound == false"
         *
         * @throw Exception If "throwIfPathNotExist == true" and the path does not exist
         * @throw Exception If "throwIfChildNotFound == true" and the child does not exist
         */
        DomElement* getFirstChild(const QString& pathName, bool throwIfPathNotExist,
                                     bool throwIfChildNotFound) const;

        /**
         * @brief Get the previous child (with a specific name) of a specific child
         *
         * @param child             The specified child
         * @param name              The name of the previous child to search. Use a NULL
         *                          QString (QString()) if the child name is not relevant.
         * @param throwIfNotFound   If true and there is no such previous child, an
         *                          exception will be thrown
         *
         * @retval DomElement*   The previous child (with the specified name)
         * @retval nullptr          If there is no such child and "throwIfNotFound == false"
         *
         * @throw Exception If "throwIfNotFound == true" and there is no such previous child
         */
        DomElement* getPreviousChild(const DomElement* child, const QString& name = QString(),
                                        bool throwIfNotFound = false) const;

        /**
         * @brief Get the next child (with a specific name) of a specific child
         *
         * @param child             The specified child
         * @param name              The name of the next child to search. Use a NULL
         *                          QString (QString()) if the child name is not relevant.
         * @param throwIfNotFound   If true and there is no such next child, an
         *                          exception will be thrown
         *
         * @retval DomElement*   The next child (with the specified name)
         * @retval nullptr          If there is no such child and "throwIfNotFound == false"
         *
         * @throw Exception If "throwIfNotFound == true" and there is no such next child
         */
        DomElement* getNextChild(const DomElement* child, const QString& name = QString(),
                                    bool throwIfNotFound = false) const;


        // Sibling Handling Methods

        /**
         * @brief Get the previous sibling element (with a specific name)
         *
         * @param name              The name of the previous sibling to search. Use a NULL
         *                          QString (QString()) if the child name is not relevant.
         * @param throwIfNotFound   If true and there is no such previous sibling, an
         *                          exception will be thrown
         *
         * @retval DomElement*   The previous sibling element (with the specified name)
         * @retval nullptr          If there is no such sibling and "throwIfNotFound == false"
         *
         * @throw Exception If "throwIfNotFound == true" and there is no such previous sibling
         */
        DomElement* getPreviousSibling(const QString& name = QString(),
                                          bool throwIfNotFound = false) const;

        /**
         * @brief Get the next sibling element (with a specific name)
         *
         * @param name              The name of the next sibling to search. Use a NULL
         *                          QString (QString()) if the child name is not relevant.
         * @param throwIfNotFound   If true and there is no such next sibling, an
         *                          exception will be thrown
         *
         * @retval DomElement*   The next sibling element (with the specified name)
         * @retval nullptr          If there is no such sibling and "throwIfNotFound == false"
         *
         * @throw Exception If "throwIfNotFound == true" and there is no such next sibling
         */
        DomElement* getNextSibling(const QString& name = QString(),
                                      bool throwIfNotFound = false) const;


        // Conversion Methods

        sexpresso::Sexp toSExpressions() const noexcept;

        /**
         * @brief Serialize this DomElement into a QXmlStreamWriter (recursively)
         *
         * @param writer        The QXmlStreamWriter object to write into
         */
        void writeToQXmlStreamWriter(QXmlStreamWriter& writer) const noexcept;

        /**
         * @brief Construct a DomElement object from a QDomElement object (recursively)
         *
         * @param domElement    The QDomElement to copy
         * @param doc           The DOM Document of the newly created DomElement (only
         *                      needed for the root element)
         *
         * @return The created DomElement (the caller takes the ownership!)
         */
        static DomElement* fromQDomElement(QDomElement domElement, DomDocument* doc = nullptr) noexcept;


    private:

        // make some methods inaccessible...
        DomElement() = delete;
        DomElement(const DomElement& other) = delete;
        DomElement& operator=(const DomElement& rhs) = delete;


        // Private Methods

        /**
         * @brief Private constructor to create a DomElement from a QDomElement
         *
         * @param domElement    The QDomElement to copy
         * @param parent        The parent of the newly created DomElement
         * @param doc           The DOM Document of the newly created DomElement (only
         *                      needed for the root element)
         */
        explicit DomElement(QDomElement domElement, DomElement* parent = nullptr,
                               DomDocument* doc = nullptr) noexcept;

        /**
         * @brief Check if a QString represents a valid tag name for elements and attributes
         *
         * Valid characters:
         *  - a-z
         *  - A-Z
         *  - _ (underscore)
         *
         * @param name  The tag name to check
         *
         * @retval true     If valid
         * @retval false    If invalid
         */
        static bool isValidTagName(const QString& name) noexcept;

        /**
         * @brief Serialization template method
         *
         * @tparam T    Type of the object to be serialized
         *
         * @param obj   Input object
         *
         * @return      Output string
         */
        template <typename T>
        static QString objectToString(const T& obj) noexcept;

        /**
         * @brief Deserialization template method
         *
         * @tparam T            Type of the object to be deserialized
         *
         * @param str           Input string
         * @param throwIfEmpty  If true and the string is empty, an exception will be thrown.
         *                      If false and the string is empty, defaultValue will be returned.
         *
         * @retval T            The created element of type T
         * @retval defaultValue If the string is empty and "throwIfEmpty == false"
         *
         * @throws Exception if an error occurs
         */
        template <typename T>
        static T stringToObject(const QString& str, bool throwIfEmpty, const T& defaultValue = T());


        // Attributes
        DomDocument* mDocument;  ///< the DOM document of the tree (only needed in the root node, otherwise nullptr)
        DomElement* mParent;     ///< the parent element (if available, otherwise nullptr)
        QString mName;              ///< the tag name of this element
        QString mText;              ///< the text of this element (only if there are no childs)
        QList<DomElement*> mChilds;      ///< all child elements (only if there is no text)
        QMap<QString, QString> mAttributes; ///< all attributes of this element (key, value) in alphabetical order
};

/*****************************************************************************************
 *  Serialization Methods
 ****************************************************************************************/

template <>
inline QString DomElement::objectToString(const QString& obj) noexcept {
    return obj;
}

template <>
inline QString DomElement::objectToString(const char* const& obj) noexcept {
    return QString(obj);
}

template <>
inline QString DomElement::objectToString(const bool& obj) noexcept {
    return obj ? QString("true") : QString("false");
}

template <>
inline QString DomElement::objectToString(const int& obj) noexcept {
    return QString::number(obj);
}

template <>
inline QString DomElement::objectToString(const uint& obj) noexcept {
    return QString::number(obj);
}

template <>
inline QString DomElement::objectToString(const QColor& obj) noexcept {
    return obj.isValid() ? obj.name(QColor::HexArgb) : "";
}

template <>
inline QString DomElement::objectToString(const QUrl& obj) noexcept {
    return obj.isValid() ? obj.toString(QUrl::PrettyDecoded) : "";
}

template <>
inline QString DomElement::objectToString(const QDateTime& obj) noexcept {
    return obj.toUTC().toString(Qt::ISODate);
}

// all other types need to have a method "QString serializeToString() const noexcept"
template <typename T>
inline QString DomElement::objectToString(const T& obj) noexcept {
    return obj.serializeToString();
}

/*****************************************************************************************
 *  Deserialization Methods
 ****************************************************************************************/

template <>
inline QString DomElement::stringToObject(const QString& str, bool throwIfEmpty, const QString& defaultValue) {
    Q_UNUSED(defaultValue);
    Q_ASSERT(defaultValue == QString()); // defaultValue makes no sense in this method
    if (str.isEmpty() && throwIfEmpty) {
        throw RuntimeError(__FILE__, __LINE__, tr("String is empty."));
    }
    return str;
}

template <>
inline bool DomElement::stringToObject(const QString& str, bool throwIfEmpty, const bool& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    if      (s == "true")   { return true;          }
    else if (s == "false")  { return false;         }
    else if (s.isEmpty())   { return defaultValue;  }
    else { throw RuntimeError(__FILE__, __LINE__, tr("Not a valid boolean.")); }
}

template <>
inline int DomElement::stringToObject(const QString& str, bool throwIfEmpty, const int& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    bool ok = false;
    int value = s.toInt(&ok);
    if      (ok)            { return value;         }
    else if (s.isEmpty())   { return defaultValue;  }
    else { throw RuntimeError(__FILE__, __LINE__, tr("Not a valid integer.")); }
}

template <>
inline uint DomElement::stringToObject(const QString& str, bool throwIfEmpty, const uint& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    bool ok = false;
    uint value = s.toUInt(&ok);
    if (ok)                 { return value;         }
    else if (s.isEmpty())   { return defaultValue;  }
    else { throw RuntimeError(__FILE__, __LINE__, tr("Not a valid unsigned integer.")); }
}

template <>
inline QDateTime DomElement::stringToObject(const QString& str, bool throwIfEmpty, const QDateTime& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    QDateTime obj = QDateTime::fromString(s, Qt::ISODate).toLocalTime();
    if (obj.isValid())      { return obj;           }
    else if (s.isEmpty())   { return defaultValue;  }
    else { throw RuntimeError(__FILE__, __LINE__, tr("Not a valid datetime.")); }
}

template <>
inline QColor DomElement::stringToObject(const QString& str, bool throwIfEmpty, const QColor& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    QColor obj(s);
    if (obj.isValid())      { return obj;           }
    else if (s.isEmpty())   { return defaultValue;  }
    else { throw RuntimeError(__FILE__, __LINE__, tr("Not a valid color.")); }
}

template <>
inline QUrl DomElement::stringToObject(const QString& str, bool throwIfEmpty, const QUrl& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    QUrl obj(s, QUrl::StrictMode);
    if (obj.isValid())      { return obj;           }
    else if (s.isEmpty())   { return defaultValue;  }
    else { throw RuntimeError(__FILE__, __LINE__, tr("Not a valid URL.")); }
}

// all other types need to have a static method "T deserializeFromString(const QString& str)"
template <typename T>
inline T DomElement::stringToObject(const QString& str, bool throwIfEmpty, const T& defaultValue) {
    QString s = stringToObject<QString>(str, throwIfEmpty);
    try {
        return T::deserializeFromString(str); // can throw
    } catch (const Exception& e) {
        if (str.isEmpty()) {
            return defaultValue;
        } else {
            throw;
        }
    }
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace librepcb

#endif // LIBREPCB_DOMELEMENT_H
