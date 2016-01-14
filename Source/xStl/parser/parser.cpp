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

#include "xStl/xStlPrecompiled.h"
/*
 * Parser.cpp
 *
 * Implementation of the Parser class.
 *
 * Author: Elad Raz <e@eladraz.com>
 */
#include "xStl/types.h"
#include "xStl/except/assert.h"
#include "xStl/except/trace.h"
#include "xStl/data/array.h"
#include "xStl/data/string.h"
#include "xStl/data/char.h"
#include "xStl/parser/parser.h"
#include "xStl/parser/braces.h"

// The check macro for the parser
#define PARSER_INTERNAL_CHECK(n, msg) PARSER_CHECK(n, *this, msg)


const cString Parser::unknownFilename = XSTL_STRING("<Unknown>");

Parser::Parser(const char* data,
               char* pointer,
               uint length,
               uint lineNumber,
               const cString& filename) :
    m_data(data),
    m_length(length),
    m_end(data + length),
    m_pointer(pointer),
    m_lineNumber(lineNumber),
    m_filename(filename)
{
    CHECK((m_data != NULL) && (m_length > 0));
    CHECK(m_pointer >= m_data);
    CHECK(m_pointer < m_end);
}

Parser::Parser(const Parser& other) :
    m_data(other.m_data),
    m_length(other.m_length),
    m_end(other.m_end),
    m_pointer(other.m_pointer),
    m_lineNumber(other.m_lineNumber),
    m_filename(other.m_filename)
{
}

uint Parser::getIterator() const
{
    return (uint)(m_pointer - m_data);
}

uint Parser::getLinenumber() const
{
    return m_lineNumber;
}

const cString& Parser::getFilename() const
{
    return m_filename;
}

void Parser::checkNotEOS()
{
    PARSER_INTERNAL_CHECK(!isEOS(), "Unexcepected end-of-file reached");
}

bool Parser::isEOS()
{
    return internalIsEOS(m_pointer);
}

char Parser::peekChar()
{
    checkNotEOS();
    return *m_pointer;
}

char Parser::readChar()
{
    checkNotEOS();
    char ret = *(m_pointer++);

    if (cChar::isNewLine(ret))
    {
        m_lineNumber++;
    }

    return ret;
}

bool Parser::internalIsEOS(char* pointer)
{
    ASSERT(pointer <= m_end);
    if ((pointer == m_end) ||
        (*pointer == 0))
    {
        return true;
    }

    return false;
}

void Parser::readBlanks()
{
    getBlanks();
}

bool Parser::readBlanksUntilEOL()
{
    if (isEOS())
        return true;

    char ch = peekChar();
    while (cChar::isBlanket(ch))
    {
        readChar();
        if (isEOS())
            return true;

        ch = peekChar();
    }

    if (cChar::isNewLine(ch))
        return true;

    return false;
}

cString Parser::getBlanks()
{
    if (isEOS())
        return cString();

    cString ret;
    char ch = peekChar();

    while (cChar::isBlanket(ch) || cChar::isNewLine(ch))
    {
        readChar();
        ret+= ch;
        if (isEOS())
            break;
        ch = peekChar();
    }

    return ret;
}

cString Parser::readWord()
{
    cString ret;

    if (isEOS())
        return ret;
    char ch = peekChar();

    PARSER_INTERNAL_CHECK(!cChar::isBlanket(ch), "Letter was excpected");

    while ((!cChar::isBlanket(ch)) &&
           (!cChar::isNewLine(ch)))
    {
        readChar();
        ret+= ch;
        if (isEOS())
            break;
        ch = peekChar();
    }

    return ret;
}

cString Parser::readCString(bool isFirstLetter)
{
    char ch = readChar();
    // The first character must be a letter
    if (isFirstLetter)
    {
        PARSER_INTERNAL_CHECK(cChar::isLetter(ch) || (ch == '_'), "Letter was excpected");
    }

    // Read the string
    cString ret(ch);

    if (!isEOS()) do
    {
        ch = peekChar();
        if (cChar::isLetter(ch) ||
            cChar::isDigit(ch) ||
            (ch == '_'))
        {
            ret+= (character)readChar();
        } else
            break;
    } while (!isEOS());

    return ret;
}

cString Parser::readQuateString(bool isCString)
{
    // The first character must be a quate
    PARSER_INTERNAL_CHECK(readChar() == '"', "'string' was excpected here");

    // Read the string
    cString ret;
    char ch = readChar();
    while (ch != '"')
    {
        if ((ch == '\\') && isCString)
        {
            // Read the next character and parse it.
            ch = readChar();
            switch (ch)
            {
            case 'a': ch = '\a'; break; // Alert (Bell)
            case 'b': ch = '\b'; break; // backspace
            case 'f': ch = '\f'; break; // Formfeed
            case 'n': ch = '\n'; break; // Newline
            case 'r': ch = '\r'; break; // Carraige return
            case 't': ch = '\t'; break; // tab
            case 'v': ch = '\v'; break; // Vertical tab
            case '\'': ch = '\''; break; // '
            case '\"': ch = '\"'; break; // "
            case '\\': ch = '\\'; break; // \ char.
            case '?': ch = '\?'; break; // Literal question mark
            default:
                // Unknown character
                PARSER_THROW(*this, "Unknown special character");
            }
        }

        // And append the character
        ret+= (character)(ch);
        // Read the next character
        ch = readChar();
    }

    return ret;
}

