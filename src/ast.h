#ifndef AST_H
#define AST_H

#include "token.h"

typedef struct Declaration Declaration;
typedef struct DeclarationList DeclarationList;

typedef struct Statement Statement;

typedef struct Expression Expression;
typedef struct ExpressionList ExpressionList;
typedef struct ArgumentList ArgumentList;

typedef struct WhenEntry WhenEntry;
typedef struct WhenEntryList WhenEntryList;

typedef struct Block Block;
typedef struct FunctionBody FunctionBody;
typedef struct ParameterList ParameterList;
typedef struct Function Function;
typedef struct NamedFunction NamedFunction;
typedef struct NamedFunctionList NamedFunctionList;

typedef enum ExprContext { LOAD, STORE } ExprContext;

typedef enum FunctionNotation { FUNC_EXPRESSION, FUNC_BLOCK } FunctionNotation;

typedef struct AST {
    DeclarationList* body;
} AST;

typedef struct Declaration {
    enum {
        DECL_CLASS,
        DECL_FUNCTION,
        DECL_VARIABLE,
        DECL_STATEMENT
    } type;

    union {
        struct {
            Token identifier;
            Token superclass;
            NamedFunctionList* body;
        } classDecl;

        struct {
            NamedFunction* function;
        } functionDecl;

        struct {
            Token identifier;
            Expression* value;
        } variableDecl;

        Statement* statement;
    } as;
} Declaration;

typedef struct Statement {
    enum {
        STMT_FOR,
        STMT_WHILE,
        STMT_BREAK,
        STMT_CONTINUE,
        STMT_WHEN,
        STMT_IF,
        STMT_RETURN,
        STMT_PRINT,
        STMT_BLOCK,
        STMT_EXPRESSION
    } type;

    union {
        struct {
            Declaration* initializer;
            Expression* condition;
            Expression* increment;
            Statement* body;
        } forStmt;

        struct {
            Expression* condition;
            Statement* body;
        } whileStmt;

        struct {
            Token keyword;
        } breakStmt;

        struct {
            Token keyword;
        } continueStmt;

        struct {
            Expression* control;
            WhenEntryList* entries;
            Statement* elseBranch;
        } whenStmt;
        
        struct {
            Expression* condition;
            Statement* thenBranch;
            Statement* elseBranch;
        } ifStmt;

        struct {
            Token keyword;
            Expression* expression;
        } returnStmt;

        struct {
            Expression* expression;
        } printStmt;

        struct {
            Block* block;
        } blockStmt;

        Expression* expression;
    } as;
} Statement;

typedef struct Expression {
    enum {
        EXPR_CALL,
        EXPR_PROPERTY,
        EXPR_SUPER,
        EXPR_ASSIGNMENT,
        EXPR_COMPOUND_ASSIGNMNET,
        EXPR_LOGICAL,
        EXPR_CONDITIONAL,
        EXPR_BINARY,
        EXPR_UNARY,
        EXPR_PREFIX_INC,
        EXPR_POSTFIX_INC,
        EXPR_LITERAL,
        EXPR_LAMBDA,
        EXPR_IDENTIFIER,
    } type;

    union {
        struct {
            Expression* callee;
            ArgumentList* arguments;
        } callExpr;

        struct {
            Expression* object;
            Token property;
            ExprContext context;
        } propertyExpr;

        struct {
            Token keyword;
            Token method;
        } superExpr;

        struct {
            Expression* target;
            Expression* value;
        } assignmentExpr;

        struct {
            Expression* target;
            Token op;
            Expression* value;
        } compoundAssignmentExpr;

        struct {
            Expression* left;
            Token op;
            Expression* right;
        } logicalExpr;

        struct {
            Expression* condition;
            Expression* thenBranch;
            Expression* elseBranch;
        } conditionalExpr;

        struct {
            Expression* left;
            Token op;
            Expression* right;
        } binaryExpr;

        struct {
            Token op;
            Expression* target;
        } prefixIncExpr;

        struct {
            Token op;
            Expression* target;
        } postfixIncExpr;

        struct {
            Token op;
            Expression* expression;
        } unaryExpr;

        struct {
            Token value;
        } literalExpr;

        struct {
            Function* function;
        } lambdaExpr;

        struct {
            Token identifier;
            ExprContext context;
        } identifierExpr;
    } as;
} Expression;

