/*

    srcFactsParser.cpp

    Implementation file for srcFacts parsing class

*/

#include "srcFactsParser.hpp"
#include <algorithm>

// provides literal string operator""sv
using namespace std::literals::string_view_literals;

/*
    Override function that handles
    element start tags
    Input: std::string_view qName, std::string_view prefix, std::string_view localName, int depth
    Output: N/A
*/
void srcFactsParser::handleElementStartTag(std::string_view qName, std::string_view prefix, std::string_view localName, const int &depth){
    inTagLiteral = false;
    inTagComment = false;
    if (localName == "expr"sv) {
        ++exprCount;
    } else if (localName == "decl"sv) {
        ++declCount;
    } else if (localName == "comment"sv) {
        ++commentCount;
    } else if (localName == "function"sv) {
        ++functionCount;
    } else if (localName == "unit"sv) {
        ++unitCount;
        if (depth == 1)
            isArchive = true;
    } else if (localName == "class"sv) {
        ++classCount;
    } else if (localName == "return"sv) {
        ++returnCount;
    }
    if (localName == "literal"sv) {
        inTagLiteral = true;
    } else if (localName == "comment"sv) {
        inTagComment = true;
    }

    return;
}

/*
    Override function that handles
    characters
    Input: std::string_view characters, int depth
    Output: N/A
*/
void srcFactsParser::handleCharacters(std::string_view characters, const int &depth){
    if (characters == "<"sv || characters == ">"sv || characters == "&"sv) {
        ++textsize;
    } else {
        loc += static_cast<int>(std::count(characters.cbegin(), characters.cend(), '\n'));
        textsize += static_cast<int>(characters.size());
    }

    return;
}

/*
    Override function that handles
    attributes
    Input: std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, int depth
    Output: N/A
*/
void srcFactsParser::handleAttribute(std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, const int &depth){
    if (localName == "url"sv)
        url = value;
    if (inTagLiteral) {
        if (localName == "type"sv && value == "string"sv) {
            ++literalStringCount;
        }
    } else if (inTagComment) {
        if (localName == "type"sv && value == "line"sv) {
            ++lineCommentCount;
        }
    }
    inTagLiteral = false;
    inTagComment = false;

    return;
}

/*
    Override function that handles
    CDATA
    Input: std::string_view content, int depth
    Output: N/A
*/
void srcFactsParser::handleCDATA(std::string_view content, const int &depth){
    textsize += static_cast<int>(content.size());
    loc += static_cast<int>(std::count(content.begin(), content.end(), '\n'));

    return;
}

/*
    Virtual function that handles
    starting the document
    Input: int depth
    Output: N/A
*/
void srcFactsParser::handleStartDocument(const int &depth){

}

/*
    Virtual function that handles
    XML declarations
    Input: std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone, int depth
    Output: N/A
*/
void srcFactsParser::handleXMLDeclaration(std::string_view version, const std::optional<std::string_view> &encoding, const std::optional<std::string_view> &standalone, const int &depth){

}

/*
    Virtual function that handles
    element end tags
    Input: std::string_view qName, std::string_view prefix, std::string_view localName, int depth
    Output: N/A
*/
void srcFactsParser::handleElementEndTag(std::string_view qName, std::string_view prefix, std::string_view localName, const int &depth){

}

/*
    Virtual function that handles
    XML namespace
    Input: std::string_view prefix, std::string_view uri, int depth
    Output: N/A
*/
void srcFactsParser::handleXMLNamespace(std::string_view prefix, std::string_view uri, const int &depth){

}

/*
    Virtual function that handles
    XML comments
    Input: std::string_view value, int depth
    Output: N/A
*/
void srcFactsParser::handleXMLComment(std::string_view value, const int &depth){

}

/*
    Virtual function that handles
    processing instructions
    Input: std::string_view target, std::string_view data, int depth
    Output: N/A
*/
void srcFactsParser::handleProcessingInstruction(std::string_view target, std::string_view data, const int &depth){

}

/*
    Virtual function that handles
    ending the document
    Input: int depth
    Output: N/A
*/
void srcFactsParser::handleEndDocument(const int &depth){

}

/*
    Get method for URL
    Input: N/A
    Output: URL
*/
std::string srcFactsParser::getURL(){
    return url;
}

/*
    Get method for textsize
    Input: N/A
    Output: Textsize
*/
int srcFactsParser::getTextsize(){
    return textsize;
}

/*
    Get method for LOC
    Input: N/A
    Output: LOC
*/
int srcFactsParser::getLOC(){
    return loc;
}

/*
    Get method for ExprCount
    Input: N/A
    Output: ExprCount
*/
int srcFactsParser::getExprCount(){
    return exprCount;
}

/*
    Get method for FunctionCount
    Input: N/A
    Output: FunctionCount
*/
int srcFactsParser::getFunctionCount(){
    return functionCount;
}

/*
    Get method for ClassCount
    Input: N/A
    Output: ClassCount
*/
int srcFactsParser::getClassCount(){
    return classCount;
}

/*
    Get method for UnitCount
    Input: N/A
    Output: UnitCount
*/
int srcFactsParser::getUnitCount(){
    return unitCount;
}

/*
    Get method for DeclCount
    Input: N/A
    Output: DeclCount
*/
int srcFactsParser::getDeclCount(){
    return declCount;
}

/*
    Get method for CommentCount
    Input: N/A
    Output: CommentCount
*/
int srcFactsParser::getCommentCount(){
    return commentCount;
}

/*
    Get method for ReturnCount
    Input: N/A
    Output: ReturnCount
*/
int srcFactsParser::getReturnCount(){
    return returnCount;
}

/*
    Get method for LiteralStringCount
    Input: N/A
    Output: LiteralStringCount
*/
int srcFactsParser::getLiteralStringCount(){
    return literalStringCount;
}

/*
    Get method for LineCommentCount
    Input: N/A
    Output: LineCommentCount
*/
int srcFactsParser::getLineCommentCount(){
    return lineCommentCount;
}

/*
    Get method for IsArchive
    Input: N/A
    Output: IsArchive
*/
bool srcFactsParser::getIsArchive(){
    return isArchive;
}
