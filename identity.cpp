/*
    identity.cpp

    An identity transformation of XML. The input is XML and the
    output is the equivalent XML.

    Limitation:
    * CDATA is not complete
*/

#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include "XMLParser.hpp"

// provides literal string operator""sv
using namespace std::literals::string_view_literals;

const int BUFFER_SIZE = 16 * 16 * 4096;

int main() {
    std::string buffer(BUFFER_SIZE, ' ');
    bool fromStartTag = false;
    bool fromAttribute = false;

    //Construct parser object and register handlers
    XMLParser parser(buffer);
    parser.registerXMLDeclarationHandler([&](std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone, int depth){
        if (fromStartTag || fromAttribute){
            fromStartTag = false;
            fromAttribute = false;
            std::cout << ">";
        }
        std::cout << "<?xml version=\"" << version << "\"";
        if (encoding){
            std::cout << " encoding=\"" << *encoding << "\"";
        }
        if (standalone){
            std::cout << " standalone=\"" << *standalone << "\"";
        }
        std::cout << "?>\n";
    });
    parser.registerElementStartTagHandler([&](std::string_view qName, std::string_view prefix, std::string_view localName, int depth){
        if (fromStartTag || fromAttribute){
            fromStartTag = false;
            fromAttribute = false;
            std::cout << ">";
        }
        std::cout << "<" << qName;
        fromStartTag = true;
    });
    parser.registerElementEndTagHandler([&](std::string_view qName, std::string_view prefix, std::string_view localName, int depth){
        if (fromStartTag || fromAttribute){
            fromStartTag = false;
            fromAttribute = false;
            std::cout << ">";
        }
        std::cout << "</" << qName << ">";
    });
    parser.registerCharactersHandler([&](std::string_view characters, int depth){
        if (fromStartTag || fromAttribute){
            fromStartTag = false;
            fromAttribute = false;
            std::cout << ">";
        }
        if (characters == "<"sv){
            std::cout << "&lt;";
        } else if (characters == ">"sv){
            std::cout << "&gt;";
        } else if (characters == "&"sv){
            std::cout << "&amp;";
        } else {
            std::cout << characters;
        }
    });
    parser.registerAttributeHandler([&](std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, int depth){
        std::cout << " " << qName << "=\"" << value << "\"";
        fromAttribute = true;
    });
    parser.registerXMLNamespaceHandler([&](std::string_view prefix, std::string_view uri, int depth){
        std::cout << " xmlns";
        if (prefix != ""sv){
            std::cout << ":" << prefix;
        }
        std::cout << "=\"" << uri << "\"";
    });
    parser.registerXMLCommentHandler([&](std::string_view value, int depth){
        std::cout << "<!--" << value << "-->";
    });
    parser.registerCDATAHandler([&](std::string_view value, int depth){
        std::cout << "<![CDATA[" << value << "]]>";
    });
    parser.registerProcessingInstructionHandler([&](std::string_view target, std::string_view data, int depth){
        std::cout << "<?" << target << " " << data << "?>";
    });
    parser.registerEndDocumentHandler([&](int depth){
        std::cout << "\n";
    });
    parser.parse();

    return 0;
}
