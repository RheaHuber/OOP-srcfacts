/*

    XMLParser.cpp

    Implementation file for XML parsing class

*/

#include "XMLParser.hpp"
#include "refillBuffer.hpp"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <bitset>

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
void XMLParser::beginParsing(){
    TRACE("START DOCUMENT");

    return;
}

/*
    Traces the end of an XML document
    Input: N/A
    Output: Trace
*/
void XMLParser::endParsing(){
    TRACE("END DOCUMENT");

    return;
}

/*
    Returns if the parser is inside a namespace
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a namespace
*/
bool XMLParser::isNamespace(std::string::const_iterator &cursor){
    return (XMLParser::inTag && (strncmp(std::addressof(*cursor), "xmlns", 5) == 0) && (cursor[5] == ':' || cursor[5] == '='));
}

/*
    Returns if the parser is inside an attribute
    Input: N/A
    Output: Bool value for if the iterator is inside an attribute
*/
bool XMLParser::isAttribute(){
    return XMLParser::inTag;
}

/*
    Returns if the parser is inside an XML comment
    AND is NOT inside a namespace OR attribute
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an XML comment
*/
bool XMLParser::isXMLComment(std::string::const_iterator &cursor){
    return (XMLParser::inXMLComment || (cursor[1] == '!' && *cursor == '<' && cursor[2] == '-' && cursor[3] == '-'));
}

/*
    Returns if the parser is inside CDATA
    AND is NOT inside a namespace, attribute, OR XML comment
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside CDATA
*/
bool XMLParser::isCDATA(std::string::const_iterator &cursor){
    return (XMLParser::inCDATA || (cursor[1] == '!' && *cursor == '<' && cursor[2] == '[' && (strncmp(std::addressof(cursor[3]), "CDATA[", 6) == 0)));
}

/*
    Returns if the parser is inside an XML declaration
    AND is NOT inside a namespace, attribute, XML comment, OR CDATA
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an XML declaration
*/
bool XMLParser::isXMLDecl(std::string::const_iterator &cursor){
    return (cursor[1] == '?' && *cursor == '<' && (strncmp(std::addressof(*cursor), "<?xml ", 6) == 0));
}

/*
    Returns if the parser is inside a processing instruction
    AND is NOT inside a namespace, attribute, XML comment, CDATA, OR XML declaration
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a processing instruction
*/
bool XMLParser::isProcessInstruction(std::string::const_iterator &cursor){
    return (cursor[1] == '?' && *cursor == '<');
}

/*
    Returns if the parser is inside an end tag
    AND is NOT inside a namespace, attribute, XML comment, CDATA, XML declaration, OR processing instruction
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside an end tag
*/
bool XMLParser::isEndTag(std::string::const_iterator &cursor){
    return (cursor[1] == '/' && *cursor == '<');
}

/*
    Returns if the parser is inside a start tag
    AND is NOT inside a namespace, attribute, XML comment, CDATA, XML declaration, processing instruction, OR end tag
    Input: Start iterator of segment to parse
    Output: Bool value for if the iterator is inside a start tag
*/
bool XMLParser::isStartTag(std::string::const_iterator &cursor){
    return (*cursor == '<');
}

/*
    Parses an xml namespace
    Input: Start and end iterators of segment to parse; depth; and prefix and uri variables by ref
    Output: Parsed prefix and uri by ref; adjusted depth value by ref
*/
void XMLParser::parseNamespace(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, int &depth, std::string_view &prefix, std::string_view &uri){
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
        XMLParser::inTag = false;
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", XMLParser::inTagPrefix, "qName", XMLParser::inTagQName, "localName", XMLParser::inTagLocalName);
        XMLParser::inTag = false;
    }

    return;
}

/*
    Parses an xml attribute
    Input: Start and end iterators of segment to parse; depth; and qName, prefix, localName, and value variables by ref
    Output: qName, prefix, localName, and value by ref; adjusted depth value by ref
*/
void XMLParser::parseAttribute(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, int &depth, std::string_view &qName, std::string_view &prefix, std::string_view &localName, std::string_view &value){
    const auto nameEnd = std::find_if_not(cursor, cursorEnd, [&] (char c) { return tagNameMask[c]; });
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
        XMLParser::inTag = false;
        ++depth;
    } else if (*cursor == '/' && cursor[1] == '>') {
        std::advance(cursor, 2);
        TRACE("END TAG", "prefix", XMLParser::inTagPrefix, "qName", XMLParser::inTagQName, "localName", XMLParser::inTagLocalName);
        XMLParser::inTag = false;
    }

    return;
}

