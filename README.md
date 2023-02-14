# srcFacts

Calculates various counts on a source-code project, including files, functions,
comments, etc.

Input is a srcML form of the project source code. An example srcML file for libxml2
is included.

The srcReport main program includes code to directly parse XML interleaved with code
to produce the report.

Notes:
* The integrated XML parser handles start tags, end tags, empty elements, attributes,
characters, namespaces, (XML) comments, and CDATA.
* Program should be fast. Run on 3 GB srcML of the linux kernel takes under 20 seconds
on an SSD Macbook Pro Mid 2015 2.2 GHz Intel Core i7. Takes very little RAM.
