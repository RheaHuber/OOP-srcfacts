/*
    srcFacts.cpp

    Produces a report with various measures of source code.
    Supports C++, C, Java, and C#.

    Input is an XML file in the srcML format.

    Output is a markdown table with the measures.

    Output performance statistics to stderr.

    Code includes an embedded XML parser:
    * No checking for well-formedness
    * No DTD declarations
*/

#include <iostream>
#include <locale>
#include <iterator>
#include <string>
#include <algorithm>
#include <string_view>
#include <optional>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <memory>
#include <string.h>
#include "XMLParser.hpp"
#include "srcFactsParser.hpp"

// provides literal string operator""sv
using namespace std::literals::string_view_literals;

const int BUFFER_SIZE = 16 * 16 * 4096;

int main() {
    const auto start = std::chrono::steady_clock::now();
    std::string buffer(BUFFER_SIZE, ' ');

    //Construct a handler and parser object
    srcFactsParser handler;
    XMLParser parser(handler, buffer);

    //Parse the document
    parser.parse();

    //Print the report
    const auto finish = std::chrono::steady_clock::now();
    const auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double> >(finish - start).count();
    const double mlocPerSec = handler.getLOC() / elapsed_seconds / 1000000;
    int files = handler.getUnitCount();
    if (handler.getIsArchive())
        --files;
    std::cout.imbue(std::locale{""});
    int valueWidth = std::max(5, static_cast<int>(log10(parser.getTotalBytes()) * 1.3 + 1));
    std::cout << "# srcFacts: " << handler.getURL() << '\n';
    std::cout << "| Measure      | " << std::setw(valueWidth + 3) << "Value |\n";
    std::cout << "|:-------------|-" << std::setw(valueWidth + 3) << std::setfill('-') << ":|\n" << std::setfill(' ');
    std::cout << "| srcML bytes  | " << std::setw(valueWidth) << parser.getTotalBytes() << " |\n";
    std::cout << "| Characters   | " << std::setw(valueWidth) << handler.getTextsize()       << " |\n";
    std::cout << "| Files        | " << std::setw(valueWidth) << files          << " |\n";
    std::cout << "| LOC          | " << std::setw(valueWidth) << handler.getLOC()            << " |\n";
    std::cout << "| Classes      | " << std::setw(valueWidth) << handler.getClassCount()    << " |\n";
    std::cout << "| Functions    | " << std::setw(valueWidth) << handler.getFunctionCount() << " |\n";
    std::cout << "| Declarations | " << std::setw(valueWidth) << handler.getDeclCount()     << " |\n";
    std::cout << "| Expressions  | " << std::setw(valueWidth) << handler.getExprCount()     << " |\n";
    std::cout << "| Comments     | " << std::setw(valueWidth) << handler.getCommentCount()  << " |\n";
    std::cout << "| Returns      | " << std::setw(valueWidth) << handler.getReturnCount()  << " |\n";
    std::cout << "| Lit Strings  | " << std::setw(valueWidth) << handler.getLiteralStringCount()  << " |\n";
    std::cout << "| Line Comments| " << std::setw(valueWidth) << handler.getLineCommentCount()  << " |\n";
    std::clog << '\n';
    std::clog << std::setprecision(3) << elapsed_seconds << " sec\n";
    std::clog << std::setprecision(3) << mlocPerSec << " MLOC/sec\n";

    return 0;
}
