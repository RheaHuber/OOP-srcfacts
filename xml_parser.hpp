/*

    xml_parser.hpp

    Include file for low-level XML parsing functions

*/

#ifndef XML_PARSER_HPP
#define XML_PARSER_HPP

#include <string>
#include <optional>

/*
    Traces the start of an XML document
    Input: N/A
    Output: Trace
*/
void beginParsing();

/*
    Traces the end of an XML document
    Input: N/A
    Output: Trace
*/
void endParsing();

/*
    Returns if the parser is inside a namespace
    Input: Start iterator of segment to parse; inTag
    Output: Bool value for if the iterator is inside a namespace
*/
bool isNamespace(std::string::const_iterator &cursor, bool &inTag);

/*
    Returns if the parser is inside an XML comment
    AND is NOT inside a namespace OR attribute
    Input: Start iterator of segment to parse; inXMLComment
    Output: Bool value for if the iterator is inside an XML comment
*/
bool isXMLComment(std::string::const_iterator &cursor, bool &inXMLComment);

/*
    Returns if the parser is inside CDATA
    AND is NOT inside a namespace, attribute, OR XML comment
    Input: Start iterator of segment to parse; inCDATA
    Output: Bool value for if the iterator is inside CDATA
*/
bool isCDATA(std::string::const_iterator &cursor, bool &inCDATA);

/*
    Returns if the parser is inside an XML declaration
    AND is NOT inside a namespace, attribute, XML comment, OR CDATA
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an XML declaration
*/
bool isXMLDecl(std::string::const_iterator &cursor);

/*
    Returns if the parser is inside a processing instruction
    AND is NOT inside a namespace, attribute, XML comment, CDATA, OR XML declaration
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a processing instruction
*/
bool isProcessInstr(std::string::const_iterator &cursor);

/*
    Returns if the parser is inside an end tag
    AND is NOT inside a namespace, attribute, XML comment, CDATA, XML declaration, OR processing instruction
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an end tag
*/
bool isEndTag(std::string::const_iterator &cursor);

/*
    Returns if the parser is inside a start tag
    AND is NOT inside a namespace, attribute, XML comment, CDATA, XML declaration, processing instruction, OR end tag
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a start tag
*/
bool isStartTag(std::string::const_iterator &cursor);

/*
    Parses an xml namespace
    Input: Start and end iterators of segment to parse; inTag, depth, inTagPrefix, inTagQName, and inTagLocalName; and prefix and uri variables by ref
    Output: Parsed prefix and uri by ref; adjusted inTag and depth values by ref
*/
void parseNamespace(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, bool &inTag, int &depth, const std::string_view &inTagPrefix, const std::string &inTagQName, const std::string_view &inTagLocalName, std::string_view &prefix, std::string_view &uri);

/*
    Parses an xml attribute
    Input: Start and end iterators of segment to parse; nameEnd, inTag, depth, inTagPrefix, inTagQName, and inTagLocalName; and qName, prefix, localName, and value variables by ref
    Output: qName, prefix, localName, and value by ref; adjusted inTag and depth values by ref
*/
void parseAttribute(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, const std::string::const_iterator &nameEnd, bool &inTag, int &depth, const std::string_view &inTagPrefix, const std::string &inTagQName, const std::string_view &inTagLocalName, std::string_view &qName, std::string_view &prefix, std::string_view &localName, std::string_view &value);

/*
    Parses an xml comment
    Input: Start and end iterators of segment to parse; inXMLComment; comment variable by ref
    Output: Parsed comment by ref
*/
void parseComment(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, bool &inXMLComment, std::string_view &comment);

/*
    Parses xml CDATA
    Input: Start and end iterators of segment to parse; inCDATA; characters variable by ref
    Output: Parsed characters by ref
*/
void parseCDATA(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, bool &inCDATA, std::string_view &characters);

/*
    Parses xml declaration
    Input: Start and end iterators of segment to parse; startXMLDecl, endXMLDecl, and tagEnd; version, encoding, and standalone variables by ref
    Output: Parsed version, encoding, and standalone as ref
*/
void parseDeclaration(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, const std::string_view &startXMLDecl, const std::string_view &endXMLDecl, std::string::const_iterator &tagEnd, std::string_view &version, std::optional<std::string_view> &encoding, std::optional<std::string_view> &standalone);

/*
    Parses xml processing instruction
    Input: Start and end iterators of segment to parse; tagEnd and nameEnd ; target and data variables by ref
    Output: Parsed target and data as ref
*/
void parseProcessInstr(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd, std::string::const_iterator &nameEnd, std::string_view &target, std::string_view &data);

/*
    Parses xml end tag
    Input: Start and end iterators of segment to parse; nameEnd, colonPosition, and depth; prefix, qName, and localName variables by ref
    Output: Parsed prefix, qName, and localName as ref; adjusted depth value by ref
*/
void parseEndTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &nameEnd, size_t &colonPosition, int &depth, std::string_view &prefix, std::string_view &qName, std::string_view &localName);

/*
    Parses xml start tag
    Input: Start and end iterators of segment to parse; nameEnd and colonPosition; prefix, qName, and localName variables by ref
    Output: Parsed prefix, qName, and localName as ref
*/
void parseStartTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &nameEnd, size_t &colonPosition, std::string_view &prefix, std::string_view &qName, std::string_view &localName);

/*
    Parses character entity references
    Input: Start iterator of segment to parse; characters variable by ref
    Output: Parsed characters as ref
*/
void parseCharEntity(std::string::const_iterator &cursor, std::string_view &characters);

/*
    Parses character non-entity references
    Input: Start iterator of segment to parse; tagEnd; characters variable by ref
    Output: Parsed characters as ref
*/
void parseCharNonEntity(std::string::const_iterator &cursor, const std::string::const_iterator &tagEnd, std::string_view &characters);

#endif
