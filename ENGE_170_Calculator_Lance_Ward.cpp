#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

/*
Final Project
Calculator by Lance Ward

Dr. Walker's ENGE 170
*/


using namespace std;

bool parenthesesCheck;
void fsm(string& expression);

enum States {
    start, resolve_parentheses, handle_exponent, handle_mult_div, handle_add_sub, stop
};

bool shouldRemove(char c) {
    return !isdigit(c) && c != '+' && c != '-' && c != '*' && c != '/' && c != '^' && c != '(' && c != ')' && c != 'e' && c != '.' && c != '%';
}

bool isInteger(double numb) {
    return numb == floor(numb);
}

int getPrecision(const string& number) {
    size_t pointPos = number.find('.');
    if (pointPos != string::npos) {
        return number.length() - pointPos - 1;
    }
    return 0;
}

double applyOperator(double left, double right, char op) {
    //cout << "Calling applyOperator function: Applying operator " << op << " to " << left << " and " << right << endl;
    switch (op) {
    case '+': return left + right;
    case '-': return left - right;
    case '*': return left * right;
    case '/':
        if (right == 0) {
            throw invalid_argument("Error: Cannot divide by zero, undefined");
        } else {
            return left / right;
        }
    case '%':
        if (right == 0) {
            throw invalid_argument("Error: Cannot do modulus with zero, undefined");
        } else {
            return fmod(left, right);
        }
    case '^':
        if (left < 0 && !isInteger(right)) {
            throw invalid_argument("Error: Imaginary number detected, undefined");
        } else {
            return pow(left, right);
        }
    default: throw invalid_argument("Unknown operator! Try again.");
    }
}

string evaluateSequentially(const string& expression, const string& targetOperators) {
    //cout << "Calling evaluateSequentially with input expression: " << expression << endl;
    istringstream iss(expression);
    vector<double> numbers;
    vector<char> operators;
    double num;
    char op;

    if (iss.peek() == '-' && parenthesesCheck == false) {
        iss.get();
        if (iss >> num) {
            if (iss.peek() == '^') {
                    numbers.push_back(-1);
                    operators.push_back('*');
                    numbers.push_back(num);
            } else {
                numbers.push_back(-num);
            }
        }
    } else {
        if (iss >> num) {
            numbers.push_back(num);
        }
    }

    while (iss >> op >> num) {
        //cout << "Streaming operator: " << op << " and number: " << num << endl;
        operators.push_back(op);
        numbers.push_back(num);
    }

    //cout << "Beginning applyOperator while loop in evaluateSequentially..." << endl;
    size_t i = 0;
    
    while (i < operators.size()) {
        if (targetOperators.find(operators[i]) != string::npos) {
            //cout << "Processing expression: " << numbers[i] << " " << operators[i] << " " << numbers[i + 1] << endl;
            double result = applyOperator(numbers[i], numbers[i + 1], operators[i]);
            numbers[i] = result;
            numbers.erase(numbers.begin() + i + 1);
            operators.erase(operators.begin() + i);
        } else {
            ++i;
        }
    }

    ostringstream newExpression;
    
    if (!numbers.empty()) {
        newExpression << numbers[0];
        for (size_t i = 1; i < numbers.size(); ++i) {
            newExpression << operators[i - 1] << numbers[i];
        }
    }

    //cout << "Resulting expression newExpression: " << newExpression.str() << endl;
    return newExpression.str();
}

string resolveInnermostParentheses(string& expression) {
//cout << "Calling resolveInnerMostParentheses with input expression: " << expression << endl;
    
    size_t startPos = expression.find_last_of('(');
    
    while (startPos != string::npos) {
        parenthesesCheck = true;
        size_t endPos = expression.find(')', startPos);
        if (endPos == string::npos) {
            throw invalid_argument("Error: Mismatched parentheses!");
        } else if (endPos == startPos + 1) {
            throw invalid_argument("Error: Empty set of parentheses detected.");
        }

        string subExpr = expression.substr(startPos + 1, endPos - startPos - 1);
        //cout << "Sub-expression found inside parentheses: " << subExpr << endl;
        fsm(subExpr);

        expression.replace(startPos, endPos - startPos + 1, subExpr);

        if (startPos > 0 && expression[startPos - 1] == '-') {
            //cout << "'-' operator found before parentheses, applying negative to result." << endl;
            expression[startPos - 1] = '+'; 
            expression.insert(startPos, "0-");
        }
        
    startPos = expression.find_last_of('(');
    
    }
    return expression;
}

