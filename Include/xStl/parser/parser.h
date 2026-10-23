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

/**
 * @addtogroup xstl_parser
 * @{
 */

/**
 * @file parser.h
 * @brief A complete set of textual library for parsing text strings.
 *
 * @author Elad Raz
 */

#include "xStl/types.h"
#include "xStl/data/char.h"
#include "xStl/data/array.h"
#include "xStl/parser/types.h"
#include "xStl/parser/variable.h"
#include "xStl/parser/except.h"

/**
 * @brief The parser class knows how to parse a textual information.
 *
 * @details The class uses a pointer to a char* and a length of the data. The
 *    class uses a 'uint' typedef as an iterator inside the stream. The iterator
 *    is managed internally in order to keep the line-number.
 *
 * NOTICE: The construction of this class is fast, so multiple instances of
 * the parsing class can (and should) be instantiated for a single stream
 *
 * Usage:
 *
 * @code
 *   cBuffer data = cFile("document").readAllStream();
 *   // If you would like to call Parser::easyParsing to make your life
 *   // easier...
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
 * @endcode
 *
 * @note The memory should be managed by the upper management class. The buffer
 *    which is transferred to the class should be allocated and filled with
 *    data. The class doesn't free/duplicate any memory. The buffer pointer must
 *    be valid during the lifetime of this class.
 * @todo Add unicode support.
 */
class Parser
{
public:
    /**
     * @brief The unknown filename
     */
    static const cString unknownFilename;

    /**
     * @brief Constructor for the parser.
     *
     * @param data Pointer to the raw data of the textual information.
     * @param pointer Pointer to beginning of the analyzing stream
     * @param length The size of the data in bytes.
     * @param lineNumber The line-number from the beginning of the stream of
     *    'pointer' This information can only be given by the management class
     * @param filename The name of the file which the parser is working on.
     */
    Parser(const char* data,
           char* pointer,
           uint  length,
           uint  lineNumber,
           const cString& filename = unknownFilename);

    /**
     * @brief Return a pointer inside 'data' which represents the current
     *    location of
     *   the parser class
     *
     * @return A pointer inside 'data' which represents the current location of
     *    the parser class.
     */
    uint getIterator() const;

    /**
     * @brief Return the line number of the current iterator.
     *
     * @return The line number of the current iterator.
     */
    uint getLinenumber() const;

    /**
     * @brief Return the filename of the parsing utility
     *
     * @return The filename of the parsing utility.
     */
    const cString& getFilename() const;

    /**
     * @brief Tests whether an iterator reaches the end of the stream
     *
     * @return True if the parser reached the end of the stream.
     */
    bool isEOS();

    /**
     * @brief Peek the next queued character
     *
     * @return The next character, which stays in the stream.
     */
    char peekChar();

    /**
     * @brief Read a single character
     *
     * @return The character which was read.
     */
    char readChar();

    /**
     * @brief Read all blank characters and stop when the iterator reaches a
     *    non-blank
     *   character.
     *
     * @details If the pointer points to the end of the stream, the function
     *    returns
     *   normally.
     */
    void readBlanks();

    /**
     * @brief Read all blanks until the end of the line (\\n\\r) is found.
     *
     * @details Used for processors where end-of-line has some significant
     *    dividing lines
     *
     * @return True if end of line OR END OF STREAM is found. For any other
     *    character return false.
     */
    bool readBlanksUntilEOL();

    /**
     * @brief Return the next blanks character.
     *
     * @return The next blanks character.
     */
    cString getBlanks();

    /**
     * @brief Read a word from the text.
     *
     * @details The stream must point to textual data. The word ends at a
     *    blanket or end-of-stream
     *
     * @return The content of the word.
     */
    cString readWord();

    /**
     * @brief Reads a C name.
     *
     * @details The name must contain only the following characters: letters,
     *    digits and _.
     *
     * @param isFirstLetter True means that the first character must be a
     *    letter.
     * @return The read string.
     * @throws cException if EOS is reached or the first digit is not a letter.
     */
    cString readCString(bool isFirstLetter = true);

    /**
     * @brief Read a string which starts with a quote character and ends with a
     *    quote character.
     *
     * @details The function parses the internal escape sequences (a backslash
     *    followed by t, n, a backslash or a quote character) into their
     *    original meaning.
     *
     * @param isCString Set to true in order to read c-style quote strings. Set
     *    to false otherwise (an escaped newline sequence remains as it is).
     * @return The content of the quoted string.
     * @throws cException if EOS is reached or the first character is not a
     *    quote.
     */
    cString readQuoteString(bool isCString = true);

    /**
     * @brief Reads a string which contains BLABLA$(VariableName)BLABLA.bla
     *    names.
     *
     * @details The strings can contain numbers, letters, variables, dots and
     *    underscores.
     *
     * @throws cException if EOS is reached or the string is not formatted.
     * @return The string which was read.
     */
    cString readSpecialString();

    /**
     * @brief Try to read an integer from the stream.
     *
     * @details The integer can be formatted as a C integer format: Decimal
     *    number 123, or hexadecimal with a prefix + of 0x, or a base 8 number
     *    with a prefix of 0.
     *
     * The parsing stops at the first non-digit character. If the character is
     * an alphabetic character (such as A..Z, a..z) an exception will be raised.
     *
     * @throws cException if the first character is not a digit.
     * @throws cException if an overflow occurred.
     * @note The read number must be in unsigned format.
     * @todo Add base 8 parser.
     * @return The number which was read.
     */
    uint readCUnsignedInteger();