/*
    Parses an xml comment
    Input: Start and end iterators of segment to parse; comment variable by ref
    Output: Parsed comment by ref
*/
void XMLParser::parseComment(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &comment){
    if (cursor == cursorEnd) {
        std::cerr << "parser error : Unterminated XML comment\n";
        exit(1);
    }
    if (!XMLParser::inXMLComment)
        std::advance(cursor, 4);
    constexpr std::string_view endComment = "-->"sv;
    auto tagEnd = std::search(cursor, cursorEnd, endComment.begin(), endComment.end());
    XMLParser::inXMLComment = tagEnd == cursorEnd;
    const std::string_view commentTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    comment = commentTemp;
    if (!XMLParser::inXMLComment)
        cursor = std::next(tagEnd, endComment.size());
    else{
        cursor = tagEnd;
    }
    TRACE("COMMENT", "comment", comment);

    return;
}

/*
    Parses xml CDATA
    Input: Start and end iterators of segment to parse; characters variable by ref
    Output: Parsed characters by ref
*/
void XMLParser::parseCDATA(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &characters){
    if (cursor == cursorEnd) {
        std::cerr << "parser error : Unterminated CDATA\n";
        exit(1);
    }
    constexpr std::string_view endCDATA = "]]>"sv;
    if (!XMLParser::inCDATA)
        std::advance(cursor, 9);
    auto tagEnd = std::search(cursor, cursorEnd, endCDATA.begin(), endCDATA.end());
    XMLParser::inCDATA = tagEnd == cursorEnd;
    const std::string_view charactersTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    characters = charactersTemp;
    cursor = std::next(tagEnd, endCDATA.size());
    if (!XMLParser::inCDATA)
        cursor = std::next(tagEnd, endCDATA.size());
    else{
        cursor = tagEnd;
    }
    TRACE("CDATA", "characters", characters);

    return;
}

/*
    Verifies the XML declaration is complete
    Input: Start and end iterators of segment to parse; tagEnd
    Output: If declaration is incomplete, outputs error message and exits
*/
void XMLParser::validateDeclaration(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd){
    if ((tagEnd = std::find(cursor, cursorEnd, '>')) == cursorEnd) {
        std::cerr << "parser error: Incomplete XML declaration\n";
        exit(1);
    }

    return;
}

/*
    Parses xml declaration
    Input: Start and end iterators of segment to parse; startXMLDecl, endXMLDecl, and tagEnd; version, encoding, and standalone variables by ref
    Output: Parsed version, encoding, and standalone as ref
*/
void XMLParser::parseDeclaration(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view startXMLDecl, std::string_view endXMLDecl, std::string::const_iterator &tagEnd, std::string_view &version, std::optional<std::string_view> &encoding, std::optional<std::string_view> &standalone){
    std::advance(cursor, startXMLDecl.size());
    cursor = std::find_if_not(cursor, tagEnd, isspace);

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
    Verifies the processing instruction is complete 
    Input: Start and end iterators of segment to parse; tagEnd, endPI
    Output: If instruction is incomplete, outputs error message and exits
*/
void XMLParser::validateProcessInstruction(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd, std::string_view endPI){
    if ((tagEnd = std::search(cursor, cursorEnd, endPI.begin(), endPI.end())) == cursorEnd) {
        std::cerr << "parser error: Incomplete processing instruction\n";
        exit(1);
    }

    return;
}

/*
    Parses xml processing instruction
    Input: Start and end iterators of segment to parse; tagEnd; target and data variables by ref
    Output: Parsed target and data as ref
*/
void XMLParser::parseProcessInstruction(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd, std::string_view &target, std::string_view &data){
    std::advance(cursor, 2);
    auto nameEnd = std::find_if_not(cursor, tagEnd, [&] (char c) { return tagNameMask[c]; });
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
    Verifies the element end tag is complete
    Input: Start and end iterators of segment to parse; tagEnd
    Output: If element end tag is incomplete, outputs error message and exits
*/
void XMLParser::validateEndTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd){
    if ((tagEnd = std::find(cursor, cursorEnd, '>')) == cursorEnd) {
        std::cerr << "parser error: Incomplete element end tag\n";
        exit(1);
    }

    return;
}

/*
    Parses xml end tag
    Input: Start and end iterators of segment to parse; depth; prefix, qName, and localName variables by ref
    Output: Parsed prefix, qName, and localName as ref; adjusted depth value by ref
*/
void XMLParser::parseEndTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, int &depth, std::string_view &prefix, std::string_view &qName, std::string_view &localName){
    std::advance(cursor, 2);
    if (*cursor == ':') {
        std::cerr << "parser error : Invalid end tag name\n";
        exit(1);
    }
    auto nameEnd = std::find_if_not(cursor, cursorEnd, [&] (char c) { return tagNameMask[c]; });
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : Unterminated end tag '" << std::string_view(std::addressof(*cursor), std::distance(cursor, nameEnd)) << "'\n";
        exit(1);
    }
    size_t colonPosition = 0;
    if (*nameEnd == ':') {
        colonPosition = std::distance(cursor, nameEnd);
        nameEnd = std::find_if_not(std::next(nameEnd), cursorEnd, [&] (char c) { return tagNameMask[c]; });
    }
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
    Verifies the element start tag is complete
    Input: Start and end iterators of segment to parse; tagEnd
    Output: If element start tag is incomplete, outputs error message and exits
