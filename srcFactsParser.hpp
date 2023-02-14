/*

    srcFacts Parser.hpp

    Include file for srcFacts parsing class

*/

#ifndef SRCFACTSPARSER_HPP
#define SRCFACTSPARSER_HPP

#include "XMLParserHandler.hpp"

class srcFactsParser : public XMLParserHandler {
private:
    std::string url;
    int textsize = 0;
    int loc = 0;
    int exprCount = 0;
    int functionCount = 0;
    int classCount = 0;
    int unitCount = 0;
    int declCount = 0;
    int commentCount = 0;
    int returnCount = 0;
    int literalStringCount = 0;
    int lineCommentCount = 0;
    bool isArchive = false;
    bool inTagLiteral = false;
    bool inTagComment = false;

    /*
        Override function that handles
        element start tags
        Input: std::string_view qName, std::string_view prefix, std::string_view localName, int depth
        Output: N/A
    */
    void handleElementStartTag(std::string_view qName, std::string_view prefix, std::string_view localName, const int &depth) override;

    /*
        Override function that handles
        characters
        Input: std::string_view characters, int depth
        Output: N/A
    */
    void handleCharacters(std::string_view characters, const int &depth) override;

    /*
        Override function that handles
        attributes
        Input: std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, int depth
        Output: N/A
    */
    void handleAttribute(std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, const int &depth) override;

    /*
        Override function that handles
        CDATA
        Input: std::string_view content, int depth
        Output: N/A
    */
    void handleCDATA(std::string_view content, const int &depth) override;

    /*
        Virtual function that handles
        starting the document
        Input: int depth
        Output: N/A
    */
    void handleStartDocument(const int &depth) override;

    /*
        Virtual function that handles
        XML declarations
        Input: std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone, int depth
        Output: N/A
    */
    void handleXMLDeclaration(std::string_view version, const std::optional<std::string_view> &encoding, const std::optional<std::string_view> &standalone, const int &depth) override;

    /*
        Virtual function that handles
        element end tags
        Input: std::string_view qName, std::string_view prefix, std::string_view localName, int depth
        Output: N/A
    */
    void handleElementEndTag(std::string_view qName, std::string_view prefix, std::string_view localName, const int &depth) override;

    /*
        Virtual function that handles
        XML namespace
        Input: std::string_view prefix, std::string_view uri, int depth
        Output: N/A
    */
    void handleXMLNamespace(std::string_view prefix, std::string_view uri, const int &depth) override;

    /*
        Virtual function that handles
        XML comments
        Input: std::string_view value, int depth
        Output: N/A
    */
    void handleXMLComment(std::string_view value, const int &depth) override;

    /*
        Virtual function that handles
        processing instructions
        Input: std::string_view target, std::string_view data, int depth
        Output: N/A
    */
    void handleProcessingInstruction(std::string_view target, std::string_view data, const int &depth) override;

    /*
        Virtual function that handles
        ending the document
        Input: int depth
        Output: N/A
    */
    void handleEndDocument(const int &depth) override;

public:
    /*
        Get method for URL
        Input: N/A
        Output: URL
    */
    std::string getURL();

    /*
        Get method for textsize
        Input: N/A
        Output: Textsize
    */
    int getTextsize();

    /*
        Get method for LOC
        Input: N/A
        Output: LOC
    */
    int getLOC();

    /*
        Get method for ExprCount
        Input: N/A
        Output: ExprCount
    */
    int getExprCount();

    /*
        Get method for FunctionCount
        Input: N/A
        Output: FunctionCount
    */
    int getFunctionCount();

    /*
        Get method for ClassCount
        Input: N/A
        Output: ClassCount
    */
    int getClassCount();

    /*
        Get method for UnitCount
        Input: N/A
        Output: UnitCount
    */
    int getUnitCount();

    /*
        Get method for DeclCount
        Input: N/A
        Output: DeclCount
    */
    int getDeclCount();

    /*
        Get method for CommentCount
        Input: N/A
        Output: CommentCount
    */
    int getCommentCount();

    /*
        Get method for ReturnCount
        Input: N/A
        Output: ReturnCount
    */
    int getReturnCount();

    /*
        Get method for LiteralStringCount
        Input: N/A
        Output: LiteralStringCount
    */
    int getLiteralStringCount();

    /*
        Get method for LineCommentCount
        Input: N/A
        Output: LineCommentCount
    */
    int getLineCommentCount();

    /*
        Get method for IsArchive
        Input: N/A
        Output: IsArchive
    */
    bool getIsArchive();
};

#endif
