/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

#ifndef __TBA_XSTL_PARSER_PARSER_H
#define __TBA_XSTL_PARSER_PARSER_H

/*
 * Parser.h
 *
 * A complete set of textual library for parsing text strings.
 *
 * Author: Elad Raz <e@eladraz.com>
 */

#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/array.h"
#include "xStl/parser/types.h"
#include "xStl/parser/variable.h"
#include "xStl/parser/except.h"

/*
 * The parser class knows how to parse a textual information. The class uses
 * a pointer to a char* and a length of the data. The class uses a 'uint' typedef
 * as an iterator inside the stream. The iterator is managed internally in
 * order to keep the line-number.
 *
 * NOTICE: The construction of this class is fast, so multiple instances of
 *         the parsing class can (and should) be instantiated for a single stream
 *
 * Usage:
 *   cBuffer data = cFile("document").readAllStream();
 *   // If you would like to call Parser::easyParsing to make your life easier...
 *   Parser parser(data.getBuffer(), data.getSize());
 *
 *   // Reading a C style function:
 *
 *   parser.readBlanks();
 *   cString returnType = parser.readCString(); parser.readBlanks(ptr);
 *   cString functionName = parser.readCString(); parser.readBlanks();
 *   CHECK(parser.readChar() == '(');
 *   cString argument1Type = parser.readCString(); parser.readBlanks();
 *   ...
 *
 * NOTE: The memory should be managed by the upper management class. The buffer
 *       which is transferred to the class should be allocated and filled with data.
 *       The class doesn't free/duplicate any memory. The buffer pointer must
 *       be valid during the lifetime of this class.
 *
 * TODO: Add unicode support.
 */
class Parser
{
public:
    // The unknown filename
    static const cString unknownFilename;

    /*
     * Constructor for the parser.
     *
     * data       - Pointer to the raw data of the textual information.
     * pointer    - Pointer to beginning of the analyzing stream
     * length     - The size of the data in bytes.
     * lineNumber - The line-number from the beginning of the stream of 'pointer'
     *              This information can only be given by the management class
     * filename   - The name of the file which the parser is working on.
     */
    Parser(const char* data,
           char* pointer,
           uint  length,
           uint  lineNumber,
           const cString& filename = unknownFilename);

    /*
     * Return a pointer inside 'data' which represents the current location of
     * the parser class
     */
    uint getIterator() const;

    /*
     * Return the line number of the current iterator.
     */
    uint getLinenumber() const;

    /*
     * Return the filename of the parsing utility
     */
    const cString& getFilename() const;

    /*
     * Tests whether an iterator reaches the end of the stream
     */
    bool isEOS();

    /*
     * Peek the next queued character
     */
    char peekChar();

    /*
     * Read a single character
     */
    char readChar();

    /*
     * Read all blank characters and stop when the iterator reaches a non-blank
     * character.
     *
     * If the pointer points to the end of the stream, the function returns
     * normally.
     */
    void readBlanks();

    /*
     * Read all blanks until the end of the line (\n\r) is found.
     *
     * Used for processors where end-of-line has some significant dividing lines
     *
     * Return true if end of line OR END OF STREAM is found.
     * For any other character return false
     */
    bool readBlanksUntilEOL();

    /*
     * Return the next blanks character.
     */
    cString getBlanks();

    /*
     * Read a word from the text. The stream must point to textual data.
     * The word ends at a blanket or end-of-stream
     *
     * Return the content of the word
     */
    cString readWord();

    /*
     * Reads a C name. The name must contain only the following
     * characters: letters, digits and _.
     *
     * isFirstLetter - True means that the first character must be
     *                 a letter.
     *
     * Return the read string.
     *
     * Throws an exception if EOS is reached or the first digit is not a letter.
     */
    cString readCString(bool isFirstLetter = true);

    /*
     * The function reads a string starting with quote (") and ends with a quote.
     * The function parses the internal special characters such as '\t' '\n' and
     * '\\', '\"' into their original meaning.
     *
     * isCString - Set to true in order to read c-style quote strings
     *             Set to false otherwise (all \n will remain \n...)
     *
     * Throws an exception if EOS is reached or the first digit is not a quote (").
     */
    cString readQuoteString(bool isCString = true);

    /*
     * Reads a string which contains BLABLA$(VariableName)BLABLA.bla names.
     * The strings can contain numbers, letters, variables, dots and underscores.
     *
     * Throws an exception if EOS is reached or the string is not formatted.
     */
    cString readSpecialString();