*/
void XMLParser::validateStartTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd){
    if ((tagEnd = std::find(cursor, cursorEnd, '>')) == cursorEnd) {
        std::cerr << "parser error: Incomplete element start tag\n";
        exit(1);
    }

    return;
}

/*
    Parses xml start tag
    Input: Start and end iterators of segment to parse; prefix, qName, and localName variables by ref
    Output: Parsed prefix, qName, and localName as ref
*/
void XMLParser::parseStartTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &prefix, std::string_view &qName, std::string_view &localName){
    std::advance(cursor, 1);
    if (*cursor == ':') {
        std::cerr << "parser error : Invalid start tag name\n";
        exit(1);
    }
    auto nameEnd = std::find_if_not(cursor, cursorEnd, [&] (char c) { return tagNameMask[c]; });
    if (nameEnd == cursorEnd) {
        std::cerr << "parser error : Unterminated start tag '" << std::string_view(std::addressof(*cursor), std::distance(cursor, nameEnd)) << "'\n";
        exit(1);
    }
    size_t colonPosition = 0;
    if (*nameEnd == ':') {
        colonPosition = std::distance(cursor, nameEnd);
        nameEnd = std::find_if_not(std::next(nameEnd), cursorEnd, [&] (char c) { return tagNameMask[c]; });
    }
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

    if (*cursor != '>' && (*cursor != '/' || cursor[1] != '>')) {
        XMLParser::inTagQName = qName;
        XMLParser::inTagPrefix = std::string_view(qName.data(), prefix.size());
        XMLParser::inTagLocalName = std::string_view(qName.data() + prefix.size() + 1);
        XMLParser::inTag = true;
    }

    return;
}

