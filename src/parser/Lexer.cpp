#include "Lexer.h"

using Pattern::TokenType;
using std::string;
using std::cout;
using std::endl;
using std::map;

/**
 * Constructor for Lexer
 * @param file - file to read and tokenize
 */
Lexer::Lexer(string const& file) :
        fileContent(readFile(file)),
        tokenList(tokenize())
{
}

/**
 * Prints tokens of Lexer
 */
void Lexer::printTokens() {
    printTokens(getTokenList());
}

/**
 * Prints give list of tokens
 * @param tokens - token list to print
 */
void Lexer::printTokens(const std::vector<Token> &tokens) {
    std::string tokStr;

    std::cout << "Lexical Analysis:" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    for (Token const& t : tokens) {
        tokStr = TOKEN_STRINGS[static_cast<int>(t.getType())];
        tokStr += (t.getValue().empty()) ? "" : (": " + t.getValue());
        cout << tokStr << endl;
    }

    std::cout << std::endl;
}

/**
 * Reads file and converts to string
 * @param file - to read
 * @return string contents of file
 */
std::string Lexer::readFile(string const& file) const {
    std::ifstream inputFile;
    inputFile.open(file);

    if (!inputFile) {
        std::cout << "File " << file << " failed to open" << std::endl;
        exit(1);
    }

    /**
     * BEGIN CITATION
     * Reading file into string
     * https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c
     */
    std::string content(dynamic_cast<std::stringstream const&>(std::stringstream() << inputFile.rdbuf()).str());

    /**
     * END CITATION
     */
    inputFile.close();
    return content;
}

/**
 * Generates a list of tokens from the file of the Lexer
 * @return tokenized list
 */
std::vector<Token> Lexer::tokenize() const {
    //Stores a list of keywords
    map<string, TokenType> keywords = getKeywords();

    //Tracks the start of the lexeme to be parsed
    unsigned long lexemeStart = 0;

    //Tracks the current line/char counts
    unsigned long lineCount = 1;
    unsigned long charCount = 1;

    //Stores the length of the current token
    unsigned long tokLen = 1;

    //Gets the file content and length of the file
    std::string content = getFileContent();
    size_t len = content.length();

    //Stores the list of parsed tokens and the current token
    std::vector<Token> tokens;
    Pattern::TokenType token;

    while (lexemeStart < len - 1) {
        try {
            std::string subStr = content.substr(lexemeStart, len);

            //Parses comments
            if ((token = isComment(subStr, tokLen, lineCount)) != TokenType::NONE) {
                tokens.emplace_back(Token(token, subStr.substr(0, tokLen), lineCount, charCount));
            //Parses string literals
            } else if ((token = isStrLiteral(subStr, tokLen)) != TokenType::NONE) {
                tokens.emplace_back(Token(token, subStr.substr(0, tokLen), lineCount, charCount));
            //Parses operators
            } else if ((token = isOperator(subStr, tokLen)) != TokenType::NONE) {
                //If token is not a newline and not in a comment or string literal
                if (token != TokenType::NEWLINE && token != TokenType::WHITESPACE) {
                    tokens.emplace_back(Token(token, "", lineCount, charCount));
                } else if (token == TokenType::NEWLINE) {
                    lineCount++;
                    charCount = 0;
                }
            } else {
                //Stores lexeme up to next comment, operator, or string literal
                std::string lexeme = buffer(subStr);

                //Parses keywords
                if ((token = isKeyword(lexeme, tokLen, keywords)) != TokenType::NONE) {
                    tokens.emplace_back(Token(token, "", lineCount, charCount));
                //Parses ints
                } else if ((token = isNumeric(lexeme, tokLen)) != TokenType::NONE) {
                    tokens.emplace_back(Token(token, lexeme, lineCount, charCount));
                //Parses identifiers
                } else if ((token = isIdentifier(lexeme, tokLen)) != TokenType::NONE) {
                    tokens.emplace_back(Token(token, lexeme, lineCount, charCount));
                } else {
                    throw ParseException("Invalid token");
                }
            }

            //Increments lexemeStart to the beginning of the current lexeme to parse
            lexemeStart += tokLen;

            //Increments the char count of the current line
            charCount += tokLen;
        } catch (ParseException& e) {
            cout << "Error: " << e.what() << ", line " << lineCount << ", column " << charCount;
            exit(1);
        }
    }
    return tokens;
}

