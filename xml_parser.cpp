/*

    xml_parser.cpp

    Implementation file for low-level XML parsing functions

*/

#include "xml_parser.hpp"
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <cstring>

// provides literal string operator""sv
using namespace std::literals::string_view_literals;

// trace parsing
#ifdef TRACE
#undef TRACE
#define HEADER(m) std::clog << std::setw(10) << std::left << m <<"\t"
#define FIELD(l, n) l << ":|" << n << "| "
#define TRACE0(m)
#define TRACE1(m, l1, n1) HEADER(m) << FIELD(l1,n1) << '\n';
#define TRACE2(m, l1, n1, l2, n2) HEADER(m) << FIELD(l1,n1) << FIELD(l2,n2) << '\n';
#define TRACE3(m, l1, n1, l2, n2, l3, n3) HEADER(m) << FIELD(l1,n1) << FIELD(l2,n2) << FIELD(l3,n3) << '\n';
#define TRACE4(m, l1, n1, l2, n2, l3, n3, l4, n4) HEADER(m) << FIELD(l1,n1) << FIELD(l2,n2) << FIELD(l3,n3) << FIELD(l4,n4) << '\n';
#define GET_TRACE(_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME
#define TRACE(...) GET_TRACE(__VA_ARGS__, TRACE4, _UNUSED, TRACE3, _UNUSED, TRACE2, _UNUSED, TRACE1, _UNUSED, TRACE0)(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/*
    Traces the start of an XML document
    Input: N/A
    Output: Trace
*/
void beginParsing(){
    TRACE("START DOCUMENT");

    return;
}

/*
    Traces the end of an XML document
    Input: N/A
    Output: Trace
*/
void endParsing(){
    TRACE("END DOCUMENT");

    return;
}

/*
    Returns if the parser is inside a namespace
    Input: Start iterator of segment to parse; inTag
    Output: Bool value for if the iterator is inside a namespace
*/
bool isNamespace(std::string::const_iterator &cursor, bool &inTag){
    return (inTag && (strncmp(std::addressof(*cursor), "xmlns", 5) == 0) && (cursor[5] == ':' || cursor[5] == '='));
}

/*
    Returns if the parser is inside an XML comment
    AND is NOT inside a namespace OR attribute
    Input: Start iterator of segment to parse; inXMLComment
    Output: Bool value for if the iterator is inside an XML comment
*/
bool isXMLComment(std::string::const_iterator &cursor, bool &inXMLComment){
    return (inXMLComment || (cursor[1] == '!' && *cursor == '<' && cursor[2] == '-' && cursor[3] == '-'));
}

/*
    Returns if the parser is inside CDATA
    AND is NOT inside a namespace, attribute, OR XML comment
    Input: Start iterator of segment to parse; inCDATA
    Output: Bool value for if the iterator is inside CDATA
*/
bool isCDATA(std::string::const_iterator &cursor, bool &inCDATA){
    return (inCDATA || (cursor[1] == '!' && *cursor == '<' && cursor[2] == '[' && (strncmp(std::addressof(cursor[3]), "CDATA[", 6) == 0)));
}

/*
    Returns if the parser is inside an XML declaration
    AND is NOT inside a namespace, attribute, XML comment, OR CDATA
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an XML declaration
*/
bool isXMLDecl(std::string::const_iterator &cursor){
    return (cursor[1] == '?' && *cursor == '<' && (strncmp(std::addressof(*cursor), "<?xml ", 6) == 0));
}

/*
    Returns if the parser is inside a processing instruction
    AND is NOT inside a namespace, attribute, XML comment, CDATA, OR XML declaration
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a processing instruction
*/
bool isProcessInstr(std::string::const_iterator &cursor){
    return (cursor[1] == '?' && *cursor == '<');
}

/*
    Returns if the parser is inside an end tag
    AND is NOT inside a namespace, attribute, XML comment, CDATA, XML declaration, OR processing instruction
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an end tag
*/
bool isEndTag(std::string::const_iterator &cursor){
    return (cursor[1] == '/' && *cursor == '<');
}