typedef struct ExpressionList {
    Expression* expression;
    ExpressionList* next;
} ExpressionList;

typedef struct ArgumentList {
    Expression* expression;
    ArgumentList* next;
} ArgumentList;

typedef struct WhenEntry {
    ExpressionList* cases;
    Statement* body;
} WhenEntry;

typedef struct WhenEntryList {
    WhenEntry* entry;
    WhenEntryList* next;
} WhenEntryList;

typedef struct Block {
    DeclarationList* body;
} Block;

typedef struct FunctionBody {
    FunctionNotation notation;
    union {
        Expression* expression;
        Block* block;
    } as;
} FunctionBody;

typedef struct ParameterList {
    Token parameter;
    ParameterList* next;
} ParameterList;

typedef struct Function {
    ParameterList* parameters;
    FunctionBody* body;
} Function;

typedef struct NamedFunction {
    Token identifier;
    Function* function;
} NamedFunction;

typedef struct NamedFunctionList {
    NamedFunction* function;
    NamedFunctionList* next;
} NamedFunctionList;

typedef struct DeclarationList {
    Declaration* declaration;
    DeclarationList* next;
} DeclarationList;

AST* ast_new_tree(DeclarationList* body);
void ast_delete_tree(AST* ast);

void ast_delete_declaration(Declaration* declaration);
Declaration* ast_new_class_decl(Token identifier, Token superclass, NamedFunctionList* body);
void ast_delete_class_decl(Declaration* declaration);
Declaration* ast_new_function_decl(NamedFunction* function);
void ast_delete_function_decl(Declaration* declaration);
Declaration* ast_new_variable_decl(Token identifier, Expression* value);
void ast_delete_variable_decl(Declaration* declaration);
Declaration* ast_new_statement_decl(Statement* statement);
void ast_delete_statement_decl(Declaration* declaration);

void ast_delete_statement(Statement* statement);
Statement* ast_new_for_stmt(Declaration* initializer, Expression* condition, Expression* increment, Statement* body);
void ast_delete_for_stmt(Statement* statement);
Statement* ast_new_while_stmt(Expression* condition, Statement* body);
void ast_delete_while_stmt(Statement* statement);
Statement* ast_new_break_stmt(Token keyword);
void ast_delete_break_stmt(Statement* statement);
Statement* ast_new_continue_stmt(Token keyword);
void ast_delete_continue_stmt(Statement* statement);
Statement* ast_new_when_stmt(Expression* control, WhenEntryList* entries, Statement* elseBranch);
void ast_delete_when_stmt(Statement* statement);
Statement* ast_new_if_stmt(Expression* condition, Statement* thenBranch, Statement* elseBranch);
void ast_delete_if_stmt(Statement* statement);
Statement* ast_new_return_stmt(Token keyword, Expression* expression);
void ast_delete_return_stmt(Statement* statement);
Statement* ast_new_print_stmt(Expression* expression);
void ast_delete_print_stmt(Statement* statement);
Statement* ast_new_block_stmt(Block* block);
void ast_delete_block_stmt(Statement* statement);
Statement* ast_new_expression_stmt(Expression* expression);
void ast_delete_expression_stmt(Statement* statement);