/*
    Parses character entity references
    Input: Start iterator of segment to parse; characters variable by ref
    Output: Parsed characters as ref
*/
void XMLParser::parseCharEntity(std::string::const_iterator &cursor, std::string_view &characters){
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
    Input: Start and end iterators of segment to parse; characters variable by ref
    Output: Parsed characters as ref
*/
void XMLParser::parseCharNonEntity(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &characters){
    const auto tagEnd = std::find_if(cursor, cursorEnd, [] (char c) { return c == '<' || c == '&'; });
    const std::string_view charactersTemp(std::addressof(*cursor), std::distance(cursor, tagEnd));
    characters = charactersTemp;
    std::advance(cursor, characters.size());
    TRACE("CHARACTERS", "characters", characters);

    return;
}

/*
    Default Constructor
    Input: XMLParserHandler object and input buffer as std::string
    Output: Initialized data members
*/
XMLParser::XMLParser(XMLParserHandler &inputHandler, const std::string &inputBuffer)
    : handler(inputHandler), buffer(inputBuffer), tagNameMask("00000111111111111111111111111110100001111111111111111111111111100000001111111111011000000000000000000000000000000000000000000000")
{}

/*
    Returns the total number of bytes read by the parser
    Input: N/A
    Output: Total bytes read as long
*/
long XMLParser::getTotalBytes(){
    return XMLParser::totalBytes;
}

/*
    Performs the main parsing loop
    Input: N/A
    Output: N/A
*/
void XMLParser::parse(){
    int depth = 0;
    std::string::const_iterator cursor = XMLParser::buffer.cend();
    std::string::const_iterator cursorEnd = XMLParser::buffer.cend();

    beginParsing();
    handler.handleStartDocument(depth);

    while (true) {

        if (std::distance(cursor, cursorEnd) < 5) {
            // refill buffer and adjust iterator
            int bytesRead = refillBuffer(cursor, cursorEnd, XMLParser::buffer);
            if (bytesRead < 0) {
                std::cerr << "parser error : File input error\n";
                exit(1);
            }
            XMLParser::totalBytes += bytesRead;
            if (!XMLParser::inXMLComment && !XMLParser::inCDATA && cursor == cursorEnd)
                break;

        } else if (isNamespace(cursor)) {
            // parse XML namespace
            std::string_view prefix(""sv);
            std::string_view uri(""sv);
            parseNamespace(cursor, cursorEnd, depth, prefix, uri);
            handler.handleXMLNamespace(prefix, uri, depth);

        } else if (isAttribute()) {
            // parse attribute
            std::string_view qName(""sv);
            std::string_view prefix(""sv);
            std::string_view localName(""sv);
            std::string_view value(""sv);
            parseAttribute(cursor, cursorEnd, depth, qName, prefix, localName, value);
            handler.handleAttribute(qName, prefix, localName, value, depth);

        } else if (isXMLComment(cursor)) {
            // parse XML comment
            std::string_view comment(""sv);
            parseComment(cursor, cursorEnd, comment);
            handler.handleXMLComment(comment, depth);

        } else if (isCDATA(cursor)) {
            // parse CDATA
            std::string_view characters(""sv);
            parseCDATA(cursor, cursorEnd, characters);
            handler.handleCDATA(characters, depth);

        } else if (isXMLDecl(cursor)) {
            // parse XML declaration
            constexpr std::string_view startXMLDecl = "<?xml";
            constexpr std::string_view endXMLDecl = "?>";
            auto tagEnd = std::find(cursor, cursorEnd, '>');
            if (tagEnd == cursorEnd) {
                int bytesRead = refillBuffer(cursor, cursorEnd, XMLParser::buffer);
                if (bytesRead < 0) {
                    std::cerr << "parser error : File input error\n";
                    exit(1);
                }
                XMLParser::totalBytes += bytesRead;
                validateDeclaration(cursor, cursorEnd, tagEnd);
            }
            std::string_view version(""sv);
            std::optional<std::string_view> encoding;
            std::optional<std::string_view> standalone;
            parseDeclaration(cursor, cursorEnd, startXMLDecl, endXMLDecl, tagEnd, version, encoding, standalone);
            handler.handleXMLDeclaration(version, encoding, standalone, depth);

        } else if (isProcessInstruction(cursor)) {
            // parse processing instruction
            constexpr std::string_view endPI = "?>";
            auto tagEnd = std::search(cursor, cursorEnd, endPI.begin(), endPI.end());
            if (tagEnd == cursorEnd) {
                int bytesRead = refillBuffer(cursor, cursorEnd, XMLParser::buffer);
                if (bytesRead < 0) {
                    std::cerr << "parser error : File input error\n";
                    exit(1);
                }
                XMLParser::totalBytes += bytesRead;
                validateProcessInstruction(cursor, cursorEnd, tagEnd, endPI);
            }
            std::string_view target(""sv);
            std::string_view data(""sv);
            parseProcessInstruction(cursor, cursorEnd, tagEnd, target, data);
            handler.handleProcessingInstruction(target, data, depth);

        } else if (isEndTag(cursor)) {
            // parse end tag
            if (std::distance(cursor, cursorEnd) < 100) {
                auto tagEnd = std::find(cursor, cursorEnd, '>');
                if (tagEnd == cursorEnd) {
                    int bytesRead = refillBuffer(cursor, cursorEnd, XMLParser::buffer);
                    if (bytesRead < 0) {
                        std::cerr << "parser error : File input error\n";
                        exit(1);
                    }
                    XMLParser::totalBytes += bytesRead;
                    validateEndTag(cursor, cursorEnd, tagEnd);
                }
            }
            std::string_view prefix(""sv);
            std::string_view qName(""sv);
            std::string_view localName(""sv);
            parseEndTag(cursor, cursorEnd, depth, prefix, qName, localName);
            handler.handleElementEndTag(qName, prefix, localName, depth);

        } else if (isStartTag(cursor)) {
            // parse start tag
            if (std::distance(cursor, cursorEnd) < 200) {
                auto tagEnd = std::find(cursor, cursorEnd, '>');
                if (tagEnd == cursorEnd) {
                    int bytesRead = refillBuffer(cursor, cursorEnd, XMLParser::buffer);
                    if (bytesRead < 0) {
                        std::cerr << "parser error : File input error\n";
                        exit(1);
                    }
                    XMLParser::totalBytes += bytesRead;
                    validateStartTag(cursor, cursorEnd, tagEnd);
                }
            }
            std::string_view prefix(""sv);
            std::string_view qName(""sv);
            std::string_view localName(""sv);
            parseStartTag(cursor, cursorEnd, prefix, qName, localName);
            handler.handleElementStartTag(qName, prefix, localName, depth);
            if (*cursor == '>') {
                std::advance(cursor, 1);
                ++depth;
            } else if (*cursor == '/' && cursor[1] == '>') {
                std::advance(cursor, 2);
            }

        } else if (depth == 0) {
            // parse characters before or after XML
            cursor = std::find_if_not(cursor, cursorEnd, isspace);

        } else if (*cursor == '&') {
            // parse character entity references
            std::string_view characters;
            parseCharEntity(cursor, characters);
            handler.handleCharacters(characters, depth);

        } else {
            // parse character non-entity references
            std::string_view characters(""sv);
            parseCharNonEntity(cursor, cursorEnd, characters);
            handler.handleCharacters(characters, depth);
        }
    }

    handler.handleEndDocument(depth);
    endParsing();

    return;
}
