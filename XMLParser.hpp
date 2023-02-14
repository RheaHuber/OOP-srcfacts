/*

    XMLParser.hpp

    Include file for XML parsing class

*/

#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <string>
#include <optional>
#include <bitset>
#include <functional>
#include <string_view>
#include "XMLParserHandler.hpp"

class XMLParser {
private:
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
        Input: Start iterator of segment to parse
        Output: Bool value for if the iterator is inside a namespace
    */
    bool isNamespace(std::string::const_iterator &cursor);

    /*
        Returns if the parser is inside an attribute
        Input: N/A
        Output: Bool value for if the iterator is inside an attribute
    */
    bool isAttribute();

    /*
        Returns if the parser is inside an XML comment
        AND is NOT inside a namespace OR attribute
        Input: Start iterator of segment to parse
        Output: Bool value for if the iterator is inside an XML comment
    */
    bool isXMLComment(std::string::const_iterator &cursor);

    /*
        Returns if the parser is inside CDATA
        AND is NOT inside a namespace, attribute, OR XML comment
        Input: Start iterator of segment to parse
        Output: Bool value for if the iterator is inside CDATA
    */
    bool isCDATA(std::string::const_iterator &cursor);

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
    bool isProcessInstruction(std::string::const_iterator &cursor);

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
        Input: Start and end iterators of segment to parse; depth; and prefix and uri variables by ref
        Output: Parsed prefix and uri by ref; adjusted depth value by ref
    */
    void parseNamespace(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, int &depth, std::string_view &prefix, std::string_view &uri);

    /*
        Parses an xml attribute
        Input: Start and end iterators of segment to parse; depth; and qName, prefix, localName, and value variables by ref
        Output: qName, prefix, localName, and value by ref; adjusted depth value by ref
    */
    void parseAttribute(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, int &depth, std::string_view &qName, std::string_view &prefix, std::string_view &localName, std::string_view &value);

    /*
        Parses an xml comment
        Input: Start and end iterators of segment to parse; comment variable by ref
        Output: Parsed comment by ref
    */
    void parseComment(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &comment);

    /*
        Parses xml CDATA
        Input: Start and end iterators of segment to parse; characters variable by ref
        Output: Parsed characters by ref
    */
    void parseCDATA(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &characters);

    /*
        Verifies the XML declaration is complete
        Input: Start and end iterators of segment to parse; tagEnd
        Output: If declaration is incomplete, outputs error message and exits
    */
   void validateDeclaration(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd);

    /*
        Parses xml declaration
        Input: Start and end iterators of segment to parse; startXMLDecl, endXMLDecl, and tagEnd; version, encoding, and standalone variables by ref
        Output: Parsed version, encoding, and standalone as ref
    */
    void parseDeclaration(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view startXMLDecl, std::string_view endXMLDecl, std::string::const_iterator &tagEnd, std::string_view &version, std::optional<std::string_view> &encoding, std::optional<std::string_view> &standalone);

    /*
        Verifies the processing instruction is complete 
        Input: Start and end iterators of segment to parse; tagEnd, endPI
        Output: If instruction is incomplete, outputs error message and exits
    */
   void validateProcessInstruction(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd, std::string_view endPI);

    /*
        Parses xml processing instruction
        Input: Start and end iterators of segment to parse; tagEnd; target and data variables by ref
        Output: Parsed target and data as ref
    */
    void parseProcessInstruction(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd, std::string_view &target, std::string_view &data);

    /*
        Verifies the element end tag is complete
        Input: Start and end iterators of segment to parse; tagEnd
        Output: If element end tag is incomplete, outputs error message and exits
    */
   void validateEndTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd);

    /*
        Parses xml end tag
        Input: Start and end iterators of segment to parse; depth; prefix, qName, and localName variables by ref
        Output: Parsed prefix, qName, and localName as ref; adjusted depth value by ref
    */
    void parseEndTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, int &depth, std::string_view &prefix, std::string_view &qName, std::string_view &localName);

    /*
        Verifies the element start tag is complete
        Input: Start and end iterators of segment to parse; tagEnd
        Output: If element start tag is incomplete, outputs error message and exits
    */
   void validateStartTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string::const_iterator &tagEnd);

    /*
        Parses xml start tag
        Input: Start and end iterators of segment to parse; prefix, qName, and localName variables by ref
        Output: Parsed prefix, qName, and localName as ref
    */
    void parseStartTag(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &prefix, std::string_view &qName, std::string_view &localName);

    /*
        Parses character entity references
        Input: Start iterator of segment to parse; characters variable by ref
        Output: Parsed characters as ref
    */
    void parseCharEntity(std::string::const_iterator &cursor, std::string_view &characters);

    /*
        Parses character non-entity references
        Input: Start and end iterators of segment to parse; characters variable by ref
        Output: Parsed characters as ref
    */
    void parseCharNonEntity(std::string::const_iterator &cursor, std::string::const_iterator &cursorEnd, std::string_view &characters);

    std::string inTagQName;
    std::string_view inTagPrefix;
    std::string_view inTagLocalName;
    bool inTag = false;
    bool inXMLComment = false;
    bool inCDATA = false;
    long totalBytes = 0;
    std::string buffer;
    std::bitset<128> tagNameMask;
    XMLParserHandler& handler;

public:
    /*
        Default Constructor
        Input: XMLParserHandler object and input buffer as std::string
        Output: Initialized data members
    */
    XMLParser(XMLParserHandler &inputHandler, const std::string &inputBuffer);

    /*
        Virtual default destructor
        Input: N/A
        Output: N/A
    */
    virtual ~XMLParser() = default;

    /*
        Returns the total number of bytes read by the parser
        Input: N/A
        Output: Total bytes read as long
    */
    long getTotalBytes();

    /*
        Performs the main parsing loop
        Input: N/A
        Output: N/A
    */
    void parse();
};

#endif