cString Parser::readSpeicalString()
{
    cString ret;
    char ch = readChar();

    while (cChar::isLetter(ch) || cChar::isDigit(ch) ||
           (ch == '_') || (ch == '.') || (ch == '$'))
    {
        if (ch == '$')
        {
            // Read a special mark.
            PARSER_INTERNAL_CHECK(readChar() == '(', "Open brace was excpected here");
            cString add = readCString();
            PARSER_INTERNAL_CHECK(readChar() == ')', "Close brace was excpected here");
            ret+= XSTL_CHAR('$'); ret+= XSTL_CHAR('(');
            ret+= add;
            ret+= XSTL_CHAR(')');
        } else
        {
            ret+= (character)ch;
        }

        // Exit when EOS riches.
        if (isEOS())
        {
            break;
        }

        // Read the next character
        ch = readChar();
    }

    // Error at first digit
    PARSER_INTERNAL_CHECK(ret.length() > 0, "Special string have been excpected");

    return ret;
}

uint Parser::readCUnsignedInteger()
{
    uint ret;
    PARSER_INTERNAL_CHECK(isCUnsignedInteger(&ret, true),
                          "Invalid C integer format");
    return ret;
}

bool Parser::isCUnsignedInteger(uint* retNumber,
                                bool shouldAdvancePosition)
{
    // Duplicate this parser
    Parser dup(*this);
    Parser* object = &dup;
    if (shouldAdvancePosition)
        object = this;

    // Flags which marks wheter the base of the number is 16.
    bool isHex = false;
    // Test the first character special marks
    cString number = object->readCString(false);
    uint i = 0;
    if (number[0] == '0')
    {
        // Test that the number is not zero
        if (number.length() == 1)
        {
            if (retNumber != NULL)
                *retNumber = 0;
            return true;
        }
        // Base 8 or hex number
        if (number[1] != 'x')
            return false; // PARSER_INTERNAL_CHECK "Hexdecimal number was excpected"
        i = 2;
        isHex = true;
    }

    // Try to read CUnsignedInteger
    uint ret = 0;
    uint base = isHex ? 16 : 10;
    for (; i < number.length(); i++)
    {
        character nextDigit = cChar::getUppercase(number[i]);
        if (cChar::isLetter(nextDigit))
        {
            // Test 64-bit number reading
            if (nextDigit == 'L')
            {
                if (i != (number.length() - 1))
                    return false; // PARSER_INTERNAL_CHECK("Long number invalid position");
                // Stop scan and return value.
                break;
            }

            // Hex number
            if (!(isHex && (nextDigit >= 'A') && (nextDigit <= 'F')))
                return false; // PARSER_INTERNAL_CHECK("Digit out of range");
            // TODO! Check for overflow
            ret*= base;
            ret+= nextDigit - 'A' + 10;
        } else
        {
            if (!cChar::isDigit(nextDigit))
                return false;
            // TODO! Check for overflow
            ret*= base;
            ret+= cChar::getDigit(nextDigit);
        }
    }

    // Check for return value
    if (retNumber != NULL)
        *retNumber = ret;
    return true;
}

uint Parser::readHexString()
{
    // Test the first character special marks
    cString number = readCString(false);

    uint ret = 0;
    for (uint i = 0; i < number.length(); i++)
    {
        character nextDigit = cChar::getUppercase(number[i]);
        if (cChar::isLetter(nextDigit))
        {
            // Test 64-bit number reading
            if ((nextDigit == 'L') || (nextDigit == 'H'))
            {
                PARSER_INTERNAL_CHECK(i == (number.length() - 1), "Long number invalid position");
                return ret;
            }

            // Hex number
            PARSER_INTERNAL_CHECK((nextDigit >= 'A') && (nextDigit <= 'F'), "Digit out of range");
            ret*= 16;
            ret+= nextDigit - 'A' + 10;
        } else
        {
            ret*= 16;
            ret+= cChar::getDigit(nextDigit);
        }
    }

    return ret;
}

bool Parser::readSignedValue()
{
    checkNotEOS();
    if (peekChar() == '-')
    {
        readChar();
        return true;
    } else if (peekChar() == '+')
    {
        readChar();
    }
    return false;
}

cString Parser::peekCString(bool isFirstLetter)
{
    // Save the index
    char* pointer = m_pointer;
    // Peek the string.
    cString ret(readCString(isFirstLetter));
    // Restore the index
    // There is no dangerous for reading new line.
    m_pointer = pointer;

    return ret;
}