void ast_delete_expression(Expression* expression);
Expression* ast_new_call_expr(Expression* callee, ArgumentList* arguments);
void ast_delete_call_expr(Expression* expression);
Expression* ast_new_property_expr(Expression* object, Token property, ExprContext context);
void ast_delete_property_expr(Expression* expression);
Expression* ast_new_super_expr(Token keyword, Token method);
void ast_delete_super_expr(Expression* expression);
Expression* ast_new_assignment_expr(Expression* target, Expression* value);
void ast_delete_assignment_expr(Expression* expression);
Expression* ast_new_compound_assignment_expr(Expression*, Token op, Expression* value);
void ast_delete_compound_assignment_expr(Expression* expression);
Expression* ast_new_logical_expr(Expression* left, Token op, Expression* right);
void ast_delete_logical_expr(Expression* expression);
Expression* ast_new_conditional_expr(Expression* condition, Expression* thenBranch, Expression* elseBranch);
void ast_delete_conditional_expr(Expression* expression);
Expression* ast_new_binary_expr(Expression* left, Token op, Expression* right);
void ast_delete_binary_expr(Expression* expression);
Expression* ast_new_unary_expr(Token op, Expression* expression);
void ast_delete_unary_expr(Expression* expression);
Expression* ast_new_postfix_inc_expr(Token op, Expression* expression);
void ast_delete_postfix_inc_expr(Expression* expression);
Expression* ast_new_prefix_inc_expr(Token op, Expression* expression);
void ast_delete_prefix_inc_expr(Expression* expression);
Expression* ast_new_literal_expr(Token value);
void ast_delete_literal_expr(Expression* expression);
Expression* ast_new_lambda_expr(Function* function);
void ast_delete_lambda_expr(Expression* expression);
Expression* ast_new_identifier_expr(Token identifier, ExprContext context);
void ast_delete_identifier_expr(Expression* expression);

ExpressionList* ast_new_expression_node(Expression* expression);
void ast_expression_list_append(ExpressionList** list, Expression* expression);
void ast_delete_expression_list(ExpressionList* list);
size_t ast_expression_list_length(ExpressionList* list);

ArgumentList* ast_new_argument_node(Expression* expression);
void ast_argument_list_append(ArgumentList** list, Expression* expression);
void ast_delete_argument_list(ArgumentList* list);
size_t ast_argument_list_length(ArgumentList* list);

ParameterList* ast_new_parameter_node(Token parameter);
void ast_parameter_list_append(ParameterList** list, Token parameter);
void ast_delete_parameter_list(ParameterList* list);
size_t ast_parameter_list_length(ParameterList* list);

WhenEntry* ast_new_when_entry(ExpressionList* cases, Statement* body);
void ast_delete_when_entry(WhenEntry* entry);

WhenEntryList* ast_new_when_entry_node(WhenEntry* entry);
void ast_when_entry_list_append(WhenEntryList** list, WhenEntry* entry);
void ast_delete_when_entry_list(WhenEntryList* list);
size_t ast_when_entry_list_length(WhenEntryList* list);

Block* ast_new_block(DeclarationList* body);
void ast_delete_block(Block* block);

void ast_delete_function_body(FunctionBody* body);
FunctionBody* ast_new_expression_function_body(Expression* expression);
void ast_delete_expression_function_body(FunctionBody* body);
FunctionBody* ast_new_block_function_body(Block* block);
void ast_delete_block_function_body(FunctionBody* body);

Function* ast_new_function(ParameterList* parameters, FunctionBody* body);
void ast_delete_function(Function* function);

NamedFunction* ast_new_named_function(Token identifier, Function* function);
void ast_delete_named_function(NamedFunction* function);

NamedFunctionList* ast_new_named_function_node(NamedFunction* function);
void ast_named_function_list_append(NamedFunctionList** list, NamedFunction* function);
void ast_delete_named_function_list(NamedFunctionList* list);
size_t ast_named_function_list_length(NamedFunctionList* list);

DeclarationList* ast_new_declaration_node(Declaration* declaration);
void ast_declaration_list_append(DeclarationList** list, Declaration* declaration);
void ast_delete_declaration_list(DeclarationList* list);
size_t ast_declaration_list_length(DeclarationList* list);

#endif