/**
 * Returns a string buffer containing a delimited lexeme
 * (lexeme up until a comment, string, or operator)
 * @param stream - token stream
 * @return lexeme to parse as keyword, number or id
 */
std::string Lexer::buffer(std::string& stream) const {
    unsigned long len = stream.length();
    unsigned long pos = 1;
    unsigned long lineCount = 0;
    unsigned long tokLen = 0;
    std::string buffer;
    std::string lookahead;

    //Repeat until at end of stream of characters
    while (pos < len) {
        //Increment through characters
        pos++;
        //Store current string up to position
        buffer = stream.substr(0, pos);
        //Store last two characters in lookahead string
        lookahead = buffer.substr(buffer.length() - 2, 2);

        //If the lookahead contains an operator, or the beginning of a comment/string,
        //store the string up to that point and break from the loop
        if (isOperator(lookahead, tokLen) != TokenType::NONE
            || isComment(lookahead, tokLen, lineCount) != TokenType::NONE
            || isStrLiteral(lookahead, tokLen) != TokenType::NONE) {

            buffer = buffer.substr(0, buffer.length() - 2);
            break;
        }
    }

    return buffer;
}

/**
 * Checks if a given string is a comment
 * @param s - string to check
 * @param tokLen - current length of token
 * @param lineCount - number of lines in file
 * @return type of token - COMMENT if comment or NONE if not
 */
Pattern::TokenType Lexer::isComment(std::string &s, unsigned long &tokLen, unsigned long &lineCount) const {
    unsigned long len = s.length();
    tokLen = 0;
    unsigned long lines = 0;

    //Detects an open comment sequence
    if (len >= 2 && s.substr(0, 2) == "{-") {
        tokLen += 2;

        //Repeat until token is the size of given string
        while (tokLen < len) {
            //Record any newline encountered
            if (s[tokLen + 1] == '\n') lines++;

            //If a close comment is encountered, record the length of the token,
            //number of lines, and return a comment token type
            if (s.substr(static_cast<unsigned long>(tokLen), 2) == "-}") {
                tokLen += 2;
                lineCount += lines;
                return TokenType::COMMENT;
            }

            tokLen++;
        }

        //If entire string has been check but no close comment has been found,
        //throw error
        throw ParseException("Unclosed comment");
    } else {
        //Otherwise return NONE token type
        return TokenType::NONE;
    }
}

/**
 * Checks is a given string is a string literal
 * @param s - string to check
 * @param tokLen - length of current token
 * @return type of token - STRING if literal or NONE if not
 */
Pattern::TokenType Lexer::isStrLiteral(std::string &s, unsigned long &tokLen) const {
    unsigned long len = s.length();
    tokLen = 0;
    std::string symbol = s.substr(0 , 1);

    //Checks for double or single quote as first character
    if (symbol == "\"" || symbol == "\'") {
        tokLen++;

        //Step through the string while not at the end
        while (tokLen < len - 1) {
            //If another quote of the same type is encountered,
            //return type STRING as string literal has been closed
            if (s.substr(static_cast<unsigned long>(tokLen), 1) == symbol) {
                tokLen++;
                return TokenType::STRING;
            }

            tokLen++;
        }

        //If execution reaches this stage string literal has not been closed,
        //throw error
        throw ParseException("Unclosed string literal");
    } else {
        //Otherwise token type is not a STRING
        return TokenType::NONE;
    }
}

/**
 * Checks if a given string starts with an operator
 * @param s - string to check
 * @param tokLen - length of current token
 * @return type of token (NONE if not an operator or whitespace)
 */
