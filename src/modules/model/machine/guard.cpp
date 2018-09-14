#include "guard.h"
using namespace std;

Guard::Guard() {

}

Guard::Guard(std::string left, int tmin, int tmax, std::string right) {
    if (left == "[")
        this->left = Bracket::Square;
    else if (left == "(")
        this->left = Bracket::Curly;
    this->tmin = tmin;
    this->tmax = tmax;
    if (right == "]")
        this->right = Bracket::Square;
    else if (right == ")")
        this->right = Bracket::Curly;
}

Guard::Guard(Bracket left, int tmin, int tmax, Bracket right) {
    this->left = left;
    this->tmin = tmin;
    this->tmax = tmax;
    this->right = right;
}

bool Guard::isIntersectionEmpty(Guard other) {
    Bracket left;
    int tmin;
    int tmax;
    Bracket right;

    if (this->tmin > other.tmin) {
        tmin = this->tmin;
        left = this->left;
    }
    else if (this->tmin < other.tmin) {
        tmin = other.tmin;
        left = other.left;
    }
    else {
        tmin = this->tmin;
        if (this->left == Bracket::Square && other.left == Bracket::Square) {
            left = Bracket::Square;
        }
        else {
            left = Bracket::Curly;
        }
    }

    if (this->tmax < other.tmax) {
        tmax = this->tmax;
        right = this->right;
    }
    else if (this->tmax > other.tmax) {
        tmax = other.tmax;
        right = other.right;
    }
    else {
        tmax = this->tmax;
        if (this->right == Bracket::Square && other.right == Bracket::Square) {
            right = Bracket::Square;
        }
        else {
            right = Bracket::Curly;
        }
    }

    if (tmin == tmax && left != right) {
        return true;
    }
    return tmin > tmax;
}

Guard Guard::intersect(Guard other) {
    Bracket left;
    int tmin;
    int tmax;
    Bracket right;

    if (this->tmin > other.tmin) {
        tmin = this->tmin;
        left = this->left;
    }
    if (this->tmin < other.tmin) {
        tmin = other.tmin;
        left = other.left;
    }
    else {
        tmin = this->tmin;
        if (this->left == Bracket::Square && other.left == Bracket::Square) {
            left = Bracket::Square;
        }
        else {
            left = Bracket::Curly;
        }
    }

    if (this->tmax < other.tmax) {
        tmax = this->tmax;
        right = this->right;
    }
    if (this->tmax > other.tmax) {
        tmax = other.tmax;
        right = other.right;
    }
    else {
        tmax = this->tmax;
        if (this->right == Bracket::Square && other.right == Bracket::Square) {
            right = Bracket::Square;
        }
        else {
            right = Bracket::Curly;
        }
    }
    return Guard(left, tmin, tmax, right);
}

bool Guard::equals(Guard other) {
    return (this->left == other.left
            && this->tmin == other.tmin
            && this->tmax == other.tmax
            && this->right == other.right);
}

bool Guard::contains(int x) {
    bool guard1;
    if (this->left == Bracket::Square) {
        guard1 = x >= this->tmin;
    }
    else {
        guard1 = x > this->tmin;
    }
    bool guard2;
    if (this->right == Bracket::Square) {
        guard2 = x <= this->tmax;
    }
    else {
        guard2 = x < this->tmax;
    }
    return guard1 && guard2;
}

std::string Guard::toString() {
    string guardString;
    if (this->left == Bracket::Square)
        guardString+="[";
    else
        guardString+="(";
    guardString += to_string(this->tmin) + ",";
    if (this->tmax != inf)
        guardString += to_string(this->tmax);
    else
        guardString += "∞";
    if (this->right == Bracket::Square)
        guardString+="]";
    else
        guardString+=")";
    return guardString;
}

Guard Guard::substracted(int counter) {
    int newTmin = this->tmin - counter;
    int newTmax = this->tmax - counter;
    Bracket newLeft = this->left;
    Bracket newRight = this->right;
    if (newTmin < 0) {
        newLeft = Bracket::Square;
        newTmin = 0;
    }
    if (newTmax < 0) {
        newRight = Bracket::Curly;
        newTmax = 0;
    }
    return Guard(left, newTmin, newTmax, newRight);
}
