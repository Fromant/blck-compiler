#ifndef ERAXC_AST_H
#define ERAXC_AST_H

#include <iostream>

#include "../frontend/syntax/enums.h"

namespace eraxc::syntax::AST {

    struct typedef_node {
        int type;
    };

    struct expr_node {
        operator_type op = NONE;
        operator_type unary = NONE;
        operator_type postfix = NONE;

        bool isInstant = false;
        bool hasParenthesis = false;

        union data {
            expr_node* parenthesis;
            size_t d = 0xFFFFFFFF;
        };

        data data;

        expr_node* right = nullptr;

        expr_node() = default;

        expr_node(operator_type op, size_t data, bool isInstant, operator_type unary = NONE) {
            this->op = op;
            this->data.d = data;
            this->isInstant = isInstant;
            this->unary = unary;
        };

        void print() const {
            auto it = this;

            while (it != nullptr) {
                if (!it->hasParenthesis) {
                    if (it->isInstant) {
                        std::cout << ' ' << find_op(it->unary) << it->data.d << find_op(it->postfix) << ' '
                            << find_op(it->op);
                    }
                    else {
                        std::cout << ' ' << find_op(it->unary) << '{' << it->data.d << '}' << find_op(it->postfix)
                            << ' ' << find_op(it->op);
                    }
                }
                else {
                    std::cout << ' ' << find_op(it->unary) << '(';
                    it->data.parenthesis->print();
                    std::cout << ')' << find_op(it->postfix) << ' ' << find_op(it->op);
                }
                it = it->right;
            }
        }
    };

    struct decl_node {
        bool assigned = false;
        size_t id = -1;
        size_t type = -1;
        expr_node assign_to{};

        void print() const {
            std::cout << type << ":{" << id << '}';
            if (assigned) {
                std::cout << " =";
                assign_to.print();
                std::cout << ";\n";
            }
            else std::cout << ';' << '\n';
        }
    };

    struct statement_node {
        enum statement_type {
            DECLARATION, EXPRESSION, RETURN, SELECTION, LOOP, NONE
        };

        statement_type type = NONE;

        union statement_data {
            expr_node* expr = nullptr;
            decl_node decl;
            expr_node* ret;
            //                selection_onde sel;
            //                loop_node loop;
        };

        void print() const {
            if (type == DECLARATION) {
                data.decl.print();
            }
            else if (type == EXPRESSION) {
                data.expr->print();
                std::cout << ";\n";
            }
            else if (type == RETURN) {
                std::cout << "return ";
                data.ret->print();
                std::cout << ";\n";
            }
        }

        statement_data data;
    };

    struct func_node {
        size_t id;
        size_t return_typename;
        std::vector<decl_node> args;
        std::vector<statement_node> body;

        void print() {
            std::cout << return_typename << " {" << id << "} (";
            //print parameters
            const int n = (int)args.size();
            if (n > 0) {
                for (int i = 0; i < n - 1; i++) {
                    std::cout << args[i].type << " {" << args[i].id << "}, ";
                }
                std::cout << args[n - 1].type << " {" << args[n - 1].id << '}';
            }
            std::cout << ')' << ' ' << '{' << '\n';
            for (statement_node& stat : body) {
                std::cout << '\t';
                stat.print();
            }
            std::cout << "}\n";
        }
    };

    struct node {
        enum node_type {
            DECLARATION, STATEMENT, EXPRESSION, FUNCTION, NONE
        };

        node_type type = NONE;

        union data {
            decl_node decl;
            expr_node expr;
            func_node* func = nullptr;
        };

        data d;

        void print() const;
    };
}

#endif //ERAXC_AST_H
