/*
    xmlstats.cpp

    Markdown report with the number of each part of XML.
    E.g., the number of start tags, end tags, attributes,
    character sections, etc.
*/

#include <iostream>
#include <string>
#include <string_view>
#include <optional>
#include <iomanip>
#include "XMLParser.hpp"

// provides literal string operator""sv
using namespace std::literals::string_view_literals;

const int BUFFER_SIZE = 16 * 16 * 4096;

int main() {
    std::string url;
    int startDocumentCount = 0;
    int XMLDeclarationCount = 0;
    int startTagCount = 0;
    int endTagCount = 0;
    int charSectionCount = 0;
    int attributeCount = 0;
    int XMLNamespaceCount = 0;
    int XMLCommentCount = 0;
    int CDATACount = 0;
    int processingInstructionCount = 0;
    int endDocumentCount = 0;
    std::string buffer(BUFFER_SIZE, ' ');

    //Construct parser and register handlers
    XMLParser parser(buffer);
    parser.registerStartDocumentHandler([&](int depth){
        ++startDocumentCount;
        return;
    });
    parser.registerXMLDeclarationHandler([&](std::string_view version, std::optional<std::string_view> encoding, std::optional<std::string_view> standalone, int depth){
        ++XMLDeclarationCount;
        return;
    });
    parser.registerElementStartTagHandler([&](std::string_view qName, std::string_view prefix, std::string_view localName, int depth){
        ++startTagCount;
        return;
    });
    parser.registerElementEndTagHandler([&](std::string_view qName, std::string_view prefix, std::string_view localName, int depth){
        ++endTagCount;
        return;
    });
    parser.registerCharactersHandler([&](std::string_view characters, int depth){
        ++charSectionCount;
        return;
    });
    parser.registerAttributeHandler([&](std::string_view qName, std::string_view prefix, std::string_view localName, std::string_view value, int depth){
        if (localName == "url"sv){
            url = value;
        }
        ++attributeCount;
        return;
    });
    parser.registerXMLNamespaceHandler([&](std::string_view prefix, std::string_view uri, int depth){
        ++XMLNamespaceCount;
        return;
    });
    parser.registerXMLCommentHandler([&](std::string_view value, int depth){
        ++XMLCommentCount;
        return;
    });
    parser.registerCDATAHandler([&](std::string_view value, int depth){
        ++CDATACount;
        return;
    });
    parser.registerProcessingInstructionHandler([&](std::string_view target, std::string_view data, int depth){
        ++processingInstructionCount;
        return;
    });
    parser.registerEndDocumentHandler([&](int depth){
        ++endDocumentCount;
        return;
    });

    //Parse the document
    parser.parse();

    //Output a report
    std::cout << "# xmlstats: " << url << "\n";
    std::cout << "| Item                   | " << std::setw(8) << "Number |\n";
    std::cout << "|:-----------------------|-" << std::setw(9) << std::setfill('-') << ":|\n" << std::setfill(' ');
    std::cout << "| Start of Document      | " << std::setw(6) << startDocumentCount << " |\n";
    std::cout << "| XML Declaration        | " << std::setw(6) << XMLDeclarationCount << " |\n";
    std::cout << "| Element Start Tag      | " << std::setw(6) << startTagCount << " |\n";
    std::cout << "| Element End Tag        | " << std::setw(6) << endTagCount << " |\n";
    std::cout << "| Character Section      | " << std::setw(6) << charSectionCount << " |\n";
    std::cout << "| Attribute              | " << std::setw(6) << attributeCount << " |\n";
    std::cout << "| XML Namespace          | " << std::setw(6) << XMLNamespaceCount << " |\n";
    std::cout << "| XML Comment            | " << std::setw(6) << XMLCommentCount << " |\n";
    std::cout << "| CDATA                  | " << std::setw(6) << CDATACount << " |\n";
    std::cout << "| Processing Instruction | " << std::setw(6) << processingInstructionCount << " |\n";
    std::cout << "| End of Document        | " << std::setw(6) << endDocumentCount << " |\n";

    return 0;
}
