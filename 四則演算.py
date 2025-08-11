import ast
import operator

# 二項演算子
OPS_BIN = {
    ast.Add: operator.add,
    ast.Sub: operator.sub,
    ast.Mult: operator.mul,
    ast.Div: operator.truediv
}

# 単項演算子
OPS_UNARY = {
    ast.UAdd: operator.pos,
    ast.USub: operator.neg
}

def safe_eval(expr, variables):
    """
    四則演算・括弧・単項マイナスのみ許可して文字列式を評価する
    """
    node = ast.parse(expr, mode='eval').body

    def _eval(n):
        # 二項演算
        if isinstance(n, ast.BinOp):
            left = _eval(n.left)
            right = _eval(n.right)
            if type(n.op) not in OPS_BIN:
                raise ValueError(f"許可されていない演算子: {n.op}")
            return OPS_BIN[type(n.op)](left, right)

        # 単項演算（+A, -A）
        elif isinstance(n, ast.UnaryOp):
            operand = _eval(n.operand)
            if type(n.op) not in OPS_UNARY:
                raise ValueError(f"許可されていない単項演算子: {n.op}")
            return OPS_UNARY[type(n.op)](operand)

        # 変数
        elif isinstance(n, ast.Name):
            if n.id not in variables:
                raise ValueError(f"未定義の変数: {n.id}")
            return variables[n.id]

        # 数値
        elif isinstance(n, ast.Constant):
            if not isinstance(n.value, (int, float)):
                raise ValueError("数値以外のリテラルは許可されません")
            return n.value

        else:
            raise ValueError(f"許可されていない構文: {n}")

    return _eval(node)

# 使用例
vars_dict = {
    "A": 10,
    "B": 20,
    "C": 30,
    "D": 5
}

expressions = [
    "A + B * C / D",    # 通常
    "(A + B) * C / D",  # 括弧優先
    "-A + B",           # 単項マイナス
    "A + (-B) * C"      # 括弧＋単項マイナス
]

for expr in expressions:
    print(f"{expr} = {safe_eval(expr, vars_dict)}")


#include <iostream>
#include <string>
#include "exprtk.hpp"  // exprtk.hpp をインクルード

int main() {
    typedef double T;
    std::string expression_str = "A + B * C / D";

    exprtk::symbol_table<T> symbol_table;
    T A = 10, B = 20, C = 30, D = 5;
    symbol_table.add_variable("A", A);
    symbol_table.add_variable("B", B);
    symbol_table.add_variable("C", C);
    symbol_table.add_variable("D", D);

    exprtk::expression<T> expression;
    expression.register_symbol_table(symbol_table);

    exprtk::parser<T> parser;
    if (!parser.compile(expression_str, expression)) {
        std::cerr << "パースエラー: " << parser.error() << "\n";
        return 1;
    }

    std::cout << expression.value() << std::endl; // 130
    return 0;
}




#include <iostream>
#include <stack>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

double applyOp(double a, double b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    }
    return 0;
}

double evaluate(const std::string &tokens, const std::map<std::string, double> &vars) {
    std::stack<double> values;
    std::stack<char> ops;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (isspace(tokens[i])) continue;

        // 変数 (アルファベット)
        if (isalpha(tokens[i])) {
            std::string var(1, tokens[i]);
            values.push(vars.at(var));
        }
        // 数値
        else if (isdigit(tokens[i])) {
            double val = 0;
            while (i < tokens.size() && isdigit(tokens[i])) {
                val = (val * 10) + (tokens[i] - '0');
                i++;
            }
            i--;
            values.push(val);
        }
        // '('
        else if (tokens[i] == '(') {
            ops.push(tokens[i]);
        }
        // ')'
        else if (tokens[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(a, b, op));
            }
            ops.pop();
        }
        // 演算子
        else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(tokens[i])) {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(a, b, op));
            }
            ops.push(tokens[i]);
        }
    }

    while (!ops.empty()) {
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(a, b, op));
    }

    return values.top();
}

int main() {
    std::map<std::string, double> vars = {{"A", 10}, {"B", 20}, {"C", 30}, {"D", 5}};
    std::string expr = "A+B*C/D";

    double result = evaluate(expr, vars);
    std::cout << result << std::endl; // 130
}