void Parser::readBracesBlock()
{
    XSTL_TRY
    {
        // Read the open braces
        Braces b(readChar());
        // Skip all characters
        bool stop = false;
        // Skip string characters
        bool isString = false;

        while ((!stop) && (m_pointer < m_end))
        {
            // Test newline
            char ch = readChar();
            if (cChar::isNewLine(ch))
            {
                m_lineNumber++;
                isString = false;
            }

            if (ch == '"')
            {
                isString = !isString;
            }

            if (!isString)
            {
                // Eat the character
                stop = b.eatCharacter(ch);
            }
        }

        if (!stop)
        {
            XSTL_THROW(BracesException, '?', b.getNextCloserCharacer());
        }

        if (m_pointer == m_end)
        {
            // We have reached the EOS...
            m_pointer--;
        }
    }
    XSTL_CATCH(BracesException& e)
    {
        // Translate exception
        cString exception(XSTL_STRING("Braces error. Found '"));
        exception+= e.m_founded;
        exception+= XSTL_STRING("' excpected '");
        exception+= e.m_excpected;
        exception+= XSTL_STRING("'.");

        PARSER_THROW(*this, exception)
    }
}

BasicVariable Parser::readVariable(Types::Type type)
{
    // Prepare the variable.
    BasicVariable retData(type);

    // Switch the different types
    if (Types::isIntegerType(type))
    {
        // Try to read the signed number
        bool isSigned = false;
        if (Types::isSignedIntegerType(type))
        {
            isSigned = readSignedValue();
        }

        // And read the number.
        uint number = readCUnsignedInteger();

        if (isSigned)
        {
            PARSER_INTERNAL_CHECK(number <= 0x80000000, "Overflow occuered");
            int signedNumber = -((int)number);
            retData.setSignedInteger(signedNumber);
        } else
        {
            if (Types::isSignedIntegerType(type))
            {
                retData.setSignedInteger(number);
            } else
            {
                retData.setUnsignedInteger(number);
            }
        }
    } else if (Types::isStringType(type))
    {
        retData.setString(readQuateString());
    } else
    {
        // Unknown type exception.
        PARSER_THROW(*this, "Unknown variable type");
    }

    return retData;
}

void Parser::removeComments(char* stream, uint length)
{
    // The buffer and it's remaining length
    if (length == 0) return;

    // The space character which is replaced
    static const char space = ' ';

    // The state of the comments:
    //   true means that we are inside a comment block,
    //   false means that we are outside a comment block
    bool isComment = false;
    // The type of the comment:
    //   true means that the comment are from type '//' and newline stops the comment
    //   flase means that the comment are from type '/*/ and only '*/' stops the comment
    bool isEndl = false;
    // The internal state, wheter a '/' is found
    bool isMagicSlash = false;
    // The internal state, wheter a '*' is found
    bool isMagicStar = false;
    // Test whether we are inside a string
    bool isString = false;

    // Iterator over the text
    for (uint i = 0; i < length; i++)
    {
        // Magic for comments
        if ((stream[i] == '/')  && (!isString))
        {
            if (!isComment)
            {
                if (isMagicSlash)
                {
                    // Find a sequence of '//' outside a comment block
                    isMagicSlash = false;
                    isEndl = true;
                    isComment = true;
                    // Reset the last two // character
                    stream[i] = space;
                    stream[i - 1] = space;
                } else
                {
                    isMagicSlash = true;
                }
            } else
            {
                // We are inside a comment block, check */
                if (isMagicStar)
                {
                    isMagicSlash = false;
                    isComment = false;
                    // Reset the last two */ character
                    stream[i] = space;
                    stream[i - 1] = space;
                }
            }
            // Reset the sequence counter
            isMagicStar = false;
        } else if ((stream[i] == '*') && (!isString))
        {
            if (!isComment)
            {
                if (isMagicSlash)
                {
                    // Find a sequence of '/*' outside a comment block
                    isEndl = false;
                    isComment = true;
                    // Reset the last two /* character
                    stream[i] = space;
                    stream[i - 1] = space;
                } else
                {
                    isMagicStar = true;
                }
            } else
            {
                stream[i] = space;
                isMagicStar = true;
            }
            // Reset the sequence counter
            isMagicSlash = false;
        } else if (stream[i] == '"')
        {
            if (!isComment)
            {
                isString = !isString;
            } else
            {
                stream[i] = space;
            }
        } else
        {
            // Reset the sequence counter
            isMagicSlash = false;
            isMagicStar = false;
            bool newLine = cChar::isNewLine(stream[i]);
            isString = newLine ? false : isString;
            // Test whether we should eat the character or we are in a comment
            if (isComment)
            {
                if ((isEndl) && (newLine))
                {
                    // Newline found for // comment block
                    isComment = false;
                } else
                {
                    if (!newLine)
                    {
                        stream[i] = space;
                    }
                }
            }
        }
    }
}

void Parser::removeDoubleNewLine(char* stream, uint length)
{
    char* i;
    char* end = stream + length - 1;
    for (i = stream; i < end; i++)
    {
        if (((*i == '\r') && (*(i+1) == '\n')) ||
            ((*i == '\n') && (*(i+1) == '\r')))
        {
            *i = ' ';
            *(i+1) = '\n';
        }
    }
}

void Parser::easyParsing(char* stream, uint length)
{
    removeComments(stream, length);
    removeDoubleNewLine(stream, length);
}