/*
    Returns if the parser is inside a start tag
    AND is NOT inside a namespace, attribute, XML comment, CDATA, XML declaration, processing instruction, OR end tag
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a start tag
*/
bool isStartTag(std::string::const_iterator &cursor){
    return (*cursor == '<');
}

/*
    Parses an xml namespace
    Input: Start and end iterators of segment to parse; inTag, depth, inTagPrefix, inTagQName, and inTagLocalName; and prefix and uri variables by ref
    Output: Parsed prefix and uri by ref; adjusted inTag and depth values by ref
*/
void parseNamespace(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, bool &inTag, int &depth, const std::string_view &inTagPrefix, const std::string &inTagQName, const std::string_view &inTagLocalName, std::string_view &prefix, std::string_view &uri){
    std::advance(cursor, 5);
    const auto nameEnd = std::find(cursor, cursorEnd, '=');
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    int prefixSize = 0;
    if (*cursor == ':') {
        std::advance(cursor, 1);
        prefixSize = std::distance(cursor, nameEnd);
    }
    const std::string_view prefixTemp(std::addressof(*cursor), prefixSize);
    prefix = prefixTemp;
    cursor = std::next(nameEnd);
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (cursor == cursorEnd) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    const char delimiter = *cursor;
    if (delimiter != '"' && delimiter != '\'') {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    std::advance(cursor, 1);
    const auto valueEnd = std::find(cursor, cursorEnd, delimiter);
    if (valueEnd == cursorEnd) {
        std::cerr << "parser error : incomplete namespace\n";
        exit(1);
    }
    const std::string_view uriTemp(std::addressof(*cursor), std::distance(cursor, valueEnd));
    uri = uriTemp;
    cursor = std::next(valueEnd);
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
    TRACE("NAMESPACE", "prefix", prefix, "uri", uri);
    if (*cursor == '>') {
        std::advance(cursor, 1);
        inTag = false;
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", inTagPrefix, "qName", inTagQName, "localName", inTagLocalName);
        inTag = false;
    }

    return;
}

/*
    Parses an xml attribute
    Input: Start and end iterators of segment to parse; nameEnd, inTag, depth, inTagPrefix, inTagQName, and inTagLocalName; and qName, prefix, localName, and value variables by ref
    Output: qName, prefix, localName, and value by ref; adjusted inTag and depth values by ref
*/
void parseAttribute(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, const std::string::const_iterator &nameEnd, bool &inTag, int &depth, const std::string_view &inTagPrefix, const std::string &inTagQName, const std::string_view &inTagLocalName, std::string_view &qName, std::string_view &prefix, std::string_view &localName, std::string_view &value){
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : Empty attribute name" << '\n';
        exit(1);
    }
    const std::string_view qNameTemp(std::addressof(*cursor), std::distance(cursor, nameEnd));
    qName = qNameTemp;
    size_t colonPosition = qNameTemp.find(':');
    if (colonPosition == 0) {
        std::cerr << "parser error : Invalid attribute name " << qNameTemp << '\n';
        exit(1);
    }
    if (colonPosition == std::string::npos)
        colonPosition = 0;
    const std::string_view prefixTemp(std::addressof(*qNameTemp.cbegin()), colonPosition);
    prefix = prefixTemp;
    if (colonPosition != 0)
        colonPosition += 1;
    const std::string_view localNameTemp(std::addressof(*qNameTemp.cbegin()) + colonPosition, qNameTemp.size() - colonPosition);
    localName = localNameTemp;
    cursor = nameEnd;
    if (isspace(*cursor))
        cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (cursor == cursorEnd) {
        std::cerr << "parser error : attribute " << qNameTemp << " incomplete attribute\n";
        exit(1);
    }
    if (*cursor != '=') {
        std::cerr << "parser error : attribute " << qNameTemp << " missing =\n";
        exit(1);
    }
    std::advance(cursor, 1);
    if (isspace(*cursor))
        cursor = std::find_if_not(cursor, cursorEnd, isspace);
    const char delimiter = *cursor;
    if (delimiter != '"' && delimiter != '\'') {
        std::cerr << "parser error : attribute " << qNameTemp << " missing delimiter\n";
        exit(1);
    }
    std::advance(cursor, 1);
    auto valueEnd = std::find(cursor, cursorEnd, delimiter);
    if (valueEnd == cursorEnd) {
        std::cerr << "parser error : attribute " << qNameTemp << " missing delimiter\n";
        exit(1);
    }
    const std::string_view valueTemp(std::addressof(*cursor), std::distance(cursor, valueEnd));
    value = valueTemp;
    cursor = std::next(valueEnd);
    TRACE("ATTRIBUTE", "prefix", prefix, "qname", qName, "localName", localName, "value", value);
    if (isspace(*cursor))
        cursor = std::find_if_not(std::next(cursor), cursorEnd, isspace);
    if (*cursor == '>') {
        std::advance(cursor, 1);
        inTag = false;
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", inTagPrefix, "qName", inTagQName, "localName", inTagLocalName);
        inTag = false;
    }

    return;
}