    /**
     * @brief Read and check whether the next token is a c style integer.
     *
     * @details See 'readCUnsignedInteger' for more information
     *
     * @param retNumber A pointer into the read integer. If this pointer is null
     *    then the function only returns true or false
     * @param shouldAdvancePosition Set to true if the token position should be
     *    advanced. Set to false to indicate that the position should remain the
     *    same.
     * @return True if the next c-string is encoded as unsigned int.
     */
    bool isCUnsignedInteger(uint* retNumber = NULL,
                            bool shouldAdvancePosition = false);

    // TODO!
    /**
     * @brief Alias of readCUnsignedInteger. A real 64 bit parser is not
     *    implemented yet.
     */
    #define read64bitNumber readCUnsignedInteger
    /**
     * @brief Alias of isCUnsignedInteger. A real 64 bit parser is not
     *    implemented yet.
     */
    #define is64bitNumber isCUnsignedInteger

    /**
     * @brief The same as 'readCUnsignedInteger' except the reading format
     *    expects a
     *   number in base 16.
     *
     * @return The number which was read.
     */
    uint readHexString();

    /**
     * @brief For signed integers this variable is trying to read its sign.
     *
     * @return True if a minus (-) character is found before the number.
     */
    bool readSignedValue();

    /**
     * @brief The function retrieves the next C style string without polling it
     *    from the
     *   stream.
     *
     * @see readCString
     * @param isFirstLetter True means that the first character must be a
     *    letter.
     * @return The C style string which follows in the stream.
     */
    cString peekCString(bool isFirstLetter = true);

    /**
     * @brief Read a block of braces.
     *
     * @throws cException if the block is invalid
     */
    void readBracesBlock();

    /**
     * @brief Read a variable from a certain type.
     *
     * @return The value of the variable.
     * @param type The type of the variable to read.
     */
    BasicVariable readVariable(Types::Type type);

    /**
     * @brief Remove the C style comments ( '/ /' and '/ *' ) The comments are
     *    replaced
     *   with blanks.
     *
     * @param stream The stream from which the comments should be removed
     * @param length The size of 'stream' in bytes
     */
    static void removeComments(char* stream, uint length);

    /**
     * @brief The function replaces all double newline characters such as
     *    "\\r\\n" or
     *   "\\r\\n" with a space and a single '\\n' character
     *
     * @param stream The stream
     * @param length The size of 'stream' in bytes
     */
    static void removeDoubleNewLine(char* stream, uint length);

    /**
     * @brief Makes the parsing functions easy to write and run.
     *
     * @details Calls the 'removeComments' and 'removeDoubleNewLine' functions.
     *
     * @param stream The stream
     * @param length The size of 'stream' in bytes
     * @see removeComments()
     * @see removeDoubleNewLine()
     */
    static void easyParsing(char* stream, uint length);

private:
    /**
     * @brief Internal implementation of isEOS.
     *
     * @details Uses outside pointer.
     *
     * @param pointer Iterator of the stream
     * @see isEOS
     */
    bool internalIsEOS(char* pointer);

    /**
     * @brief Test that the end-of-file is not reached yet.
     *
     * @throws cException otherwise
     */
    void checkNotEOS();

    /**
     * @brief Deny operator = and copy-constructor, only this class can
     *    duplicate the
     *   parser class
     */
    Parser(const Parser& other);
    Parser& operator = (const Parser& other);

    /**
     * @brief Pointer to the raw data of the textual information
     */
    const char* m_data;
    /**
     * @brief The size of the data in bytes
     */
    const uint m_length;
    /**
     * @brief The end of the iterator
     */
    const char* m_end;
    /**
     * @brief The current iterator
     */
    char* m_pointer;
    /**
     * @brief The current line number
     */
    uint m_lineNumber;
    /**
     * @brief The filename which is parsed.
     */
    const cString& m_filename;
};

/**
 * @brief The macro which throws an exception from a parser.
 *
 * @param parser The parser, which supplies the file name and the line number.
 * @param msg The error message.
 */
#define PARSER_THROW(parser, msg) XSTL_THROW(ParserException, (parser).getFilename(), (parser).getLinenumber(), (msg));
/**
 * @brief The macro which throws an exception from a parser.
 *
 * @param n The condition which should be true.
 * @param parser The parser, which supplies the file name and the line number.
 * @param msg The error message.
 */
#define PARSER_CHECK(n, parser, msg) if (!(n)) { PARSER_THROW(parser, msg); };
/**
 * @brief Catch exceptions and translate them to parser exceptions
 *
 * @param n The statement to execute. Any exception it throws is translated.
 * @param parser The parser, which supplies the file name and the line number.
 * @param msg The error message of the parser exception.
 */
#define PARSER_CATCH(n, parser, msg) XSTL_TRY { n; } XSTL_CATCH_ALL { PARSER_THROW(parser, msg); };


/** @} */
#endif // __TBA_XSTL_PARSER_PARSER_H
