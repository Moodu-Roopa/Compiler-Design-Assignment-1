#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 100

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER_CONSTANT,
    TOKEN_FLOAT_CONSTANT,
    TOKEN_STRING_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_RESERVED_WORD,

    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE, TOKEN_MODULUS,
    TOKEN_INCREMENT, TOKEN_DECREMENT,

    TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS_THAN, TOKEN_LESS_EQUAL,
    TOKEN_GREATER_THAN, TOKEN_GREATER_EQUAL,

    TOKEN_LOGICAL_AND, TOKEN_LOGICAL_OR, TOKEN_LOGICAL_NOT,

    TOKEN_BITWISE_AND, TOKEN_BITWISE_OR, TOKEN_BITWISE_XOR,
    TOKEN_BITWISE_NOT, TOKEN_LEFT_SHIFT, TOKEN_RIGHT_SHIFT,

    TOKEN_ASSIGN, TOKEN_PLUS_ASSIGN, TOKEN_MINUS_ASSIGN, TOKEN_MUL_ASSIGN,
    TOKEN_DIV_ASSIGN, TOKEN_MOD_ASSIGN, TOKEN_AND_ASSIGN, TOKEN_OR_ASSIGN,
    TOKEN_XOR_ASSIGN, TOKEN_LSHIFT_ASSIGN, TOKEN_RSHIFT_ASSIGN,

    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_LBRACKET, TOKEN_RBRACKET,

    TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_COLON, TOKEN_DOT,
    TOKEN_QUESTION, TOKEN_BACKSLASH, TOKEN_PREPROCESSOR,

    TOKEN_ERROR
} TokenType;

char *reserved_words[] = {
    "int", "float", "char", "double", "void", "short", "long", "signed", "unsigned",
    "if", "else", "while", "for", "do", "switch", "case", "default", "break",
    "continue", "return", "goto", "sizeof", "typedef", "struct", "union", "enum",
    "const", "volatile", "extern", "static", "auto", "register", NULL
};

char yytext[MAX_LEN];
int token_counter = 1;
int yyleng;
int yylineno = 1;

const char* token_type_name(TokenType type) {
    static const char* names[] = {
        "EOF", "IDENTIFIER", "INTEGER_CONSTANT", "FLOAT_CONSTANT", "STRING_LITERAL",
        "CHARACTER_LITERAL", "RESERVED_WORD",
        "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MODULUS", "INCREMENT", "DECREMENT",
        "EQUAL", "NOT_EQUAL", "LESS_THAN", "LESS_EQUAL", "GREATER_THAN", "GREATER_EQUAL",
        "LOGICAL_AND", "LOGICAL_OR", "LOGICAL_NOT",
        "BITWISE_AND", "BITWISE_OR", "BITWISE_XOR", "BITWISE_NOT", "LEFT_SHIFT", "RIGHT_SHIFT",
        "ASSIGN", "PLUS_ASSIGN", "MINUS_ASSIGN", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
        "AND_ASSIGN", "OR_ASSIGN", "XOR_ASSIGN", "LSHIFT_ASSIGN", "RSHIFT_ASSIGN",
        "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET",
        "SEMICOLON", "COMMA", "COLON", "DOT", "QUESTION", "BACKSLASH", "PREPROCESSOR",
        "ERROR"
    };
    return names[type];
}

// void print_token(TokenType type, const char* text) {
//     printf("%-10s %-15s %-10d %s\n", text, token_type_name(type), type, "TOKEN ID");
//     token_counter++; return;
//     printf("[Line %d] %-20s: %s\n", yylineno, token_type_name(type), text);
// }
void print_token(TokenType type, const char* text) {
    printf("%-10s %-15s %-10d\n", text, token_type_name(type), type);
}

int is_reserved_word(char *str) {
    for (int i = 0; reserved_words[i] != NULL; i++) {
        if (strcmp(str, reserved_words[i]) == 0)
            return 1;
    }
    return 0;
}

int is_operator_char(char c) {
    return strchr("+-*/%<>=!&|^~", c) != NULL;
}