void fsm(string& expression) {
//cout << "Calling fsm with input expression: " << expression << endl;
    
    States state = start;

    while (state != stop) {
        switch (state) {
        case start:
            if (parenthesesCheck == false) {
                
            }
            state = resolve_parentheses;
            break;
        case resolve_parentheses:
            expression = resolveInnermostParentheses(expression);
            state = handle_exponent;
            break;
        case handle_exponent:
            expression = evaluateSequentially(expression, "^");
            state = handle_mult_div;
            break;
        case handle_mult_div:
            expression = evaluateSequentially(expression, "*/%");
            state = handle_add_sub;
            break;
        case handle_add_sub:
            expression = evaluateSequentially(expression, "+-");
            state = stop;
            break;
        case stop:
            break;
        }
    }
}

void doThings() {
    while (true) {
        string expression;
        parenthesesCheck = false;
        cout << "\nEnter the expression to calculate (or type 'exit' to close): ";
        getline(cin, expression);

        if (expression == "exit") {
            cout << "Thanks for trying out my C++ calculator" << endl;
            break;
        }

        if (expression.empty()) {
            cout << "Error: No user input at all!" << endl;
            continue;
        }

        expression.erase(remove_if(expression.begin(), expression.end(), shouldRemove), expression.end());

        bool errorFound = false;
        bool minusFound = false;
        
        if (!isdigit(expression.front()) && (expression.front() != '-' && expression.front() != '(')) {
            cout << "Error: Expression cannot start with '" << expression.front() << "'" << endl;
            continue;
        }
        
        for (size_t i = 0; i < expression.size() - 1; ++i) {
            if (!isdigit(expression[i]) && !isdigit(expression[i + 1]) && expression[i] != '(' && expression[i] != ')' && expression[i + 1] != '(' &&
                ( (expression[i] == expression[i + 1] && expression[i+1] != '-') || (expression[i] != '-' && expression[i+1] != '-') ) &&
                (expression[i] != 'e' && expression[i+1] != '+') ) {
                cout << "Error: Invalid operator sequence '" << expression[i] << expression[i + 1] << "'" << endl;
                cout << "Operators cannot be performed on other operators! Only the minus sign can follow another operator to signify a negative number following." << endl;
                errorFound = true;
                break;  
            }
        }
        
        if (errorFound) {
            continue;  
        }
        
        do {
            minusFound = false;
            size_t i = 0;
            while (i < expression.size() - 1) {
                if (expression[i] == '-' && expression[i+1] == '-') {
                    //cout << "Converting -- to + at position " << i << endl;
                    expression[i] = '+';
                    expression.erase(i+1, 1);
                    minusFound = true;
                } else if (expression[i] == '+' && expression[i+1] == '-') {
                    //cout << "Converting +- to - at position " << i << ", converting to -" << endl;
                    expression[i] = '-';
                    expression.erase(i+1, 1);
                    minusFound = true;
                } else if (expression[i] == '-' && expression[i+1] == '+') {
                    cout << "Error: Invalid operator sequence '-+'" << endl;
                    cout << "Operators cannot be performed on other operators! Only the minus sign can follow another operator to signify a negative number following." << endl;
                    errorFound = true;
                    break;
                } else {
                    i++;
                }
            }
        } while (minusFound);
        
        if (errorFound) {
            continue;  
        }
        
        if (expression.empty()) {
            cout << "Error: No valid input! No characters recognized. Try again." << endl;
            continue;
        }
        
        istringstream iss(expression);
        string token;
        int maxPrecision = 0;

        while (iss >> token) {
            stringstream convert(token);
            double num;
            if (convert >> num && (convert.peek() == EOF || convert.peek() == '.')) {
                int currentPrecision = getPrecision(token);
                if (currentPrecision > maxPrecision) {
                    //cout << "New max precision set at " << currentPrecision << endl;
                    maxPrecision = currentPrecision;
                }
            }
        }
    
        try {
            fsm(expression);
            //cout << "Final expression after FSM: " << expression << endl;
        }
        catch (const invalid_argument& e) {
            cout << e.what() << endl;
            continue;
        }

        cout << fixed << setprecision(maxPrecision) << "Result: " << expression << endl;
    }
}

int main() {
    
    cout << "Calculator can handle +, -, *, /, %, ^, (, ), floating values, and engineering notation." << endl;
    cout << "Calculator can also handle double negatives, e.g. '5---5' == '5-5' and '5^-3' == '5^(-3)'." << endl;
    cout << "Calculator does NOT handle implicit multiplication (with parentheses), undefined, or imaginary numbers.\n" << endl;
    cout << "Note: Calculator parses ALL characters except +-*/%^()0123456789e before calculations," << endl;
    cout << "So spaces and invalid characters will be removed before beginning calculation." << endl;
    
    doThings();
    
    return 0;
}