/*
    Parses an xml comment
    Input: Start and end iterators of segment to parse; inXMLComment; comment variable by ref
    Output: Parsed comment by ref
*/
void parseComment(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, bool &inXMLComment, std::string_view &comment){
    if (cursor == cursorEnd) {
        std::cerr << "parser error : Unterminated XML comment\n";
        exit(1);
    }
    if (!inXMLComment)
        std::advance(cursor, 4);
    constexpr std::string_view endComment = "-->"sv;
    auto tagEnd = std::search(cursor, cursorEnd, endComment.begin(), endComment.end());
    inXMLComment = tagEnd == cursorEnd;
    const std::string_view commentTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    comment = commentTemp;
    if (!inXMLComment)
        cursor = std::next(tagEnd, endComment.size());
    else{
        cursor = tagEnd;
    }
    TRACE("COMMENT", "comment", comment);

    return;
}

/*
    Parses xml CDATA
    Input: Start and end iterators of segment to parse; inCDATA; characters variable by ref
    Output: Parsed characters by ref
*/
void parseCDATA(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, bool &inCDATA, std::string_view &characters){
    if (cursor == cursorEnd) {
        std::cerr << "parser error : Unterminated CDATA\n";
        exit(1);
    }
    constexpr std::string_view endCDATA = "]]>"sv;
    if (!inCDATA)
        std::advance(cursor, 9);
    auto tagEnd = std::search(cursor, cursorEnd, endCDATA.begin(), endCDATA.end());
    inCDATA = tagEnd == cursorEnd;
    const std::string_view charactersTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    characters = charactersTemp;
    cursor = std::next(tagEnd, endCDATA.size());
    if (!inCDATA)
        cursor = std::next(tagEnd, endCDATA.size());
    else{
        cursor = tagEnd;
    }
    TRACE("CDATA", "characters", characters);

    return;
}