void analyze(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (isspace(str[i])) {
            if (str[i] == '\n') yylineno++;
            i++;
            continue;
        }

        // Comments
        if (str[i] == '/' && str[i + 1] == '/') {
            i += 2;
            while (str[i] != '\0' && str[i] != '\n') i++;
            continue;
        }
        if (str[i] == '/' && str[i + 1] == '*') {
            i += 2;
            while (str[i] != '\0' && !(str[i] == '*' && str[i + 1] == '/')) {
                if (str[i] == '\n') yylineno++;
                i++;
            }
            if (str[i] != '\0') i += 2;
            continue;
        }

        // Identifiers and reserved words
        if (isalpha(str[i]) || str[i] == '_') {
            int j = 0;
            while (isalnum(str[i]) || str[i] == '_') yytext[j++] = str[i++];
            yytext[j] = '\0'; yyleng = j;
            print_token(is_reserved_word(yytext) ? TOKEN_RESERVED_WORD : TOKEN_IDENTIFIER, yytext);
            continue;
        }

        // Numbers
        if (isdigit(str[i]) || (str[i] == '.' && isdigit(str[i + 1]))) {
            int j = 0, has_dot = 0, has_exp = 0;
            if (str[i] == '.') {
                has_dot = 1;
                yytext[j++] = str[i++];
            }
            while (isdigit(str[i])) yytext[j++] = str[i++];
            if (str[i] == '.' && !has_dot) {
                has_dot = 1;
                yytext[j++] = str[i++];
                while (isdigit(str[i])) yytext[j++] = str[i++];
            }
            if ((str[i] == 'e' || str[i] == 'E')) {
                has_exp = 1;
                yytext[j++] = str[i++];
                if (str[i] == '+' || str[i] == '-') yytext[j++] = str[i++];
                while (isdigit(str[i])) yytext[j++] = str[i++];
            }
            if (strchr("fFuUlL", str[i])) {
                yytext[j++] = str[i++];
            }
            yytext[j] = '\0'; yyleng = j;
            print_token((has_dot || has_exp) ? TOKEN_FLOAT_CONSTANT : TOKEN_INTEGER_CONSTANT, yytext);
            continue;
        }

        // String literals
        if (str[i] == '"') {
            int j = 0;
            yytext[j++] = str[i++];
            while (str[i] && str[i] != '"') {
                if (str[i] == '\\') yytext[j++] = str[i++];
                yytext[j++] = str[i++];
            }
            if (str[i] == '"') yytext[j++] = str[i++];
            yytext[j] = '\0'; yyleng = j;
            print_token(yytext[j - 1] == '"' ? TOKEN_STRING_LITERAL : TOKEN_ERROR, yytext);
            continue;
        }

        // Character literals
        if (str[i] == '\'') {
            int j = 0;
            yytext[j++] = str[i++];
            if (str[i] == '\\') yytext[j++] = str[i++];
            yytext[j++] = str[i++];
            if (str[i] == '\'') yytext[j++] = str[i++];
            yytext[j] = '\0'; yyleng = j;
            print_token(yytext[j - 1] == '\'' ? TOKEN_CHARACTER_LITERAL : TOKEN_ERROR, yytext);
            continue;
        }

        // Operators (longest match)
        int j = 0;
        yytext[j++] = str[i++];
        if (is_operator_char(yytext[0])) {
            while (is_operator_char(str[i]) && j < 3) yytext[j++] = str[i++];
            yytext[j] = '\0'; yyleng = j;
            while (j > 0) {
                yytext[j] = '\0';
                TokenType type = TOKEN_ERROR;
                #define MATCH(op, tok) else if (strcmp(yytext, op) == 0) type = tok
                if (0) {}
                MATCH("++", TOKEN_INCREMENT); MATCH("--", TOKEN_DECREMENT);
                MATCH("==", TOKEN_EQUAL); MATCH("!=", TOKEN_NOT_EQUAL);
                MATCH("<=", TOKEN_LESS_EQUAL); MATCH(">=", TOKEN_GREATER_EQUAL);
                MATCH("&&", TOKEN_LOGICAL_AND); MATCH("||", TOKEN_LOGICAL_OR);
                MATCH("+=", TOKEN_PLUS_ASSIGN); MATCH("-=", TOKEN_MINUS_ASSIGN);
                MATCH("*=", TOKEN_MUL_ASSIGN); MATCH("/=", TOKEN_DIV_ASSIGN);
                MATCH("%=", TOKEN_MOD_ASSIGN); MATCH("&=", TOKEN_AND_ASSIGN);
                MATCH("|=", TOKEN_OR_ASSIGN); MATCH("^=", TOKEN_XOR_ASSIGN);
                MATCH("<<=", TOKEN_LSHIFT_ASSIGN); MATCH(">>=", TOKEN_RSHIFT_ASSIGN);
                MATCH("<<", TOKEN_LEFT_SHIFT); MATCH(">>", TOKEN_RIGHT_SHIFT);
                MATCH("+", TOKEN_PLUS); MATCH("-", TOKEN_MINUS);
                MATCH("*", TOKEN_MULTIPLY); MATCH("/", TOKEN_DIVIDE);
                MATCH("%", TOKEN_MODULUS); MATCH("=", TOKEN_ASSIGN);
                MATCH("<", TOKEN_LESS_THAN); MATCH(">", TOKEN_GREATER_THAN);
                MATCH("&", TOKEN_BITWISE_AND); MATCH("|", TOKEN_BITWISE_OR);
                MATCH("^", TOKEN_BITWISE_XOR); MATCH("~", TOKEN_BITWISE_NOT);
                MATCH("!", TOKEN_LOGICAL_NOT);
                #undef MATCH
                if (type != TOKEN_ERROR) { print_token(type, yytext); break; }
                j--; i--;
            }
            if (j == 0) { yytext[1] = '\0'; print_token(TOKEN_ERROR, yytext); }
            continue;
        }

        // Delimiters and punctuation
        TokenType type = TOKEN_ERROR;
        switch (str[i - 1]) {
            case '(': type = TOKEN_LPAREN; break;
            case ')': type = TOKEN_RPAREN; break;
            case '{': type = TOKEN_LBRACE; break;
            case '}': type = TOKEN_RBRACE; break;
            case '[': type = TOKEN_LBRACKET; break;
            case ']': type = TOKEN_RBRACKET; break;
            case ';': type = TOKEN_SEMICOLON; break;
            case ',': type = TOKEN_COMMA; break;
            case ':': type = TOKEN_COLON; break;
            case '.': type = TOKEN_DOT; break;
            case '?': type = TOKEN_QUESTION; break;
            case '\\': type = TOKEN_BACKSLASH; break;
            case '#': type = TOKEN_PREPROCESSOR; break;
        }
        if (type != TOKEN_ERROR) {
            yytext[0] = str[i - 1];
            yytext[1] = '\0'; yyleng = 1;
            print_token(type, yytext);
        }
    }
}