    /*
     * Try to read an integer from the stream. The integer can be formatted as
     * a C integer format: Decimal number 123, or hexadecimal with a prefix +
     * of 0x, or a base 8 number with a prefix of 0.
     *
     * The parsing stops at the first non-digit character. If the character is
     * an alphabetic character (such as A..Z, a..z) an exception will be raised.
     *
     * NOTE:
     * The read number must be in unsigned format.
     *
     * TODO! Add base 8 parser.
     *
     * Throws an exception if the first character is not a digit.
     * Throws an exception if an overflow occurred.
     */
    uint readCUnsignedInteger();

    /*
     * Read and check whether the next token is a c style integer.
     * See 'readCUnsignedInteger' for more information
     *
     * retNumber - A pointer into the read integer. If this pointer is null
     *             then the function only returns true or false
     * shouldAdvancePosition - Set to true if the token position should be
     *                         advanced. Set to false to indicate that the
     *                         position should remain the same.
     *
     * Return true if the next c-string is encoded as unsigned int.
     */
    bool isCUnsignedInteger(uint* retNumber = NULL,
                            bool shouldAdvancePosition = false);

    // TODO!
    #define read64bitNumber readCUnsignedInteger
    #define is64bitNumber isCUnsignedInteger

    /*
     * The same as 'readCUnsignedInteger' except the reading format expects
     * a number in base 16.
     */
    uint readHexString();

    /*
     * For signed integers this variable is trying to read its sign.
     *
     * Return true if a minus (-) character is found before the number.
     */
    bool readSignedValue();

    /*
     * The function retrieves the next C style string without polling it from
     * the stream.
     *
     * See readCString.
     */
    cString peekCString(bool isFirstLetter = true);

    /*
     * Read a block of braces.
     *
     * Throws an exception if the block is invalid
     */
    void readBracesBlock();

    /*
     * Read a variable from a certain type.
     *
     * Return the value of the variable
     */
    BasicVariable readVariable(Types::Type type);

    /*
     * Remove the C style comments ( '/ /' and '/ *' )
     * The comments are replaced with blanks.
     *
     * stream - The stream from which the comments should be removed
     * length - The size of 'stream' in bytes
     */
    static void removeComments(char* stream, uint length);

    /*
     * The function replaces all double newline characters such as
     * "\r\n" or "\r\n" with a space and a single '\n' character
     *
     * stream - The stream
     * length - The size of 'stream' in bytes
     */
    static void removeDoubleNewLine(char* stream, uint length);

    /*
     * Makes the parsing functions easy to write and run.
     * Calls the 'removeComments' and 'removeDoubleNewLine' functions.
     * See removeComments()
     * See removeDoubleNewLine()
     *
     * stream - The stream
     * length - The size of 'stream' in bytes
     */
    static void easyParsing(char* stream, uint length);

private:
    /*
     * Internal implementation of isEOS. Uses outside pointer.
     *
     * pointer - Iterator of the stream
     *
     * See isEOS
     */
    bool internalIsEOS(char* pointer);

    /*
     * Test that the end-of-file is not reached yet.
     *
     * Throw an exception otherwise
     */
    void checkNotEOS();

    // Deny operator = and copy-constructor, only this class can duplicate the
    // parser class
    Parser(const Parser& other);
    Parser& operator = (const Parser& other);

    // Pointer to the raw data of the textual information
    const char* m_data;
    // The size of the data in bytes
    const uint m_length;
    // The end of the iterator
    const char* m_end;
    // The current iterator
    char* m_pointer;
    // The current line number
    uint m_lineNumber;
    // The filename which is parsed.
    const cString& m_filename;
};

// The macro which throws an exception from a parser.
#define PARSER_THROW(parser, msg) XSTL_THROW(ParserException, (parser).getFilename(), (parser).getLinenumber(), (msg));
// The macro which throws an exception from a parser.
#define PARSER_CHECK(n, parser, msg) if (!(n)) { PARSER_THROW(parser, msg); };
// Catch exceptions and translate them to parser exceptions
#define PARSER_CATCH(n, parser, msg) XSTL_TRY { n; } XSTL_CATCH_ALL { PARSER_THROW(parser, msg); };

#endif // __TBA_XSTL_PARSER_PARSER_H
