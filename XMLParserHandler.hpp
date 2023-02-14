/*

    XMLParserHandler.hpp

    Include file for XML parsing handler class

*/

#ifndef XMLPARSERHANDLER_HPP
#define XMLPARSERHANDLER_HPP

#include <string_view>
#include <optional>

class XMLParserHandler {
public:
    /*
        Virtual function that handles
        starting the document
        Input: int depth
        Output: N/A
    */
    virtual void handleStartDocument(const int &depth) = 0;

    /*
        Virtual function that handles
        XML declarations
        Input: std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone, int depth
        Output: N/A
    */
    virtual void handleXMLDeclaration(std::string_view version, const std::optional<std::string_view> &encoding, const std::optional<std::string_view> &standalone, const int &depth) = 0;

    /*
        Virtual function that handles
        element start tags
        Input: std::string_view qName, std::string_view prefix, std::string_view localName, int depth
        Output: N/A
    */
    virtual void handleElementStartTag(std::string_view qName, std::string_view prefix, std::string_view localName, const int &depth) = 0;

    /*
        Virtual function that handles
        element end tags
        Input: std::string_view qName, std::string_view prefix, std::string_view localName, int depth
        Output: N/A
    */
    virtual void handleElementEndTag(std::string_view qName, std::string_view prefix, std::string_view localName, const int &depth) = 0;

    /*
        Virtual function that handles
        characters
        Input: std::string_view characters, int depth
        Output: N/A
    */
    virtual void handleCharacters(std::string_view characters, const int &depth) = 0;

    /*
        Virtual function that handles
        attributes
        Input: std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, int depth
        Output: N/A
    */
    virtual void handleAttribute(std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, const int &depth) = 0;

    /*
        Virtual function that handles
        XML namespace
        Input: std::string_view prefix, std::string_view uri, int depth
        Output: N/A
    */
    virtual void handleXMLNamespace(std::string_view prefix, std::string_view uri, const int &depth) = 0;

    /*
        Virtual function that handles
        XML comments
        Input: std::string_view value, int depth
        Output: N/A
    */
    virtual void handleXMLComment(std::string_view value, const int &depth) = 0;

    /*
        Virtual function that handles
        CDATA
        Input: std::string_view content, int depth
        Output: N/A
    */
    virtual void handleCDATA(std::string_view content, const int &depth) = 0;

    /*
        Virtual function that handles
        processing instructions
        Input: std::string_view target, std::string_view data, int depth
        Output: N/A
    */
    virtual void handleProcessingInstruction(std::string_view target, std::string_view data, const int &depth) = 0;

    /*
        Virtual function that handles
        ending the document
        Input: int depth
        Output: N/A
    */
    virtual void handleEndDocument(const int &depth) = 0;
};

#endif