// int main() {
//     FILE *file = fopen("input.c", "r");
//     if (!file) {
//         printf("Error opening input file.\n");
//         return 1;
//     }

//     char program[5000];
//     int i = 0;
//     char c;
//     while ((c = fgetc(file)) != EOF && i < sizeof(program) - 1) {
//         program[i++] = c;
//     }
//     program[i] = '\0';
//     fclose(file);

//     printf("Lexical Analysis Output:\n\n");
//     printf("%-10s %-15s %-10s %s\n", "INPUT", "TOKEN TYPE", "TOKEN ID", "LABEL");
//     printf("---------------------------------------------------------------\n");
//     analyze(program);
//     return 0;
// }
int main() {
    FILE *file = fopen("input.c", "r");
    if (!file) {
        printf("Error opening input file.\n");
        return 1;
    }

    char program[5000];
    int i = 0;
    char c;
    while ((c = fgetc(file)) != EOF && i < sizeof(program) - 1) {
        program[i++] = c;
    }
    program[i] = '\0';
    fclose(file);

    printf("Lexical Analysis Output:\n\n");
    printf("%-10s %-15s %-10s\n", "INPUT", "TOKEN TYPE", "TOKEN ID");
    printf("------------------------------------------------------\n");

    analyze(program);
    return 0;
}