/*
    Parses xml declaration
    Input: Start and end iterators of segment to parse; startXMLDecl, endXMLDecl, and tagEnd; version, encoding, and standalone variable by ref
    Output: Parsed version, encoding, and standalone as ref
*/
void parseDeclaration(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, const std::string_view &startXMLDecl, const std::string_view &endXMLDecl, std::string::const_iterator &tagEnd, std::string_view &version, std::optional<std::string_view> &encoding, std::optional<std::string_view> &standalone){
    // parse required version
    if (cursor == tagEnd) {
        std::cerr << "parser error: Missing space after before version in XML declaration\n";
        exit(1);
    }
    auto nameEnd = std::find(cursor, tagEnd, '=');
    const std::string_view attr(std::addressof(*cursor), std::distance(cursor, nameEnd));
    cursor = std::next(nameEnd);
    const char delimiter = *cursor;
    if (delimiter != '"' && delimiter != '\'') {
        std::cerr << "parser error: Invalid start delimiter for version in XML declaration\n";
        exit(1);
    }
    std::advance(cursor, 1);
    auto valueEnd = std::find(cursor, tagEnd, delimiter);
    if (valueEnd == tagEnd) {
        std::cerr << "parser error: Invalid end delimiter for version in XML declaration\n";
        exit(1);
    }
    if (attr != "version"sv) {
        std::cerr << "parser error: Missing required first attribute version in XML declaration\n";
        exit(1);
    }
    const std::string_view versionTemp(std::addressof(*cursor), std::distance(cursor, valueEnd));
    version = versionTemp;
    cursor = std::next(valueEnd);
    cursor = std::find_if_not(cursor, tagEnd, isspace);

    // parse optional encoding and standalone attributes
    std::optional<std::string_view> encodingTemp;
    std::optional<std::string_view> standaloneTemp;
    if (cursor != (tagEnd - 1)) {
        nameEnd = std::find(cursor, tagEnd, '=');
        if (nameEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute in XML declaration\n";
            exit(1);
        }
        const std::string_view attr2(std::addressof(*cursor), std::distance(cursor, nameEnd));
        cursor = std::next(nameEnd);
        char delimiter2 = *cursor;
        if (delimiter2 != '"' && delimiter2 != '\'') {
            std::cerr << "parser error: Invalid end delimiter for attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        std::advance(cursor, 1);
        valueEnd = std::find(cursor, tagEnd, delimiter2);
        if (valueEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        if (attr2 == "encoding"sv) {
            encodingTemp = std::string_view(std::addressof(*cursor), std::distance(cursor, valueEnd));
        } else if (attr2 == "standalone"sv) {
            standaloneTemp = std::string_view(std::addressof(*cursor), std::distance(cursor, valueEnd));
        } else {
            std::cerr << "parser error: Invalid attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        cursor = std::next(valueEnd);
        cursor = std::find_if_not(cursor, tagEnd, isspace);
    }
    if (cursor != (tagEnd - endXMLDecl.size() + 1)) {
        nameEnd = std::find(cursor, tagEnd, '=');
        if (nameEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute in XML declaration\n";
            exit(1);
        }
        const std::string_view attr2(std::addressof(*cursor), std::distance(cursor, nameEnd));
        cursor = std::next(nameEnd);
        const char delimiter2 = *cursor;
        if (delimiter2 != '"' && delimiter2 != '\'') {
            std::cerr << "parser error: Invalid end delimiter for attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        std::advance(cursor, 1);
        valueEnd = std::find(cursor, tagEnd, delimiter2);
        if (valueEnd == tagEnd) {
            std::cerr << "parser error: Incomplete attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        if (!standaloneTemp && attr2 == "standalone"sv) {
            standaloneTemp = std::string_view(std::addressof(*cursor), std::distance(cursor, valueEnd));
        } else {
            std::cerr << "parser error: Invalid attribute " << attr2 << " in XML declaration\n";
            exit(1);
        }
        cursor = std::next(valueEnd);
        cursor = std::find_if_not(cursor, tagEnd, isspace);
    }
    encoding = encodingTemp;
    standalone = standaloneTemp;
    std::advance(cursor, endXMLDecl.size());
    cursor = std::find_if_not(cursor, cursorEnd, isspace);
    TRACE("XML DECLARATION", "version", version, "encoding", (encoding ? *encoding : ""), "standalone", (standalone ? *standalone : ""));

    return;
}

/*
    Parses xml processing instruction
    Input: Start and end iterators of segment to parse; tagEnd and nameEnd ; target and data variable by ref
    Output: Parsed target and data as ref
*/
void parseProcessInstr(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd, std::string::const_iterator &nameEnd, std::string_view &target, std::string_view &data){
    if (nameEnd == tagEnd) {
        std::cerr << "parser error : Unterminated processing instruction '" << std::string_view(std::addressof(*cursor), std::distance(cursor, nameEnd)) << "'\n";
        exit(1);
    }
    const std::string_view targetTemp(std::addressof(*cursor), std::distance(cursor, nameEnd));
    target = targetTemp;
    cursor = std::find_if_not(nameEnd, tagEnd, isspace);
    const std::string_view dataTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    data = dataTemp;
    cursor = tagEnd;
    std::advance(cursor, 2);
    TRACE("PI", "target", target, "data", data);

    return;
}

/*
    Parses xml end tag
    Input: Start and end iterators of segment to parse; nameEnd, colonPosition, and depth; prefix, qName, and localName variables by ref
    Output: Parsed prefix, qName, and localName as ref; adjusted depth value by ref
*/
void parseEndTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &nameEnd, size_t &colonPosition, int &depth, std::string_view &prefix, std::string_view &qName, std::string_view &localName){
    const std::string_view prefixTemp(std::addressof(*cursor), colonPosition);
    prefix = prefixTemp;
    const std::string_view qNameTemp(std::addressof(*cursor), std::distance(cursor, nameEnd));
    qName = qNameTemp;
    if (qNameTemp.empty()) {
        std::cerr << "parser error: EndTag: invalid element name\n";
        exit(1);
    }
    if (colonPosition)
        ++colonPosition;
    const std::string_view localNameTemp(std::addressof(*cursor) + colonPosition, std::distance(cursor, nameEnd) - colonPosition);
    localName = localNameTemp;
    cursor = std::next(nameEnd);
    --depth;
    TRACE("END TAG", "prefix", prefix, "qName", qName, "localName", localName);

    return;
}

/*
    Parses xml start tag
    Input: Start and end iterators of segment to parse; nameEnd and colonPosition; prefix, qName, and localName variables by ref
    Output: Parsed prefix, qName, and localName as ref
*/
void parseStartTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &nameEnd, size_t &colonPosition, std::string_view &prefix, std::string_view &qName, std::string_view &localName){
    const std::string_view prefixTemp(std::addressof(*cursor), colonPosition);
    prefix = prefixTemp;
    const std::string_view qNameTemp(std::addressof(*cursor), std::distance(cursor, nameEnd));
    qName = qNameTemp;
    if (qName.empty()) {
        std::cerr << "parser error: StartTag: invalid element name\n";
        exit(1);
    }
    if (colonPosition)
        ++colonPosition;
    const std::string_view localNameTemp(std::addressof(*cursor) + colonPosition, std::distance(cursor, nameEnd) - colonPosition);
    localName = localNameTemp;
    cursor = nameEnd;
    TRACE("START TAG", "prefix", prefix, "qName", qName, "localName", localName);
    if (*cursor != '>')
        cursor = std::find_if_not(cursor, cursorEnd, isspace);
    if (*cursor != '>' && (*cursor == '/' && cursor[1] == '>'))
        TRACE("END TAG", "prefix", prefix, "qName", qName, "localName", localName);

    return;
}

/*
    Parses character entity references
    Input: Start iterator of segment to parse; characters variable by ref
    Output: Parsed characters as ref
*/
void parseCharEntity(std::string::const_iterator &cursor, std::string_view &characters){
    if (cursor[1] == 'l' && cursor[2] == 't' && cursor[3] == ';') {
        characters = "<";
        std::advance(cursor, 4);
    } else if (cursor[1] == 'g' && cursor[2] == 't' && cursor[3] == ';') {
        characters = ">";
        std::advance(cursor, 4);
    } else if (cursor[1] == 'a' && cursor[2] == 'm' && cursor[3] == 'p' && cursor[4] == ';') {
        characters = "&";
        std::advance(cursor, 5);
    } else {
        characters = "&";
        std::advance(cursor, 1);
    }
    TRACE("ENTITYREF", "characters", characters);

    return;
}

/*
    Parses character non-entity references
    Input: Start iterator of segment to parse; tagEnd; characters variable by ref
    Output: Parsed characters as ref
*/
void parseCharNonEntity(std::string::const_iterator &cursor, const std::string::const_iterator &tagEnd, std::string_view &characters){
    const std::string_view charactersTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    characters = charactersTemp;
    std::advance(cursor, characters.size());
    TRACE("CHARACTERS", "characters", characters);

    return;
}