Pattern::TokenType Lexer::isOperator(std::string &s, unsigned long &tokLen) const {
    std::string sub = s.substr(0, 2);
    char first = sub.at(0);
    char second = sub.at(1);
    tokLen = 1;

    //Check first (and possibly second) character
    // in string to detect the correct operator
    switch (first) {
        case '<':
            if (second == '=') {
                tokLen++;
                return TokenType::LTE;
            } else {
                return TokenType::LT;
            }
        case '>':
            if (second == '=') {
                tokLen++;
                return TokenType::GTE;
            } else {
                return TokenType::GT;
            }
        case '=':
            if (second == '=') {
                tokLen++;
                return TokenType::EQ;
            } else {
                throw ParseException("Invalid token '='");
            }
        case ':':
            if (second == '=') {
                tokLen++;
                return TokenType::ASSIGN;
            } else {
                throw ParseException("Invalid token ':'");
            }
        case ';':
            return TokenType::SEMI;
        case '+':
            return TokenType::PLUS;
        case '-':
            return TokenType::MINUS;
        case '/':
            return TokenType::DIVIDE;
        case '*':
            return TokenType::MULTIPLY;
        case '(':
            return TokenType::LPAREN;
        case ')':
            return TokenType::RPAREN;
        case '\n':
                return TokenType::NEWLINE;
        case '\t':
        case ' ':
        case '\r':
            return TokenType::WHITESPACE;
        case ',':
            return TokenType::COMMA;
        default:
            return TokenType::NONE;
    }
}

/**
 * Checks if given string is a keyword from the map of keywords
 * @param s - string to check
 * @param tokLen - length of token
 * @param keywords - map containing pairs of
 * keywords and their token types
 * @return token type at the keyword key in the map
 * or NONE if not a keyword
 */
Pattern::TokenType Lexer::isKeyword(string &s, unsigned long &tokLen, map<string, TokenType> const &keywords) const {
    tokLen = static_cast<int>(s.length());

    //Checks if keyword is in the map
    if (keywords.find(s) != keywords.end()) {
        //Returns token type if keyword is present
        return keywords.at(s);
    } else {
        //Otherwise return NONE
        return TokenType::NONE;
    }
}

/**
 * Checks if token is a number (digit+)
 * @param s - string to check
 * @param tokLen - length of token
 * @return token type - NUM if numeric or NONE if not
 */
Pattern::TokenType Lexer::isNumeric(string &s, unsigned long &tokLen) const {
    tokLen = static_cast<int>(s.length());

    //Checks for valid number
    for (char c : s) {
        if (c < '0' || c > '9') {
            return TokenType::NONE;
        }
    }

    return TokenType::NUM;
}

/**
 * Checks if a string conforms to the identifier format
 * @param s - string to check
 * @param tokLen - length of token
 * @return token type - ID if string matches regex pattern, otherwise NONE
 */
Pattern::TokenType Lexer::isIdentifier(string &s, unsigned long &tokLen) const {
    //ID token can start with letter or underscores
    // followed by zero or more letters, underscores, or numbers
    string id = "([a-zA-Z_]+)([a-zA-Z_0-9])*";
    std::regex pattern(id);

    tokLen = static_cast<int>(s.length());

    if (std::regex_match(s, pattern)) {
        return TokenType::ID;
    } else {
        return TokenType::NONE;
    }
}


/**
 * Creates a map defining which string keywords map to which tokens
 * @return map of keywords/tokens
 */
map<string, TokenType> Lexer::getKeywords() const {
    std::map<std::string, Pattern::TokenType> keywords {
            {"begin", TokenType::BEGIN},
            {"end", TokenType::END},
            {"program", TokenType::PROGRAM},
            {"procedure", TokenType::PROCEDURE},
            {"return", TokenType::RETURN},
            {"if", TokenType::IF},
            {"then", TokenType::THEN},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"print", TokenType::PRINT},
            {"println", TokenType::PRINTLN},
            {"var", TokenType::VAR},
            {"get", TokenType::GET},
            {"and", TokenType::AND},
            {"or", TokenType::OR},
            {"not", TokenType::NOT},
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE}
    };

    return keywords;
}

/**
 * Getter for fileContent
 * @return - fileContent
 */
const string &Lexer::getFileContent() const {
    return fileContent;
}

/**
 * Getter for token list
 * @return - list of tokens
 */
const std::vector<Token> &Lexer::getTokenList() const {
    return tokenList;
}

